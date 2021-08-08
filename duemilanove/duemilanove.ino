// CPS2 Desuicider for Arduino starter kits by davvid.
// Based on CPS2 Desuicider 1.06 by arcadehacker.blogspot.com

#include <ezButton.h>
#include <ezOutput.h>

#include "game-data.h"
#include "cps2.h"

// The main button initiates the reprogramming procedure.
// Wire a button to Digital pin 7,
#define PIN_BUTTON 7

// An LED is used to display status.
// Wire an LED to Digital pin 8.
#define PIN_LED 8

// Short presses are button presses that held for less than SHORT_PRESS_TIME.
#define BUTTON_SHORT_PRESS_TIME 3000  // milliseconds

// Arduino Duemilanove uses SerialUSB. Changes this to Serial for others.
#define IO Serial

// Arduino board components.
struct board_t {
    ezButton button;
    ezOutput led;
};

// Main data.
struct data_t {
    bool button_pressed;
    int button_pressed_time;
    int num_games;
    int current_game_idx;
};

struct data_t data = {
    false,  // button_pressed
    0,      // button_pressed_time
    0,      // number of games
    0,      // current_game_idx
};

struct board_t board = {
    // Inputs
    ezButton(PIN_BUTTON),
    // Outputs
    ezOutput(PIN_LED),
};

void setup()
{
    board.button.setDebounceTime(50);
    data.num_games = sizeof(GameList) / sizeof(GameList[0]);
    data.current_game_idx = 111;  // mvscj

    PrepareOutput();
    IO.begin(9600);
    while (!IO) {
        // Wait for the USB serial port to become ready.
        ;
    }
    PrintIntro();
}

void PrintIntro()
{
    IO.println("            CPS2 Desuicide               ");
    IO.println("        By ArcadeHacker and davvid       ");
    IO.println("");
    IO.println("Tap the button briefly to go to the next game.");
    IO.println("Press and hold the button for 3 seconds to select and start.");
    IO.println("");
    IO.println("The LED blinks slowly when initially pressed.");
    IO.println("The LED blinks quickly once 3 seconds have passed.");
    IO.println("");

    IO.print(data.num_games);
    IO.println(" games loaded.");
    IO.println("");

    PrintCurrentGame();
}

void PrintCurrentGame()
{
    // Copy program memory to a local buffer.
    char buf[20] = {0};
    int idx = data.current_game_idx;
    strcpy_P(buf, (char*)pgm_read_word(&(GameList[idx])));

    IO.print("#");
    IO.print(idx);
    IO.print(" - ");
    IO.write((const char*)buf, sizeof(buf));
    IO.println("");
}

void ProgramCPS2(int prg)
{
    unsigned char bits[8] = {0};
    unsigned char mask = 1; // Bit
    unsigned char value;

    IO.println("");
    IO.println("Unlocking...");
    ProgramUnlock();

    IO.println("");
    IO.println("Programming CPS2...");

    for (int i = 0; i < 20; ++i) {
        value =  pgm_read_byte_near(Keys[prg + i]);

        IO.print(i * 5);
        IO.print("% ");

        for (int b = 7; b > -1; --b) {
            bits[b] = (value & (mask << b)) != 0;

            if (bits[b] == 1) {
                digitalWrite(DATA, HIGH);
            } else {
                digitalWrite(DATA, LOW);
            }
            CLK();
        }
    }

    digitalWrite(SETUP1, LOW);
    digitalWrite(SETUP2, HIGH);

    delay(CLOCK_DELAY_TIME);
    digitalWrite(CLOCK, LOW);

    IO.println("100%");
    delay(1000);

    IO.println("");
    IO.println("Done! Disconnect the Arduino and test the PCB.");
    IO.println("");
    delay(2000);
}

void loop()
{
    board.button.loop();
    board.led.loop();

    bool pressed = board.button.isPressed();
    bool released = board.button.isReleased();
    int current_time = millis();

    if (pressed) {
        // The button was just pressed.
        data.button_pressed = true;
        data.button_pressed_time = current_time;
        // Initiate a slow blink. We will increment the current game index to
        // the next entry upon release.
        board.led.blink(500, 500);

    } else if (released) {
        // The button was just released.
        board.led.low();
        data.button_pressed = false;

        // If the button press was short then we skip forward to the next game.
        int elapsed_time = current_time - data.button_pressed_time;
        if (elapsed_time <= BUTTON_SHORT_PRESS_TIME) {
            int next_idx = (data.current_game_idx + 1) % data.num_games;
            data.current_game_idx = next_idx;

        } else {
            ProgramCPS2(data.current_game_idx);
        }

        PrintCurrentGame();

    } else if (data.button_pressed) {
        // The button is pressed but the state has not changed.
        // If the button is pressed then start blinking quickly once we've
        // surpassed the BUTTON_SHORT_PRESS_TIME.
        int elapsed_time = current_time - data.button_pressed_time;
        if (elapsed_time > BUTTON_SHORT_PRESS_TIME) {
            // Fast blink -- ready to program once released.
            board.led.blink(125, 125);
        }
    }
}
