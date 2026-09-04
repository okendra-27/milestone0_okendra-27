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
