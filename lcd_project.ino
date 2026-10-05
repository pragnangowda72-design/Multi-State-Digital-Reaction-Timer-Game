#include <LiquidCrystal.h>

// =====================================================
// LCD CONNECTIONS
// RS  -> Arduino D12
// E   -> Arduino D11
// D4  -> Arduino D5
// D5  -> Arduino D4
// D6  -> Arduino D3
// D7  -> Arduino D2
// =====================================================

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


// =====================================================
// PUSH BUTTONS
// =====================================================

const int START_BUTTON    = 7;
const int REACTION_BUTTON = 8;
const int RESET_BUTTON    = 9;


// =====================================================
// RGB LED
// Your RGB LED:
// RED    -> D10
// COMMON -> GND
// BLUE   -> A1
// GREEN  -> A0
// =====================================================

const int RED_LED   = 10;
const int GREEN_LED = A0;
const int BLUE_LED  = A1;


// =====================================================
// GAME VARIABLES
// =====================================================

unsigned long reactionStartTime = 0;
unsigned long reactionTime = 0;
unsigned long bestTime = 0;

int roundNumber = 0;


// =====================================================
// GAME STATES
// =====================================================

enum GameState
{
  READY,
  WAITING,
  REACTION,
  RESULT
};

GameState state = READY;


// =====================================================
// RGB LED FUNCTIONS
// =====================================================

void red()
{
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
}

void green()
{
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, LOW);
}

void blue()
{
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, HIGH);
}

void off()
{
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
}


// =====================================================
// READY SCREEN
// =====================================================

void showReady()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("REACTION GAME");

  lcd.setCursor(0, 1);
  lcd.print("Press START");

  green();
}


// =====================================================
// WAIT FOR BUTTON RELEASE
// =====================================================

void waitForRelease(int buttonPin)
{
  while (digitalRead(buttonPin) == LOW)
  {
    delay(10);
  }
}


// =====================================================
// SETUP
// =====================================================

void setup()
{
  // Buttons
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(REACTION_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  // RGB LED
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  // LCD
  lcd.begin(16, 2);

  // Random seed
  randomSeed(analogRead(A5));

  // Initial screen
  showReady();
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{

  // ===================================================
  // RESET BUTTON
  // ===================================================

  if (digitalRead(RESET_BUTTON) == LOW)
  {
    roundNumber = 0;
    bestTime = 0;

    waitForRelease(RESET_BUTTON);

    state = READY;

    showReady();

    delay(300);

    return;
  }


  // ===================================================
  // READY STATE
  // ===================================================

  if (state == READY)
  {
    if (digitalRead(START_BUTTON) == LOW)
    {
      waitForRelease(START_BUTTON);

      roundNumber++;

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("WAIT...");

      lcd.setCursor(0, 1);
      lcd.print("Don't press!");

      blue();

      state = WAITING;


      // Random delay between 2 and 5 seconds
      unsigned long waitingTime = random(2000, 5001);

      unsigned long waitStart = millis();


      // =================================================
      // WAITING STATE
      // =================================================

      while (millis() - waitStart < waitingTime)
      {

        // -----------------------------------------------
        // Player pressed too early
        // -----------------------------------------------

        if (digitalRead(REACTION_BUTTON) == LOW)
        {
          waitForRelease(REACTION_BUTTON);

          lcd.clear();

          lcd.setCursor(0, 0);
          lcd.print("TOO EARLY!");

          lcd.setCursor(0, 1);
          lcd.print("Try again");

          red();

          delay(2000);

          state = READY;

          showReady();

          return;
        }


        // -----------------------------------------------
        // Reset during waiting
        // -----------------------------------------------

        if (digitalRead(RESET_BUTTON) == LOW)
        {
          roundNumber = 0;
          bestTime = 0;

          waitForRelease(RESET_BUTTON);

          state = READY;

          showReady();

          return;
        }
      }


      // =================================================
      // REACTION STATE
      // =================================================

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("GO!!!");

      lcd.setCursor(0, 1);
      lcd.print("PRESS NOW");

      red();

      // Start timing when GO appears
      reactionStartTime = millis();

      state = REACTION;


      // =================================================
      // WAIT FOR REACTION BUTTON
      // =================================================

      while (digitalRead(REACTION_BUTTON) == HIGH)
      {

        // Reset during reaction
        if (digitalRead(RESET_BUTTON) == LOW)
        {
          roundNumber = 0;
          bestTime = 0;

          waitForRelease(RESET_BUTTON);

          state = READY;

          showReady();

          return;
        }
      }


      // =================================================
      // CALCULATE REACTION TIME
      // =================================================

      reactionTime = millis() - reactionStartTime;

      waitForRelease(REACTION_BUTTON);


      // =================================================
      // RESULT
      // =================================================

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Reaction:");

      lcd.setCursor(10, 0);
      lcd.print(reactionTime);
      lcd.print("ms");


      // Update best time
      if (bestTime == 0 || reactionTime < bestTime)
      {
        bestTime = reactionTime;
      }


      lcd.setCursor(0, 1);
      lcd.print("Best:");
      lcd.print(bestTime);
      lcd.print("ms");

      green();

      state = RESULT;

      delay(3000);


      // =================================================
      // NEXT ROUND
      // =================================================

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Round ");
      lcd.print(roundNumber + 1);

      lcd.setCursor(0, 1);
      lcd.print("Press START");

      green();

      state = READY;
    }
  }
}
