#include <avr/pgmspace.h>
#include <TimerOne.h>
#include <digitalWriteFast.h>

// Defines.
#define DEBUG 1
#define clockPin 2

// Globals.
unsigned long startUs = 0;
unsigned long endUs = 0;
int clockTuningPin = 2;  
int tuningValue = 0; 

/**
 * Setup clock and switch ports, timer 1 interrupt.
 */
void setup(){
  pinMode(clockPin, OUTPUT);
  Timer1.initialize();                    // Initialize Timer1, and set a 1 second period.
  Timer1.attachInterrupt(generateClock);  // Attaches generateClock() as a timer overflow interrupt.

#ifdef DEBUG
  Serial.begin(115000);
  Serial.println("Setup clock and switch manager.");
#endif
}

/**
 * Manages:
 * 
 *  - the three pulse sewitches,
 *  - eight ON-ON switches,
 *  - the clock fine selector potentiometer,
 *  - and the clock duration.
 */
void loop() { 

  // Read the clock tuning knob.
  tuningValue = analogRead(clockTuningPin);
  Serial.print("Clock tuning = ");
  Serial.print(tuningValue);
  Serial.println(".");
  delay(100);
}

/**
 * Timer1 interrupt handler.
 */
void generateClock() { 
#ifdef DEBUG
  startUs = micros();
#endif

  // Send a clock pulse. Interval will be about 4 microseconds.
  digitalWriteFast(clockPin, HIGH);
  digitalWriteFast(clockPin, LOW);

#ifdef DEBUG
  endUs = micros();
  Serial.print("Pulse = ");
  Serial.println(endUs - startUs);
  Serial.print(" Us.");
#endif                
}
