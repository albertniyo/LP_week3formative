# Project 1 — ELF Binary Forensics and Comparative Threat Assessment

## Source

- `data_sync.c` — scan a directory, list names that would need sync.
- `data_sync_v2.c` — same scan plus `stat()` size metadata and timestamped log file.

## Compilation 

```sh
gcc -Wall -Wextra -no-pie data_sync.c -o data_sync
gcc -Wall -Wextra -g -no-pie data_sync_v2.c -o data_sync_v2
```

`-no-pie` produces classic `EXEC` ELF type (matches the investigated binaries). `-g` on v2 embeds DWARF debug sections.

## Investigation (do not execute the binaries)

```sh
./evidence.sh          # regenerates evidence.txt from file/readelf/nm/objdump
```

**Disclaimer:** No execution of the target binaries, no `ldd` on untrusted samples at all.

## Deliverables

| File | Role |
|---|---|
| `data_sync.c` / `data_sync_v2.c` | source codes|
| `data_sync` / `data_sync_v2` | compiled ELF |
| `evidence.txt` | command outputs |
| `forensic_report.md` | forensic assessment with comparison table |

## Interpretation notes
We interprete those concepts as 
- **Sections vs segments:** sections are link/debug named regions; segments or program headers are what the loader maps into memory.
- **Undefined vs imported vs NEEDED:** `U` / `nm -D` entries are dynamic imports; `NEEDED` names the shared libraries that must supply them (`libc.so.6` here).
- **Why `nm` may miss symbols:** strip removes `.symtab`; some calls are inlined; so only dynamic symbols appear in `nm -D`.
