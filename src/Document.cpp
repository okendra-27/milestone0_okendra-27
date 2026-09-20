#include "Document.hpp"
#include <fstream>

Document::Document(std::string title, std::string contents) 
    : id_(title),
    title_(std::move(title)),
    sourcePath_(),
    contents_(std::move(contents))
{

}

Document::Document(std::string id, std::string title, std::string contents)
    : id_(std::move(id)),
    title_(std::move(title)),
    contents_(std::move(contents))
{
}

bool Document::operator==(const Document& other) const 
{
    return id_ == other.id_
        && title_ == other.title_
        && sourcePath_ == other.sourcePath_
        && contents_ == other.contents_;
}

bool Document::operator!=(const Document& other) const 
{
    return !(*this == other);

}

bool Document::load(const std::string& path) 
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    std::string newContent(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    std::size_t pos = path.find_last_of("/\\");
    std::string newTitle = path.substr(pos == std::string::npos ? 0 : pos + 1);

    sourcePath_ = path;
    title_ = std::move(newTitle);
    contents_ = std::move(newContent);

    return true;

}

const std::string& Document::title() const noexcept 
{
    return title_;
}

const std::string& Document::id() const noexcept
{
    return id_;
}

const std::string& Document::sourcePath() const noexcept 
{
    return sourcePath_;

}

const std::string& Document::contents() const noexcept 
{
    return contents_;
  
}

void Document::setTitle(std::string title) 
{
    title_ = std::move(title);
  
}

void Document::setId(std::string id)
{
    id_ = std::move(id);
}

std::size_t Document::characterCount() const noexcept 
{
    return contents_.size();
    
}

bool Document::empty() const noexcept 
{
    return contents_.empty();
   
}
