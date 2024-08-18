#include <LiquidCrystal.h>

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(5, 6, 8, 9, 10, 11);

int redled = 2;
int greenled = 3;
int buzzer = 4;
int sensor = A0;
int sensorThresh = 400; // Threshold value for drunk
int motorControlPin = 7; // Pin to control motor via transistor
int startSwitch = 12; // Pin for start switch

bool systemOn = false; // Flag to control system state

void setup() {
  pinMode(redled, OUTPUT);
  pinMode(greenled, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(sensor, INPUT);
  pinMode(motorControlPin, OUTPUT);
  pinMode(startSwitch, INPUT_PULLUP); // Use internal pull-up resistor
  Serial.begin(9600);
  lcd.begin(16, 2);
}

void loop() {
  // Check if the start switch is pressed to toggle the system state
  if (digitalRead(startSwitch) == LOW) {
    systemOn = !systemOn;
    delay(200); // Debounce delay
  }

  if (systemOn) {
    // Preheat the MQ3 sensor
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Preheating");
    lcd.setCursor(0, 1);
    lcd.print("sensor...");
    delay(2000); // 20-second preheat delay
    lcd.clear();

    // Main system functionality
    int analogValue = analogRead(sensor); // Value read by gas sensor
    Serial.print(analogValue);
    float alcoholLevel = map(analogValue, 0, 1023, 0, 100);
    if (analogValue > sensorThresh) { // Condition if drunk
      digitalWrite(redled, HIGH);
      digitalWrite(greenled, LOW);
      // Play the buzzer with a frequency=4000Hz for 5 seconds.
      tone(buzzer, 5000, 4000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ALCOHOL DETECTED.");
      lcd.setCursor(0, 1);
      lcd.print("Consumption: ");
      lcd.print(alcoholLevel);
      lcd.print("%");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SENDING ALERT");
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.print("STOPPING BIKE...  ");
      delay(1000);
      lcd.clear();
      digitalWrite(motorControlPin, LOW); // Stop the motor
      delay(1000);
    } else { // Condition if safe
      digitalWrite(greenled, HIGH);
      digitalWrite(redled, LOW);
      digitalWrite(motorControlPin, HIGH); // Start the motor
      noTone(buzzer); // Ensure buzzer is off
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SAFE");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("ALL CLEAR");
      delay(1000);
    }
  } else {
    // If the system is off, turn off all indicators and stop the motor
    digitalWrite(redled, LOW);
    digitalWrite(greenled, LOW);
    noTone(buzzer);
    digitalWrite(motorControlPin, LOW); // Ensure motor is off
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Off");
  }
}
