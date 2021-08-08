// CPS2 Desuicider 1.06 by arcadehacker.blogspot.com

// Changes
// Version 1.00: [Eduardo] Initial code.
// Version 1.01: [Artemio] Moved strings and keys to flash memory.
// Version 1.02: [Artemio] Removed spaces from game names, refactored code a bit, added left and right menu navigation.
// Version 1.03: [Artemio] Changed startup and end values in lines for consistent results on boards without CN9.
// Version 1.04: [Eduardo] Removed clones. Revised clock delay for improved compatibility.
// Version 1.05: [Eduardo] Fixed keys for batcirj, spf2t. Added games: ringdesth, spf2tu, ssf2th. Thank you smf.
// Version 1.06: [Eduardo] Added back clones to avoid user confusion. Added ssf2tba, xmcotab. Thanks Brian & smf.
//               [Artemio] Modified clock signal order for improved compatibility with board revs 4 & 5
// Version 1.07: [Eduardo] Fixed keys for batcirj, spf2t, previously broken again in 1.06. Thanks Konosuke.

#include <LiquidCrystal.h>

#include "game-data.h"
#include "cps2.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


int lcd_key       = btnNONE;
int adc_key_in    = 0;
int adc_key_prev  = 0;

// delay amount
int time = CLOCK_DELAY_TIME;

char buffer[20];

unsigned char mask = 1; // Bit
unsigned char bits[8];
int c = -1;


void DisplayIntro()
{
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("CPS2 Desuicide");
  lcd.setCursor(0,1);
  lcd.print("By ArcadeHacker");
  delay(2500);
}

void DisplayMenu()
{
  lcd.setCursor(0,0);
  lcd.print("Select a game:  ");
  lcd.setCursor(0,1);
  lcd.print(" --press down-- ");
}

void setup() {
  PrepareOutput();
  
  DisplayIntro();
  DisplayMenu();
}


void ProgramCPS2(int prg)
{
  int i;
  int test;
  unsigned char cByte;

  
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Unlocking...    ");                  
  
  ProgramUnlock();      
  
  lcd.setCursor(0,0);   
  lcd.print("Programming     ");
  lcd.setCursor(0,1);
  lcd.print("CPS2...        %");    
  
  //Serial.begin(9600);
  
  for(i=0; i<20; i++)        
  {   
    cByte =  pgm_read_byte_near(Keys[prg]+i);
    //Serial.print(cByte, HEX);
    //Serial.print(' ');

    lcd.setCursor(13,1);
    lcd.print(i*5, DEC);  
    
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
  
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Done!           ");                   
  delay(1000);       
  
  
  //while (analogRead (0) != 638) { 
    lcd.setCursor(0,0); lcd.print("--disconnect and ");   lcd.setCursor(0,1); lcd.print("test pcb--"); 
  //}
  delay(2000); 
}

int read_LCD_buttons()
{
  adc_key_in = analogRead(0);   
  delay(5);
  int k = (analogRead(0) - adc_key_in); 
  if (5 < abs(k)) return btnNONE;  
  
  if (adc_key_in > 1000) return btnNONE; 
  if (adc_key_in < 50)   return btnRIGHT;  
  if (adc_key_in < 195)  return btnUP; 
  if (adc_key_in < 380)  return btnDOWN; 
  if (adc_key_in < 555)  return btnLEFT; 
  if (adc_key_in < 790)  return btnSELECT;   
  
  return btnNONE;  
}     

void DisplayName(int game)
{
  int len = 0, i = 0;

  if(c == -1)
    return;
  lcd.setCursor(0,1);            // move to the begining of the second line
  strcpy_P(buffer, (char*)pgm_read_word(&(GameList[c]))); 
  len = strlen(buffer);
  for(i = len; i < 16; i++)
    buffer[i] = ' ';
  buffer[i] = '\0';
  lcd.print(buffer);
}

int FindNextIndex(int pos, int dir)
{
  int TempIndex = pos, found = -1;
  int comp = 4; // 3 and 4 work well, both with disadvantages
  int listsize =  sizeof(GameList)/sizeof(GameList[0])-1;

  if(pos == -1)
    return 0;
  
  strncpy_P(buffer, (char*)pgm_read_word(&(GameList[pos])), comp);
  if(strlen(buffer) == 3) // Adjust for msh
    comp = 3;
  do
  {
    if(dir)
    {
      if(TempIndex < listsize)
        TempIndex++;
    }
    else
    {
      if(TempIndex > 0)
        TempIndex--;
    }
  
    found = strncmp_P(buffer, (char*)pgm_read_word(&(GameList[TempIndex])), comp);
  }while(found == 0 && TempIndex < listsize && TempIndex != 0);

  return TempIndex;
}

void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
  int listsize =  sizeof(GameList)/sizeof(GameList[0])-1;
  adc_key_prev = lcd_key ;       // Looking for changes
  lcd_key = read_LCD_buttons();  // read the buttons
  
  if (adc_key_prev != lcd_key)
  { 
    switch (lcd_key)               // depending on which button was pushed, we perform an action
    {
      case btnRIGHT:
      {
        /*
        if (c+10 < listsize)
          c += 10;
        else
          c = listsize;
        */
        c = FindNextIndex(c, 1);
        break;
      }
      
      case btnLEFT:
      {
        /*
        if (c > 10)
          c -= 10;
        else
          c = 0;    
        */ 
        c = FindNextIndex(FindNextIndex(c, 0), 0); // Calling it once gets the last item.
        if(c) c++; // Get First Item in category
        break;
      }
      
      case btnUP:
      {
        if (c !=0)
        {
          if (c > 0) {c--;} else {c++;}
        }
        break;
      }
      
      case btnDOWN:
      {
        if (c != listsize) 
          {c++;}
        break;
      }
      
      case btnSELECT:
      {
        if (c != -1) 
        {
          ProgramCPS2(c);  

          lcd_key = btnNONE;
          do
          {
            lcd_key = read_LCD_buttons();  // read the buttons
          }while(lcd_key == btnNONE);
          
          PrepareOutput();
          DisplayMenu();
        }
        break;
      }
    }
    DisplayName(c);
  }
}
