#ifndef AIWS_PROCESSING_TYPES_HPP
#define AIWS_PROCESSING_TYPES_HPP

#include <cstddef>
#include <string>

struct Chunk {
    std::string id;
    std::string document_id;
    std::string document_title;
    std::string normalized_text;
    std::size_t sequence = 0;
    std::size_t token_count = 0;
    std::size_t source_begin = 0;
    std::size_t source_end = 0;
};

struct SearchResult {
    Chunk chunk;
    double score = 0.0;
};

struct ContextItem {
    Chunk chunk;
    std::string normalized_text;
    std::size_t token_count = 0;
    bool truncated = false;
};

#endif
