#include <LiquidCrystal.h>
#define LCD_RS_PIN A5
#define LCD_E_PIN A4
#define LCD_D4_PIN 6
#define LCD_D5_PIN 7
#define LCD_D6_PIN 8
#define LCD_D7_PIN 9

LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN,
                 LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN );

#define LED_1_PIN 12
#define LED_2_PIN 11
#define LED_3_PIN 10
#define ECHO_PIN 3
#define TRIGGER_PIN 4

unsigned long lastTimeUltrasonicTrigger = millis();
unsigned long ultrasonicTriggerDelay = 60;

volatile unsigned long pulseInTimeBegin;
volatile unsigned long pulseInTimeEnd;
volatile bool newDistanceAvailable = false;

double previousDistance = 400.0;

void triggerUltrasonicSensor()
{
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
}

double getUltrasonicDistance()
{
  double durationMicros = pulseInTimeEnd - pulseInTimeBegin;
  double distance = durationMicros / 58.0; // cm (148.0: inches)
  if (distance > 400.0) {
    return previousDistance;
  }
  distance = previousDistance * 0.7 + distance * 0.3;
  previousDistance = distance;
  return distance;
}

void echoPinInterrupt()
{
  if (digitalRead(ECHO_PIN) == HIGH) { // start measuring
    pulseInTimeBegin = micros();
  }
  else { // stop measuring
    pulseInTimeEnd = micros();
    newDistanceAvailable = true;
  }
}

void powerLEDsFromDistance(double distance)
{
  if (distance >= 100) {
    digitalWrite(LED_1_PIN, LOW);
    digitalWrite(LED_2_PIN, LOW);
    digitalWrite(LED_3_PIN, HIGH);
  }
  else if (distance >= 15) {
    digitalWrite(LED_1_PIN, LOW);
    digitalWrite(LED_2_PIN, HIGH);
    digitalWrite(LED_3_PIN, LOW);
  }
  else {
    digitalWrite(LED_1_PIN, HIGH);
    digitalWrite(LED_2_PIN, LOW);
    digitalWrite(LED_3_PIN, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  lcd.begin(16, 2);

  attachInterrupt(digitalPinToInterrupt(ECHO_PIN),
                  echoPinInterrupt,
                  CHANGE);
}

void loop() {
  unsigned long timeNow = millis();
  
  if (timeNow - lastTimeUltrasonicTrigger > ultrasonicTriggerDelay) {
    lastTimeUltrasonicTrigger += ultrasonicTriggerDelay;
    triggerUltrasonicSensor();
  }

  if (newDistanceAvailable) {
    newDistanceAvailable = false;
    double distance = getUltrasonicDistance();
    
    // Control LEDs based on distance
    powerLEDsFromDistance(distance);

    // Clear the previous display before printing new values
    lcd.clear();
    
    // Set the cursor position to the start of the first line
    lcd.setCursor(0, 0);

    // Print distance with one decimal point
    lcd.print("Dist: ");
    lcd.print(distance, 1); // Display with 1 decimal point
    
    // Optionally print "cm" to clarify units
    lcd.print(" cm");
  }
}



