void checkButtons() {
  for (byte i = 0; i < numberOfButtons; i++) {
    bool state = digitalRead(buttonPin[i]);
    if (state != prevButtonState[i]) {
      prevButtonState[i] = state;
      if (state == LOW) {
        timeout = false;
        setTimeoutMillis = currentMillis;
        switch (i) {
          case 0:
            button0();
            break;
          case 1:
            button1();
            break;
          case 2:
            button2();
            break;
        }
      }
    }
  }
}

void button0() {
  blinkState = false;
  prevBlink = currentMillis;
  if (blinkVal == 0) {
    blinkVal++;
    sr.set(digit[3][10], LOW);
    if (secondsState) {
      secondsState = false;
      eraseMinutesSeconds();
      displayMinutes();
    }
    eraseHours();
  } else if (blinkVal == 1) {
    blinkVal++;
    displayHours();
    eraseMinutesSeconds();
  } else {
    blinkVal = 0;
    displayMinutes();
    timeout = true;
  }
}

void button1() {
  DateTime now = rtc.getTime();
  blinkState = true;
  prevBlink = currentMillis;
  if (blinkVal == 1) {
    if (now.hour < 23) {
      now.hour += 1;
    } else {
      now.hour = 0;
    }
    rtc.setTime(now);
    eraseHours();
    displayHours();
  } else if (blinkVal == 2) {
    if (now.minute < 59) {
      now.minute += 1;
    } else {
      now.minute = 0;
    }
    now.second = 0;
    rtc.setTime(now);
    eraseMinutesSeconds();
    displayMinutes();
  } else {
    if (secondsState) {
      secondsState = false;
      timeout = true;
      if (second % 2 == 0) {
        sr.set(digit[3][10], HIGH);
      }
    } else {
      secondsState = true;
      sr.set(digit[3][10], LOW);
    }
    updateTimeDisplayed();
  }
}

void button2() {
  DateTime now = rtc.getTime();
  blinkState = true;
  prevBlink = currentMillis;
  if (blinkVal == 1) {
    if (now.hour > 0) {
      now.hour -= 1;
    } else {
      now.hour = 23;
    }
    rtc.setTime(now);
    eraseHours();
    displayHours();
  } else if (blinkVal == 2) {
    if (now.minute > 0) {
      now.minute -= 1;
    } else {
      now.minute = 59;
    }
    now.second = 0;
    rtc.setTime(now);
    eraseMinutesSeconds();
    displayMinutes();
  } else {
    if (!secondsState) {
      if (dimmState) {
        dimmState = false;
        tubeState = true;
        digitalWrite(2, tubeState);
      } else {
        dimmState  = true;
      }
    }
  }
}
