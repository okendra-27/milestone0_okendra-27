#ifndef AIWORKSPACE_DOCUMENT_CHUNK_HPP
#define AIWORKSPACE_DOCUMENT_CHUNK_HPP

#include <vector>

#include "Document.hpp"
#include "aiws/processing_types.hpp"

class DocumentChunker {
public:
    std::vector<Chunk> chunk(const Document& document) const;
};

#endif
