# KFS learning workbook implementation

Approved design: docs/specs/2026-10-14-kfs-learning-workbook-design.md.

1. Author M00–M05 foundations from CHECKLIST.md, actual source and verified OSDev/Multiboot/NASM/GNU documentation. Produce structured content outside the repository.
2. In parallel, author M06–M11 and B01/B02 from the actual source and verified GDT/stack/Intel/PS2 resources. Preserve mandatory-first and assessment distinctions.
3. In parallel, implement learn.html's offline editorial interface, quiz state, per-option feedback, exercises, glossary, review, tools, local persistence/export/reset and responsive/print styles. The integration owner embeds both content sets and adds a README entry point.
4. Review content consistency and source provenance. Run the actual page in Chromium; exercise quiz transitions, review, notes, persistence, export/reset, storage failures and conceptual tools. Inspect desktop/mobile renders, overflow, keyboard access, print and offline behavior. Do not build or modify the kernel.
5. Remove any temporary scaffolding from the deliverable; retain a complete standalone HTML file. Report actual verification and how to open the workbook.
