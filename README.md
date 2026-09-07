# 🛑 Collision Prevention System — Dual-Radar Emergency Braking

An Arduino-based collision prevention system that uses two sweeping ultrasonic sensors to build a live radar view of obstacles ahead of the vehicle, and automatically triggers emergency braking the instant something gets too close — no driver input required.

---

## 🚗 Why this exists

Most collision-avoidance demos only use a single fixed sensor, which misses obstacles outside its narrow field of view. This system instead sweeps **two ultrasonic sensors in opposite directions** using servos, giving wider, overlapping coverage in front of the vehicle, and renders it as a live radar display so the detection logic isn't a "black box" — you can actually see what the car sees.

---

## ⚙️ How it works

**1. Dual-sensor radar sweep**
Two ultrasonic sensors are mounted on servos that sweep from 0°–180° in opposite directions (`servo1` sweeps forward while `servo2` sweeps in reverse), giving combined coverage across the front of the vehicle without blind spots.

**2. Live radar visualization**
A TFT display (ILI9341) draws a classic radar screen — concentric range rings — and plots a red pixel at the detected obstacle's position each sweep, calculated from the sensor's angle and measured distance using basic trigonometry. The screen clears and redraws range rings after every full sweep.

**3. Distance sensing**
Each ultrasonic sensor measures distance via pulse timing (`pulseIn`), with a 30ms timeout that defaults to a safe "no obstacle" reading (400cm) if no echo returns — preventing false positives from sensor dropout.

**4. Automatic emergency braking**
If either sensor detects an obstacle within **20 cm**, the system immediately calls `applyEmergencyBrake()`:
- Briefly reverses motor polarity for active electronic braking (a short counter-torque pulse)
- Cuts all motor power completely
- Displays a red "EMERGENCY BRAKE!" alert on the TFT screen
- Locks the system until manual reset, ensuring the vehicle doesn't unexpectedly resume motion

---

## 🔩 Hardware Used

| Component | Purpose |
|---|---|
| Arduino (Uno/Mega-class board) | Main controller |
| 2× Ultrasonic Sensors (HC-SR04 type) | Distance sensing, mounted on servos |
| 2× Servo Motors | Sweeping sensor mounts for wide-angle coverage |
| ILI9341 TFT Display (SPI) | Live radar visualization |
| DC Motor Driver (2-pin control + PWM) | Drive motor + emergency braking |

---

## 🛡️ Safety Behavior

| Condition | Action |
|---|---|
| Obstacle within 20 cm (either sensor) | Emergency brake triggered |
| No echo received (sensor timeout) | Treated as "clear" (400cm) to avoid false trips |
| After emergency brake | System halts and requires manual reset |

---

## 🧰 Tech Stack

- **Platform:** Arduino (AVR-based board)
- **Libraries:** `SPI.h`, `Adafruit_GFX.h`, `Adafruit_ILI9341.h`, `Servo.h`
- **Language:** C++ (Arduino)

---

## 🚀 Getting Started

1. Wire up the two ultrasonic sensors, servos, TFT display, and motor driver as per the pin definitions in `COLLISION_PREVENTION.ino`.
2. Open the sketch in the Arduino IDE with the Adafruit GFX and ILI9341 libraries installed.
3. Flash it to the Arduino.
4. Power the drive motor — the system will start sweeping, displaying the radar, and braking automatically on close obstacles.

---

## 🔭 Future Improvements

- Add a buzzer/audible alert alongside the visual warning
- Log brake-trigger events with timestamp and distance for post-test analysis
- Replace the fixed 20cm threshold with a speed-adaptive braking distance
- Add IMU-based validation to avoid false trips from ground/road-surface echoes

---

## 👤 Author

**Prakhar Dixit**
Electronics & Communication Engineering (VLSI Design & Technology) — Zakir Husain College of Engineering & Technology, AMU
Recruitment & Workshop Coordinator and contributor, SAE ZHCET
[LinkedIn](https://www.linkedin.com/in/prakhar-dixit-a2bb95377)
