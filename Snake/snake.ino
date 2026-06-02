// --- Пины аппаратной части ---
const int row[] = {25, 32, 22, 33, 15, 21, 17, 2};
const int col[] = {19, 18, 5, 23, 4, 26, 27, 16};

const int DATA  = 14;
const int LATCH = 13;
const int CLOCK = 12;
const int btnCols[4] = {34, 35, 36, 39};

// Видеопамять (кадр)
volatile byte frame[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// --- Переменные кнопок ---
bool lastBtnState[4][4] = {false};
uint32_t lastScanTime = 0;
const uint32_t scanInterval = 20; 

// --- Игровая логика Змейки ---
int8_t snakeX[64];
int8_t snakeY[64];
int snakeLength = 3;

int8_t dirX = 1; // 1 = вправо, -1 = влево, 0 = стоп
int8_t dirY = 0; // 1 = вниз, -1 = вверх, 0 = стоп

int8_t foodX, foodY;
bool gameOver = false;

uint32_t lastGameTick = 0;
int gameSpeed = 300; // Скорость игры (шаг каждые 300 мс)

TaskHandle_t DisplayTaskHandle;

void setup() {
    Serial.begin(115200);

    // Настройка матрицы
    for (int i = 0; i < 8; i++) {
        pinMode(row[i], OUTPUT);
        pinMode(col[i], OUTPUT);
        digitalWrite(row[i], LOW);
        digitalWrite(col[i], HIGH);
    }

    // Настройка регистра и клавиатуры
    pinMode(DATA,  OUTPUT);
    pinMode(LATCH, OUTPUT);
    pinMode(CLOCK, OUTPUT);
    for (int i = 0; i < 4; i++) {
        pinMode(btnCols[i], INPUT);
    }
    sendToRegister(0b00000000);

    // Запуск фоновой отрисовки (Ядро 0)
    xTaskCreatePinnedToCore(displayTask, "Display_Task", 2048, NULL, 1, &DisplayTaskHandle, 0);

    // Инициализация первой игры
    resetGame();
}

void sendToRegister(byte data) {
    digitalWrite(LATCH, LOW);
    shiftOut(DATA, CLOCK, MSBFIRST, data);
    digitalWrite(LATCH, HIGH);
}

// Фоновая задача рендеринга (работает всегда)
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

// Опрос клавиатуры
void scanButtons() {
    if (millis() - lastScanTime < scanInterval) return;
    lastScanTime = millis();

    for (int r = 0; r < 4; r++) {
        sendToRegister(1 << r);
        delayMicroseconds(10); 
        
        for (int c = 0; c < 4; c++) {
            bool currentState = (digitalRead(btnCols[c]) == HIGH);
            if (currentState && !lastBtnState[r][c]) {
                handleKeyPress(r, c);
            }
            lastBtnState[r][c] = currentState;
        }
    }
    sendToRegister(0b00000000);
}

// --- ИГРОВОЙ ДВИЖОК ---

void resetGame() {
    snakeLength = 3;
    snakeX[0] = 3; snakeY[0] = 4; // Голова
    snakeX[1] = 2; snakeY[1] = 4; // Тело
    snakeX[2] = 1; snakeY[2] = 4; // Хвост
    dirX = 1; dirY = 0; // Движемся вправо
    gameOver = false;
    spawnFood();
    updateFrame();
}

void spawnFood() {
    bool validPlace = false;
    while (!validPlace) {
        foodX = random(0, 8);
        foodY = random(0, 8);
        validPlace = true;
        // Проверяем, чтобы еда не появилась внутри змейки
        for (int i = 0; i < snakeLength; i++) {
            if (snakeX[i] == foodX && snakeY[i] == foodY) {
                validPlace = false;
                break;
            }
        }
    }
}

void updateFrame() {
    if (gameOver) return; // Оставляем последний кадр или экран смерти

    // 1. Очищаем виртуальный экран
    for (int i = 0; i < 8; i++) frame[i] = 0;

    // 2. Рисуем еду
    frame[foodY] |= (1 << foodX);

    // 3. Рисуем змейку (голова и тело одинаковы на одноцветной матрице)
    for (int i = 0; i < snakeLength; i++) {
        frame[snakeY[i]] |= (1 << snakeX[i]);
    }
}

void gameTick() {
    if (gameOver) return;

    if (millis() - lastGameTick < gameSpeed) return;
    lastGameTick = millis();

    // Сдвигаем координаты тела (от хвоста к голове)
    for (int i = snakeLength - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    // Двигаем голову
    snakeX[0] += dirX;
    snakeY[0] += dirY;

    // Проверка столкновения со стенами
    if (snakeX[0] < 0 || snakeX[0] >= 8 || snakeY[0] < 0 || snakeY[0] >= 8) {
        gameOver = true;
        showGameOver();
        return;
    }

    // Проверка столкновения с самим собой
    for (int i = 1; i < snakeLength; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            gameOver = true;
            showGameOver();
            return;
        }
    }

    // Проверка съедания еды
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        if (snakeLength < 64) snakeLength++;
        spawnFood();
        // Можно немного ускорять игру с каждой съеденной точкой:
        // if (gameSpeed > 100) gameSpeed -= 10; 
    }

    updateFrame();
}

void showGameOver() {
    // Включаем все диоды в знак проигрыша
    for (int i = 0; i < 8; i++) frame[i] = 0xFF;
}

// Обработка ввода
void handleKeyPress(int r, int c) {
    // Если игра окончена, любая кнопка перезапускает её
    if (gameOver) {
        resetGame();
        return;
    }

    // Раскладка управления (крестовина). 
    // Защита `dirY != 1` не дает змейке пойти вверх, если она сейчас идет вниз.
    
    if (r == 1 && c == 1 && dirY != 1)  { dirX = 0; dirY = -1; } // Вверх
    if (r == 3 && c == 1 && dirY != -1) { dirX = 0; dirY = 1; }  // Вниз
    if (r == 2 && c == 0 && dirX != 1)  { dirX = -1; dirY = 0; } // Влево
    if (r == 2 && c == 2 && dirX != -1) { dirX = 1; dirY = 0; }  // Вправо
}

void loop() {
    scanButtons();
    gameTick();
}