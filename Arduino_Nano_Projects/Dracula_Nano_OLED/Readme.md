# 🧛 Arduino Dracula OLED Song Visualizer : Madhumitha Kolkar :)

An Arduino Nano project that plays out the lyrics of "Dracula" on a **0.96" SSD1306 OLED Display**, syncing each word to animated text effects (pop, shake, invert, glitch, zoom, typewriter, bounce) over a rotating animated background — a 3D wireframe cube or lightning storm.

---

## 📺 Demo

[![Watch the demo](https://img.youtube.com/vi/syzg_v7sfk4/0.jpg)](https://youtube.com/shorts/syzg_v7sfk4?si=WfKQ_iScSKbiA3_C)

---

## 🎮 Features

- Lyric-timed word display driven by a PROGMEM lyric table (delay, duration, effect, size per word)
- Seven text effects: Pop, Shake, Invert, Glitch, Zoom In, Typewriter, Bounce
- Two animated backgrounds: a rotating 3D wireframe cube and a procedural lightning storm with sparkles
- Background automatically switches to lightning during high-intensity effects (Invert, Shake, Zoom In)
- Loops continuously, restarting the sequence after each full playthrough
- Runs entirely on an Arduino Nano with minimal RAM usage (lyrics stored in flash)

---

## 🛠️ Hardware Required

| Component | Quantity |
|-----------|:--------:|
| Arduino Nano | 1 |
| 0.96" SSD1306 OLED Display (I²C) | 1 |
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

---

## 📚 Libraries

Install the following libraries from the Arduino Library Manager:

- Adafruit GFX Library
- Adafruit SSD1306

---

## 🚀 Getting Started

1. Clone this repository.

```bash
git clone https://github.com/MadhumithaKolkar/Current-Affairs.git
```

2. Open `Dracula.ino` in the Arduino IDE.

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

## ✍️ Customizing the Lyrics

Words and timing live in the `lyrics[]` table at the top of `Dracula.ino`. Each entry controls:

| Field | Meaning |
|-------|---------|
| `delayBeforeMs` | Pause before this word appears |
| `durationMs` | How long the word stays on screen |
| `word` | The text to display (max 9 characters) |
| `effect` | One of the `Effect` enum values |
| `size` | Text size multiplier |

Edit the table to sync your own song lyrics and effects.

---

## 📂 Project Structure

```
Dracula_Nano_OLED/
│
├── Dracula.ino
└── Readme.md
```

---

## 💡 Future Improvements

- 🔊 Audio sync via microphone or line-in
- 🎨 Additional text effects
- 🔋 Battery-powered handheld version

---

## 🤝 Contributing

Contributions are welcome!

If you'd like to add new effects, backgrounds, or lyric sets, feel free to fork the repository and submit a pull request.

---

## 📄 License

This project is licensed under the MIT License.

---

## ⭐ Show Your Support

If you found this project useful or fun, consider giving it a ⭐ on GitHub!
