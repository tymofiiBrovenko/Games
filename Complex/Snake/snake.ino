const int row[] = {25, 32, 22, 33, 15, 21, 17, 2};
const int col[] = {19, 18, 5, 23, 4, 26, 27, 16};

const int DATA  = 14;
const int LATCH = 13;
const int CLOCK = 12;
const int btnCols[4] = {34, 35, 36, 39};

volatile byte frame[8] = {0, 0, 0, 0, 0, 0, 0, 0};

bool lastBtnState[4][4] = {false};
uint32_t lastScanTime = 0;
const uint32_t scanInterval = 20;

int8_t snakeX[64];
int8_t snakeY[64];
int snakeLength = 3;

int8_t dirX = 1;
int8_t dirY = 0;

int8_t foodX, foodY;
bool gameOver = false;

uint32_t lastGameTick = 0;
int gameSpeed = 300;

TaskHandle_t DisplayTaskHandle;

void sendToRegister(byte data) {
    digitalWrite(LATCH, LOW);
    shiftOut(DATA, CLOCK, MSBFIRST, data);
    digitalWrite(LATCH, HIGH);
}

void displayTask(void * pvParameters) {
    for (;;) {
        for (int r = 0; r < 8; r++) {
            for (int i = 0; i < 8; i++) digitalWrite(row[i], LOW);
            for (int c = 0; c < 8; c++) {
                int bit = (frame[r] >> c) & 1;
                digitalWrite(col[c], bit ? LOW : HIGH);
            }
            digitalWrite(row[r], HIGH);
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}

void resetGame() {
    snakeLength = 3;
    snakeX[0] = 3; snakeY[0] = 4;
    snakeX[1] = 2; snakeY[1] = 4;
    snakeX[2] = 1; snakeY[2] = 4;
    dirX = 1; dirY = 0;
    gameOver = false;
    
    bool validPlace = false;
    while (!validPlace) {
        foodX = random(0, 8);
        foodY = random(0, 8);
        validPlace = true;
        for (int i = 0; i < snakeLength; i++) {
            if (snakeX[i] == foodX && snakeY[i] == foodY) validPlace = false;
        }
    }
    
    for (int i = 0; i < 8; i++) frame[i] = 0;
    frame[foodY] |= (1 << foodX);
    for (int i = 0; i < snakeLength; i++) frame[snakeY[i]] |= (1 << snakeX[i]);
}

void handleKeyPress(int r, int c) {
    if (gameOver) {
        resetGame();
        return;
    }
    if (r == 1 && c == 1 && dirY != 1)  { dirX = 0; dirY = -1; }
    else if (r == 3 && c == 1 && dirY != -1) { dirX = 0; dirY = 1; }
    else if (r == 2 && c == 0 && dirX != 1)  { dirX = -1; dirY = 0; }
    else if (r == 2 && c == 2 && dirX != -1) { dirX = 1; dirY = 0; }
}

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
    for (int i = 0; i < 4; i++) pinMode(btnCols[i], INPUT);
    sendToRegister(0);
    xTaskCreatePinnedToCore(displayTask, "Display_Task", 2048, NULL, 1, &DisplayTaskHandle, 0);
    resetGame();
}

void loop() {
    if (millis() - lastScanTime >= scanInterval) {
        lastScanTime = millis();
        for (int r = 0; r < 4; r++) {
            sendToRegister(1 << r);
            delayMicroseconds(10);
            for (int c = 0; c < 4; c++) {
                bool state = (digitalRead(btnCols[c]) == HIGH);
                if (state && !lastBtnState[r][c]) handleKeyPress(r, c);
                lastBtnState[r][c] = state;
            }
        }
        sendToRegister(0);
    }

    if (!gameOver && millis() - lastGameTick >= gameSpeed) {
        lastGameTick = millis();
        for (int i = snakeLength - 1; i > 0; i--) {
            snakeX[i] = snakeX[i - 1];
            snakeY[i] = snakeY[i - 1];
        }
        snakeX[0] += dirX;
        snakeY[0] += dirY;

        if (snakeX[0] < 0 || snakeX[0] >= 8 || snakeY[0] < 0 || snakeY[0] >= 8) gameOver = true;
        for (int i = 1; i < snakeLength; i++) if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) gameOver = true;

        if (snakeX[0] == foodX && snakeY[0] == foodY) {
            if (snakeLength < 64) snakeLength++;
            bool validPlace = false;
            while (!validPlace) {
                foodX = random(0, 8); foodY = random(0, 8);
                validPlace = true;
                for (int i = 0; i < snakeLength; i++) if (snakeX[i] == foodX && snakeY[i] == foodY) validPlace = false;
            }
        }
        
        for (int i = 0; i < 8; i++) frame[i] = 0;
        frame[foodY] |= (1 << foodX);
        for (int i = 0; i < snakeLength; i++) frame[snakeY[i]] |= (1 << snakeX[i]);
        if (gameOver) for (int i = 0; i < 8; i++) frame[i] = 0xFF;
    }
}