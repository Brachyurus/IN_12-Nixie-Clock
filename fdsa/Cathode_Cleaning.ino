void doTheTwitch() {
  for (byte i = 0; i < 48; i++) {
    sr.set(i, LOW);
  }
  if (dimmState) {
    for (byte f = 0; f < 67; f++) {
      for (byte i = 0; i < 11; i++) {
        for (byte j = 0; j < 4; j++) {
          sr.set(twitch[j][i], HIGH);
        }
        delay(5);
        for (byte j = 0; j < 4; j++) {
          sr.set(twitch[j][i], LOW);
        }
        delay(15);
      }
    }
  } else {
    for (byte f = 0; f < 67; f++) {
      for (byte i = 0; i < 11; i++) {
        for (byte j = 0; j < 4; j++) {
          sr.set(twitch[j][i], HIGH);
        }
        delay(20);
        for (byte j = 0; j < 4; j++) {
          sr.set(twitch[j][i], LOW);
        }
      }
    }
  }
  displayHours();
  displayMinutes();
}
