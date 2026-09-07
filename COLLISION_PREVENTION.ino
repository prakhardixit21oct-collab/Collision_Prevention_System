#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Servo.h>

// TFT Pins
#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Ultrasonic Pins
#define TRIG1 22
#define ECHO1 23
#define TRIG2 24
#define ECHO2 25

// Motor Control Pins (Brake System)
#define IN1 4
#define IN2 5
#define ENA 6

#define BRAKE_THRESHOLD_CM 20  // Emergency braking distance

Servo servo1;
Servo servo2;

int angle = 0;
int stepDir = 1;

long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
  if (duration == 0) return 400; // Return max range if no pulse
  return duration * 0.034 / 2;
}

void setup() {
  pinMode(TRIG1, OUTPUT); pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT); pinMode(ECHO2, INPUT);
  
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);

  servo1.attach(2);
  servo2.attach(3);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  
  // Draw Radar Baseline
  tft.drawCircle(160, 240, 60, ILI9341_GREEN);
  tft.drawCircle(160, 240, 120, ILI9341_GREEN);
  tft.drawCircle(160, 240, 180, ILI9341_GREEN);
  
  // Start Drive Forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);
}

void loop() {
  servo1.write(angle);
  servo2.write(180 - angle); // Inverse sweep for dual coverage
  delay(15);

  long dist1 = readDistance(TRIG1, ECHO1);
  long dist2 = readDistance(TRIG2, ECHO2);

  // Plot Radar Line on TFT (Left Radar Sensor)
  float rad = angle * 0.0174533;
  int x1 = 160 + (dist1 * cos(rad));
  int y1 = 240 - (dist1 * sin(rad));

  if (dist1 < 200) {
    tft.drawPixel(x1, y1, ILI9341_RED);
  }

  // Automatic Emergency Brake Logic
  if (dist1 <= BRAKE_THRESHOLD_CM || dist2 <= BRAKE_THRESHOLD_CM) {
    applyEmergencyBrake();
  }

  angle += stepDir;
  if (angle <= 0 || angle >= 180) {
    stepDir = -stepDir; // Reverse sweep direction
    tft.fillScreen(ILI9341_BLACK); // Clear radar trace per full sweep
    tft.drawCircle(160, 240, 60, ILI9341_GREEN);
    tft.drawCircle(160, 240, 120, ILI9341_GREEN);
    tft.drawCircle(160, 240, 180, ILI9341_GREEN);
  }
}

void applyEmergencyBrake() {
  // Electronic braking: Reverse polarity briefly, then full stop
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 255);
  delay(100); 
  
  // Full Cutoff
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  // Visual Alert on TFT
  tft.fillRect(40, 90, 240, 60, ILI9341_RED);
  tft.setCursor(60, 110);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("EMERGENCY BRAKE!");

  while(1); // Lock system until manual reset
}