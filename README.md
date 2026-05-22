# Digital Voting System

**Module:** CTEN 415 — Microcontrollers

---

## Overview

A PIN-protected digital voting system implemented on a **PIC16F877A** microcontroller. Voters select candidates via a **4×4 matrix keypad** and results are displayed on a **20×4 LCD**. LED indicators provide visual feedback at startup and vote confirmation.

---

## Features

- **PIN authentication** before voting begins (prevents unauthorised access)
- **Candidate selection** via keypad keys A–D
- **Vote confirmation** prompt using the `#` key
- **Green LED** blinks on successful vote cast
- **Red LED** blinks 3× at startup and remains on during operation
- **Admin vote tally** view via `*` key + PIN entry
- Press `1` to restart voting for the next voter

---

## Hardware Components

| Component | Specification |
|---|---|
| Microcontroller | PIC16F877A |
| Display | 20×4 LCD (LM044L) |
| Input | 4×4 Matrix Keypad |
| Crystal Oscillator | 8 MHz |
| Capacitors | 2× 22 pF (non-polarised) |
| Resistors | 220 Ω, 10 kΩ, 47 kΩ (pull-down on all keypad pins) |
| LEDs | 1× Red (startup/status), 1× Green (vote confirmation) |
| Power Supply | 5 V DC |

---

## Pin Mapping

### LCD (Port D)

| LCD Signal | PIC Pin |
|---|---|
| RS | RD2 |
| EN | RD3 |
| D4 | RD4 |
| D5 | RD5 |
| D6 | RD6 |
| D7 | RD7 |

### Keypad (Port B)

| Signal | PIC Pin | Direction |
|---|---|---|
| Row 1 | RB0 | Output |
| Row 2 | RB1 | Output |
| Row 3 | RB2 | Output |
| Row 4 | RB3 | Output |
| Col 1 | RB4 | Input |
| Col 2 | RB5 | Input |
| Col 3 | RB6 | Input |
| Col 4 | RB7 | Input |

### LEDs

| LED | PIC Pin |
|---|---|
| Red (status) | RA0 |
| Green (confirmation) | RC2 |

---

## Software Setup

### Requirements

- [MPLAB X IDE](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide)
- [XC8 Compiler](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers)
- [Proteus Design Suite](https://www.labcenter.com/) (for simulation)

### Build Steps

1. Open MPLAB X and create a **Standalone Project** targeting **PIC16F877A**.
2. Select **XC8** as the compiler.
3. Add `lcd.h` to **Header Files** and `D_VOTING SYSTEM.c` to **Source Files**.
4. Set Configuration Bits: `FOSC = HS`, all protection/watchdog bits `OFF`.
5. **Clean and Build** — this generates `HS.hex` in the output directory.

### Simulation (Proteus)

1. Create a new Proteus project with a DEFAULT schematic template (no PCB layout).
2. Place and wire: PIC16F877A, LM044L LCD, 4×4 keypad, 8 MHz crystal circuit, LEDs.
3. Load `HS.hex` into the PIC16F877A component properties.
4. Run the simulation.

---

## System Flow

```
Power ON
  └─ Red LED blinks 3×
  └─ Red LED stays ON
  └─ Display: "VOTING SYSTEM"

Enter PIN Code  ──── invalid ────► "INVALID PIN" ──► retry
       │
    valid PIN
       │
       ▼
Display Candidates (A/B/C/D)
       │
       ▼
"Vote: / A-D to select"
       │
  key A–D pressed
       │
       ▼
"Vote: X / # to confirm"
       │
    # pressed
       │
       ▼
Record vote ──► Green LED blinks ──► "Thanks for Vote!"
       │
  Press 1 to restart
       │
       ▼
  (loop back to Enter PIN)

  * key at any time ──► Enter admin PIN ──► Display tally: "A:n B:n C:n D:n"
```

---

## Repository Files

| File | Description |
|---|---|
| `D_VOTING SYSTEM.c` | Main C source code |
| `lcd.h` | LCD driver header (4-bit mode, PIC16F877A) |
| `HS.hex` | Compiled HEX file ready to flash to PIC16F877A |
| `st21001137_mk21000459_DIGITAL VOTING_SYSTEM.pdsprj` | Proteus simulation project |
| `report.tex` | Full project report in LaTeX format |
| `st21001137_mk21000459_DIGITAL VOTING_SYSTEM.pdf` | Original project report (PDF) |

---

## Key Design Notes

- **47 kΩ pull-down resistors** on all keypad pins (not just rows) were necessary to suppress noise at 8 MHz operation. Prolonged power caused pin charging, leading to false key detections — grounding all pins solved this.
- Oscillator mode **HS** is required for the external 8 MHz crystal.
- Vote counts are stored in RAM only; a power cycle resets all counts. Future work: EEPROM persistence.

---

## License

This project was submitted as academic coursework at BIUST. Shared here for educational and portfolio purposes.
