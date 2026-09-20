#pragma once
#include <string>
#include <vector>

struct Paragraph {
    std::size_t begin;
    std::size_t end;
    std::string raw_content;
};

class TextProcessor {
public:
    std::string normalize(const std::string& raw_content) const;
    std::vector<Paragraph> extract_paragraphs(const std::string& raw_content) const;
};
