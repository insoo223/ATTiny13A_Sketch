/**************************************************************
  Target MCU & internal clock speed: ATtiny13 @ 9.6Mhz
  Name    : TempYut_LM35_74HC595_ATtiny13.ino
  Author  : Insoo Kim
  Date    : March 26, 2015    

  Description: 
    1.To play Yut
       Press and hold the button while powering up,
       realease the button after any led lits up. 
    2. To measure temperature (Default)
      Don't press the button while powering up.
    3.To play dice       
       No dice play available

*****************************************************************/  

//********* Mode of functions
#define TEMP 0
#define YUT 1

//********* Temperature
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


//********* Yut
#define halfSecond  500*4

#define maxCntYut 17

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

//----------function prototypes
void chkTemperature();
byte lookupTable (int );
void showTempDiceLED(byte );
void countUpTo(byte );
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
  
  if (digitalRead(SWpin) == HIGH)
    mode = TEMP;
  else
    mode = YUT;

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
  
  //C = rawV >> 1;
  C = lookupTable(rawV >> 1);
  
  showTempDiceLED(C);
  delay(halfSecond*2); // 1 seconds

/*
  showTempDiceLED(rawV/10);
  delay(halfSecond*2); // 1 seconds
  showTempDiceLED(rawV%10);
  delay(halfSecond*2); // 1 seconds
*/

  showTempDiceLED(0);
  delay(halfSecond*8); // 2 seconds
  
}//chkTemperature

//---------------------------------
byte lookupTable (int tmp)
{
  if (tmp <= 24) 
    return tmp - 1;  
  else if (tmp <= 34) 
    return tmp - 2;  
  else if (tmp <= 35) 
    return tmp - 3;  
  else if (tmp <= 42)
   if (tmp % 2 == 0) 
    return tmp - 2;  
   else
    return tmp - 3;  
  else if (tmp <= 51) 
    return tmp - 3;  
  else if (tmp <= 59)
   if (tmp % 2 == 0) 
    return tmp - 3;  
   else
    return tmp - 4;  
  else if (tmp <= 68) 
    return tmp - 4;  
}//lookupTable

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
void playYut()
{
  if (curState == LOW)
    countUpTo(maxCntYut);

  if (curState == LOW)
  {
    dispYutDice(cnt%6, YutLED);
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
  
  delay(halfSecond>>4);
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


