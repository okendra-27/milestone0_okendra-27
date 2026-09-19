# M0 Design and Understanding Note

Answer briefly in your own words. This is not intended to be a long report.

1. What responsibility belongs to `Workspace`, and what responsibilities belong to `Document`, `Prompt`, and `Message` instead?

Workspace is essentially the storage of the documents, prompts and messages. Document, prompt and message are responsible for their own functions such as text, titles, and features such as empty().

2. Why are the collections inside `Workspace` private? Explain the purpose of the const and non-const `At` overloads.

They are private so that it is not modified from a different source. The const 'At' makes it so the reference is only able to be read and not edited. Where as the non-const is for making an editable version of the reference.

3. Explain one meaningful test you added. What behavior does it check, and what implementation error could it catch?

One test I added was the prompt_basic_test which essentially makes sure that the appropriate title and text is added. It could catch the implementation error where the title or text is not inserted/updated correctly.

4. Describe one implementation decision that you verified, tested, or revised before submitting your work.

I verified the method throws on out of range indices. Returning an error is not enough and an exception was needed in order to enforce strictness that won't break the code.

5. If generative AI was used, disclose it as required by course policy. If no generative AI was used, state that. The disclosure itself is not used as proof of authorship or understanding.

I did not use generative AI on Milestone 0.

# Milestone 1

1. the responsibilities of your major subsystems and the principal data/control flow between them;

`TextProcessor` normalizes document and query text and finds paragraph boundaries.

`DocumentChunker` turns one `Document` into ordered `Chunk` objects with source information, normalized text, and token counts.

`CorpusIndex` owns the current searchable corpus. It stores the chunks and term frequencies for each chunk. 

`Retriever` uses the index to score and rank matching chunks. 

`ContextBuilder` takes ranked results and creates context items within a token budget. 

`ProcessingCore` coordinates these components.

The main flow is:

Document -> DocumentChunker -> CorpusIndex -> Retriever -> ContextBuilder


2. the main data structures and ownership/lifetime choices in your design; 

`Workspace` owns its documents. 

`CorpusIndex` owns its generated chunks and frequency maps. A chunk stores its document ID, title, sequence number, normalized text, token count, and original source span.

The index uses a vector to keep chunks in source order. It uses maps from terms to chunk IDs and frequencies so searches do not need to rescan every document every single time which can increase time/space complexity essentially.

3. how important corpus and rebuild invariants are maintained;


A rebuild creates new chunks, document data, and frequency data in local variables first. The old corpus is only replaced after all documents are processed successfully. This keeps the previous valid corpus available if duplicate document IDs cause an exception.

Each rebuild replaces all old index data, so removed documents and old terms do not stay. Duplicate document IDs are rejected because chunk IDs must remain unique.

4. your testing strategy and what kinds of failures your tests are intended to detect; 

The tests use `assert()`.

Text-processing tests check lowercase conversion, separators, digits, empty input, and paragraph boundaries. 

Chunking tests check the 120-token maximum, 20-token overlap, paragraph-preferred boundaries, IDs, and source positions.

Index tests check document frequency, term frequency, invalid multi-word terms, missing chunk IDs, replacement rebuilds, and duplicate-ID rebuild failures. 

Retrieval tests check empty queries, unknown terms.

Context tests check zero budgets, partial chunk truncation, and token limits.

5. at least two design alternatives you considered and why you selected your final approach.

One option was to place all M1 behavior in `ProcessingCore`. This was not selected because chunking, indexing, ranking, and context construction would become difficult to test independently.

Another option was to rescan all documents during every search. This was not selected because an index is simpler for repeated searches and directly supports document-frequency and term-frequency calculations.

I selected separate small components with simple vectors and maps. This keeps the code readable, allows direct tests for each responsibility, and avoids advanced designs that are unnecessary for M1.
