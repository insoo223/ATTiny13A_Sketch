/**************************************************************
  Target MCU & internal clock speed: ATtiny13 @ 9.6Mhz
  Name    : YutDiceTempLM35_74HC595_AllinOne_ATtiny13.ino
  Author  : Insoo Kim
  Date    : March 26, 2015    

  Description: 
    1.To measure temperature 
       Press and hold the button while powering up,
       realease the button after any led lits up. 
    2. To play dice (Default)      
      Don't press the button while powering up.
    3.To play Yut
       Can be played but not yet implemented to manipulate by end-user.
       Only set by Sketch

*****************************************************************/  

//********* Mode of functions
#define TEMP 0
#define YUT 1
#define DICE 2

//********* Temperature
//#define debugSerial
//#define debugIntOnly
//#define TargetBdUNO

//USB tiny programmer
//#define RefVtg 5030

//mV resolution is required to catch more precise temperature.
//so that, i measured power supply with mini digital meter.
//for 5 & 12V DC pwr supplier
#define RefVtg 5000

//for LG Dacom 070 handset
//#define RefVtg 4080 

//Pin connected to ST_CP of 74HC595
byte latchPin = 0;
//Pin connected to SH_CP of 74HC595
byte clockPin = 3;
//Pin connected to DS of 74HC595
byte dataPin = 2;
  
//switch pin
byte SWpin = 1;

//********* Yut & Dice
// #define halfSecond  500*4
#define halfSecond 500

#define maxCntDice 7 //255
#define maxCntYut 1 //17

byte cnt=0;
byte mode=0;
byte curState;

byte YutLED[6] = {
		0b00000000, //0 Back Doh
		0b00001000, //1 Doh
		0b10000100, //2 Gae
		0b00111000, //3 Girl
		0b11000110, //4 Yut
		0b11001110 // 5 Mo
	};//YutLED[6]
byte DiceLED[7] = {
		0b00000000, //0 Blank
		0b00001000, //1 
		//0b10000100, //2 
    0b00110000, //2 
		0b01001010, //3 
		0b11000110, //4 
		0b11001110, // 5 
		0b11110110 //6 
	};//DiceLED[7]

//----------function prototypes
void chkTemperature();
void showTempDiceLED(byte );
void countUpTo(byte );
void playDice();
void playYut();
void dispYutDice(byte , byte *);

//---------------------------------
//this sets the ground pin to LOW and the input voltage pin to high
void setup()
{
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(SWpin, INPUT);
  
  //Switch Open (default)
  if (digitalRead(SWpin) == HIGH)
    mode = DICE;
    //mode = TEMP;
    
  //switch closed (need to push button)
  else
    //mode = TEMP;
    mode = YUT;
    //mode = DICE;

  //analogReference(EXTERNAL);
}//setup

//---------------------------------
void loop()
{
  curState = digitalRead(SWpin);

  switch (mode)
  {
    case TEMP:
      chkTemperature();
      break;

    case DICE:
      playDice();
      break;
      
    case YUT:
      playYut();  
      break;
  }//switch
  
}//loop

//---------------------------------
void chkTemperature()
{
  int rawV;
  int C;

  // pin3 (PB4, ADC2)
  // if connected to pin1 (PB5, RESET), you'll be in trouble 
  rawV = analogRead(2);
  //C = rawV * 0.48828125;
  // To fit into 1K program space of ATtiny13
  // we sacrifice some precision by using * 1/2 in stead of * 0.48828125.
  // Experiments show its measurement of temperature is +/- 1 Celcius degree
  C = rawV >> 1;
  showTempDiceLED(C);
  delay(halfSecond*2); // 1 seconds
  showTempDiceLED(0);
  delay(halfSecond*4); // 2 seconds

  chkButton();
  
}//chkTemperature

//---------------------------------
void chkButton()
{
}

//---------------------------------
void showTempDiceLED(byte num)
{
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(latchPin, LOW);
  
  // shift out the bits:
  //Q0 of 74HC595 is not used in my dice circuit, so that shift left by one bit 
  shiftOut(dataPin, clockPin, MSBFIRST, num << 1);
  
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}//showTempDiceLED

//---------------------------------
void playDice()
{
  if (curState == LOW)
    countUpTo(maxCntDice);

  if (curState == LOW)
  {
    dispYutDice(cnt%6, DiceLED);
    //delay(100);
  }
  else
  {
    dispYutDice(cnt%7, DiceLED);
  }
  delay(5);
}//playDice

//---------------------------------
void playYut()
{

  if (curState == LOW)
    countUpTo(maxCntYut);

  if (curState == LOW)
  {
    dispYutDice(cnt%6, YutLED);
    //delay(10);
  }
  else
  {
      switch (cnt % maxCntYut)
      {
        case 0: //back Doh
        case 14: //back Doh
          dispYutDice(0, YutLED);
          
          break;
        case 1: // Doh
        case 12: // Doh
        case 15: // Doh
          dispYutDice(1, YutLED);
          break;
        case 2: // Gae
        case 6: // Gae
        case 9: // Gae
        case 11: // Gae
        case 13: // Gae
        case 16: // Gae
          dispYutDice(2, YutLED);
          break;
        case 3: // Girl
        case 7: // Girl
        case 10: // Girl
          dispYutDice(3, YutLED);
          break;
        case 4: // Yout
        case 8: // Yout
          dispYutDice(4, YutLED);
          break; 
        case 5: // Mo
          dispYutDice(5, YutLED);
          break;
      }//switch
      //delay(halfSecond);
  }//else
  delay(10);
}//playYut

//---------------------------
void   countUpTo(byte maxNum)
{  
  if (cnt < maxNum)
    cnt++;
  else
    cnt = 0;
    
}//countUpTo

//---------------------------
void dispYutDice(byte n, byte *arr)
{
  digitalWrite(latchPin, LOW);

  // shift out the bits:
  shiftOut(dataPin, clockPin, MSBFIRST, arr[n]);
  
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}//disp        


