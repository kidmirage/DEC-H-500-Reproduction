#include <avr/pgmspace.h>
#include <TimerOne.h>
#include <TimerThree.h>
#include <digitalWriteFast.h>

// Comment out for no debug.
//#define DEBUG 1

// Clock pulses will eminate from this pin.
#define clockOut 2

// Pulse rocker pins.
#define pulseIn1 13
#define pulseOut1 23
#define pulseIn2 12
#define pulseOut2 25
#define pulseIn3 11
#define pulseOut3 27

// Switch rocker pins.
#define switchIn1 3
#define switchOut1 43
#define switchIn2 4
#define switchOut2 41
#define switchIn3 5
#define switchOut3 39
#define switchIn4 6
#define switchOut4 37
#define switchIn5 7
#define switchOut5 35
#define switchIn6 8
#define switchOut6 33
#define switchIn7 9
#define switchOut7 31
#define switchIn8 10
#define switchOut8 29

// Clock range fine input.
#define clockFineTuning A2

// Clock range coarse inputs.
#define clockRange1_10 42
#define clockRange10_100 44
#define clockRange100_1000 46
#define clockRange1000_10000 48
#define clockRange10000_100000 50
#define clockRange100000_1000000 52

// Clock tuning state.
long fineTuningValue = 0; 
long oldFineTuningValue = 0;
int coarseRangeSelected = 0;
int oldCoarseRangeSelected = 0;
long numberOfClocks = 0;

// Pulse rockers current state.
int pulseState1;
int pulseCount1;
int pulseState2;
int pulseCount2;
int pulseState3;
int pulseCount3;

// Switch rockers current state.
int switchState1;
int switchCount1;
int switchState2;
int switchCount2;
int switchState3;
int switchCount3;
int switchState4;
int switchCount4;
int switchState5;
int switchCount5;
int switchState6;
int switchCount6;
int switchState7;
int switchCount7;
int switchState8;
int switchCount8;
/**
 * Setup clock and switch ports, timer 1 interrupt.
 */
void setup(){

#ifdef DEBUG
  // Initialize serial debugging.
  Serial.begin(115000);
  Serial.println("Setup clock and switch manager.");
#endif

  // Setup the clock output pin.
  pinMode(clockOut, OUTPUT);
  digitalWrite(clockOut, LOW);

  // Setup the pulse rockers.
  pinMode(pulseIn1, INPUT_PULLUP);
  pinMode(pulseOut1, OUTPUT);
  pulseState1 = digitalRead(pulseIn1);
  digitalWrite(pulseOut1, (pulseState1+1)%2);
#ifdef DEBUG
  Serial.print("Pulse 1 = ");
  Serial.print(((pulseState1+1)%2)==0?"LOW":"HIGH");
  Serial.println(".");
#endif

  pinMode(pulseIn2, INPUT_PULLUP);
  pinMode(pulseOut2, OUTPUT);
  pulseState2 = digitalRead(pulseIn2);
  digitalWrite(pulseOut2, (pulseState2+1)%2);
#ifdef DEBUG
  Serial.print("Pulse 2 = ");
  Serial.print(((pulseState2+1)%2)==0?"LOW":"HIGH");
  Serial.println(".");
#endif

  pinMode(pulseIn3, INPUT_PULLUP);
  pinMode(pulseOut3, OUTPUT);
  pulseState3 = digitalRead(pulseIn3);
  digitalWrite(pulseOut3, (pulseState3+1)%2);
#ifdef DEBUG
  Serial.print("Pulse 3 = ");
  Serial.print(((pulseState3+1)%2)==0?"LOW":"HIGH");
  Serial.println(".");
#endif

  // Setup the switch rockers.
  pinMode(switchIn1, INPUT_PULLUP);
  pinMode(switchOut1, OUTPUT);
  switchState1 = ((digitalRead(switchIn1)+1)%2);
  digitalWrite(switchOut1, switchState1);
#ifdef DEBUG
  Serial.print("Switch 1 = ");
  Serial.print(switchState1==0?"LOW":"HIGH");
  Serial.println(".");
#endif

  pinMode(switchIn2, INPUT_PULLUP);
  pinMode(switchOut2, OUTPUT);
  switchState2 = ((digitalRead(switchIn2)+1)%2);
  digitalWrite(switchOut2, switchState2);
#ifdef DEBUG
  Serial.print("Switch 2 = ");
  Serial.print(switchState2==0?"LOW":"HIGH");
  Serial.println(".");
#endif

  pinMode(switchIn3, INPUT_PULLUP);
  pinMode(switchOut3, OUTPUT);
  switchState3 = ((digitalRead(switchIn3)+1)%2);
  digitalWrite(switchOut3, switchState3);
#ifdef DEBUG
  Serial.print("Switch 3 = ");
  Serial.print(switchState3==0?"LOW":"HIGH");
  Serial.println(".");
#endif

  pinMode(switchIn4, INPUT_PULLUP);
  pinMode(switchOut4, OUTPUT);
  switchState4 = ((digitalRead(switchIn4)+1)%2);
  digitalWrite(switchOut4, switchState4);
#ifdef DEBUG
  Serial.print("Switch 4 = ");
  Serial.print(switchState4==0?"LOW":"HIGH");
  Serial.println(".");
#endif

  pinMode(switchIn5, INPUT_PULLUP);
  pinMode(switchOut5, OUTPUT);
  switchState5 = ((digitalRead(switchIn5)+1)%2);
  digitalWrite(switchOut5, switchState5);
#ifdef DEBUG
  Serial.print("Switch 5 = ");
  Serial.print(switchState5==0?"LOW":"HIGH");
  Serial.println(".");
#endif
  
  pinMode(switchIn6, INPUT_PULLUP);
  pinMode(switchOut6, OUTPUT);
  switchState6 = ((digitalRead(switchIn6)+1)%2);
  digitalWrite(switchOut6, switchState6);
#ifdef DEBUG
  Serial.print("Switch 6 = ");
  Serial.print(switchState6==0?"LOW":"HIGH");
  Serial.println(".");
#endif
  
  pinMode(switchIn7, INPUT_PULLUP);
  pinMode(switchOut7, OUTPUT);
  switchState7 = ((digitalRead(switchIn7)+1)%2);
  digitalWrite(switchOut7, switchState7);
#ifdef DEBUG
  Serial.print("Switch 7 = ");
  Serial.print(switchState7==0?"LOW":"HIGH");
  Serial.println(".");
#endif

  pinMode(switchIn8, INPUT_PULLUP);
  pinMode(switchOut8, OUTPUT);
  switchState8 = ((digitalRead(switchIn8)+1)%2);
  digitalWrite(switchOut8, switchState8);
#ifdef DEBUG
  Serial.print("Switch 8 = ");
  Serial.print(switchState8==0?"LOW":"HIGH");
  Serial.println(".");
#endif

  // Setup the clock coarse range inputs.
  pinMode(clockRange1_10, INPUT_PULLUP);
  pinMode(clockRange10_100, INPUT_PULLUP);
  pinMode(clockRange100_1000, INPUT_PULLUP);
  pinMode(clockRange1000_10000, INPUT_PULLUP);
  pinMode(clockRange10000_100000, INPUT_PULLUP);
  pinMode(clockRange100000_1000000, INPUT_PULLUP);

  // Initialize the clock pulse interrupt handler.
  Timer1.initialize();                    // Initialize Timer1, and set a 1 second period.
  Timer1.attachInterrupt(generateClock);  // Attaches generateClock() as a timer overflow interrupt.

  // Initialize the one second interrupt handler.
  Timer3.initialize();                    // Initialize Timer3, and set a 1 second period.
  Timer3.attachInterrupt(oneSecond);      // Attaches oneSecond() as a timer overflow interrupt.
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
  int state;
  long clockInterval;

  // Check the pulse rockers.
  state = digitalRead(pulseIn1);
  if (state != pulseState1) {
    pulseCount1++;
    if (pulseCount1 >= 3) {
      digitalWrite(pulseOut1, (state+1)%2);
      pulseState1 = state;
      pulseCount1 = 0;
#ifdef DEBUG
      Serial.print("Pulse 1 = ");
      Serial.print(((state+1)%2)==0?"LOW":"HIGH");
      Serial.println(".");
#endif
    }
  } else {
    pulseCount1 = 0;
  }

  state = digitalRead(pulseIn2);
  if (state != pulseState2) {
    pulseCount2++;
    if (pulseCount2 >= 3) {
      digitalWrite(pulseOut2, (state+1)%2);
      pulseState2 = state;
      pulseCount2 = 0;
#ifdef DEBUG
      Serial.print("Pulse 2 = ");
      Serial.print(((state+1)%2)==0?"LOW":"HIGH");
      Serial.println(".");
#endif
    }
  } else {
    pulseCount2 = 0;
  }

  state = digitalRead(pulseIn3);
  if (state != pulseState3) {
    pulseCount3++;
    if (pulseCount3 >= 3) {
      digitalWrite(pulseOut3, (state+1)%2);
      pulseState3 = state;
      pulseCount3 = 0;
#ifdef DEBUG
      Serial.print("Pulse 3 = ");
      Serial.print(((state+1)%2)==0?"LOW":"HIGH");
      Serial.println(".");
#endif
    }
  } else {
    pulseCount3 = 0;
  }

  // Check the switch rockers.
  state = ((digitalRead(switchIn1)+1)%2);
  if (state != switchState1) {
    switchCount1++;
    if (switchCount1 >= 3) {
      digitalWrite(switchOut1, state);
      switchState1 = state;
      switchCount1 = 0;
#ifdef DEBUG
      Serial.print("Switch 1 = ");
      Serial.print(state==0?"LOW":"HIGH");
      Serial.println(".");
#endif
    }
  } else {
    switchCount1 = 0;
  }

  state = ((digitalRead(switchIn2)+1)%2);
  if (state != switchState2) {
    switchCount2++;
    if (switchCount2 >= 3) {
      digitalWrite(switchOut2, state);
      switchState2 = state;
      switchCount2 = 0;
#ifdef DEBUG
      Serial.print("Switch 2 = ");
      Serial.print(state==0?"LOW":"HIGH");
      Serial.println(".");
#endif
    }
  } else {
    switchCount2 = 0;
  }

  state = ((digitalRead(switchIn3)+1)%2);
  if (state != switchState3) {
    switchCount3++;
    if (switchCount3 >= 3) {
      digitalWrite(switchOut3, state);
      switchState3 = state;
      switchCount3 = 0;
#ifdef DEBUG
      Serial.print("Switch 3 = ");
      Serial.print(state==0?"LOW":"HIGH");
      Serial.println(".");
#endif
    }
  } else {
    switchCount3 = 0;
  }

  state = ((digitalRead(switchIn4)+1)%2);
  if (state != switchState4) {
    switchCount4++;
    if (switchCount4 >= 3) {
      digitalWrite(switchOut4, state);
      switchState4 = state;
      switchCount4 = 0;
#ifdef DEBUG
      Serial.print("Switch 4 = ");
      Serial.print(state==0?"LOW":"HIGH");
      Serial.println(".");
#endif
    }
  } else {
    switchCount4 = 0;
  }

  state = ((digitalRead(switchIn5)+1)%2);
  if (state != switchState5) {
    switchCount5++;
    if (switchCount5 >= 3) {
      digitalWrite(switchOut5, state);
      switchState5 = state;
      switchCount5 = 0;
#ifdef DEBUG
      Serial.print("Switch 5 = ");
      Serial.print(state==0?"LOW":"HIGH");
      Serial.println(".");
#endif
    }
  } else {
    switchCount5 = 0;
  }

  state = ((digitalRead(switchIn6)+1)%2);
  if (state != switchState6) {
    switchCount6++;
    if (switchCount6 >= 3) {
      digitalWrite(switchOut6, state);
      switchState6 = state;
      switchCount6 = 0;
#ifdef DEBUG
      Serial.print("Switch 6 = ");
      Serial.print(state==0?"LOW":"HIGH");
      Serial.println(".");
#endif
    }
  } else {
    switchCount6 = 0;
  }

  state = ((digitalRead(switchIn7)+1)%2);
  if (state != switchState7) {
    switchCount7++;
    if (switchCount7 >= 3) {
      digitalWrite(switchOut7, state);
      switchState7 = state;
      switchCount7 = 0;
#ifdef DEBUG
      Serial.print("Switch 7 = ");
      Serial.print(state==0?"LOW":"HIGH");
      Serial.println(".");
#endif
    }
  } else {
    switchCount7 = 0;
  }

  state = ((digitalRead(switchIn8)+1)%2);
  if (state != switchState8) {
    switchCount8++;
    if (switchCount8 >= 3) {
      digitalWrite(switchOut8, state);
      switchState8 = state;
      switchCount8 = 0;
#ifdef DEBUG
      Serial.print("Switch 8 = ");
      Serial.print(state==0?"LOW":"HIGH");
      Serial.println(".");
#endif
    }
  } else {
    switchCount8 = 0;
  }

  // Check to see if a coarse tuning range has been selected.
  coarseRangeSelected = 0;
  if (digitalRead(clockRange1_10) == LOW) {
    coarseRangeSelected = 1;
  }
  else if (digitalRead(clockRange10_100) == LOW) {
    coarseRangeSelected = 2;
  }
  else if (digitalRead(clockRange100_1000) == LOW) {
    coarseRangeSelected = 3;
  }
  else if (digitalRead(clockRange1000_10000) == LOW) {
    coarseRangeSelected = 4;
  }
  else if (digitalRead(clockRange10000_100000) == LOW) {
    coarseRangeSelected = 5;
  }
  else if (digitalRead(clockRange100000_1000000) == LOW) {
    coarseRangeSelected = 6;
  }
 
  // Read the clock tuning knob.
  fineTuningValue = 11 - (analogRead(clockFineTuning) / 100 + 1);
  if (fineTuningValue == 0) fineTuningValue++;
  
  if (oldFineTuningValue != fineTuningValue || coarseRangeSelected != oldCoarseRangeSelected) { 
  #ifdef DEBUG
      Serial.print("Clock tuning = ");
      Serial.print(fineTuningValue);
      Serial.println(".");
      Serial.print("Range selected = ");
      Serial.print(coarseRangeSelected);
      Serial.println(".");
#endif
      // Calculate a new clock interval.
      switch (coarseRangeSelected) {
        case 0: // No range selected. Default 1 clock/second.
          clockInterval = 1000000;          
          break;
        case 1: // 1 to 10 clocks/second.
          clockInterval = 100000 * fineTuningValue;             
          break;
        case 2: // 10 to 100 clocks/second.
          clockInterval = 10000 * fineTuningValue;
          break;
        case 3: // 100 to 1000 clocks/second.
          clockInterval = 1000 * fineTuningValue;
          break;
        case 4: // 1000 to 10000 clocks/second.
          clockInterval = 100 * fineTuningValue;
          break;
        case 5: // 10000 to 100000 clocks/second.
          clockInterval = 10 * fineTuningValue;
          break;
        case 6: // 100000 to 1000000 clocks/second.
          clockInterval = 1 * fineTuningValue;
          break;
      }
    // Remember the current settings.
    oldFineTuningValue = fineTuningValue;
    oldCoarseRangeSelected = coarseRangeSelected;

    // Set the new clock interval.
#ifdef DEBUG
    Serial.print("Clock interval = ");
    Serial.print(clockInterval);
    Serial.println(".");
#endif
    Timer1.setPeriod(clockInterval);
  }
  delay(20);
}

/**
 * Timer1 interrupt handler.
 */
void generateClock() {

#ifdef DEBUG
  unsigned long startUs;
  unsigned long endUs;
  startUs = micros();
#endif

  // Send a clock pulse. Interval will be about 4 microseconds.
  digitalWriteFast(clockOut, HIGH);
  digitalWriteFast(clockOut, LOW);

#ifdef DEBUG
  numberOfClocks++;
  endUs = micros();
  Serial.print("Pulse = ");
  Serial.print(endUs - startUs);
  Serial.println(" Us.");
#endif                
}

/**
 * Timer3 interrupt handler.
 */
void oneSecond() { 
#ifdef DEBUG
 Serial.print("Clocks = ");
 Serial.print(numberOfClocks);
 Serial.println(".");
 numberOfClocks = 0; 
#endif            
}
