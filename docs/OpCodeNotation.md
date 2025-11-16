# CHIP-8 Opcode Notation Explained

A CHIP-8 opcode is always **16 bits** (2 bytes).

We write opcodes in **hex**, using 4 hex digits:

```
3xkk
6xkk
7xkk
2nnn
Dxyn
Fx1E
```

Each character you see (3, x, k, n, y…) represents **4 bits** (a nibble).

---

# The CPU splits an opcode into these 4 nibbles

Example opcode:

```
3 A 0 2   (hex)
```

Binary:

```
0011 1010 0000 0010
```

Positions:

```
[ high nibble ][ nib2 ][ nib3 ][ low nibble ]
     3            A       0         2
```

We label the nibbles like this for explanation:

```
op   x   k   k
```

Or:

```
op   x   y   n
```

Or:

```
op   n   n   n
```

Depending on the instruction format.

---

# What each letter means

### ✔ **op**

The instruction type.
The first hexadecimal digit (high nibble).
Example: `3xkk` → `3` means "SE Vx, byte"

---

### ✔ **x**

Register index
4 bits that select one of the 16 V-registers: V0–VF

Extract using:

```cpp
uint8_t x = (opcode & 0x0F00) >> 8;
```

Because:

```
3 x k k
  ↑
  2nd nibble → bits 8–11
```

---

### ✔ **y**

Another register index
Used in instructions involving two registers, like:

```
5xy0
8xy4
9xy0
```

Extract using:

```cpp
uint8_t y = (opcode & 0x00F0) >> 4;
```

Because y is the 3rd nibble.

---

### ✔ **kk**

8-bit immediate value (0–255)

Lower 8 bits of opcode:

```cpp
uint8_t kk = opcode & 0x00FF;
```

Examples:

* `3xkk` → compare Vx with kk
* `6xkk` → load Vx = kk

---

### ✔ **nnn**

12-bit memory address (0x000–0xFFF)

Extracted with:

```cpp
uint16_t nnn = opcode & 0x0FFF;
```

Because `nnn` uses the lower **12 bits**.

Examples:

* `1nnn` → jump
* `2nnn` → call
* `Annn` → I = nnn

---

### ✔ **n**

Lowest 4 bits (the last nibble)

Used in:

* sprites (`Dxyn`) → height = n
* shift instructions (`8xy6`, `8xyE`) → sometimes ignored
* skip/use lower nibble

Extract using:

```cpp
uint8_t n = opcode & 0x000F;
```

---

# Summary Table

| Symbol  | Meaning                                   |
| ------- | ----------------------------------------- |
| **op**  | instruction type (first hex digit)        |
| **x**   | register index Vx                         |
| **y**   | register index Vy                         |
| **kk**  | 8-bit value (lower byte)                  |
| **nnn** | 12-bit address (lower 12 bits)            |
| **n**   | sprite height / small value (last nibble) |

---

# Example: decoding `3A05`

Instruction:

```
3 A 0 5
```

Meaning:

```
3xkk = SE Vx, kk
x = A → VA
kk = 05
```

Decoded:

```cpp
x  = (opcode & 0x0F00) >> 8 = 0xA
kk = opcode & 0x00FF        = 0x05
```

So the instruction means:

```
If VA == 5, skip next instruction
```
