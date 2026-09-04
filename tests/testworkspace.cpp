#include <cassert>
#include <iostream>
#include <fstream>

#include "Prompt.hpp"
#include "Workspace.hpp"
#include "Document.hpp"
#include "Message.hpp"

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

int main() {
    // Prompt
    prompt_basic_test();

    // Message
    message_basic_test();

    // Document
    document_construction_test();
    document_load_failure_test();

    // Workspace
    workspace_retrieval_test();

    std::cout << "All M0/M1 tests passed\n";
    return 0;
}
