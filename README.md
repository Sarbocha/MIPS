# MIPS Simulator

Instruction-by-instruction simulator for a 5-stage pipelined MIPS-like processor.

## Supported instructions (ISA)

Opcodes accepted by the assembler/parser (case-insensitive mnemonics):

| Opcode | Operands (syntax) |
|--------|-------------------|
| `ADD` | `rd, rs, rt` |
| `SUB` | `rd, rs, rt` |
| `MUL` | `rd, rs, rt` |
| `AND` | `rd, rs, rt` |
| `OR` | `rd, rs, rt` |
| `SLL` | `rd, rt, shamt` |
| `SRL` | `rd, rt, shamt` |
| `ADDI` | `rt, rs, immediate` |
| `LW` | `rt, offset(rs)` **or** `rt, offset, rs` |
| `SW` | `rt, offset(rs)` **or** `rt, offset, rs` |
| `BEQ` | `rs, rt, label` **or** `rs, rt, targetIndex` |
| `J` | `label` **or** `targetIndex` |
| `NOP` | (none) |

Registers may be written as a decimal number or with a `$` prefix (e.g. `5` or `$5`). Labels for `BEQ` / `J` are resolved in a first pass; `targetIndex` is a decimal instruction index.

## Build

From the project root:

```bash
g++ -std=c++17 main.cpp CPU.cpp Parser.cpp -o mips_sim
```

Optional: `Encoding.cpp` is included in the repo if you want to build a small tool for 32-bit instruction listings; the simulator core does not require it.

## Run / command line

The simulator **does not** embed a default assembly path. You must pass exactly **one** input file.

**Usage:**

```text
mips_sim [--debug] <program.asm>
mips_sim <program.asm> [--debug]
```

**Examples:**

```bash
./mips_sim path/to/program.asm
./mips_sim sample.asm
./mips_sim sample.asm --debug
./mips_sim --debug sample.asm
```

**Flags:**

| Flag | Meaning |
|------|---------|
| `--debug` | Per-cycle trace: pipeline stage instructions, one `Control:` line, and `R0`–`R7` snapshot (full registers after run). |
| `-h`, `--help` | Print usage and exit successfully (no file required). |

**Exit status:**

- `0` — program ran to completion (or `--help` was used).
- `1` — missing assembly path, more than one file given, or a runtime error (e.g. cannot open file, unsupported opcode, parse failure). Errors are printed to stderr as `Error: ...`.

Running with no arguments shows usage and exits with `1`.

## Input format

- One instruction per line
- Optional labels are supported: `labelName: INSTR ...`
- Comments start with `#`
- Register syntax supports plain index (`1`) or dollar-prefixed (`$1`)
- `LW`/`SW`: either `offset(base)` (example: `LW 4, 16(2)`) or comma form `rt, offset, rs` (example: `LW 4, 16, 2`)

## Output

At the end of execution the simulator prints:

- Final register file (`R0` to `R31`)
- Final memory: `Mem[0]` … `Mem[15]` (when RAM is large enough), plus any **non-zero** words at addresses **≥ 16**

In debug mode (`--debug`), each cycle prints (simple / original-style trace):

- Pipeline latches: `IF/ID`, `ID/EX`, `EX/MEM`, `MEM/WB` with the instruction text in each stage
- One `Control:` line (decode-stage signals: reg write, memory, branch, jump)
- Register snapshot: **`R0`–`R7`** on one line (full `R0`–`R31` appears in **Final Register State** after the run)
