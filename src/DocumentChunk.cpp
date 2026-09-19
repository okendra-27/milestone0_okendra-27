#include "DocumentChunk.hpp"

#include <algorithm>
#include <cctype>
#include <string>

#include "TextProcessor.hpp"

namespace {
struct Token {
    std::string text;
    std::size_t begin;
    std::size_t end;
};

std::vector<Token> tokenize(const std::string& text) {
    std::vector<Token> tokens;
    std::size_t index = 0;
    while (index < text.size()) {
        while (index < text.size() && !std::isalnum(static_cast<unsigned char>(text[index]))) ++index;
        std::size_t begin = index;
        std::string term;
        while (index < text.size() && std::isalnum(static_cast<unsigned char>(text[index]))) {
            unsigned char ch = static_cast<unsigned char>(text[index]);
            term.push_back(std::isalpha(ch) ? static_cast<char>(std::tolower(ch)) : static_cast<char>(ch));
            ++index;
        }
        if (!term.empty()) tokens.push_back({term, begin, index});
    }
    return tokens;
}
}

std::vector<Chunk> DocumentChunker::chunk(const Document& document) const {
    std::vector<Token> tokens = tokenize(document.contents());
    std::vector<Chunk> chunks;
    if (tokens.empty()) return chunks;

    TextProcessor processor;
    std::vector<std::size_t> boundaries;
    for (const Paragraph& paragraph : processor.extract_paragraphs(document.contents())) {
        std::size_t count = 0;
        while (count < tokens.size() && tokens[count].end <= paragraph.end) ++count;
        if (count > 0 && count < tokens.size()) boundaries.push_back(count);
    }

    std::size_t begin = 0;
    std::size_t sequence = 0;
    while (begin < tokens.size()) {
        std::size_t end = std::min(begin + 120, tokens.size());
        if (end < tokens.size()) {
            for (std::size_t boundary : boundaries) {
                if (boundary >= begin + 100 && boundary <= begin + 120) end = boundary;
            }
        }
        std::string normalized_text;
        for (std::size_t index = begin; index < end; ++index) {
            if (!normalized_text.empty()) normalized_text.push_back(' ');
            normalized_text += tokens[index].text;
        }
        chunks.push_back({document.id() + "#" + std::to_string(sequence), document.id(), document.title(), normalized_text,
                          sequence, end - begin, tokens[begin].begin, tokens[end - 1].end});
        ++sequence;
        if (end == tokens.size()) break;
        begin = end - 20;
    }
    return chunks;
}
