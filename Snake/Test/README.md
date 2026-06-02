# Matrix & Keypad Test

A simple diagnostic tool to verify the hardware connectivity of the LED matrix and the 4x4 keypad.

## Purpose
* **LED Matrix:** Iterates through every LED to ensure no wiring mistakes (rows/columns) were made.
* **Keypad:** Scans the 4x4 keypad and prints the coordinates of any pressed button to the Serial Monitor.

## Wiring Checklist

| Component | ESP32 Pins |
| :--- | :--- |
| **Shift Register (74HC595)** | 14 (Data), 13 (Latch), 12 (Clock) |
| **Keypad Columns** | 34, 35, 36, 39 |
| **LED Matrix Rows** | 25, 32, 22, 33, 15, 21, 17, 2 |
| **LED Matrix Columns** | 19, 18, 5, 23, 4, 26, 27, 16 |

## Wiring
<img width="632" height="638" alt="image" src="https://github.com/user-attachments/assets/249554d1-9aa2-45ef-8dbb-540ef48eb8bc" />


## Usage
1. Connect the hardware.
2. Open the **Serial Monitor** in Arduino IDE at `115200` baud.
3. If a button is pressed, you will see the coordinates `(r, c)` printed on the screen.
4. The matrix will cycle through segments to verify that the shift register and direct GPIOs are configured correctly.

   P.S. This testing code was necessary because I am using generic, budget components. They don't have standard pin layouts, so I had to write these scripts to manually map out which pin does what. If you build this, double-check your connections with a multimeter before plugging everything in, otherwise you might burn something.
