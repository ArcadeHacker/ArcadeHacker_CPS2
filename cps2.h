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
#define CLOCK_DELAY_TIME 25
#define PROGRAM_UNLOCK_DELAY_TIME 100


void PrepareOutput()
{
    pinMode(SETUP1, OUTPUT);
    pinMode(SETUP2, OUTPUT);
    pinMode(CLOCK, OUTPUT);
    pinMode(DATA, OUTPUT);

    digitalWrite(SETUP1, LOW);
    digitalWrite(CLOCK, LOW);
    digitalWrite(DATA, LOW);
    digitalWrite(SETUP2, HIGH);
}

void CLK()
{
    digitalWrite(CLOCK, HIGH);
    delay(CLOCK_DELAY_TIME);

    digitalWrite(CLOCK, LOW);
    delay(CLOCK_DELAY_TIME);
}


void ProgramUnlock()
{
    digitalWrite(SETUP1, HIGH);
    digitalWrite(SETUP2, LOW);
    delay(PROGRAM_UNLOCK_DELAY_TIME);
}
