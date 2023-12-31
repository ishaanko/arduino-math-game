#include <LiquidCrystal.h>

#define COMMUNICATION_OUT A1
#define COMMUNICATION_IN A0

LiquidCrystal lcd(10, 9, 8, 7, 6, 5);

bool playing = true;
int score = 0;
int question = 1;
int selectedAnswer;

byte seven_seg_digits[10] = {
  B01111110, // 0
  B00110000, // 1
  B01101101, // 2
  B01111001, // 3
  B00110011, // 4
  B01011011, // 5
  B01011111, // 6
  B01110000, // 7
  B01111111, // 8
  B01110011  // 9
};

int latchPin = 12;
int clockPin = 13;
int dataPin = 11;

void printText(uint8_t line, const char *text) {
  lcd.setCursor(0, line);
  lcd.print(text); 
}

void clearAndPrintText(uint8_t line, const char *text) {
  lcd.clear();
  lcd.setCursor(0, line);
  lcd.print("                ");
  lcd.setCursor(0, line);
  lcd.print(text);
}

void startGame() {
  printText(0, "Multiplication!");
  delay(1000);
  printText(1, "Press A to Begin");
  display(0);
  while (digitalRead(2) != LOW) {
    delay(100);
  }
  
  lcd.clear();
  delay(1000);
} 

void printAnswers(char answerA[16], char answerB[16], char answerC[16]) {
  lcd.setCursor(0, 1);
  lcd.print(answerA); 
  lcd.print(" ");
  lcd.print(answerB);
  lcd.print(" ");
  lcd.print(answerC);
}

int selectAnswer() {
  if (digitalRead(2) == LOW) {
    return 1;
  } else if (digitalRead(3) == LOW) {
    return 2;
  } else if (digitalRead(4) == LOW) {
    return 3;
  } else {
    return 0;
  }
}

bool askQuestion(uint8_t counter, int num1, int num2) {
  char formattedQuestion[16];
  sprintf(formattedQuestion, "%d. %d x %d", counter, num1, num2);
  
  lcd.clear();
  printText(0, formattedQuestion);

  randomSeed(analogRead(A5));
  int answer = random(1, 4);
  int correct_answer = num1 * num2;
  char answerA[16], answerB[16], answerC[16];

  if (answer == 1) {
      sprintf(answerA, "A)%d", correct_answer);
    } else {
      randomSeed(analogRead(A5));
      sprintf(answerA, "A)%d", random(1, 226));
    }
    if (answer == 2) {
      sprintf(answerB, "B)%d", correct_answer);
    } else {
      randomSeed(analogRead(A4));
      sprintf(answerB, "B)%d", random(1, 226));
    }
    if (answer == 3) {
      sprintf(answerC, "C)%d", correct_answer);
    } else {
      randomSeed(analogRead(A5));
      sprintf(answerC, "C)%d", random(1, 226));
    }
    delay(500);

  while (true) {
    printAnswers(answerA, answerB, answerC);
    if (selectAnswer() != 0) {
      selectedAnswer = selectAnswer();
      break;
    }
  }
  if (selectedAnswer == answer) {
    return true;
  } else {
    return false;
  }
}

void display(byte digit) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, seven_seg_digits[digit]);
  digitalWrite(latchPin, HIGH);
  delayMicroseconds(100);
}

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  lcd.begin(16, 2); 

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(1, OUTPUT);
  pinMode(COMMUNICATION_OUT, OUTPUT);
  pinMode(COMMUNICATION_IN, INPUT);
}

void loop() {
  
  startGame();

  uint8_t counter = 0;
  while (playing) {
    counter++;

    randomSeed(analogRead(A5));
    int num1 = random(1, 16);
    randomSeed(analogRead(A3));
    int num2 = random(1, 16);


    bool correct = askQuestion(counter, num1, num2);

    if (correct) {
      score++;
      clearAndPrintText(0, "Correct!!!");
      delay(1000);
    } else {
      clearAndPrintText(0, "WRONG : (");
      delay(1000);
    }
    display(score);

    if (score == 9) {
      clearAndPrintText(0, "GOOD JOB");
      printText(1, "YOU WON!!!!");
      analogWrite(COMMUNICATION_OUT, 1024);
      while (true) {
        digitalWrite(1, HIGH);
        delay(500);
        digitalWrite(1, LOW);
        delay(500);
     }
    }
    if (analogRead(COMMUNICATION_IN) > 512) {
      while (true) {
        digitalWrite(1, HIGH);
        delay(200);
        digitalWrite(1, LOW);
        delay(200);
      }
    }
  }
}
