#define pinLed_G 8
#define pinLed_Y 9
#define pinLed_B 10
#define pinLed_R 11

#define pinBtn_G 2
#define pinBtn_Y 3
#define pinBtn_B 4
#define pinBtn_R 12

#define pinPsvBzzr 5

#define NUM_OF_LEDS 4
#define NUM_OF_CHOSEN_LEDS 3

int Leds[NUM_OF_LEDS] = {pinLed_G, pinLed_Y, pinLed_B, pinLed_R};
int ChosenIndexes[NUM_OF_CHOSEN_LEDS];
int Btns[NUM_OF_LEDS] = {pinBtn_G, pinBtn_Y, pinBtn_B, pinBtn_R};
int val[NUM_OF_LEDS];
int lastVal[NUM_OF_LEDS];
unsigned long lastPressTime[NUM_OF_LEDS];
int PressedIndexes[NUM_OF_LEDS];
int NumOfPressedIndexes = 0;
unsigned long gameStartTime;
bool gameStarted = false;
bool gameEnded = false;

#define TONE_SUCCESS 1000
#define TONE_FAILURE 500

void setup() {
  initializePins();
  randomSeed(analogRead(A1));
}

void loop() {
  handleGameStates();
}

void initializePins() {
  for (int k = 0; k < NUM_OF_LEDS; k++) {
    pinMode(Leds[k], OUTPUT);
    pinMode(Btns[k], INPUT_PULLUP);
    lastVal[k] = digitalRead(Btns[k]);
    lastPressTime[k] = millis();
  }
  pinMode(pinPsvBzzr, OUTPUT);
}

void handleGameStates() {
  if (gameEnded) {
    checkForStartNewGame();
  } else if (!gameStarted) {
    startGame();
  } else {
    handleGame();
  }
}

void startGame() {
  resetGame();
  selectRandomLeds();
  indicateStart();
}

void resetGame() {
  NumOfPressedIndexes = 0;
  gameStartTime = millis();
  gameStarted = true;
  gameEnded = false;
  turnOffAllLeds();
}

void selectRandomLeds() {
  bool chosen[NUM_OF_LEDS] = {false, false, false, false};
  for (int k = 0; k < NUM_OF_CHOSEN_LEDS; k++) {
    int idx;
    do {
      idx = random(NUM_OF_LEDS);
    } while (chosen[idx]);
    chosen[idx] = true;
    ChosenIndexes[k] = idx;
    digitalWrite(Leds[idx], HIGH);
    delay(1000);
    digitalWrite(Leds[idx], LOW);
    delay(100);
  }
}

void indicateStart() {
  tone(pinPsvBzzr, TONE_SUCCESS, 100);
  delay(150);
}

void handleGame() {
  int pressedBtn = GetPressedBtn();
  if (pressedBtn > -1) {
    handlePress(pressedBtn);
  }
  checkGameState();
}

void checkForStartNewGame() {
  int pressedBtn = GetPressedBtn();
  if (pressedBtn > -1) {
    startGame();
  }
}

void handlePress(int buttonIndex) {
  if (!isButtonAlreadyPressed(buttonIndex)) {
    PressedIndexes[NumOfPressedIndexes++] = buttonIndex;
    digitalWrite(Leds[buttonIndex], HIGH);
  }

  if (!isButtonInChosen(buttonIndex)) {
    indicateFailure();
    resetGame();
  }
}

bool isButtonAlreadyPressed(int buttonIndex) {
  for (int i = 0; i < NumOfPressedIndexes; i++) {
    if (PressedIndexes[i] == buttonIndex) {
      return true;
    }
  }
  return false;
}

bool isButtonInChosen(int buttonIndex) {
  for (int i = 0; i < NUM_OF_CHOSEN_LEDS; i++) {
    if (ChosenIndexes[i] == buttonIndex) {
      return true;
    }
  }
  return false;
}

void indicateFailure() {
  tone(pinPsvBzzr, TONE_FAILURE, 500);
  digitalWrite(pinLed_R, HIGH);
  delay(1000);
  digitalWrite(pinLed_R, LOW);
}

void checkGameState() {
  unsigned long elapsedTime = millis() - gameStartTime;
  
  if (elapsedTime > 1000) {
    if (NumOfPressedIndexes == NUM_OF_CHOSEN_LEDS) {
      if (areAllPressedButtonsCorrect()) {
        if (elapsedTime <= 1000) {
          indicateSuccess();
        } else {
          indicateSuccessDelayed();
        }
      } else {
        indicateFailure();
      }
      gameEnded = true;
    }
  }
}

bool areAllPressedButtonsCorrect() {
  for (int i = 0; i < NumOfPressedIndexes; i++) {
    if (!isButtonInChosen(PressedIndexes[i])) {
      return false;
    }
  }
  return true;
}

void indicateSuccess() {
  tone(pinPsvBzzr, TONE_SUCCESS, 500);
  digitalWrite(pinLed_G, HIGH);
  delay(1000);
  digitalWrite(pinLed_G, LOW);
}

void indicateSuccessDelayed() {
  tone(pinPsvBzzr, TONE_SUCCESS, 500);
  analogWrite(pinLed_G, 64);
  delay(1000);
  analogWrite(pinLed_G, 0);
}

void turnOffAllLeds() {
  for (int k = 0; k < NUM_OF_LEDS; k++) {
    digitalWrite(Leds[k], LOW);
  }
}

int GetPressedBtn() {
  int BtnPressed = -1;
  for (int k = 0; k < NUM_OF_LEDS; k++) {
    val[k] = digitalRead(Btns[k]);
    if ((val[k] == LOW) && (lastVal[k] == HIGH) && (millis() - lastPressTime[k] > 50)) {
      lastPressTime[k] = millis();
      BtnPressed = k;
    }
    lastVal[k] = val[k];
  }
  return BtnPressed;
}