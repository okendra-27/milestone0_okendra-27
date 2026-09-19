#include "TextProcessor.hpp"

#include <cctype>

std::string TextProcessor::normalize(const std::string& raw_content) const
{
    std::string result;
    bool need_space = false;
    for (unsigned char ch : raw_content) {
        if (std::isalpha(ch) || std::isdigit(ch)) {
            if (need_space) {
                result.push_back(' ');
                need_space = false;
            }
            result.push_back(std::isalpha(ch) ? static_cast<char>(std::tolower(ch)) : static_cast<char>(ch));
        } else if (!result.empty()) {
            need_space = true;
        }
    }
    return result;
}

std::vector<Paragraph> TextProcessor::extract_paragraphs(const std::string& raw_content) const
{
    std::vector<Paragraph> paragraphs;
    std::size_t begin = 0;
    std::size_t index = 0;
    while (index < raw_content.size()) {
        if (raw_content[index] != '\n') {
            ++index;
            continue;
        }
        std::size_t next = index + 1;
        while (next < raw_content.size() && (raw_content[next] == ' ' || raw_content[next] == '\t' || raw_content[next] == '\r')) {
            ++next;
        }
        if (next < raw_content.size() && raw_content[next] == '\n') {
            std::size_t end = index;
            if (end > begin && raw_content[end - 1] == '\r') {
                --end;
            }
            paragraphs.push_back({begin, end, raw_content.substr(begin, end - begin)});
            begin = next + 1;
            index = begin;
        } else {
            ++index;
        }
    }
    if (begin < raw_content.size()) {
        paragraphs.push_back({begin, raw_content.size(), raw_content.substr(begin)});
    }
    return paragraphs;
}
