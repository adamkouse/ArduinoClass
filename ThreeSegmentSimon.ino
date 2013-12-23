/*
  Three Segment Simon
  
  Created by Adam Kouse
  October, 2013
  
  A version of Simon by Milton Bradley with three segments instead of four. I made
  it three segments because I could build this project completely from parts
  that were available from the $9 Arduino kit by Borderless Electronics.
  http://www.indiegogo.com/projects/9-arduino-compatible-starter-kit-anyone-can-learn-electronics

  This code is in the public domain.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/


/**
  Includes
  These statements add extra functionality by including extra code into the sketch.
*/
#include <NewTone.h>

/**
  Constants
  Constants never change value. By putting const in front of each variable, we're
  telling the compiler that if we ever try to change their value to give us an error.
  Here we're using them to give a name to a pin number which is easier for us to
  remember. They also allow us to change pin numbers much easier in the future.
*/

/* Color Definitions */
const unsigned int red    = 0;
const unsigned int green  = 1;
const unsigned int yellow = 2;

/* Switches */
const unsigned int redSwitch    = 2;
const unsigned int greenSwitch  = 3;
const unsigned int yellowSwitch = 4;

/* LEDs */
const unsigned int redLed    = 8;
const unsigned int greenLed  = 9;
const unsigned int yellowLed = 10;

/* Buzzer */
const unsigned int buzzer = 12;

/* Tones, in Hertz unless otherwise noted */
const unsigned int redTone    = 95;
const unsigned int greenTone  = 82;
const unsigned int yellowTone = 69;


/* Delays, in milliseconds unless otherwise noted */
const unsigned int toneDuration = 400;
const unsigned int tonePause    = 50;

/* Number of seconds between successive button presses before a player loses */
const unsigned int moveTimeout  = 5000;

const unsigned int winActions = 6; // Number of actions to win the game.


/** Functions */
/*
  Do a sequence of LEDs and tones when the player loses.
  There always has to be a delay(toneDuration) after a NewTone() becuase tone
  uses a timer to play the notes which doesn't stop the CPU from executing
  instructions.
*/
void playLoseSequence(void)
{
  digitalWrite(redLed, HIGH);
  NewTone(buzzer, 360, toneDuration);
  delay(toneDuration);
  digitalWrite(redLed, LOW);
  delay(tonePause);
  digitalWrite(greenLed, HIGH);
  NewTone(buzzer, 330, toneDuration);
  delay(toneDuration);
  digitalWrite(greenLed, LOW);
  delay(tonePause);
  digitalWrite(yellowLed, HIGH);
  NewTone(buzzer, 300, toneDuration*2);
  delay(toneDuration*2);
  digitalWrite(yellowLed, LOW);
  
  return;
}

/*
  Do a sequence of LEDs and tones when the player wins by completing a sequence
  of winActions actions.
*/
void playWinSequence(void)
{
  int led;
  int sound = 400;

  digitalWrite(greenLed, HIGH);

  for (int i = 0; i < 5; ++i) {
    if (i % 2) {
      led = yellowLed;
    } else {
      led = redLed;
    }

    digitalWrite(led, HIGH);
    NewTone(buzzer, sound, toneDuration);
    delay(toneDuration);
    digitalWrite(led, LOW);
    delay(tonePause);
    sound += 20;
  }

  delay(toneDuration/2);
  for (int i = 0; i < 2; ++i) {
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, HIGH);
    NewTone(buzzer, 480, toneDuration/2);
    delay(toneDuration/2);
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    if (i) {
      digitalWrite(greenLed, LOW);
    }
    delay(toneDuration/2);  
  }

  return;
}

/*
  Display an action that consists of toggling an LED and playing its associated tone.
  0 == Red
  1 == Green
  Anything else == Yellow
*/
showAction(unsigned int a)
{
  int led, sound;

 switch (a) {
    case red:
      led = redLed;
      sound = redTone;
      break;
    case green:
      led = greenLed;
      sound = greenTone;
      break;
    default:
      led = yellowLed;
      sound = yellowTone;
  }

  digitalWrite(led, HIGH);
  NewTone(buzzer, sound, toneDuration);
  delay(toneDuration);
  digitalWrite(led, LOW);
  delay(tonePause);
  
  return;
}

/*
  Display the current set of actions up to currentAction, which is the hightest action that
  the player has successfully gotten to.
*/  
void showActions(const int actions[], unsigned int currentAction)
{
  if (currentAction > winActions) {
    currentAction = winActions;
  }

  for (int i = 0; i <= currentAction; ++i) {
    showAction(actions[i]);
  }
  
  return;
}

/*
  Get the player's move.
  Returns:
    The switch number that the player has pressed. If no switch is pressed, it returns -1.
*/
int playerMove(void)
{
  int redSwitchState, greenSwitchState, yellowSwitchState;
  int lastRedSwitchState = HIGH, lastGreenSwitchState = HIGH, lastYellowSwitchState = HIGH;
  int debounceDelay = 10;
  
  redSwitchState = digitalRead(redSwitch);
  greenSwitchState = digitalRead(greenSwitch);
  yellowSwitchState = digitalRead(yellowSwitch);
  
  if (redSwitchState != lastRedSwitchState) {
    // The red switch may have been pressed
    lastRedSwitchState = redSwitchState;
    delay(debounceDelay);
    redSwitchState = digitalRead(redSwitch);
    if (redSwitchState == lastRedSwitchState) {
      return red;
    }
  }
  
  if (greenSwitchState != lastGreenSwitchState) {
    // The green switch may have been pressed
    lastGreenSwitchState = greenSwitchState;
    delay(debounceDelay);
    greenSwitchState = digitalRead(greenSwitch);
    if (greenSwitchState == lastGreenSwitchState) {
      return green;
    }
  }
  
  if (yellowSwitchState != lastYellowSwitchState) {
    // The yellow switch may have been pressed
    lastYellowSwitchState = yellowSwitchState;
    delay(debounceDelay);
    yellowSwitchState = digitalRead(yellowSwitch);
    if (yellowSwitchState == lastYellowSwitchState) {
      return yellow;
    }
  }

  // No button presses detected  
  return -1;
}

/*
*/
int roundSuccessful(const int actions[], unsigned int currentAction)
{
  unsigned int timer, playerMove;
  
  if (currentAction > winActions) {
    currentAction = winActions;
  }

  for (int i = 0; i <= currentAction; ++i) {
    timer = millis();
    playerMove = playerMove();
    while (playerMove == -1 && millis() - timer < moveTimeout) {
      playerMove = playerMove();
    }
    
    if (playerMove == -1) {
      return 0; // The player took too long between button presses.
    }
    
    showAction(playerMove);
    delay(50);
    switch (playerMove) {
      case red:
        if (actions[i] != red) {
          return 0;
        }
        break;
      case green:
        if (actions[i] != green) {
          return 0;
        }
        break;
      case yellow:
        if (actions[i] != yellow) {
          return 0;
        }
        break;
      default:
        return 0;
    }
  }
  
  // If we've gotten this far, the player successfully passed the round.
  return 1;
}
  
/*
  The setup function is required to be implemented on every sketch.
  This function takes no arguments and returns no values. It runs one time
  every time the Arduino is turned on or reset.
  It is usually used to configure pins and other preipherals before running the
  main program found in the loop() function.
*/
void setup()
{
  /*
    Set up all of the pins for the switches as INPUTs.
    The digitalWrite() after the pinMode() activates the internal pullup resistor.
  */
  pinMode(redSwitch, INPUT);
  digitalWrite(redSwitch, HIGH);
  pinMode(greenSwitch, INPUT);
  digitalWrite(greenSwitch, HIGH);
  pinMode(yellowSwitch, INPUT);
  digitalWrite(yellowSwitch, HIGH);
  
  // Set up all of the pins for the LEDs as OUTPUTs.
  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, LOW);
  pinMode(greenLed, OUTPUT);
  digitalWrite(greenLed, LOW);
  pinMode(yellowLed, OUTPUT);
  digitalWrite(yellowLed, LOW);
  
  // Set the buzzer pin as an OUTPUT
  pinMode(buzzer, OUTPUT);
}

/*
  This function does exactly as it describes. It starts, runs to the end then does it
  all over again. It takes no parameters and returns no vaules.
*/
void loop()
{
  int actions[winActions], playerMoves[winActions];
  int i;

  // Set up the random number generator so the player gets a different sequence every time
  randomSeed(analogRead(3));

  // Populate the actions array with pseudo-random values
  for (int i = 0; i < winActions; ++i) {
    actions[i] = random(0, 3); // The beginning integer is inclusive, the ending integer, exclusive.
  }

  for (i = 0; i < winActions; ++i) {
    showActions(actions, i);
    if (!roundSuccessful(actions, i)) {
      break;
    }

    delay(1000);
  }
  
  if (i == winActions) {
    playWinSequence();
  } else {
    playLoseSequence();
  }

  delay(2000);
}
