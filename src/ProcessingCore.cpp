#include "aiws/processing_core.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <unordered_set>

namespace aiws {

namespace {
std::vector<std::string> terms(const std::string& text) {
    std::vector<std::string> result;
    std::string term;

    for (char ch : text) {
        if (ch == ' ') {
            if (!term.empty()) {
                result.push_back(term);
                term.clear();
            }
        } else {
            term.push_back(ch);
        }
    }

    if (!term.empty()) {
        result.push_back(term);
    }

    return result;
}

std::string normalized_term(
    const TextProcessor& processor,
    const std::string& input)
{
    std::vector<std::string> result =
        terms(processor.normalize(input));

    if (result.empty()) return "";

    if (result.size() != 1) {
        throw std::invalid_argument(
            "term must contain one normalized token");
    }

    return result[0];
}
}

void CorpusIndex::rebuild(const Workspace& workspace) {
    std::vector<Chunk> chunks;
    std::unordered_map<std::string, std::size_t> positions;
    std::unordered_map<std::string,
        std::unordered_map<std::string, std::size_t>> frequencies;
    std::unordered_map<std::string, std::size_t> orders;

    DocumentChunker chunker;

    for (std::size_t index = 0;
         index < workspace.documentCount();
         ++index) {

        const Document& document = workspace.documentAt(index);

        if (!orders.emplace(document.id(), index).second) {
            throw std::invalid_argument("duplicate document id");
        }

        for (const Chunk& chunk : chunker.chunk(document)) {
            positions.emplace(chunk.id, chunks.size());

            for (const std::string& term : terms(chunk.normalized_text)) {
                ++frequencies[term][chunk.id];
            }

            chunks.push_back(chunk);
        }
    }

    chunks_ = std::move(chunks);
    chunk_positions_ = std::move(positions);
    frequencies_ = std::move(frequencies);
    document_orders_ = std::move(orders);
}

std::size_t CorpusIndex::document_frequency(
    const std::string& term) const
{
    std::string value = normalized_term(processor_, term);

    auto found = frequencies_.find(value);

    return value.empty() || found == frequencies_.end()
        ? 0
        : found->second.size();
}

std::size_t CorpusIndex::term_frequency(
    const std::string& term,
    const std::string& chunk_id) const
{
    std::string value = normalized_term(processor_, term);

    auto found = frequencies_.find(value);

    if (value.empty() || found == frequencies_.end()) {
        return 0;
    }

    auto chunk = found->second.find(chunk_id);

    return chunk == found->second.end()
        ? 0
        : chunk->second;
}

const std::vector<Chunk>& CorpusIndex::chunks() const noexcept {
    return chunks_;
}

std::size_t CorpusIndex::document_order(
    const std::string& id) const
{
    return document_orders_.at(id);
}

std::vector<SearchResult> Retriever::search(
    const CorpusIndex& index,
    const std::string& query,
    int k) const
{
    if (k < 0) {
        throw std::invalid_argument("k cannot be negative");
    }

    if (k == 0) return {};

    TextProcessor processor;
    std::unordered_set<std::string> query_terms;

    for (const std::string& term :
         terms(processor.normalize(query))) {
        query_terms.insert(term);
    }

    if (query_terms.empty()) return {};

    std::vector<SearchResult> results;

    for (const Chunk& chunk : index.chunks()) {
        double base = 0.0;
        std::size_t matched = 0;

        for (const std::string& term : query_terms) {
            std::size_t frequency =
                index.term_frequency(term, chunk.id);

            if (frequency == 0) continue;

            ++matched;

            double tf =
                1.0 + std::log(static_cast<double>(frequency));

            double idf =
                std::log(
                    (static_cast<double>(index.chunks().size()) + 1.0) /
                    (static_cast<double>(
                        index.document_frequency(term)) + 1.0)
                ) + 1.0;

            base += tf * idf;
        }

        if (matched > 0) {
            double score =
                base *
                (1.0 +
                 0.10 *
                 static_cast<double>(matched) /
                 query_terms.size());

            results.push_back({
                chunk,
                std::round(score * 1000000000000.0) /
                1000000000000.0
            });
        }
    }

    std::sort(
        results.begin(),
        results.end(),
        [&index](const SearchResult& left,
                 const SearchResult& right) {

            if (left.score != right.score) {
                return left.score > right.score;
            }

            std::size_t left_order =
                index.document_order(left.chunk.document_id);

            std::size_t right_order =
                index.document_order(right.chunk.document_id);

            return left_order == right_order
                ? left.chunk.sequence < right.chunk.sequence
                : left_order < right_order;
        });

    if (results.size() > static_cast<std::size_t>(k)) {
        results.resize(static_cast<std::size_t>(k));
    }

    return results;
}

std::vector<ContextItem> ContextBuilder::build(
    const std::vector<SearchResult>& results,
    std::size_t budget) const
{
    std::vector<ContextItem> context;
    std::unordered_set<std::string> used;

    for (const SearchResult& result : results) {
        if (budget == 0 ||
            !used.insert(result.chunk.id).second) {
            continue;
        }

        if (result.chunk.token_count <= budget) {
            context.push_back({
                result.chunk,
                result.chunk.normalized_text,
                result.chunk.token_count,
                false
            });

            budget -= result.chunk.token_count;
        } else {
            std::string prefix;

            std::vector<std::string> chunk_terms =
                terms(result.chunk.normalized_text);

            for (std::size_t index = 0; index < budget; ++index) {
                if (!prefix.empty()) {
                    prefix.push_back(' ');
                }

                prefix += chunk_terms[index];
            }

            context.push_back({
                result.chunk,
                prefix,
                budget,
                true
            });

            break;
        }
    }

    return context;
}

void ProcessingCore::rebuild(
    const Workspace& workspace)
{
    index_.rebuild(workspace);
}

std::vector<SearchResult> ProcessingCore::search(
    const std::string& query,
    int k) const
{
    return retriever_.search(index_, query, k);
}

std::vector<ContextItem> ProcessingCore::build_context(
    const std::string& query,
    int k,
    std::size_t budget) const
{
    return context_builder_.build(
        search(query, k),
        budget);
}

std::size_t ProcessingCore::document_frequency(
    const std::string& term) const
{
    return index_.document_frequency(term);
}

std::size_t ProcessingCore::term_frequency(
    const std::string& term,
    const std::string& id) const
{
    return index_.term_frequency(term, id);
}

const std::vector<Chunk>& ProcessingCore::chunks() const noexcept {
    return index_.chunks();
}

} // namespace aiws