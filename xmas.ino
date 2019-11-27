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

#define AUX1 56
#define AUX2 55
#define AUX3 54

#define MODE_BUTTON 53

#define LETTER_MAX  14
#define COLOUR_MAX  3
#define MERRY_MAX 5

const int LETTER_MAP[LETTER_MAX][3] = { {M0R, M0G, M0B}, {E0R, E0G, E0B}, {R0R, R0G, R0B}, {R1R, R1G, R1B}, {Y0R, Y0G, Y0B},
                                        {C0R, C0G, C0B}, {H0R, H0G, H0B}, {R2R, R2G, R2B}, {I0R, I0G, I0B}, {S0R, S0G, S0B},
                                        {T0R, T0G, T0B}, {M1R, M1G, M1B}, {A0R, A0G, A0B}, {S1R, S1G, S1B} };

#define FALSE 0
#define TRUE  1

#define BUTTON_SHORT 30       // 30ms to detect button press
#define BUTTON_LONG  1000     // 1s to detect long button press

#define SIGN_BASIC        0   // This will be the basic, non-seizure inducing sequence
#define SIGN_CRAZY        1   // This will be a much more dazzling display of capabilities
#define SIGN_COLOUR_CHASE 2   // Demo of chase
#define SIGN_KNIGHT       3   // Demo of Knight Rider
#define SIGN_RANDOM_CHARS 4   // Demo of random characters
#define SIGN_TEST_RED     5   // Test of solid red
#define SIGN_TEST_GREEN   6   // Test of solid green
#define SIGN_TEST_BLUE    7   // Test of solid blue
#define SIGN_TEST_WHITE   8   // Test of solid white
#define SIGN_SOLID_ALTERNATE 9
#define SIGN_BREATHE 10
#define SIGN_MAX          SIGN_BREATHE
#define SIGN_SELECT       99  // Select mode state

#define SELECT_TIME   6    // x500ms

#define PWM_MAX       33   // 33 x .3ms = 10ms pwm period

#define RED_MASK     0x01
#define GREEN_MASK   0x02
#define BLUE_MASK    0x04
#define WHITE_MASK   RED_MASK | GREEN_MASK | BLUE_MASK

// Scenes - Single states          M  E  R  R  Y  C  H  R  I  S  T  M  A  S
const int CHASE[LETTER_MAX]    = { 1, 2, 4, 1, 2, 4, 1, 2, 4, 1, 2, 4, 1, 2 };

const char DEBUG_MSG[LETTER_MAX] = {'M', 'E', 'R', 'R', 'Y', 'C', 'H', 'R', 'I', 'S', 'T', 'M', 'A', 'S'};

// Globals
int letterData[LETTER_MAX] = { 0 };
int pwmData[LETTER_MAX][COLOUR_MAX] = { 0 };
int signState = SIGN_BASIC;
bool signActive = TRUE;
int brightMax = PWM_MAX;
int fadeSpeed = 0;
bool instantOn = FALSE;

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
    digitalWrite(AUX1, HIGH);
    digitalWrite(AUX2, HIGH);
    digitalWrite(AUX3, HIGH);
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
#endif
}

void handleFade()
{
  static int letterMem[LETTER_MAX] = { 0 };
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

  //Process LED fade effect
    if(millis() - fadeTime >= ((fadeSpeed < PWM_MAX ? PWM_MAX : fadeSpeed) / brightMax))
    {
      fadeTime = millis();
      
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

void configLED(int bright, int speed, bool instant)
{ 
  brightMax = bright;   // Set maximum brightness threshold
  fadeSpeed = speed;    // Set the LED fade speed (ms to fade)
  instantOn = instant;  // Set instant on instead of fade on
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

#define CRAZY_MAX 5
void (*styleFuncListCrazy[CRAZY_MAX])() = { handleRandomChars,
                                            handleSignRider,
                                            handleSignBreathe,
                                            handleSignColourChase,
                                            handleSolidAlternate
                                    };

void handleSelectMenu()
{
  int selection = 0;
  int menuDelay;

  configLED(PWM_MAX, 100, FALSE);

  while(signState == SIGN_SELECT)
  {
    for(menuDelay = 0; menuDelay < SELECT_TIME; menuDelay++)
    {
      basicOff(letterData);
      letterData[selection] = BLUE_MASK;
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
      signState = SIGN_BASIC + selection;
  }
}

void handleSignBasic()
{
  int style;
  configLED(PWM_MAX, 3000, FALSE);
       
  for(style = 0; style < BASIC_MAX; style++)
  {
    (*styleFuncListBasic[style])(letterData);
    if(wait(6000))
      break;
  }
}

void handleSignCrazy()
{
  int style;
  configLED(PWM_MAX, 100, FALSE);
        
  for(style = 0; style < CRAZY_MAX; style++)
  {
    (*styleFuncListCrazy[style])();
    if(!signActive)
      break;
  }
}

void handleSignColourChase()
{
  int saved = BLUE_MASK;
  int index;

  configLED(PWM_MAX, 400, FALSE);
        
  basicConstantLoad(letterData, CHASE);
  for(index = 0; index < 21; index++)
  {
    saved = shiftRight(letterData, saved);
    if(wait(400))
      break;
  }
}

void handleSignRider()
{
  int index;
  int repeat;
  configLED(PWM_MAX, 600, TRUE);

  styleRedColor(letterData);
  letterData[0] = GREEN_MASK;
  for(repeat = 0; repeat < 5; repeat++)
  {
    for(index = 0; index < 26; index++)
    {
      if(wait(80))
        break;
            
      if(index < 13 )
        rotateRight(letterData);
      else
        rotateLeft(letterData);
    }
  }
}

void handleRandomChars()
{
  int index;
  int colour;
  int count;
  int pos[LETTER_MAX];

  configLED(PWM_MAX, 1000, FALSE);
  basicOff(letterData);
  randomizeArray(pos, LETTER_MAX);
          
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
        return;
    }
  }
          
  for(index = 0; index < LETTER_MAX; index++)
  {   
    letterData[pos[index]] = 0;
    if(wait(300))
      return;
  }
}

void handleSolidAlternate()
{
  int style;
  configLED(PWM_MAX, 3500, FALSE);
       
  for(style = 0; style < SOLID_MAX; style++)
  {
    (*styleFuncListSolid[style])(letterData);
    if(wait(5000))
      break;
  }
}

void handleSignBreathe()
{
  int style;
  int loop;

  configLED(PWM_MAX, 150, FALSE);
  basicOff(letterData);

  for(loop = 1; loop < 8; loop++)
  {
    letterData[6] = loop;
    letterData[7] = loop;
        
    for(style = 0; style < 13; style++)
    {
      if(wait(200))
        return;
            
      if(style < (6))
        breatheOut(letterData);
      else
        breatheIn(letterData);
    }

    basicOff(letterData);
    wait(200);
  }
}

void loop()
{
  if(signActive)
  {
    switch(signState)
    {
      case SIGN_SELECT:
        handleSelectMenu();
        break;

      case SIGN_BASIC:
        handleSignBasic();
        break;

      case SIGN_CRAZY:
        handleSignCrazy();
        break;

      case SIGN_COLOUR_CHASE:
        handleSignColourChase();
        break;

      case SIGN_KNIGHT:
        handleSignRider();
        break;

      case SIGN_RANDOM_CHARS:
        handleRandomChars();
        break;
                        
      case SIGN_TEST_RED:
        configLED(PWM_MAX, 0, TRUE);
        styleRedColor(letterData);
        wait(100);
        break;

      case SIGN_TEST_GREEN:
        configLED(PWM_MAX, 0, TRUE);      
        styleGreenColor(letterData);
        wait(100);
        break;

      case SIGN_TEST_BLUE:
        configLED(PWM_MAX, 0, TRUE);      
        styleBlueColor(letterData);
        wait(100);
        break;

      case SIGN_TEST_WHITE:
        configLED(PWM_MAX, 0, TRUE);      
        styleWhiteColor(letterData);
        wait(100);
        break;
      
      case SIGN_SOLID_ALTERNATE:
        handleSolidAlternate();
        break;
      
      case SIGN_BREATHE:
        handleSignBreathe();
        break;
        
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
