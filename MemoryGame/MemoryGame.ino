const int LED[4] = {2, 3, 4, 5};
const int BTN[4] = {8, 9, 10, 11};
const int BUZ = 13;

const int FREQ[4] = {262, 330, 392, 523};

int seq[100];
int lvl = 0;

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(LED[i], OUTPUT);
    digitalWrite(LED[i], LOW);
    pinMode(BTN[i], INPUT_PULLUP);
  }
  pinMode(BUZ, OUTPUT);
  digitalWrite(BUZ, LOW);
}

void LAS(int idx, int dur) {
  digitalWrite(LED[idx], HIGH);
  tone(BUZ, FREQ[idx]);
  delay(dur);
  digitalWrite(LED[idx], LOW);
  noTone(BUZ);
}

int waitBtn() {
  while (true) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(BTN[i]) == LOW) {
        delay(40);
        while (digitalRead(BTN[i]) == LOW);
        return i;
      }
    }
  }
}

void loop() {
   delay(1000);

  seq[lvl] = random(4);
  lvl++;

  for (int i = 0; i < lvl; i++) {
    LAS(seq[i], 500);
    delay(100);
  }

  delay(500);

  for (int i = 0; i < lvl; i++) {
    int pressed = waitBtn();
    LAS(pressed, 300);

    if (pressed != seq[i]) {
      for (int j = 0; j < 4; j++) digitalWrite(LED[j], HIGH);
      delay(500);
      for (int j = 0; j < 4; j++) digitalWrite(LED[j], LOW);
      lvl = 0;
      return;
    }
  }
}