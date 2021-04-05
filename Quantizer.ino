#include "digitalWriteFast.h"

#define SLEW_PIN A0
#define TRANSPOSE_PIN A1
#define LED0_PIN A2
#define TRIG_IN_PIN A3
#define GATE_PIN A4
#define CV_IN_PIN A5
#define TRIG_SENSE_PIN 0
#define CS_PIN 1
#define ROW1_PIN 2
#define ROW2_PIN 3
#define COL3_PIN 4
#define COL4_PIN 5
#define LED3_PIN 6
#define LED2_PIN 7
#define LED1_PIN 8
#define ROW3_PIN 9
#define COL1_PIN 10
#define SDI_PIN 11
#define COL2_PIN 12
#define SCK_PIN 13


#define LOW_DAC_GAIN

 
int activeSwitches = 0b111111111111;

void setupSwitchPins()
{
  pinMode(COL1_PIN, OUTPUT);
  pinMode(COL2_PIN, OUTPUT);
  pinMode(COL3_PIN, OUTPUT);
  pinMode(COL4_PIN, OUTPUT);
  digitalWriteFast(COL1_PIN, HIGH);
  digitalWriteFast(COL2_PIN, HIGH);
  digitalWriteFast(COL3_PIN, HIGH);
  digitalWriteFast(COL4_PIN, HIGH);
  pinMode(ROW1_PIN, INPUT_PULLUP);
  pinMode(ROW2_PIN, INPUT_PULLUP);
  pinMode(ROW3_PIN, INPUT_PULLUP);
}

void readSwitches()
{
  activeSwitches = 0;

  digitalWriteFast(COL1_PIN, LOW);
  delayMicroseconds(2);
  activeSwitches |= ((PIND >> 2) & 0b11) ;
  activeSwitches |= (PINB & 0b10) << (1 + 0);
  digitalWriteFast(COL1_PIN, HIGH);
  
  digitalWriteFast(COL2_PIN, LOW);
  delayMicroseconds(2);
  activeSwitches |= (PIND & 0b1100) << (-2 + 3);
  activeSwitches |= (PINB & 0b10) << (1 + 3);
  digitalWriteFast(COL2_PIN, HIGH);
  
  digitalWriteFast(COL3_PIN, LOW);
  delayMicroseconds(2);
  activeSwitches |= (PIND & 0b1100) << (-2 + 6);
  activeSwitches |= (PINB & 0b10) << (1 + 6);
  digitalWriteFast(COL3_PIN, HIGH);
  
  digitalWriteFast(COL4_PIN, LOW);
  delayMicroseconds(2);
  activeSwitches |= (PIND & 0b1100) << (-2 + 9);
  activeSwitches |= (PINB & 0b10) << (1 + 9);
  digitalWriteFast(COL4_PIN, HIGH);

  activeSwitches ^= 0b111111111111;
}

// 0 is no LED lit, 1-12 are for the LEDs from top to bottom
void setLed(int index)
{
  pinMode(LED0_PIN, INPUT);
  pinMode(LED1_PIN, INPUT);
  pinMode(LED2_PIN, INPUT);
  pinMode(LED3_PIN, INPUT);
  switch (index)
  {
    case 0:
      break;
    case 1:
      pinMode(LED0_PIN, OUTPUT);
      pinMode(LED1_PIN, OUTPUT);
      digitalWriteFast(LED0_PIN, HIGH);
      digitalWriteFast(LED1_PIN, LOW);
      break;
    case 2:
      pinMode(LED0_PIN, OUTPUT);
      pinMode(LED1_PIN, OUTPUT);
      digitalWriteFast(LED0_PIN, LOW);
      digitalWriteFast(LED1_PIN, HIGH);
      break;
    case 3:
      pinMode(LED0_PIN, OUTPUT);
      pinMode(LED2_PIN, OUTPUT);
      digitalWriteFast(LED0_PIN, HIGH);
      digitalWriteFast(LED2_PIN, LOW);
      break;
    case 4:
      pinMode(LED0_PIN, OUTPUT);
      pinMode(LED2_PIN, OUTPUT);
      digitalWriteFast(LED0_PIN, LOW);
      digitalWriteFast(LED2_PIN, HIGH);
      break;
    case 5:
      pinMode(LED0_PIN, OUTPUT);
      pinMode(LED3_PIN, OUTPUT);
      digitalWriteFast(LED0_PIN, HIGH);
      digitalWriteFast(LED3_PIN, LOW);
      break;
    case 6:
      pinMode(LED0_PIN, OUTPUT);
      pinMode(LED3_PIN, OUTPUT);
      digitalWriteFast(LED0_PIN, LOW);
      digitalWriteFast(LED3_PIN, HIGH);
      break;
    case 7:
      pinMode(LED1_PIN, OUTPUT);
      pinMode(LED2_PIN, OUTPUT);
      digitalWriteFast(LED1_PIN, HIGH);
      digitalWriteFast(LED2_PIN, LOW);
      break;
    case 8:
      pinMode(LED1_PIN, OUTPUT);
      pinMode(LED2_PIN, OUTPUT);
      digitalWriteFast(LED1_PIN, LOW);
      digitalWriteFast(LED2_PIN, HIGH);
      break;
    case 9:
      pinMode(LED1_PIN, OUTPUT);
      pinMode(LED3_PIN, OUTPUT);
      digitalWriteFast(LED1_PIN, HIGH);
      digitalWriteFast(LED3_PIN, LOW);
      break;
    case 10:
      pinMode(LED1_PIN, OUTPUT);
      pinMode(LED3_PIN, OUTPUT);
      digitalWriteFast(LED1_PIN, LOW);
      digitalWriteFast(LED3_PIN, HIGH);
      break;
    case 11:
      pinMode(LED2_PIN, OUTPUT);
      pinMode(LED3_PIN, OUTPUT);
      digitalWriteFast(LED2_PIN, HIGH);
      digitalWriteFast(LED3_PIN, LOW);
      break;
    case 12:
      pinMode(LED2_PIN, OUTPUT);
      pinMode(LED3_PIN, OUTPUT);
      digitalWriteFast(LED2_PIN, LOW);
      digitalWriteFast(LED3_PIN, HIGH);
      break;
  }
}

// cvIn is from 0 to 1023, representing 0-10V
// Returns 0 for C0
//         1 for C#0
//        ...
//        12 for C1
//        ...
//       119 for B9
//       120 for C10
int quantize(int cvIn)
{
  for (int octave = 0; octave < 10; ++octave)
  {
    // Don't bother checking each note if the ADC value is above the end of this octave
    int adcValueForNextOctave = (octave + 1) * 100;
    if (adcValueForNextOctave < cvIn) continue;

    for (byte note = 0; note < 12; ++note)
    {
      int adcValueForThisNote = (octave * 100) + (note * 8) + (note / 3);
      if ((activeSwitches >> note) & 1)
      {
        if (adcValueForThisNote >= cvIn)
        {
          return octave * 12 + note;
        }
      }
    }
  }
  return 120;
}

void writeDac(int value)
{
  if (value > 255) value = 255;
  if (value < 0) value = 0;
  
//  Serial.print("writeDac: ");
//  Serial.print(value);
//  Serial.print("\t0x");
  value <<= 4;
  value |= 0x1000;

#ifdef LOW_DAC_GAIN
  value |= 0x2000;
#endif

  
//  Serial.println(value, HEX);

//  digitalWrite(CS_PIN, HIGH);
//  delay(1);
  digitalWriteFast(CS_PIN, LOW);
//  digitalWrite(SCK_PIN, LOW);

//  for (int i = 15; i >= 0; --i)
//  {
//    digitalWrite(SDI_PIN, (value >> i) & 1);
//    digitalWrite(SCK_PIN, HIGH);
//    digitalWrite(SCK_PIN, LOW);
//  }

  
  shiftOut(SDI_PIN, SCK_PIN, MSBFIRST, (value >> 8) & 0xFF);
  shiftOut(SDI_PIN, SCK_PIN, MSBFIRST, value & 0xFF);
  digitalWriteFast(CS_PIN, HIGH);

  digitalWriteFast(GATE_PIN, LOW);
//  delay(1);
  digitalWriteFast(GATE_PIN, HIGH);
}

//bool trigSenseHigh = true;

bool isTrigConnected()
{
  bool trigSensePullup = false;
  // Allow trigger sense to be pulled low, which will be inverted and produce a high on the trigger in if nothing is connected
  pinMode(TRIG_SENSE_PIN, INPUT);
  
  // We check if the trigger is connected by checking if the trigger input matches the trigger sense pin.
  // If they match, we toggle the trigger sense and check again. Repeat some number of times to prevent false positives.
  for (int i = 0; i < 3; ++i)
  {
    delay(1);
    
    bool trig = (PORTC >> 3) & 1;
    bool expectedTrig = !trigSensePullup;
    // If trigger pullup is disabled, we expect trigger in to be high
    if (trig != expectedTrig)
    {
//      Serial.print("Exiting on ");
//      Serial.println(i);
      return true;
    }

    trigSensePullup = !trigSensePullup;
    // Use INPUT_PULLUP because the trigger sense pin will be directly connected to the input briefly while the plug is being inserted.
    if (trigSensePullup)  pinMode(TRIG_SENSE_PIN, INPUT_PULLUP);
    else                  pinMode(TRIG_SENSE_PIN, INPUT);
//    if (trigSenseHigh)  digitalWrite(TRIG_SENSE_PIN, LOW);
//    else                digitalWrite(TRIG_SENSE_PIN, HIGH);
//    trigSenseHigh = !trigSenseHigh;
  }

  // If trigger in matched trigger sense every time, there is nothing connected.
  return false;
}

// Returns -1 for disconnected, 0 for low, 1 for high
int readTrigger()
{
  if (!isTrigConnected()) return -1;
  // Invert
  if ((PORTC >> 3) & 1) return 0;
  return 1;
}


void setup() 
{
  setupSwitchPins();
  pinMode(GATE_PIN, OUTPUT);
  pinMode(SCK_PIN, OUTPUT);
  pinMode(SDI_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(CV_IN_PIN, INPUT);
  pinMode(SLEW_PIN, INPUT);
  pinMode(TRANSPOSE_PIN, INPUT);
  pinMode(TRIG_SENSE_PIN, INPUT);
  pinMode(TRIG_IN_PIN, INPUT);

//  Serial.begin(9600);
//  Serial.println("Hello");
}

int lastNote;
int lastTrig = -1;

#define PITCH_EXTRA_BITS 2
int targetPitch = 0;
int currentPitch = 0;

void loop()
{

////  pinMode(TRIG_SENSE_PIN, INPUT);
////  delay(1);
////  digitalWrite(GATE_PIN, digitalRead(TRIG_IN_PIN));
////  delay(1000);
////  pinMode(TRIG_SENSE_PIN, INPUT_PULLUP);
////  delay(1);
////  digitalWrite(GATE_PIN, digitalRead(TRIG_IN_PIN));
////  delay(2000);
////  return;
//  
//  // Test code
//  if (isTrigConnected())
//  {
//    digitalWrite(GATE_PIN, !digitalRead(TRIG_IN_PIN));
//  }
//  else
//    digitalWrite(GATE_PIN, LOW);
////  digitalWrite(GATE_PIN, digitalRead(TRIG_IN_PIN));
////  delay(1000);
//
//  return;



  int cv = analogRead(CV_IN_PIN);
  int trig = readTrigger();
  int shift = analogRead(TRANSPOSE_PIN);
  int slew = 1023 - analogRead(SLEW_PIN);
  shift = 1023 - shift;
  shift /= 10;
  if (shift > 100) shift = 100;
//  shift /= 79;
  cv += shift;
  readSwitches();
//  Serial.print("Input: ");
//  Serial.print(pot);
//  Serial.print("\t");
  
  if (activeSwitches == 0)
  {
    setLed(0);
    lastNote = -1;
    writeDac(0);
    digitalWriteFast(GATE_PIN, LOW);
  }
  else
  {
    // On rising edge of trigger, or if trigger isn't connected, update CV
//    if ((trig == 1 && lastTrig != 1) || 
//        trig == -1)
    {
      int note = quantize(cv);
      setLed(note % 12 + 1);
  //    digitalWrite(GATE_PIN, HIGH);

      
#ifdef LOW_DAC_GAIN
      targetPitch = note << (2 + PITCH_EXTRA_BITS);
#else
      targetPitch = note << (1 + PITCH_EXTRA_BITS);
#endif
  

//#ifdef LOW_DAC_GAIN
//      writeDac(note << 2);
//#else
//      writeDac(note << 1);
//#endif
      lastNote = note;
    }

//    if (currentPitch != targetPitch)
    {
      // If the slew knob is all the way down (or close enough),
      // jump straight to the desired pitch.
      if (slew < 10)
      {
        currentPitch = targetPitch;
      }
      else
      {
        // delta = 4 would be moving by 100 cents per loop, when in LOW_DAC_GAIN mode
        int delta = 0;
        if (slew < 100) delta = 12;
        else if (slew < 200) delta = 10;
        else if (slew < 300) delta = 9;
        else if (slew < 400) delta = 8;
        else if (slew < 500) delta = 7;
        else if (slew < 600) delta = 6;
        else if (slew < 700) delta = 5;
        else if (slew < 800) delta = 4;
        else if (slew < 900) delta = 3;
        else if (slew < 1000) delta = 2; 
        else delta = 1;
        if (currentPitch < targetPitch) currentPitch++;
        if (currentPitch > targetPitch) currentPitch--;
      }
      
      
//      currentPitch = targetPitch;
      writeDac(currentPitch >> PITCH_EXTRA_BITS);
    }
    
    

    // If the trigger is high or isn't connected, assert the gate output
    if (trig == 1 || trig == -1)
    {
      digitalWriteFast(GATE_PIN, HIGH);
    }
    else
    {
      digitalWriteFast(GATE_PIN, LOW);
    }

    
  }
  
  lastTrig = trig;



  
}
