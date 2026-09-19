#include <cassert>
#include <iostream>
#include <stdexcept>

#include "Prompt.hpp"
#include "Workspace.hpp"
#include "Document.hpp"
#include "Message.hpp"
#include "TextProcessor.hpp"
#include "DocumentChunk.hpp"
#include "aiws/processing_core.hpp"

void prompt_basic_test() {
    Prompt p("Hello World", "Prompt recieved");
    assert(p.title() == "Hello World");
    assert(p.text() == "Prompt recieved");
    assert(!p.empty());

    p.setTitle("Title Update");
    p.setText("Text Update");
    assert(p.title() == "Title Update");
    assert(p.text() == "Text Update");

}


void message_basic_test() {
    Message m(MessageRole::Assistant, "Hello!");
    assert(m.role() == MessageRole::Assistant);
    assert(m.text() == "Hello!");
    assert(!m.empty());

    m.setRole(MessageRole::System);
    m.setText("Updated");
    assert(m.role() == MessageRole::System);
    assert(m.text() == "Updated");
}

void document_construction_test() {
    Document d("Hello", "World");
    assert(d.title() == "Hello");
    assert(d.contents() == "World");
    assert(d.sourcePath().empty());
    assert(!d.empty());
}

void document_load_failure_test() {
    Document d("OldTitle", "OldContents");
    bool ok = d.load("file_does_not_exist_999.txt");
    assert(!ok);

    assert(d.title() == "OldTitle");
    assert(d.contents() == "OldContents");
    assert(d.sourcePath().empty());
}

void workspace_retrieval_test() {
    Workspace w("Demo");

    w.addDocument(Document("D1", "Text1"));
    w.addPrompt(Prompt("P1", "Prompt1"));
    w.addMessage(Message(MessageRole::Assistant, "Hi"));

    assert(w.documentAt(0).title() == "D1");
    assert(w.promptAt(0).title() == "P1");
    assert(w.messageAt(0).role() == MessageRole::Assistant);
}

void normalization_text_processing_test() {
    TextProcessor tp;

    assert(tp.normalize("Hello, WORLD") == "hello world");

    assert(tp.normalize("R2-D2") == "r2 d2");

    assert(tp.normalize("!!!") == "");

    assert(tp.normalize("A---B") == "a b");
}


void paragraph_text_processing_test() {
    TextProcessor processor;
    std::vector<Paragraph> paragraphs = processor.extract_paragraphs("first\n \t\nsecond\r\n\r\nthird");
    assert(paragraphs.size() == 3);
    assert(paragraphs[0].raw_content == "first");
    assert(paragraphs[1].raw_content == "second");
    assert(paragraphs[2].raw_content == "third");
}

std::string words(const std::string& prefix, int count) {
    std::string result;

    for (int index = 0; index < count; ++index) {
        if (!result.empty()) {
            result += " ";
        }

        result += prefix + std::to_string(index);
    }

    return result;
}

void chunk_boundary_test() {
    DocumentChunker chunker;
    Document document("long", "Long", words("word", 140));
    std::vector<Chunk> chunks = chunker.chunk(document);
    assert(chunks.size() == 2);
    assert(chunks[0].id == "long#0");
    assert(chunks[0].token_count == 120);
    assert(chunks[1].id == "long#1");
    assert(chunks[1].token_count == 40);
    assert(chunks[1].normalized_text.find("word100") == 0);
    assert(chunks[0].source_begin == 0);
    assert(chunks[0].source_end < document.contents().size());
}

void paragraph_preference_chunk_test() {
    DocumentChunker chunker;
    Document document("paragraph", "Paragraph", words("first", 110) + "\n\n" + words("second", 30));
    std::vector<Chunk> chunks = chunker.chunk(document);
    assert(chunks.size() == 2);
    assert(chunks[0].token_count == 110);
    assert(chunks[1].token_count == 50);
    assert(chunks[1].normalized_text.find("first90") == 0);
}

void corpus_index_rebuild_test() {
    Workspace workspace("Corpus");
    workspace.addDocument(Document("one", "One", "Apple apple banana"));
    workspace.addDocument(Document("two", "Two", "banana carrot"));
    CorpusIndex index;
    index.rebuild(workspace);
    assert(index.chunks().size() == 2);
    assert(index.document_frequency("APPLE") == 1);
    assert(index.document_frequency("banana") == 2);
    assert(index.term_frequency("apple", "one#0") == 2);
    assert(index.term_frequency("apple", "missing#0") == 0);
    assert(index.term_frequency("!!!", "one#0") == 0);
    bool threw = false;
    try { index.term_frequency("apple banana", "one#0"); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    Workspace replacement("Replacement");
    replacement.addDocument(Document("three", "Three", "durian"));
    index.rebuild(replacement);
    assert(index.chunks().size() == 1);
    assert(index.document_frequency("apple") == 0);
    assert(index.document_frequency("durian") == 1);

    Workspace duplicate("Duplicate");
    duplicate.addDocument(Document("same", "First", "old"));
    duplicate.addDocument(Document("same", "Second", "new"));
    threw = false;
    try { index.rebuild(duplicate); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    assert(index.chunks().size() == 1);
    assert(index.document_frequency("durian") == 1);
}

void ranking_and_context_test() {
    Workspace workspace("Search");
    workspace.addDocument(Document("first", "First", "apple apple banana"));
    workspace.addDocument(Document("second", "Second", "apple banana"));
    ProcessingCore core;
    core.rebuild(workspace);
    std::vector<SearchResult> results = core.search("apple banana apple", 5);
    assert(results.size() == 2);
    assert(results[0].chunk.document_id == "first");
    assert(results[0].score > results[1].score);
    assert(core.search("!!!", 5).empty());
    assert(core.search("unknown", 5).empty());
    assert(core.search("apple", 0).empty());
    bool threw = false;
    try { core.search("apple", -1); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    std::vector<ContextItem> context = core.build_context("apple", 2, 2);
    assert(context.size() == 1);
    assert(context[0].token_count == 2);
    assert(context[0].normalized_text == "apple apple");
    assert(context[0].truncated);
    assert(core.build_context("apple", 2, 0).empty());
}

void deterministic_and_end_to_end_test() {
    Workspace workspace("End To End");
    workspace.addDocument(Document("a", "Alpha", "shared alpha"));
    workspace.addDocument(Document("b", "Beta", "shared beta"));
    ProcessingCore core;
    core.rebuild(workspace);
    std::vector<SearchResult> results = core.search("shared", 2);
    assert(results.size() == 2);
    assert(results[0].chunk.document_id == "a");
    assert(results[1].chunk.document_id == "b");
    std::vector<ContextItem> context = core.build_context("shared", 2, 4);
    assert(context.size() == 2);
    assert(context[0].chunk.document_title == "Alpha");
    assert(context[1].chunk.document_title == "Beta");
    assert(context[0].token_count + context[1].token_count == 4);
}

int main() {
    prompt_basic_test();
    message_basic_test();
    document_construction_test();
    document_load_failure_test();
    workspace_retrieval_test();
    normalization_text_processing_test();
    paragraph_text_processing_test();
    chunk_boundary_test();
    paragraph_preference_chunk_test();
    corpus_index_rebuild_test();
    ranking_and_context_test();
    deterministic_and_end_to_end_test();
    std::cout << "All M1 tests passed\n";
    return 0;
}
