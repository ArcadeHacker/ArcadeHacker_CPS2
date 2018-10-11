// Forked from:
// CPS2 Desuicider 1.06 by arcadehacker.blogspot.com, https://github.com/ArcadeHacker/ArcadeHacker_CPS2


//  CPS2 Board CN9 interface pins
#define DATA        2   //CN9 #2 
#define SETUP1      3   //CN9 #3 
#define CLOCK       11  //CN9 #4
#define SETUP2      12  //CN9 #5

//  CPS2 Board without CN9, CN2 interface pins
//  ------------------------------------------
//  DATA    ->  CN2 A32
//  SETUP1  ->  CN2 A30
//  CLOCK   ->  CN2 A31
//  SETUP2  ->  CN2 A29

//Replace "0x0F,0x00,0x02,0x40,0x00,0x08,0x04,0xC1,0x3A,0x62,0x03,0x03,0x4B,0xDA,0x00,0x66,0xCD,0xFA,0x95,0x30" with the decryption key for your game
const unsigned char Key[] PROGMEM =    { 0x0F,0x00,0x02,0x40,0x00,0x08,0x04,0xC1,0x3A,0x62,0x03,0x03,0x4B,0xDA,0x00,0x66,0xCD,0xFA,0x95,0x30}; //xmcotau

// delay amount
//int time = 25; //original delay, but takes around 9 sec to boot
// unsure if the shorter delay may cause compatibility issues on some board revisions
int time = 1;

char buffer[20];  

unsigned char mask = 1; // Bit 
unsigned char bits[8];

void PrepareOutput()
{
  pinMode(SETUP1, OUTPUT); pinMode(SETUP2, OUTPUT);
  pinMode(CLOCK, OUTPUT); pinMode(DATA, OUTPUT); 

  digitalWrite(SETUP1, LOW);
  digitalWrite(CLOCK, LOW);
  digitalWrite(DATA, LOW);
  digitalWrite(SETUP2, HIGH);
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
}

void setup() 
{
  PrepareOutput();
  ProgramCPS2();
  PrepareOutput();
}

void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
  //No need for loop() to do anything
  //Only needs setup() to run at boot
}
