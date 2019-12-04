// XMAS Light Driver for Ardino Mega
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

#define STB 55
#define STR 54
#define STG 56

#define MODE_BUTTON 53

#define LETTER_MAX  14
#define COLOUR_MAX  3
#define MERRY_MAX   5

const int LETTER_MAP[LETTER_MAX][COLOUR_MAX] =  { {M0R, M0G, M0B}, {E0R, E0G, E0B}, {R0R, R0G, R0B}, {R1R, R1G, R1B}, {Y0R, Y0G, Y0B},
                                                  {C0R, C0G, C0B}, {H0R, H0G, H0B}, {R2R, R2G, R2B}, {I0R, I0G, I0B}, {S0R, S0G, S0B},
                                                  {T0R, T0G, T0B}, {M1R, M1G, M1B}, {A0R, A0G, A0B}, {S1R, S1G, S1B} };

const int STAR_MAP[COLOUR_MAX] = {STB, STR, STG};

#define FALSE 0
#define TRUE  1

#define BUTTON_SHORT 30       // 30ms to detect button press
#define BUTTON_LONG  1000     // 1s to detect long button press

// Blue Level
#define SIGN_START_MENU   0
#define SIGN_RANDOMIZE    0   // This will be a much more dazzling display of capabilities
#define SIGN_BASIC        1   // This will be the basic, non-seizure inducing sequence for Nishat
#define SIGN_COLOUR_CHASE 2   // Demo of chase
#define SIGN_KNIGHT       3   // Demo of Knight Rider
#define SIGN_RANDOM_CHARS 4   // Demo of random characters
#define SIGN_SOLID_ALTERNATE 5
#define SIGN_BREATHE      6
#define SIGN_STACK        7
#define SIGN_COLOUR_PUKE  8
#define SIGN_OVERLAP      9
#define SIGN_LETTER_CHASE 10
#define SIGN_NEW1         11
#define SIGN_NEW2         12
#define SIGN_NEW3         13

// Green Level
#define SIGN_TEST_RED     14   // Test of solid red
#define SIGN_TEST_GREEN   15   // Test of solid green
#define SIGN_TEST_BLUE    16   // Test of solid blue
#define SIGN_TEST_WHITE   17   // Test of solid white
#define SIGN_MAX          SIGN_TEST_WHITE
#define SIGN_SELECT       99  // Select mode state

#define SELECT_TIME   6    // x500ms
#define PLAY_MEM      3    // Number of routines to avoid during randomize

#define PWM_MAX       33   // 33 x .3ms = 10ms pwm period

#define RED_MASK     0x01
#define GREEN_MASK   0x02
#define BLUE_MASK    0x04
#define WHITE_MASK   RED_MASK | GREEN_MASK | BLUE_MASK

#define BREATHE_MAX   4
const int BREATHE_COLOR[BREATHE_MAX] = { 3, 6, 5, 7 };
#define LET_CH_MAX    16
const int LETTER_CHASE_COLOUR[LET_CH_MAX] = { 7, 0, 0, 0, 2, 0, 0, 0, 7, 0, 0, 0, 1, 0, 0, 0 };

// Scenes - Single states          M  E  R  R  Y  C  H  R  I  S  T  M  A  S
const int CHASE[LETTER_MAX]    = { 1, 2, 4, 1, 2, 4, 1, 2, 4, 1, 2, 4, 1, 2 };
const int POPO1A[LETTER_MAX]   = { 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
const int POPO1B[LETTER_MAX]   = { 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 7, 7, 7, 7 };
const int POPO2A[LETTER_MAX]   = { 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4 };
const int POPO2B[LETTER_MAX]   = { 7, 7, 7, 7, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4 };

const char DEBUG_MSG[LETTER_MAX] = {'M', 'E', 'R', 'R', 'Y', 'C', 'H', 'R', 'I', 'S', 'T', 'M', 'A', 'S'};

// Globals
int letterData[LETTER_MAX] = { 0 };
int starData = 0;
int pwmData[LETTER_MAX][COLOUR_MAX] = { 0 };
int pwmStar[COLOUR_MAX] = { 0 };
int signState = SIGN_RANDOMIZE;
bool signActive = TRUE;
int brightMax = PWM_MAX;
int fadeSpeed = 0;
bool instantOn = FALSE;
int selection = 0;
int starFollow = 0;

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

    for(colour = 0; colour < COLOUR_MAX; colour++)
    {
      pinMode(STAR_MAP[colour], OUTPUT);
      digitalWrite(STAR_MAP[colour], LOW);
    }
  #endif

  pinMode(MODE_BUTTON, INPUT);
  randomSeed(analogRead(15));
}

void handlePWM()
{
#if !DEBUGMODE
  static int pwm = 0;
  int colour;
  int index;
  
  //Process LED PWM continuously, every ~300us
    pwm++;
      
    if(pwm > PWM_MAX)
      pwm = 0;
 
     for(index = 0; index < LETTER_MAX; index++)
      for(colour = 0; colour < COLOUR_MAX; colour++)
        digitalWrite(LETTER_MAP[index][colour], pwmData[index][colour] > pwm);

    for(colour = 0; colour < COLOUR_MAX; colour++)
      digitalWrite(STAR_MAP[colour], pwmStar[colour] > pwm);        
#endif
}

void handleFade()
{
  static int letterMem[LETTER_MAX] = { 0 };
  static int starMem = 0;
  static unsigned long fadeTime = millis();
  int colour = 0;
  int index;
  
#if DEBUGMODE
  for(index = 0; index < LETTER_MAX; index++)
  {
    if(letterData[index] != letterMem[index])
    {
      colour++;
      letterMem[index] = letterData[index];
    }
  }
  if(colour)
  {
    Serial.print((String)"\033[1;37m>>>  ");
    for(index = 0; index < LETTER_MAX; index++)
    {
      Serial.print((String)"\033[1;"+(30 + letterData[index])+"m" + DEBUG_MSG[index] + " ");
      if(index == 4)
        Serial.print("  ");    
    }
    Serial.print((String)"\033[1;37m <<<\033[39D");
  }
#else
  // Process LED fade effect
  if(millis() - fadeTime >= ((fadeSpeed < PWM_MAX ? PWM_MAX : fadeSpeed) / brightMax))
  {
    fadeTime = millis();

    // Update star follow data
    if(starFollow < LETTER_MAX)
      starData = letterData[starFollow];
    else
      starData = 0;
    
    for(index = 0; index < LETTER_MAX; index++)
      for(colour = 0; colour < COLOUR_MAX; colour++)
      {
        // Check if letter-colour is requested on
        if(letterData[index] & (1 << colour))
        {
          // Check if already on
          if(letterMem[index] & (1 << colour))
            pwmData[index][colour] = brightMax;

          // Fade up this letter-colour
          else
          {
            if(pwmData[index][colour] < brightMax)
            {
              if(instantOn)
                pwmData[index][colour] = PWM_MAX;
              else
                pwmData[index][colour]++;
            }
            if(pwmData[index][colour] == brightMax)
              letterMem[index] |= (1 << colour);
          }
        }
          
        // This letter-colour is requested off
        else
        {
          // Check if already off
          if(!(letterMem[index] & (1 << colour)))
            pwmData[index][colour] = 0;

          // Fade down this letter-colour
          else
          {
            if(pwmData[index][colour] > 0)
              pwmData[index][colour]--;
            if(pwmData[index][colour] == 0)
              letterMem[index] &= ~(1 << colour);
          }
        }
      }

    for(colour = 0; colour < COLOUR_MAX; colour++)
    {
      // Check if star colour is requested on
      if(starData & (1 << colour))
      {
        // Check if already on
        if(starMem & (1 << colour))
          pwmStar[colour] = brightMax;

        // Fade up this letter-colour
        else
        {
          if(pwmStar[colour] < brightMax)
          {
            if(instantOn)
              pwmStar[colour] = PWM_MAX;
            else
              pwmStar[colour]++;
          }
          if(pwmStar[colour] == brightMax)
            starMem |= (1 << colour);
          }
        }
          
      // This star colour is requested off
      else
      {
        // Check if already off
        if(!(starMem & (1 << colour)))
            pwmStar[colour] = 0;

        // Fade down this letter-colour
        else
        {
          if(pwmStar[colour] > 0)
            pwmStar[colour]--;
          if(pwmStar[colour] == 0)
            starMem &= ~(1 << colour);
        }
      }
    }
  }
#endif
}

bool wait(unsigned long delayTime)
{
  static unsigned long prevTime = millis();
  bool retVal = FALSE;
  
  while(millis() - prevTime < delayTime)
  {
    // Process LED fade effect
    handleFade();

    // Process LED PWM control
    handlePWM();

    //Scan the pushbutton
    if(handleModeButton())
    {
      retVal = TRUE;
      break;
    }
  }

  prevTime = millis();
  return(retVal);
}

bool handleModeButton()
{
  static unsigned long scanTime = millis();
  static bool lastState = 1;
  static int debounce = 0;
  static bool possibleLong = FALSE;
  bool state = 0;
  
  // Scan button state every 1ms
  if(millis() - scanTime >= 1)
  {
    scanTime += 1;
    state = digitalRead(MODE_BUTTON);
      
    if(state != lastState)
    {
      debounce++;

      // Check for long button press
      if(debounce >= BUTTON_LONG)
      {
        // Check if long press from within menu
        if(signState == SIGN_SELECT)
          selection = SIGN_MAX;

        signState = SIGN_SELECT; 
        signActive = TRUE;
        lastState = state;
        possibleLong = FALSE;
        return(TRUE);
      }

      // Check for button release
      else if((debounce >= BUTTON_SHORT) && (state == 1))
        lastState = state;

      // Check for possible long press on next release
      else if((debounce >= BUTTON_SHORT) && (state == 0))
        possibleLong = TRUE;
    }
    else
    {
      // Check for short button press (released after short threshold)
      if((possibleLong == TRUE) && (state == 1))
      {
        if((signActive) && (signState < SIGN_SELECT))
        {
          signActive = FALSE;
          fadeSpeed = 0;
        }
        else
          signActive = TRUE;
          
        possibleLong = FALSE;
        return(TRUE);
      }
      debounce = 0;
    }
  }
  return(FALSE);
}

void configLED(int bright, int speed, bool instant, int follow)
{ 
  brightMax = bright;   // Set maximum brightness threshold
  fadeSpeed = speed;    // Set the LED fade speed (ms to fade)
  instantOn = instant;  // Set instant on instead of fade on
  starFollow = follow;  // Set the letter that the star should copy
}

// basic styles
void basicOff(int *data)
{
  int i;
  
  for (i = 0; i < LETTER_MAX; i++)
    data[i] = 0;
}

void basicAlternateColor(int *data, int colorMask1, int colorMask2)
{
  int i;
  
  for (i = 0; i < LETTER_MAX; i++)
    data[i] = (i % 2) ? colorMask1 : colorMask2;
}

void basicAlternateWord(int *data, int colorMask1, int colorMask2)
{
  int i;
  
  for (i = 0; i < LETTER_MAX; i++)
    data[i] = i < MERRY_MAX ? colorMask1 : colorMask2;
}

void basicSingleColor(int *data, int colorMask)
{
  int i;
  
  for (i = 0; i < LETTER_MAX; i++)
    data[i] = colorMask;
}

void basicConstantLoad(int *data, const int *constant)
{
  int i;
  
  for(i = 0; i < LETTER_MAX; i++)
    data[i] = constant[i];
}

void breatheOut(int *data)
{
  int i;
  for(i=0;i<6;i++)
  {
    data[i] = data[i+1];
    data[13-i] = data[13-i-1];
  }
}

void breatheIn(int *data)
{
  int i;
  for(i=0;i<6;i++)
  {
    data[6-i] = data[6-i-1];
    data[7+i] = data[7+i+1];
  }
  data[0]=0;
  data[13]=0;
}

void rotateLeft(int *data)
{
  int i;
  int saved = data[0];
  
  for(i = 0; i < (LETTER_MAX - 1); i++)
  {
    data[i] = data[i + 1];
  }
  data[LETTER_MAX - 1] = saved;
}

void rotateRight(int *data)
{
  int i;
  int saved = data[LETTER_MAX - 1];
  
  for(i = (LETTER_MAX - 1); i > 0; i--)
  {
    data[i] = data[i - 1];
  }
  data[0] = saved;
}

int shiftLeft(int *data, int newData)
{
  int retValue = data[0];
  
  rotateLeft(data);
  data[LETTER_MAX - 1] = newData;
  return(retValue);
}

int shiftRight(int *data, int newData)
{
  int retValue = data[LETTER_MAX - 1];
  
  rotateRight(data);
  data[0] = newData;
  return(retValue);
}

void randomizeArray(int *data, int size)
{
  int index;
  int pos1;
  int pos2;
  int swap;
  
  for(index = 0; index < size; index++)
    data[index] = index;

  for(index = 0; index < 10; index++)
  {
    pos1 = random(0, size);
    pos2 = random(0, size);
    swap = data[pos1];
    data[pos1] = data[pos2];
    data[pos2] = swap;
  }  
}

// ----  styles 
void styleAlternateRedGreen(int *data)
{
  basicAlternateColor(data,RED_MASK,GREEN_MASK);
}

void styleAlternateGreenRed(int *data)
{
  basicAlternateColor(data,GREEN_MASK,RED_MASK);
}

void styleAltWordRedGreen(int *data)
{
  basicAlternateWord(data,RED_MASK,GREEN_MASK);
}

void styleAltWordGreenRed(int *data)
{
  basicAlternateWord(data,GREEN_MASK,RED_MASK);
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

void styleWhiteColor(int *data)
{
  basicSingleColor(data, WHITE_MASK);
}

#define BASIC_MAX 6
void (*styleFuncListBasic[BASIC_MAX])(int *) = { styleRedColor,
                                                 styleGreenColor,
                                                 styleAltWordRedGreen,
                                                 styleAltWordGreenRed,
                                                 styleAlternateRedGreen,
                                                 styleAlternateGreenRed
                                               };

#define SOLID_MAX 4
void (*styleFuncListSolid[SOLID_MAX])(int *) = { styleRedColor,
                                                 styleGreenColor,
                                                 styleBlueColor,
                                                 styleWhiteColor
                                               };

#define RANDOMIZE_MAX 9
bool (*styleFuncListRandom[RANDOMIZE_MAX])() = { handleRandomChars,
                                                 handleSignRider,
                                                 handleSignColourChase,
                                                 handleSolidAlternate,
                                                 handleSignBreathe,
                                                 handleSignStack,
                                                 handleSignColourPuke,
                                                 handleSignOverlap,
                                                 handleSignLetterChase
                                               };

void handleSelectMenu()
{
  int colour;
  int position;
  int menuDelay;

  configLED(PWM_MAX, 100, FALSE, LETTER_MAX);

  while(signState == SIGN_SELECT)
  {
    position = selection % LETTER_MAX;
    colour = BLUE_MASK >> (selection / LETTER_MAX);
    
    for(menuDelay = 0; menuDelay < SELECT_TIME; menuDelay++)
    {
      basicOff(letterData);
      letterData[position] = colour;
      if(wait(250))
        break;

      basicOff(letterData);
      if(wait(250))
        break;
    }

    if(menuDelay < SELECT_TIME)
    {
      menuDelay = 0;
      selection++;

      if(selection > SIGN_MAX)
        selection = 0;
    }
    else
      signState = SIGN_START_MENU + selection;
  }
}

void handleSignBasic()
{
  int style;
  configLED(PWM_MAX, 3000, FALSE, 0);

  for(style = 0; style < BASIC_MAX; style++)
  {
    (*styleFuncListBasic[style])(letterData);
    if(wait(6000))
      break;
  }
}

void handleSignRandomize()
{
  static int lastPlayed[PLAY_MEM] = {0};
  int style;
  int index;

  // Randomize all the cool routines
  while(1)
  {
    style = random(0, RANDOMIZE_MAX);
      
    for(index = 0; index < PLAY_MEM; index++)
      if(style == lastPlayed[index])
        style = 999;

    if(style == 999)
      continue;

    if((*styleFuncListRandom[style])())
      break;

    for(index = (PLAY_MEM - 1); index > 0; index--)
      lastPlayed[index] = lastPlayed[index - 1];

    lastPlayed[0] = style;  
  }
}

bool handleSignColourChase()
{
  int saved = BLUE_MASK;
  int index;

  configLED(PWM_MAX, 400, FALSE, 2);
        
  basicConstantLoad(letterData, CHASE);
  for(index = 0; index < 30; index++)
  {
    saved = shiftRight(letterData, saved);
    if(wait(400))
      return(TRUE);
  }
  return(FALSE);
}

bool handleSignRider()
{
  int index;
  int repeat;
  configLED(PWM_MAX, 600, TRUE, 6);

  styleRedColor(letterData);
  letterData[0] = GREEN_MASK;
  for(repeat = 0; repeat < 4; repeat++)
  {
    for(index = 0; index < 26; index++)
    {
      if(wait(80))
        return(TRUE);
           
      if(index < 13 )
        rotateRight(letterData);
      else
        rotateLeft(letterData);
    }
  }
  return(FALSE);
}

bool handleSignStack()
{
  int index;
  int shift;
  int time;
  int colourData[LETTER_MAX];

  configLED(PWM_MAX, 50, TRUE, 0);  
  basicAlternateColor(colourData, RED_MASK, GREEN_MASK);
  basicOff(letterData);

  if(wait(300))
    return(TRUE);

  for(index = 0; index < LETTER_MAX; index++)
  {
    time = 250 / (LETTER_MAX - index);
    letterData[0] = WHITE_MASK;

    for(shift = 0; shift < (LETTER_MAX - index - 1); shift++)
    {
      if(wait(time))
        return(TRUE);

      letterData[shift + 1] = letterData[shift];
      letterData[shift] = 0;
    }

    letterData[LETTER_MAX - index - 1] = colourData[LETTER_MAX - index - 1];

    if(wait(200))
      return(TRUE);
  }

  if(wait(400))
    return(TRUE);

  configLED(PWM_MAX, 50, TRUE, LETTER_MAX - 1);  

  if(wait(400))
    return(TRUE);

  for(index = LETTER_MAX - 1; index >= 0; index--)
  {
    time = 250 / (LETTER_MAX - index);
    letterData[index] = WHITE_MASK;

    for(shift = index; shift < LETTER_MAX - 1; shift++)
    {
      letterData[shift + 1] = letterData[shift];
      letterData[shift] = 0;

      if(wait(time))
        return(TRUE);
    }
    letterData[LETTER_MAX - 1] = 0;

    if(wait(200))
      return(TRUE);
  }

  letterData[LETTER_MAX - 1] = 0;
  wait(300);
  return(FALSE);
}

bool handleRandomChars()
{
  int index;
  int colour;
  int count;
  int pos[LETTER_MAX];

  configLED(PWM_MAX, 1000, FALSE, 0);
  basicOff(letterData);
  randomizeArray(pos, LETTER_MAX);
  
  if(wait(500))
    return(TRUE);
          
  for(count = 0; count < 5; count++)
  {
    for(index = 0; index < LETTER_MAX; index++)
    {
      do
      {
        colour = random(1, 4);
        colour = (colour == 3) ? 4 : colour;
      }
      while(letterData[pos[index]] == colour);

      letterData[pos[index]] = colour;
      if(wait(200))
        return(TRUE);
    }
  }
          
  for(index = 0; index < LETTER_MAX; index++)
  {   
    letterData[pos[index]] = 0;
    if(wait(200))
      return(TRUE);
  }
  return(wait(1200));
}

bool handleSolidAlternate()
{
  int style;
  configLED(PWM_MAX, 3500, FALSE, 0);
       
  for(style = 0; style < SOLID_MAX; style++)
  {
    (*styleFuncListSolid[style])(letterData);
    if(wait(5000))
      return(TRUE);
  }
  return(FALSE);
}

bool handleSignBreathe()
{
  int style;
  int loop;

  configLED(PWM_MAX, 150, FALSE, 6);
  basicOff(letterData);

  for(loop = 0; loop < BREATHE_MAX; loop++)
  {
    letterData[6] = BREATHE_COLOR[loop];
    letterData[7] = BREATHE_COLOR[loop];
        
    for(style = 0; style < 13; style++)
    {
      if(wait(180))
        return(TRUE);
            
      if(style < (6))
        breatheOut(letterData);
      else
        breatheIn(letterData);
    }

    basicOff(letterData);
    if(wait(200))
      return(TRUE);
  }
  return(FALSE);
}

bool handleSignColourPuke()
{
  int index;
  int loop;
  int colour = 0;

  configLED(PWM_MAX, 50, FALSE, 6);

  for(loop = 0; loop < 30; loop++)
  {
    do
    {
      colour = random(1, 8);
    }
    while(letterData[0] == colour);
    
    letterData[6] = colour;
    letterData[7] = colour;
        
    for(index = 0; index < 6; index++)
    {
      if(wait(50))
        return(TRUE);
            
      breatheOut(letterData);
    }
    
    if(wait(50))
      return(TRUE);
  }
  return(FALSE);
}

bool handleSignOverlap()
{
  int pos1;
  int pos2;
  int colour1;
  int colour2;

  configLED(PWM_MAX, 150, FALSE, 0);
  basicOff(letterData);

  colour1 = random(1, 7);
  do
  {
    colour2 = random(1, 7);
  } while (colour1 & colour2);
  
  for(pos1 = 0, pos2 = LETTER_MAX - 1; pos1 < LETTER_MAX; pos1++, pos2--)
  {
    if(wait(150))
        return(TRUE);
    
    letterData[pos1] |= colour1;
    letterData[pos2] |= colour2;
  }

  if(wait(700))
    return(TRUE);

  for(pos1 = 0, pos2 = LETTER_MAX - 1; pos1 < LETTER_MAX; pos1++, pos2--)
  {
    if(wait(150))
        return(TRUE);
    
    letterData[pos1] &= ~colour1;
    letterData[pos2] &= ~colour2;
  }

  wait(150);
  return(FALSE);
}

bool handleSignLetterChase()
{
  int index;

  configLED(PWM_MAX, 200, TRUE, 0);
  basicOff(letterData);

  for(index = 0; index < (LET_CH_MAX * 6); index ++)
  {
    rotateRight(letterData);
    letterData[0] = LETTER_CHASE_COLOUR[index % LET_CH_MAX];

    if(wait(100))
      return(TRUE);
  }

  for(index = 0; index < LETTER_MAX; index ++)
  {
    rotateRight(letterData);
    letterData[0] = 0;

    if(wait(100))
      return(TRUE);
  }
  return(FALSE);
}

void loop()
{
  if(signActive)
  {
    switch(signState)
    {
      case SIGN_SELECT:       // Menu
        handleSelectMenu();
        break;

// Blue Level
      case SIGN_RANDOMIZE:    // M
        handleSignRandomize();
        break;

      case SIGN_BASIC:        // E
        handleSignBasic();
        break;
        
      case SIGN_COLOUR_CHASE: // R
        handleSignColourChase();
        break;

      case SIGN_KNIGHT:       // R
        handleSignRider();
        break;

      case SIGN_RANDOM_CHARS: // Y
        handleRandomChars();
        break;
                        
      case SIGN_SOLID_ALTERNATE: // C
        handleSolidAlternate();
        break;
      
      case SIGN_BREATHE:       // H
        handleSignBreathe();
        break;
      
      case SIGN_STACK:         // R
        handleSignStack();
        break;

      case SIGN_COLOUR_PUKE:   // I
        handleSignColourPuke();
        break;

      case SIGN_OVERLAP:       // S
        handleSignOverlap();
        break;

      case SIGN_LETTER_CHASE:  // T
        handleSignLetterChase();
        break;

// Green Level
      case SIGN_TEST_RED:      // M
        configLED(PWM_MAX, 0, TRUE, 0);
        styleRedColor(letterData);
        wait(100);
        break;

      case SIGN_TEST_GREEN:    // E
        configLED(PWM_MAX, 0, TRUE, 0);      
        styleGreenColor(letterData);
        wait(100);
        break;

      case SIGN_TEST_BLUE:     // R
        configLED(PWM_MAX, 0, TRUE, 0);      
        styleBlueColor(letterData);
        wait(100);
        break;

      case SIGN_TEST_WHITE:    // R
        configLED(PWM_MAX, 0, TRUE, 0);      
        styleWhiteColor(letterData);
        wait(100);
        break;

      case SIGN_NEW1: 
      case SIGN_NEW2: 
      case SIGN_NEW3: 
                       
      default:
        basicOff(letterData);
        wait(100);
        break;
    }
  }
  else
  {  
    basicOff(letterData);
    wait(100);
  }
}
