// IR DEFINITIONS
#define IRpin_PIN       PIND          // Pins for IR sensor (do not change)
#define IRpin           2             // Pin number for IR sensor (do not change)

#define DATA_LOC        0             // Data located in which list? 0: OFF, 1: ON

#define LOW_VAL         550           // Value to interpret as 0
#define HIGH_VAL        1650          // Value to interpret as 1

#define START_ON        4680          // Start code's ON value
#define START_OFF       4370          // Start code's OFF value

#define RANGE1_START    17            // From where to start decoding? (index starting from 0)
#define RANGE1_END      24            // Till where to decode?
#define RANGE2_START    25            // comment this #define if you do not have repeated data
#define RANGE2_END      32            // comment this #define if you do not have repeated data
#define RANGE2_INVERTED 1             // is the range2 inverted/complemented range1? 1: yes, 0: no, -1: ignore range2

#define MAXPULSE        65000         // the maximum pulse we'll listen for - 65 ms
#define MAX_PULSE_PAIRS 60            // maximum number of pulse pairs to store
#define FUZZINESS       5             // What percent variation is allowed: 2 = 50%, 3 = 33.3%, 4 = 25% 5 = 20%

// we will store up to 60 pulse pairs (this is -a lot-)
uint16_t pulses[MAX_PULSE_PAIRS][2];  // pair is high and low pulse
uint8_t currentPulse = 0;             // index for pulses we're storing

// variables for storing times in microseconds
unsigned long currMicros, lastMicros, diffMicros;

uint16_t lowFuzz, highFuzz;                     // amount of difference/fuzziness allowed in high and low values
uint16_t startOnFuzz, startOffFuzz;             // amount of difference/fuzziness allowed in Start code's on-off values

boolean newCodeToRead = false;                  // is a new IR code ready to be read?

void readIR()
{
  if(newCodeToRead)                             // if the new code is unread, exit (to prevent overwriting)
    return;
    
  currMicros = micros();

  if(currentPulse < MAX_PULSE_PAIRS)            // if the pulses array isn't full
  {
    diffMicros = currMicros - lastMicros;       // time since last pin change

    if(IRpin_PIN & (1 << IRpin))                // HIGH? (no signal)
    {
      pulses[currentPulse++][1] = diffMicros;   // save ON value
      lastMicros = currMicros;                  // current time becomes the time of last signal value change
    }
    else                                        // LOW? (signal!)
    {
      pulses[currentPulse][0] = diffMicros;     // save OFF value
      lastMicros = currMicros;                  // current time becomes the time of last signal value change

      if(currentPulse == 1)
      {
        // if Start code values do not match within fuzziness
        if(abs((int)pulses[0][1] - (int)START_ON) > startOnFuzz || abs((int)pulses[1][0] - (int)START_OFF) > startOffFuzz)
          currentPulse = 0;                     // reset pulse counter
      }
    }
  }
  else                                          // set newCodeToRead flag if maximum buffer limit reached
    newCodeToRead = true;
}

int decodeIR()
{
  unsigned long lastTime = lastMicros;
  unsigned long nowTime = micros();

  if(nowTime > lastTime)                        // This check is so diffTime is positive. lastTime can sometimes be greater if interrupt is called inbetween
  {
    unsigned long diffTime = (nowTime - lastTime);

    // if there is new code to be read, or we've been waiting too long and the code has ended
    if(newCodeToRead || (diffTime > MAXPULSE && currentPulse != 0))
    {
      // if somehow we are stuck at Off value for Start code, we need to restart
      if(diffTime > MAXPULSE && currentPulse == 1)
      {
        currentPulse = 0;                       // reset pulse counter
        return -1;                              // no new data :(
      }
            
      int offset = (DATA_LOC == 0) ? 1 : 0;     // get an opposite of DATA_LOC

      // if we have enough values for our ranges 
      if(currentPulse > RANGE1_END + offset)
      {
        #ifdef RANGE2_END                       // if #define RANGE2_END is uncommented
        if(currentPulse <= RANGE2_END + offset) // Data till RANGE2_END present?
        {
          currentPulse = 0;                     // reset pulse counter
          return -1;                            // no new data :(
        }
        #endif
        
        detachInterrupt(0);                     // disable interrupt to prevent value changing until we've decoded it

        uint8_t decodedVal1 = 0;                // decoded value for range1
        uint8_t decodedVal2 = 0;                // decoded value for range2
      
        // Go through all the values within decode range. Start backwards from end to start
        for(int i = RANGE1_END; i >= RANGE1_START; i--)
        {
          decodedVal1 = decodedVal1 << 1;       // shift to next bit
          // Does it match the high value?
          if(abs((int)pulses[i + offset][DATA_LOC] - (int)HIGH_VAL) < highFuzz)
            decodedVal1 += 1;                   // Add 1 to decoded val
        }
        
        #ifdef RANGE2_START
        // Go through all the values within decode range. Start backwards from end to start
        for(int i = RANGE2_END; i >= RANGE2_START; i--)
        {
          decodedVal2 = decodedVal2 << 1;       // shift to next bit
          // if we are inverting the values in range2
          if(RANGE2_INVERTED)
          {
            // Does it match the low value?
            if(abs((int)pulses[i + offset][DATA_LOC] - (int)LOW_VAL) < lowFuzz)
              decodedVal2 += 1;
          }
          else
          {
            // Does it match the high value?
            if(abs((int)pulses[i + offset][DATA_LOC] - (int)HIGH_VAL) < highFuzz)
              decodedVal2 += 1;
          }
        }
        #endif
          
        printpulses();                          // print the data

        newCodeToRead = false;                  // newCode has been read. Start recording for next IR code
        currentPulse = 0;                       // reset pulse counter
        attachInterrupt(0, readIR, CHANGE);     // re-enable interrupts

        #ifdef RANGE2_START                     // if range 2 was defined
        if(decodedVal1 != decodedVal2)          // if the values decoded from range1 and range2 don't match up
          return -1;                            // no new data :(
        #endif
        
        return decodedVal1;                     // success! :D
      }
      else
        currentPulse = 0;                       // reset pulse counter
    }
  }

  return -1;                                    // no new data :(
}

void printpulses(void)
{  
  // print it in an 'array' format
  Serial.println("int IRsignal[] = {");
  Serial.println("// ON, OFF");
  for (uint8_t i = 0; i < currentPulse - 1; i++)
  {
    Serial.print("\t"); // tab
    Serial.print(pulses[i][1]/10, DEC);
    Serial.print(", ");
    Serial.print(pulses[i+1][0]/10, DEC);
    Serial.println(",");
  }
  Serial.print("\t"); // tab
  Serial.print(pulses[currentPulse-1][1]/10, DEC);
  Serial.println(", 0};");
}

void setup(void)
{
  lastMicros = micros();                        // initialize lastMicros
  
  lowFuzz = LOW_VAL / FUZZINESS;
  highFuzz = HIGH_VAL / FUZZINESS;
  startOnFuzz = START_ON / FUZZINESS;           // calculate fuzziness allowed in Start code's On value
  startOffFuzz = START_OFF / FUZZINESS;         // calculate fuzziness allowed in Start code's Off value

  attachInterrupt(0, readIR, CHANGE);           // setup interrupt to call readIR() whenever INT0 pin (pin 2) changes

  Serial.begin(9600);                           // Begin serial data transfer
  Serial.println("Ready to decode IR!!!");
}

void loop(void)
{
  int decodedVal = decodeIR();
  if(decodedVal != -1)
  {
    Serial.print("Decoded Value: ");
    Serial.println(decodedVal);
  }
}

