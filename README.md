# 4-Bit CPU Simulator & IDE

A comprehensive, educational 4-bit CPU Simulator and Integrated Development Environment (IDE) built with **C++** and **Raylib**. This project features a custom CPU architecture, a fully functional Assembler with dual-language support (English/Turkish), and a graphical interface for writing, compiling, and debugging Assembly code in real-time.

## Key Features

### The CPU Architecture
* **4-Bit Data Bus:** All data operations (ACC, RAM) are 4-bit (Nibbles).
![RAM](https://github.com/bedirhan420/4bitCPU-SIM/blob/main/images/RAM.png) 
* **8-Bit Address Bus:** Can address up to 256 bytes of Program Memory (ROM).
![ROM](https://github.com/bedirhan420/4bitCPU-SIM/blob/main/images/ROM.png) 
* **Registers:**
    * **ACC (Accumulator):** 4-bit general-purpose register for arithmetic and logic.
    * **PC (Program Counter):** 8-bit register pointing to the next instruction.
    * **SP (Stack Pointer):** 8-bit register managing the call stack.
    * **IR (Instruction Register):** Holds the current executing opcode.
* **Flags:**
    * **Z (Zero Flag):** Set when a result is 0.
    * **C (Carry Flag):** Set when an arithmetic operation overflows (exceeds 15).

![REGISTERS](https://github.com/bedirhan420/4bitCPU-SIM/blob/main/images/REGISTERS.png) 


### The IDE (Integrated Development Environment)
A custom-built text editor designed specifically for Assembly coding:
* **Syntax Highlighting:** Real-time coloring for Opcodes, Labels, Numbers, Comments, and Directives.
* **Undo/Redo System:** Full history support (`Ctrl+Z`, `Ctrl+Y`).
* **Search Engine:** Find text within the code (`Ctrl+F`) with match highlighting.
* **Clipboard Support:** Copy, Cut, and Paste functionality (`Ctrl+C`, `Ctrl+V`, `Ctrl+X`).
* **Smart Selection:** Drag-to-select, double-click to select words.
* **Autosave:** Never lose your code; the system saves the state on exit.
* **Scrolling & Line Numbers:** Handles long files with dynamic scrolling.

![IDE](https://github.com/bedirhan420/4bitCPU-SIM/blob/main/images/IDE.png) 


### The Assembler
* **Two-Pass Assembly:**
    1.  **Symbol Resolution:** Maps labels (e.g., `LOOP:`) to memory addresses.
    2.  **Code Generation:** Converts mnemonics to machine code (Hex).
* **Directives:** Supports `.data` (variables) and `.code` (logic) sections.
* **Comments:** Supports line comments using `;`.
* **Safety Checks:** Ensures the program ends with a `HLT` instruction and validates operands.

### Dual Language Support (TR/EN)
The assembler and UI support dynamic language switching. You can write code using standard English Mnemonics or Turkish equivalents.
* *Example:* `LDA 5` works exactly the same as `YUK 5`.
* *Switching:* Toggle between **EN** and **TR** modes via the button in the top-right corner of the simulation view.

### Simulation & Debugging
* **Step Mode:** Execute one instruction at a time to analyze CPU state.
* **Auto-Run Mode:** Execute the program continuously with adjustable speed.
* **Visual Memory:** View the contents of RAM (Data) and ROM (Program) in real-time.
* **I/O Visualization:** Interactive switches for Input and LEDs for Output.

![SIM](https://github.com/bedirhan420/4bitCPU-SIM/blob/main/images/SIM.png) 

---

## Instruction Set Architecture (ISA)

The CPU uses a split opcode design. Standard instructions use Opcodes `0-E`. Opcode `F` is reserved for "Extended Instructions" which use the lower nibble as a Subcode.

### Standard Instructions

| Opcode | Mnemonic (EN) | Mnemonic (TR) | Operand | Description | Logic (C++) | Flags |
| :---: | :--- | :--- | :--- | :--- | :--- | :--- |
| **0** | `NOP` | `BOS` | - | No Operation | `Wait();` | - |
| **1** | `LDA` | `YUK` | `[addr]` | Load RAM to ACC | `ACC = RAM[op];` | Z |
| **2** | `LDI` | `SAB` | `val` | Load Immediate | `ACC = op;` | Z |
| **3** | `STA` | `SAK` | `[addr]` | Store ACC to RAM | `RAM[op] = ACC;` | - |
| **4** | `ADD` | `TOP` | `[addr]` | Addition | `ACC += RAM[op];` | Z, C |
| **5** | `SUB` | `CIK` | `[addr]` | Subtraction | `ACC -= RAM[op];` | Z, C |
| **6** | `AND` | `VE` | `[addr]` | Bitwise AND | `ACC &= RAM[op];` | Z |
| **7** | `OR` | `VEY` | `[addr]` | Bitwise OR | `ACC |= RAM[op];` | Z |
| **8** | `XOR` | `YAD` | `[addr]` | Bitwise XOR | `ACC ^= RAM[op];` | Z |
| **9** | `LDAI`| `DOL` | `[ptr]` | Load Indirect | `ACC = RAM[RAM[op]];`| Z |
| **A** | `STAI`| `SDK` | `[ptr]` | Store Indirect | `RAM[RAM[op]] = ACC;`| - |
| **B** | `JMP` | `GIT` | `addr` | Unconditional Jump | `PC = addr;` | - |
| **C** | `JZ` | `SIF` | `addr` | Jump if Zero | `if(Z) PC = addr;` | - |
| **D** | `JC` | `ELD` | `addr` | Jump if Carry | `if(C) PC = addr;` | - |
| **E** | `CALL`| `CAG` | `addr` | Function Call | `STACK[SP++] = PC+1; PC = addr;` | - |

### Extended Instructions (Opcode 0xF)

| Subcode | Mnemonic (EN) | Mnemonic (TR) | Description | Logic (C++) | Stack Effect |
| :---: | :--- | :--- | :--- | :--- | :--- |
| **0** | `HLT` | `DUR` | Halt Processor | `halted = true;` | - |
| **1** | `RST` | `BAS` | Reset System | `PC=0; ACC=0; SP=0;` | Clear |
| **2** | `OUT` | `YAZ` | Output to Console/LED | `cout << ACC;` | - |
| **3** | `NOT` | `DEG` | Bitwise NOT | `ACC = (~ACC) & 0xF;` | - |
| **4** | `PUSH`| `IT` | Push ACC to Stack | `STACK[SP++] = ACC;` | +1 (Data) |
| **5** | `POP` | `CEK` | Pop Stack to ACC | `ACC = STACK[--SP];` | -1 (Data) |
| **6** | `RET` | `DON` | Return from Func | `PC = STACK[--SP];` | -1 (Addr) |

---

## Input / Output System (Memory Mapped)

The CPU interacts with the outside world using specific RAM addresses. This is handled via **Memory Mapped I/O**.

### Output (Address 15)
* **Command:** `STA 15` (or `SAK 15`)
* **Behavior:** When data is written to RAM address 15, the CPU intercepts it and sends the 4-bit value to the **GPIO Unit**.
* **Visual:** The value is displayed on the 4 LEDs in the Output Panel.

![OUTPUT](https://github.com/bedirhan420/4bitCPU-SIM/blob/main/images/OUTPUT.png) 


### Input (Address 14)
* **Command:** `LDA 14` (or `YUK 14`)
* **Behavior:** When the CPU tries to read from RAM address 14, execution **pauses**.
* **Visual:** An **Input Popup** appears on the screen. The user must toggle the switches (0 or 1) to set a 4-bit value and press "SEND DATA".
* **Result:** The value is loaded into the ACC and stored in RAM[14] for reference.

![INPUT](https://github.com/bedirhan420/4bitCPU-SIM/blob/main/images/INPUT.png) 


---

## Building and Running

### Prerequisites
* C++ Compiler (g++ recommended) supporting C++17.
* **Raylib** library installed on your system.

#### macOS 
The easiest way is using **Homebrew**.
1.  **Install Command Line Tools (C++ Compiler):**
    Open your terminal and run:
    ```bash
    xcode-select --install
    ```
2.  **Install Raylib:**
    ```bash
    brew install raylib
    ```

#### Linux (Debian/Ubuntu)
Use `apt` to install `g++` and `raylib`:
```bash
sudo apt update
sudo apt install build-essential libraylib-dev
```

#### Windows 
1.  Install [MSYS2](https://www.msys2.org/).
2.  Open the MSYS2 terminal and install the toolchain and Raylib:
    ```bash
    pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-raylib
    ```

### Compilation
Use the following command to compile the project (MacOS/Linux):

* **compile & run** 
```bash
make run
```

* **run** 
```bash
./cpu_sim
```
*(Note: Adjust include/lib paths based on your OS and Raylib installation location).*

### Usage
1.  Run the executable: `./cpu_sim`
2.  Write your Assembly code in the **EDITOR** tab.
3.  Click **COMPILE**.
4.  If successful, the system switches to **SIMULATION** mode.
5.  Use **STEP** to debug or **RUN** to execute the program.

---

## Controls & Shortcuts

| Context | Shortcut | Action |
| :--- | :--- | :--- |
| **Global** | `Ctrl + S` | Save File |
| | `Ctrl + L` | Load File |
| **Editor** | `Ctrl + Z` | Undo |
| | `Ctrl + Y` / `Ctrl+Shift+Z` | Redo |
| | `Ctrl + F` | Find / Search |
| | `Ctrl + C/V/X` | Copy / Paste / Cut |
| | `Ctrl + A` | Select All |
| **Simulation** | `Space` / `Enter` | Step (Execute one instruction) |
| | `R` | Toggle Auto-Run |
| | `Backspace` | Reset System |

---

## Project Structure

* `main.cpp`: Entry point, main loop, and UI orchestration.
* `Core/`: Contains CPU, Assembler, and Instruction Set logic.
    * `CPU.h`: Registers, Fetch-Decode-Execute cycle.
    * `Assembler.h`: Parser, Label resolution, Machine code generation.
    * `InstructionSet.h`: Opcode maps, Mnemonics (EN/TR).
* `UI/`: User Interface components.
    * `TextEditor.cpp/h`: The complex IDE component.
    * `SimulationUI.h`: Drawing functions for RAM, ROM, and Registers.
* `Utils/`: Helper functions and constants.
* `Programs/`: Example assembly '.asm' files.

---
*Developed as a Computer Engineering project to demonstrate low-level computing concepts.*
