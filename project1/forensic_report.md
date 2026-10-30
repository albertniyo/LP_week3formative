# Forensic Assessment Report

## Purpose and methodology

Two ELF executables (`data_sync`, `data_sync_v2`) were investigated as possible file-synchronization utilities. Both were compiled from controlled C sources and examined with **static** tools only (`file`, `readelf`, `nm`, `objdump`). Neither binary was executed. Full command output is in `evidence.txt`.

## Key findings

Both binaries are **ELF64, little-endian, x86-64**, type **EXEC** (non-PIE), **dynamically linked** to `libc.so.6`, and **not stripped**. Entry points are `_start` at `0x401150` (`data_sync`) and `0x401130` (`data_sync_v2`); the dynamic linker / `__libc_start_main` transfers control to `main`, which is present in both symbol tables.

Shared structural sections include `.text`, `.rodata`, `.data`, `.bss`, `.dynsym`, `.dynamic`, `.symtab`, and `.strtab`. **Segments** (program headers) describe loadable memory regions (like `LOAD`, `INTERP`, `DYNAMIC`); **sections** name finer link-time regions inside those mappings.

Imported symbols for `data_sync` centre on directory scanning: `opendir`, `readdir`, `closedir`, `printf`, `perror`. 

While `data_sync_v2` retains those and adds `stat`, `fopen`/`fclose`/`fprintf`, `time`/`localtime`, `snprintf`, `strcmp`, `strerror`, and `__errno_location` — consistent with metadata checks and file logging. Only `data_sync_v2` carries DWARF `.debug_*` sections (`file` reports “with debug_info” as well).

## Comparative assessment

`data_sync (compiled -O2, no -g)` shows a smaller `.text` and no debug sections. The evidence supports the interpretation that `data_sync_v2` is a superset of data_sync with metadata inspection and logging added.

The additional imports in `data_sync_v2` are directly consistent with the extended source: `stat for metadata`, `fopen/fprintf/fclose` for a log file, `time/localtime` for timestamps. Its debug sections are explained by `-g`; its larger `.text` and `.rodata` follow from the extra code and format strings.

### Comparative Table

| Property | data_sync | data_sync_v2 |
|---|---|---|
| ELF class / arch | ELF64, LE, x86-64 | ELF64, LE, x86-64 |
| File type | EXEC (non-PIE) | EXEC (non-PIE) |
| Entry point | 0x401150 (`_start`) | 0x401130 (`_start`) |
| `main` symbol | Present (`0x4010a0`) | Present (`0x401312`) |
| Important sections | `.text`/`.rodata`/`.data`/`.bss`/`.symtab`/`.dynsym`/`.dynamic`; no `.debug_*` | Same as `data_sync` plus `.debug_info`, `.debug_line`, etc. |
| Dynamic deps | `libc.so.6` | `libc.so.6` |
| Relevant imports | `opendir`, `readdir`, `closedir`, `printf`, `perror` | Above + `stat`, `fopen`, `fprintf`, `time`, `localtime`, `snprintf`, `strerror` |
| Debugging symbols | Symtab yes; DWARF no | Symtab + DWARF yes |
| Significant differences | Smaller (~16 KiB); scan/list only | Larger (~21 KiB); `log_msg` + `stat`; built with `-g` |

The extra imports and the larger `.rodata` in `data_sync_v2` arw what the added source features require (file sizes via `stat`, log file via `fopen/fprintf`, timestamps via `time/localtime`). The debug sections reflect the -g flag and  performance-optimized data_sync has a smaller `.text` and no debug info. These differences are consistent with the code and compile-flag hypotheses; but they do not independently prove runtime behavior.

## Conclusion and limitations

Static evidence supports directory enumeration for both programs, with v2 also intending metadata inspection and append-only logging. **Imported symbols do not prove runtime behaviour** like paths taken, file contents written, or network activity cannot be confirmed without dynamic analysis. Stripping or different `-O`/`-g`/`-pie` options would change symbol visibility and layout without changing intended logic.
