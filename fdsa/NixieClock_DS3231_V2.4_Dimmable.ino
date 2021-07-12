/*
   2021.07.10
   now with a timer that turns off set mode or second more after a period of time
   and a dimming funcion
*/

#include <ShiftRegister74HC595.h>
#include <microDS3231.h>
MicroDS3231 rtc;

//           [blue]  [white]  [green]
//      (data pin, clock pin, latch pin)
ShiftRegister74HC595<6> sr(6, 5, 4);

byte digit[4][11] = {
  {0, 9, 8, 7, 6, 5, 4, 3, 2, 1, 10},
  {37, 46, 45, 44, 43, 42, 41, 40, 39, 38, 47},
  {24, 33, 32, 31, 30, 29, 28, 27, 26, 25, 34},
  {23, 14, 15, 16, 17, 18, 19, 20, 21, 22, 13},
};

byte twitch[4][11] = {
  {9, 4, 8, 3, 5, 0, 6, 1, 2, 7, 10},
  {46, 41, 45, 40, 42, 37, 43, 38, 39, 44, 47},
  {33, 28, 32, 27, 29, 24, 30, 25, 26, 31, 34},
  {14, 19, 15, 20, 18, 23, 17, 22, 21, 16, 13},
};

byte buttonPin[3] = {12, 10, 11};
bool prevButtonState[3];
byte numberOfButtons = sizeof(buttonPin);
uint32_t prevButtonCheck = 0;

byte blinkVal = 0;
uint32_t prevBlink = 0;
bool blinkState;

uint32_t prevSecondMillis = 0;
bool twitchState = false;
bool secondsState = false;
uint32_t currentMillis = 0;
uint32_t setTimeoutMillis = 0;

bool timeout = true;

uint32_t dimmMillis = 0;
bool tubeState = true;
bool dimmState = false;

byte second;
byte minute;
byte hour;
byte prevSecond;
byte prevMinute;
byte prevHour;

void setup() {
  for (byte i = 0; i < numberOfButtons; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }
  for (byte i = 0; i < 48; i++) {
    sr.set(i, LOW);
  }
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  DateTime now = rtc.getTime();
  second = now.second;
  minute = now.minute;
  hour = now.hour;
  displayHours();
  displayMinutes();
}

void loop() {
  DateTime now = rtc.getTime();

  if (now.second != second) {
    prevSecond = second;
    second = now.second;
    if (secondsState) {
      displaySeconds();
    } else {
      if (minute % 5 == 0 && second == 20 && blinkVal == 0) {
        tubeState = true;
        digitalWrite(2, tubeState);
        doTheTwitch();
      }
      if (second % 2 == 0 && blinkVal == 0) {
        sr.set(digit[3][10], HIGH);
      } else {
        sr.set(digit[3][10], LOW);
      }
    }
    if (now.minute != minute) {
      prevMinute = minute;
      minute = now.minute;
      if (!secondsState) {
        displayHours();
        displayMinutes();
      }
      if (now.hour != hour) {
        prevHour = hour;
        hour = now.hour;
        if (hour == 0) {
          dimmState = true;
        } else if (hour == 7) {
          dimmState = false;
          tubeState = true;
          digitalWrite(2, tubeState);
        }
      }
    }
  }

  currentMillis = millis();
  if (currentMillis - prevButtonCheck >= 20) {
    prevButtonCheck = currentMillis;
    checkButtons();
  }

  if (blinkVal != 0 && currentMillis - prevBlink > 400) {
    prevBlink = currentMillis;
    if (blinkState) {
      blinkState = false;
      if (blinkVal == 1) {
        eraseHours();
      } else if (blinkVal == 2) {
        eraseMinutesSeconds();
      }
    } else {
      blinkState = true;
      if (blinkVal == 1) {
        displayHours();
      } else if (blinkVal == 2) {
        displayMinutes();
      }
    }
  }

  if (!timeout) {
    if (secondsState) {
      if (currentMillis - setTimeoutMillis >= 10000) {
        timeout = true;
        secondsState = false;
        updateTimeDisplayed();
      }
    } else {
      if (currentMillis - setTimeoutMillis >= 60000) {
        timeout = true;
        blinkVal = 0;
        updateTimeDisplayed();
      }
    }
  }

  if (dimmState) {
    if (!tubeState && currentMillis - dimmMillis >= 5) {
      dimmMillis = currentMillis;
      tubeState = true;
      digitalWrite(2, tubeState);
    } else if (tubeState && currentMillis - dimmMillis >= 1) {
      dimmMillis = currentMillis;
      tubeState = false;
      digitalWrite(2, tubeState);
    }
  }
}

void updateTimeDisplayed() {
  if (secondsState) {
    sr.set(digit[3][10], LOW);
    eraseHours();
    eraseMinutesSeconds();
    sr.set(digit[2][second / 10], HIGH);
    sr.set(digit[3][second % 10], HIGH);
  } else {
    eraseMinutesSeconds();
    sr.set(digit[0][hour / 10], HIGH);
    sr.set(digit[1][hour % 10], HIGH);
    sr.set(digit[2][minute / 10], HIGH);
    sr.set(digit[3][minute % 10], HIGH);
    if (second % 2 == 0) {
      sr.set(digit[3][10], HIGH);
    }
  }
}

void displaySeconds() {
  DateTime now = rtc.getTime();
  if (second % 10 == 0) {
    sr.set(digit[2][prevSecond / 10], LOW);
    sr.set(digit[2][now.second / 10], HIGH);
  }
  sr.set(digit[3][prevSecond % 10], LOW);
  sr.set(digit[3][now.second % 10], HIGH);
}

void displayMinutes() {
  DateTime now = rtc.getTime();
  sr.set(digit[2][prevMinute / 10], LOW);
  sr.set(digit[3][prevMinute % 10], LOW);
  sr.set(digit[2][now.minute / 10], HIGH);
  sr.set(digit[3][now.minute % 10], HIGH);
}

void eraseMinutesSeconds() {
  for (byte i = 0; i < 10; i++) {
    sr.set(digit[2][i], LOW);
    sr.set(digit[3][i], LOW);
  }
}

void displayHours() {
  DateTime now = rtc.getTime();
  sr.set(digit[0][prevHour / 10], LOW);
  sr.set(digit[1][prevHour % 10], LOW);
  sr.set(digit[0][now.hour / 10], HIGH);
  sr.set(digit[1][now.hour % 10], HIGH);
}

void eraseHours() {
  for (byte i = 0; i < 10; i++) {
    sr.set(digit[0][i], LOW);
    sr.set(digit[1][i], LOW);
  }
}
