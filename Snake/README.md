# ESP32 Matrix Snake

A classic Snake game built with an ESP32. The game is displayed on an 8x8 LED matrix and controlled with a matrix keypad. The code is structured to update the display continuously in the background, keeping the gameplay and screen rendering smooth.

## How to Play

1. Use the keypad to move the snake (Up, Down, Left, Right).
2. Eat the glowing dot to grow longer.
3. Do not hit the walls or your own tail.
4. If you crash, all LEDs will light up. Press any button to restart the game.

## Real Life

<img width="2200" height="1650" alt="IMG_20260602_152725182" src="https://github.com/user-attachments/assets/e150a9e9-8ead-46aa-a147-0eda0a1bbc5a" />


## Wiring

<img width="543" height="601" alt="image" src="https://github.com/user-attachments/assets/71a1ed5e-d58d-4fc4-877f-0bf4e0018832" />


* **Shift Register:** DATA to pin 14, LATCH to pin 13, CLOCK to pin 12.
* **Keypad Columns:** Connected to pins 34, 35, 36, 39.
* **LED Matrix:** Rows and Columns connected to the remaining ESP32 pins (see code for the exact array).

## Parts List

| Part | Amount |
| :--- | :--- |
| ESP32 Development Board | 1 |
| 8x8 LED Matrix | 1 |
| 4x4 Keypad | 1 |
| 74HC595 Shift Register | 1 |
| Resistor 330 Ω | 8 |
| Resistor 10k Ω | 4 |
| Breadboard | 1 |
