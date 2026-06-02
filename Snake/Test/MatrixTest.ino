const int row[] = {25, 32, 22, 33, 15, 21, 17, 2};
const int col[] = {19, 18, 5, 23, 4, 26, 27, 16};

const int DATA  = 14;
const int LATCH = 13;
const int CLOCK = 12;

const int btnCols[4] = {34, 35, 36, 39};

byte frame[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
    for (int i = 0; i < 8; i++) {
        pinMode(row[i], OUTPUT);
        pinMode(col[i], OUTPUT);
        digitalWrite(row[i], LOW);
        digitalWrite(col[i], HIGH);
    }
    pinMode(DATA,  OUTPUT);
    pinMode(LATCH, OUTPUT);
    pinMode(CLOCK, OUTPUT);
    for (int i = 0; i < 4; i++) {
        pinMode(btnCols[i], INPUT);
    }
    sendToRegister(0b00000000);
}

void sendToRegister(byte data) {
    digitalWrite(LATCH, LOW);
    shiftOut(DATA, CLOCK, MSBFIRST, data);
    digitalWrite(LATCH, HIGH);
}

void displayFrame() {
    for (int r = 0; r < 8; r++) {
        // выключить все строки
        for (int i = 0; i < 8; i++) {
            digitalWrite(row[i], LOW);
        }
        // выставить столбцы
        for (int c = 0; c < 8; c++) {
            int bit = (frame[r] >> c) & 1;
            digitalWrite(col[c], bit ? LOW : HIGH);
        }
        // включить эту строку
        digitalWrite(row[r], HIGH);
        delayMicroseconds(500);
    }
}

void scanButtons() {
    for (int r = 0; r < 4; r++) {
        sendToRegister(1 << r);
        delayMicroseconds(10);
        for (int c = 0; c < 4; c++) {
            if (digitalRead(btnCols[c]) == HIGH) {
                // очищаем экран
                memset(frame, 0, sizeof(frame));
                // зажигаем блок 2x2
                int ledRow = r * 2;
                int ledCol = c * 2;
                frame[ledRow]     |= (0b11 << ledCol);
                frame[ledRow + 1] |= (0b11 << ledCol);
            }
        }
    }
    sendToRegister(0b00000000);
}

void loop() {
    scanButtons();
    // показываем кадр 10 раз пока не опросим кнопки снова
    for (int i = 0; i < 10; i++) {
        displayFrame();
    }
}