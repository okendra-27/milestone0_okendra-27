#ifndef AIWS_PROCESSING_CORE_HPP
#define AIWS_PROCESSING_CORE_HPP

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "DocumentChunk.hpp"
#include "TextProcessor.hpp"
#include "Workspace.hpp"
#include "aiws/processing_types.hpp"

class CorpusIndex {
public:
    void rebuild(const Workspace& workspace);
    std::size_t document_frequency(const std::string& term) const;
    std::size_t term_frequency(const std::string& term, const std::string& chunk_id) const;
    const std::vector<Chunk>& chunks() const noexcept;
    std::size_t document_order(const std::string& document_id) const;

private:
    TextProcessor processor_;
    std::vector<Chunk> chunks_;
    std::unordered_map<std::string, std::size_t> chunk_positions_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::size_t>> frequencies_;
    std::unordered_map<std::string, std::size_t> document_orders_;
};

class Retriever {
public:
    std::vector<SearchResult> search(const CorpusIndex& index, const std::string& query, int k) const;
};

class ContextBuilder {
public:
    std::vector<ContextItem> build(const std::vector<SearchResult>& results, std::size_t token_budget) const;
};

class ProcessingCore {
public:
    void rebuild(const Workspace& workspace);
    std::vector<SearchResult> search(const std::string& query, int k) const;
    std::vector<ContextItem> build_context(const std::string& query, int k, std::size_t token_budget) const;
    std::size_t document_frequency(const std::string& term) const;
    std::size_t term_frequency(const std::string& term, const std::string& chunk_id) const;
    const std::vector<Chunk>& chunks() const noexcept;

private:
    CorpusIndex index_;
    Retriever retriever_;
    ContextBuilder context_builder_;
};

#endif
