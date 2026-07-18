# 🦖 Arduino Chrome Dino Game : Madhumitha Kolkar :)

Recreate the iconic Google Chrome Dino Game on an **Arduino Nano** using a **0.96" SSD1306 OLED Display** and two push buttons. Jump over cacti, duck under obstacles, and survive as long as possible while your score keeps increasing.

---

## 🎮 Features

- Classic Chrome Dino gameplay
- Smooth jumping and ducking mechanics
- Increasing score counter
- Collision detection and Game Over screen
- Lightweight and runs entirely on an Arduino Nano
- Uses an SSD1306 OLED display over I²C

---

## 🛠️ Hardware Required

| Component | Quantity |
|-----------|:--------:|
| Arduino Nano | 1 |
| 0.96" SSD1306 OLED Display (I²C) | 1 |
| Push Buttons | 2 |
| Breadboard | 1 |
| Jumper Wires | As required |
| USB Cable | 1 |

---

## 📌 Wiring

### OLED Display

| OLED | Arduino Nano |
|------|--------------|
| VDD (VCC) | 5V |
| GND | GND |
| SCK (SCL) | A5 |
| SDA | A4 |

### Buttons

#### Jump Button

| Button Pin | Arduino |
|------------|----------|
| One Side | D2 |
| Other Side | GND |

#### Duck Button

| Button Pin | Arduino |
|------------|----------|
| One Side | D6 |
| Other Side | GND |

> The project uses `INPUT_PULLUP`, so no external resistors are required.

---

## 📚 Libraries

Install the following libraries from the Arduino Library Manager:

- Adafruit GFX Library
- Adafruit SSD1306

---

## 🚀 Getting Started

1. Clone this repository.

```bash
git clone https://github.com/yourusername/arduino-dino-game.git
```

2. Open the `.ino` file in the Arduino IDE.

3. Install the required libraries.

4. Select:

```
Board: Arduino Nano
Processor: ATmega328P
```

> If using a Nano clone, you may need to select **ATmega328P (Old Bootloader)**.

5. Select the correct serial port.

6. Upload the sketch.

---

## 🎮 Controls

| Button | Action |
|---------|--------|
| D2 | Jump |
| D6 | Duck |

---

## 📷 Preview

<p align="center">
  <img src="images/setup.jpg" width="700">
</p>

<p align="center">
  <img src="images/gameplay.gif" width="350">
</p>

---

## 📂 Project Structure

```
Arduino-Dino-Game/
│
├── Arduino_Dino_Game.ino
├── images/
│   ├── setup.jpg
│   └── gameplay.gif
├── README.md
└── LICENSE
```

---

## 💡 Future Improvements

- 🔊 Sound effects using a buzzer
- 🏆 High score stored in EEPROM
- ⚡ Increasing game speed
- 🌵 Multiple obstacle types
- ☁️ Flying birds
- 🎨 Sprite animations
- 🔋 Battery-powered handheld version

---

## 🤝 Contributing

Contributions are welcome!

If you'd like to improve gameplay, optimize the code, or add new features, feel free to fork the repository and submit a pull request.

---

## 📄 License

This project is licensed under the MIT License.

---

## ⭐ Show Your Support

If you found this project useful or fun, consider giving it a ⭐ on GitHub!
