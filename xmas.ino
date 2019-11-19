#define DEBUGMODE 0

// Pin defines
#define M0B 13
#define M0R 12
#define M0G 11

#define E0B 10
#define E0R 9
#define E0G 8

#define R0B 7
#define R0R 6
#define R0G 5

#define R1B 4
#define R1R 3
#define R1G 2

#define Y0B 1
#define Y0R 0
#define Y0G 14

#define C0B 15
#define C0R 16
#define C0G 17

#define H0B 23
#define H0R 25
#define H0G 27

#define R2B 29
#define R2R 31
#define R2G 33

#define I0B 35
#define I0R 37
#define I0G 39

#define S0B 41
#define S0R 43
#define S0G 45

#define T0B 68
#define T0R 67
#define T0G 66

#define M1B 65
#define M1R 64
#define M1G 63

#define A0B 62
#define A0R 61
#define A0G 60

#define S1B 59
#define S1R 58
#define S1G 57

#define AUX1 56
#define AUX2 55
#define AUX3 54

#define MODE_BUTTON 53

#define LETTER_MAX  14
#define COLOUR_MAX  3

const int LETTER_MAP[LETTER_MAX][3] = { {M0R, M0G, M0B}, {E0R, E0G, E0B}, {R0R, R0G, R0B}, {R1R, R1G, R1B}, {Y0R, Y0G, Y0B},
                                        {C0R, C0G, C0B}, {H0R, H0G, H0B}, {R2R, R2G, R2B}, {I0R, I0G, I0B}, {S0R, S0G, S0B},
                                        {T0R, T0G, T0B}, {M1R, M1G, M1B}, {A0R, A0G, A0B}, {S1R, S1G, S1B} };

#define FALSE 0
#define TRUE  1

#define RED_MASK     0x01
#define GREEN_MASK   0x02
#define BLUE_MASK    0x04

// Scenes - Single states             M  E  R  R  Y  C  H  R  I  S  T  M  A  S
const int ALL_OFF[LETTER_MAX]     = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
const int SOLID_RED[LETTER_MAX]   = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
const int SOLID_GREEN[LETTER_MAX] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
const int SOLID_BLUE[LETTER_MAX]  = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

const int RED_GREEN[LETTER_MAX]   = { 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2 };
const int GREEN_RED[LETTER_MAX]   = { 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1 };

const int CHASE_A1[LETTER_MAX]    = { 1, 2, 4, 1, 2, 4, 1, 2, 4, 1, 2, 4, 1, 2 };
const int CHASE_A2[LETTER_MAX]    = { 2, 4, 1, 2, 4, 1, 2, 4, 1, 2, 4, 1, 2, 4 };
const int CHASE_A3[LETTER_MAX]    = { 4, 1, 2, 4, 1, 2, 4, 1, 2, 4, 1, 2, 4, 1 };

const int CHASE_B1[LETTER_MAX]    = { 1, 3, 2, 6, 4, 5, 1, 3, 2, 6, 4, 5, 1, 3 };
const int CHASE_B2[LETTER_MAX]    = { 3, 2, 6, 4, 5, 1, 3, 2, 6, 4, 5, 1, 3, 2 };
const int CHASE_B3[LETTER_MAX]    = { 2, 6, 4, 5, 1, 3, 2, 6, 4, 5, 1, 3, 2, 6 };
const int CHASE_B4[LETTER_MAX]    = { 6, 4, 5, 1, 3, 2, 6, 4, 5, 1, 3, 2, 6, 4 };
const int CHASE_B5[LETTER_MAX]    = { 4, 5, 1, 3, 2, 6, 4, 5, 1, 3, 2, 6, 4, 5 };
const int CHASE_B6[LETTER_MAX]    = { 5, 1, 3, 2, 6, 4, 5, 1, 3, 2, 6, 4, 5, 1 };

const int END_SEQUENCE[LETTER_MAX] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

struct sequenceStep
{
  const int state[LETTER_MAX];
  const int delay;
};

const sequenceStep ALT_RED_GREEN[] = { { SOLID_RED[LETTER_MAX],    300 },
                                       { SOLID_GREEN[LETTER_MAX],  300 },
                                       { END_SEQUENCE[LETTER_MAX],   0 } };

const char DEBUG_MSG[LETTER_MAX] = {'M', 'E', 'R', 'R', 'Y', 'C', 'H', 'R', 'I', 'S', 'T', 'M', 'A', 'S'};

int letterData[LETTER_MAX] = { 0 };


void setup()
{
  int index;
  int colour;
    
  #if DEBUGMODE
    Serial.begin(115200);
  #else
    for(index = 0; index < LETTER_MAX; index++)
      for(colour = 0; colour < COLOUR_MAX; colour++)
      {
        pinMode(LETTER_MAP[index][colour], OUTPUT);
        digitalWrite(LETTER_MAP[index][colour], LOW);
      }

    pinMode(AUX1, OUTPUT);
    pinMode(AUX2, OUTPUT);
    pinMode(AUX3, OUTPUT);
    digitalWrite(AUX1, LOW);
    digitalWrite(AUX2, LOW);
    digitalWrite(AUX3, LOW);
  #endif

  pinMode(MODE_BUTTON, INPUT);
//  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), button_interrupt, FALLING);
}

void button_interrupt()
{

}

bool readButton()
{
  static bool lastState = 0;
  bool state = digitalRead(MODE_BUTTON);

// Need to add proper debounce

  if(state != lastState)
  {
    lastState = state;

    // Check if button is pressed.
    if(state == 0)
      return TRUE;
  }
  return FALSE;
}

void updateLetters()
{
  int index;

  #if DEBUGMODE
    Serial.print((String)"\033[1;37m>>>  ");
    for(index = 0; index < LETTER_MAX; index++)
    {
      Serial.print((String)"\033[1;"+(30 + letterData[index])+"m" + DEBUG_MSG[index] + " ");
      if(index == 4)
        Serial.print("  ");    
    }
    Serial.print((String)"\033[1;37m <<<\033[39D");
  #else
    int colour;

    for(index = 0; index < LETTER_MAX; index++)
      for(colour = 0; colour < COLOUR_MAX; colour++)
        digitalWrite(LETTER_MAP[index][colour], letterData[index] & (1 << colour));
  #endif
}

// basic styles

void basicOff(int *data)
{
  int i;
  for (i=0;i<LETTER_MAX;i++)
  {
      data[i] = 0;
  }
  updateLetters();
}
void basicAlternateColor(int *data, int colorMask1, int colorMask2)
{
  int i;
  for (i=0;i<LETTER_MAX;i++)
  {
      data[i] = (i % 2) ? colorMask1 : colorMask2;
  }
  updateLetters();
}

void basicSingleColor(int *data, int colorMask)
{
  int i;
  for (i=0;i<LETTER_MAX;i++)
  {
    data[i]=colorMask;
  }
  updateLetters();

  // not sure if we want to put delay here...todo

}

// ----  styles 
void styleAlternateRedGreen(int *data)
{
  basicAlternateColor(data,RED_MASK,GREEN_MASK);
}


void styleAlternateBlueGreen(int *data)
{
  basicAlternateColor(data,GREEN_MASK,BLUE_MASK);
}


void styleRedColor(int *data)
{
  basicSingleColor(data, RED_MASK);
}

void styleGreenColor(int *data)
{
  basicSingleColor(data, GREEN_MASK);
}

void styleBlueColor(int *data)
{
  basicSingleColor(data, BLUE_MASK);
}

#define STYLE_MAX 5
void (*styleFuncList[STYLE_MAX])(int *) = { styleRedColor,
                                            styleGreenColor,
                                            styleBlueColor,
                                            styleAlternateRedGreen,
                                            styleAlternateBlueGreen
                                    };

void loop()
{
  static int count = 0;
  static bool toggle = 0;
  int i;

  if(readButton())
    toggle ^= 0x01;

  if(toggle)
  {
    if(count++ > 5)
      count = 0;

    (*styleFuncList[count])(letterData);
    delay(500);
  }
  else
  {
    basicOff(letterData);
  }
}
