#include "aiws/Workspace.hpp"
#include <stdexcept>


Workspace::Workspace(std::string name) 
    :name_(std::move(name))
{

}

bool Workspace::operator==(const Workspace& other) const 
{
    return name_ == other.name_
        && documents_ == other.documents_
        && prompts_ == other.prompts_
        && messages_ == other.messages_;

}

bool Workspace::operator!=(const Workspace& other) const 
{
    return !(*this == other);

}

const std::string& Workspace::name() const noexcept 
{
    return name_;

}

void Workspace::setName(std::string name) 
{
    name_ = std::move(name);

}

void Workspace::addDocument(Document document) {
    documents_.push_back(std::move(document));

}

void Workspace::addPrompt(Prompt prompt) 
{
    prompts_.push_back(std::move(prompt));

}

void Workspace::addMessage(Message message) 
{
    messages_.push_back(std::move(message));

}

std::size_t Workspace::documentCount() const noexcept 
{
    return documents_.size();
}

std::size_t Workspace::promptCount() const noexcept 
{
    return prompts_.size();
}

std::size_t Workspace::messageCount() const noexcept 
{
    return messages_.size();
}

Document& Workspace::documentAt(std::size_t index) {
    if (index >= documents_.size()) {
        throw std::out_of_range("Out of range.. document index");
    }
    return documents_[index];
}

const Document& Workspace::documentAt(std::size_t index) const {
    if (index >= documents_.size()) {
        throw std::out_of_range("Out of range.. document index");
    }
    return documents_[index];
}

Prompt& Workspace::promptAt(std::size_t index) {
    if (index >= prompts_.size()) {
        throw std::out_of_range("Out of range.. prompt index");
    }
    return prompts_[index];
}

const Prompt& Workspace::promptAt(std::size_t index) const {
    if (index >= prompts_.size()) {
        throw std::out_of_range("Out of range.. prompt index");
    }
    return prompts_[index];
}

Message& Workspace::messageAt(std::size_t index) {
    if (index >= messages_.size()) {
        throw std::out_of_range("Out of range.. message index");
    }
    return messages_[index];
}

const Message& Workspace::messageAt(std::size_t index) const {
    if (index >= messages_.size()) {
        throw std::out_of_range("Out of range.. message index");
    }
    return messages_[index];
}