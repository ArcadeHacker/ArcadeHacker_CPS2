// Forked from:
// CPS2 Desuicider 1.06 by arcadehacker.blogspot.com, https://github.com/ArcadeHacker/ArcadeHacker_CPS2

// Power is recommended to go to +5v pin on the Arduino, CN9 pin 1
//  VIN goes to the voltage regulator which will (probably) cause a voltage drop when feeding it 5V
// GND goes to GND, CN pin 6


//  CPS2 Board CN9 interface pins
#define DATA        2   //CN9 #2 
#define SETUP1      3   //CN9 #3 
#define CLOCK       11  //CN9 #4
#define SETUP2      12  //CN9 #5

//  CPS2 Board without CN9, CN2 interface pins
//  ------------------------------------------
//  (Arduino 2)  DATA    ->  CN2 A32
//  (Arduino 11) CLOCK   ->  CN2 A31
//  (Arduino 3)  SETUP1  ->  CN2 A30
//  (Arduino 12) SETUP2  ->  CN2 A29

//Replace "0x0F,0x00,0x02,0x40,0x00,0x08,0x04,0xC1,0x3A,0x62,0x03,0x03,0x4B,0xDA,0x00,0x66,0xCD,0xFA,0x95,0x30" with the decryption key for your game
const unsigned char Key[] PROGMEM =    { 0x0F,0x00,0x02,0x40,0x00,0x08,0x70,0x43,0x80,0x00,0x00,0x10,0xE4,0x63,0x15,0xFE,0xEE,0xE5,0xB1,0x68}; //hsf2

// delay amount
//int time = 25; //original delay, but takes around 9 sec to boot
// unsure if the shorter delay may cause compatibility issues on some board revisions
int time = 1;

char buffer[20];  

unsigned char mask = 1; // Bit 
unsigned char bits[8];

void PrepareOutput()
{
  pinMode(SETUP1, OUTPUT);
  pinMode(SETUP2, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT); 

  digitalWrite(CLOCK, LOW);
  digitalWrite(DATA, LOW);
  digitalWrite(SETUP1, LOW);
  digitalWrite(SETUP2, HIGH);
}

void ReleasePins()
{
  // move them to high-impedance state
  pinMode(SETUP1, INPUT);
  pinMode(SETUP2, INPUT);
  pinMode(CLOCK, INPUT);
  pinMode(DATA, INPUT); 
}

void CLK()
{
  digitalWrite(CLOCK, HIGH); 
  delay(time);
  digitalWrite(CLOCK, LOW); 
  delay(time);  
}

void program_unlock()
{
    digitalWrite(SETUP1, HIGH);
    digitalWrite(SETUP2, LOW);
    delay(time); delay(time); delay(time); delay(time); 
} 

void ProgramCPS2()
{
  int i;
  int test;
  unsigned char cByte;            
  
  program_unlock();      
  
  //Serial.begin(9600);
  
  for(i=0; i<20; i++)        
  {   
    cByte =  pgm_read_byte_near(Key+i);
    //Serial.print(cByte, HEX);
    //Serial.print(' ');

    for (int b = 7; b > -1; b--) {  
      bits[b] = (cByte & (mask << b)) != 0;
      //printf("%d",bits[b]);
    
      if (bits[b] == 1) 
      { digitalWrite(DATA, HIGH); } 
      else 
      { digitalWrite(DATA, LOW);  }
      CLK(); 
    }
  }

  digitalWrite(SETUP1, LOW);
  digitalWrite(SETUP2, HIGH); 
  delay(time);
  digitalWrite(CLOCK, LOW);
  digitalWrite(DATA, LOW);  // reset to default
}

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  PrepareOutput();
  ProgramCPS2();
  ReleasePins();
  
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
  //No need for loop() to do anything
  //Only needs setup() to run at boot
}
