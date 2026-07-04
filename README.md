# Advanced-Reaction-Timer.

A reaction time game built around three targets instead of one. Start the game using physical buttons, wait for the random signal, then press the button matching the LED that flashed. Every successful attempt is stored in EEPROM so your last 10 reaction times survive power cycles.

---

# DEMO:

Watch demo video.

---

# Built On

This project is a direct extension of **[Reaction Timer with EEPROM Score History](https://github.com/varadkinikar783/Reaction-Timer-with-EEPROM-Score-History)** — an earlier build that measured reaction time using a single button and stored the last ten scores in EEPROM. That project solved accurate timing, EEPROM storage, random delays and cheat detection. This version takes the idea further by replacing the Serial interface with physical controls, adding three reaction targets, introducing menu navigation, and managing the entire game through a simple state machine.

---

# What does it do?

Instead of reacting to a single LED, one of three LEDs is chosen randomly every round. After a short countdown and a random delay, the chosen LED flashes briefly. Press the button corresponding to that LED as quickly as possible.

Pressing any reaction button before the LED flashes counts as cheating and immediately ends the round. Every valid reaction time is stored in EEPROM, and a dedicated History button lets you view your previous scores directly from memory on the serial monitor.

---

# Hardware

| Component | Quantity |
|-----------|---------:|
| Arduino Uno | 1 |
| LEDs (Reaction x3, Go x1) | 4 |
| Push Buttons | 5 |
| 220Ω Resistors | 4 |
| Jumper Wires | As required |

---

# Wiring

| Arduino Pin | Connection |
|-------------|------------|
| D3 | Go LED |
| D4 | Reaction LED 1 |
| D5 | Reaction LED 2 |
| D6 | Reaction LED 3 |
| D8 | Reaction Button 1 |
| D9 | Reaction Button 2 |
| D10 | Reaction Button 3 |
| D11 | Start Button |
| D12 | History Button |
| A0 | Floating analog pin for random seed |

---

# How it works

The program sits at a menu waiting for either the Start or History button.

Pressing Start begins a short countdown before generating a random delay between 800ms and 5000ms. During this waiting period the program watches the reaction buttons — pressing one early immediately ends the round as a cheat.

Once the delay expires, one randomly selected LED flashes for 50ms and the reaction timer starts. The program waits until one of the reaction buttons is pressed.

If the correct button is pressed, the reaction time is calculated using `millis()`, printed to Serial, and saved into EEPROM. If the wrong button is pressed, the round ends without saving.

The History button reads the EEPROM contents and prints the stored reaction times to Serial.

---

# Skills Demonstrated

- Building a complete project around a simple state machine
- Button debouncing using `millis()`
- Non-blocking timing for reaction measurement
- Random target selection using hardware-generated randomness
- EEPROM data storage with automatic overwrite after 10 entries
- Managing multiple inputs and outputs without relying on Serial commands
- Separating game logic into small functions instead of one large `loop()`

---

# What Broke and How It Was Fixed

**Buttons needing two presses — Failure 1.**  
The button states were being updated in the wrong order, so the first press only synchronized the previous state instead of actually registering the event. Updating the stored state only after handling the press fixed the issue.

**Rounds restarting immediately — Failure 2.**  
Several state variables weren't being reset after a completed round, so the program skipped the menu and jumped straight into another countdown. Making every state responsible for cleaning up before leaving solved it.

**Reaction timer never stopping — Failure 3.**  
The reaction phase kept running even after a valid button press because the game state wasn't changing soon enough. Moving the state transition into the reaction handling logic stopped the timer exactly when the button was pressed.

**Random delay happening instantly — Failure 4.**  
One version checked the random interval only once instead of continuously comparing elapsed time. The result was an LED that flashed immediately after the countdown. Keeping track of the start time and comparing it against `millis()` every loop restored the intended random wait.

**History storage becoming inconsistent — Failure 5.**  
Saving results while the EEPROM counter rolled over caused confusing history ordering. Resetting the counter cleanly before writing the next score turned it into a simple circular history of the most recent ten attempts.

---

# Why I Built This

The previous project already answered the question of *"how fast can someone react?"* but it still felt like pressing the same button over and over. I wanted to see how much more interesting it could become without changing the hardware very much.

Adding multiple targets suddenly made timing only half the challenge — now the program also had to choose a random LED, verify the correct button, catch people pressing early, remember scores across power cycles, and somehow keep all of those little pieces from stepping on each other.

It ended up teaching me that once projects start having multiple "things happening," keeping track of **what state the program is in** becomes just as important as writing the individual features.

---

# Future Improvements

- Store timestamps alongside reaction times
- Calculate fastest, slowest and average reaction automatically
- OLED display instead of relying on Serial Monitor
- Difficulty modes with shorter random delays
- Multiple players with separate score histories
- Sound effects using a buzzer
- Port to ESP32 with a wireless leaderboard

---

# Repository Structure

```
Multi-Button-Reaction-Timer-with-EEPROM-Score-History/
├── src/
│   └── main.cpp
├── README.md
└── platformio.ini
```
