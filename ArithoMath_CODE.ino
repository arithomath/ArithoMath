#include <Keypad.h>  // Keypad by Mark Stanley, Alexander Brevig version 3.1.1
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523

#define CHANCESCOUNT 3

#define ENV "dev"                        // dev = development stage, else production
#define NUM_SETS 3                       // Total number of sets (1, 2, 3)
int setNumbers[NUM_SETS] = { 1, 2, 3 };  // Array of possible sets
int setIndex = 0;                        // To keep track of the next unused set

int melody[] = {
  NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5,  // Opening (rising)
  NOTE_G4, NOTE_E4, NOTE_C4,           // Descending
  NOTE_F4, NOTE_A4, NOTE_C5,           // Rising again
  NOTE_G4, NOTE_F4, NOTE_E4,           // Short notes
  NOTE_C4, NOTE_C4, NOTE_C5            // Final celebration
};

int noteDurations[] = {
  8, 8, 4, 4,
  8, 8, 4,
  8, 8, 4,
  8, 8, 8,
  4, 4, 2
};

// Define keypad size (4x4 or 3x4)
const byte ROWS = 4;  // Number of rows
const byte COLS = 4;  // Number of columns
const int buzzerPin = A3;
const int greenLed = A0;
const int redLed = 12;
const int servoPin = 13;

const char q11[] PROGMEM = "Find the remainder of 12 when divided by 7";              //5
const char q12[] PROGMEM = "Find the remainder of 81 when divided by 76";             // 5
const char q13[] PROGMEM = "When you divide (16 x 4) by 62, what is the remainder?";  //2

const char q14[] PROGMEM = "Find the remainder of 34 when divided by 7.";               // 6
const char q15[] PROGMEM = "Find the remainder of 22 when divided by 5.";               // 2
const char q16[] PROGMEM = "When you divide 18 times 2 by 13, what is the remainder?";  // 10

const char q17[] PROGMEM = "Find the remainder of 40 when divided by 7.";             // 5
const char q18[] PROGMEM = "Find the remainder of 33 when divided by 6.";             // 3
const char q19[] PROGMEM = "When you divide 8 times 3 by 5, what is the remainder?";  // 4

const char q21[] PROGMEM = "If the remainder of a number is 2, what is the number if it is divided by 3 and k = 2?";  // 8
const char q22[] PROGMEM = "If the remainder of a number is 1, what is the number if it is divided by 4 and k = 1?";  // 5
const char q23[] PROGMEM = "Find the smallest positive integer if the remainder is 2 when divided by 3.";             // 2

const char q24[] PROGMEM = "If the remainder of a number is 1, what is the number if it is divided by 3 and k = 1?";  // 4
const char q25[] PROGMEM = "If the remainder of a number is 1, what is the number if it is divided by 2 and k = 2?";  // 5
const char q26[] PROGMEM = "Find the smallest positive integer if the remainder is 3 when divided by 5.";             // 3

const char q27[] PROGMEM = "If the remainder of a number is 2, what is the number if it is divided by 3 and k = 2?";  // 8
const char q28[] PROGMEM = "If the remainder of a number is 1, what is the number if it is divided by 2 and k = 1?";  // 3
const char q29[] PROGMEM = "Find the smallest positive integer if the remainder is 4 when divided by 6.";             // 4


const char q31[] PROGMEM = "Each bag contains 12 candies. If you have 200 candies, how many candies are left when distributing them equally  into bags?";  // 8
const char q32[] PROGMEM = "A farmer has 56 apples and wants to pack them in baskets, with 12 apples per basket. How many apples will be left over?";      // 8
const char q33[] PROGMEM = "A teacher has 14 notebooks and gives 6 to each student. How many notebooks are left?";                                         // 2

const char q34[] PROGMEM = "A farmer has 100 apples and wants to pack them into crates of 9. How many apples will be left over?";  // 1
const char q35[] PROGMEM = "Each bag contains 12 candies. If you have 56 candies, how many will be left after filling the bags?";  // 8
const char q36[] PROGMEM = "A teacher has 34 notebooks and gives 7 to each student. How many notebooks will be left?";             //6


const char q37[] PROGMEM = "Each crate holds 12 bottles. If you have 80 bottles, how many will be left after filling the crates?";  // 8
const char q38[] PROGMEM = "A farmer has 77 oranges and wants to pack them into boxes of 10. How many oranges will be left?";       // 7
const char q39[] PROGMEM = "A teacher has 28 pens and gives 5 to each student. How many pens will be left?";                        // 3


const char* const questions[] PROGMEM = {
  // stage 1
  q11,
  q12,
  q13,
  q14,
  q15,
  q16,
  q17,
  q18,
  q19,

  // stage 2 (x = nk + r)
  q21,
  q22,
  q23,
  q24,
  q25,
  q26,
  q27,
  q28,
  q29,

  // stage 3
  q31,
  q32,
  q33,
  q34,
  q35,
  q36,
  q37,
  q38,
  q39,
};
const short answers[] = {
  5, 5, 2,
  6, 2, 10,
  5, 3, 4,

  8, 5, 2,
  4, 5, 3,
  8, 3, 4,

  8, 8, 2,
  1, 8, 6,
  8, 7, 3
};

// Define the keymap for a 4x4 keypad
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };

int questionIndex = -1;
int currStage = 1;  // the stage
int currLevel = 1;  // the question
int currSet = 1;    // random(1, 4); // random set for each level

int errorCount = 0;
int answer;
int questionSpeed = 300;  // in millisecond

char buffer[100];
String inputString = "";

bool hasDisplayed = false;
bool isAnswering = false;
bool answerDisplayed = false;
bool boxIsOpen = false;
bool displayingQuestion = false;
bool isFinalStage = false;
bool isFinished = false;
bool askAnswer = false;

int sumAnswer = 0;  // will handle the sum of the answer for each stages to calculate mean
int mean1;          // handles mean of stage 1
int mean2;          // handles mean of stage 2
int mean3;          // handles mean of stage 3

bool isDisplayingAnswer = false;
int currentIndex;
const int displayLength = 32;
int displayPage;
String question;

// Create the keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;
void beep1() {

  int beepDuration = 200;  // Duration of each beep in milliseconds
  // First beep
  tone(buzzerPin, 10000, beepDuration);
  delay(beepDuration);  // Pause after the first beep
  noTone(buzzerPin);
}

void setup() {

  lcd.init();       // Initialize the LCD
  lcd.backlight();  // Turn on the backlight (if available)
  lcd.setCursor(0, 0);
  beep1();
  lcd.print("   ARITHOMATH   ");
  lcd.setCursor(0, 1);
  lcd.print("Loading...");

  if (isInProd())
    delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  beep1();
  lcd.print("WELCOME");  // Display text on the screen
  lcd.setCursor(0, 1);
  lcd.print("   CHALLENGER!! ");  // Display text on the screen

  pinMode(buzzerPin, OUTPUT);  // Set pin 10 as an output
  pinMode(greenLed, OUTPUT);   // Set pin 11 as an output
  pinMode(redLed, OUTPUT);     // Set pin 12 as an output
  pinMode(A3, OUTPUT);
  pinMode(A0, OUTPUT);

  myServo.attach(servoPin);
  myServo.write(180);
  Serial.begin(9600);

  if (isInProd())
    delay(2500);

  randomSeed(analogRead(1));  // Initialize random seed

  resetAll();  // reset the question, this will set displayingQuestion = true
  beep1();
  showCurrentLevel();
  displayQuestion();
}

void (*resetFunc)(void) = 0;  // Declare reset function at address 0

void loop() {
  // put your main code here, to run repeatedly:

  char key = keypad.getKey();
  if (key == 'A') {
    resetFunc();  // Restart the Arduino program
    resetAll();
  }

  if (boxIsOpen) {
    if (key == '*') {  // use * to close the box
      closeBox();
    }
  } else if (isFinished) {
    congratulations();
  } else if (isFinalStage) {
    if (key == '#') {
      inputString = "";
      displayFinal();
    } else if (key == 'D') {
      if (inputString == arrangeNumbers(mean1, mean2, mean3)) {
        congratulations();
        isFinished = true;
      } else {
        errorCount++;
        if (errorCount == 3) {
          lcd.clear();          // Clear the screen
          lcd.setCursor(0, 0);  // Start from the first position
          lcd.print("SORRY!");
          delay(2500);
          resetFunc();
        } else {
          inputString = "";
          incorrectBeep();
          displayFinal();
        }
      }
    } else if (isDigit(key)) {  // if 0-9
      inputString += key;
      Serial.print("Current input: ");
      Serial.println(inputString);
      beep();
      displayFinal();
    }
  } else if (displayingQuestion) {
    if (key == 'B') {
      if (askAnswer) {
        displayingQuestion = false;
        askAnswer = false;
        isAnswering = true;
        displayAnswer();
      } else {
        nextSegment();
      }
    } else if (key == 'C')
      prevSegment();
  } else if (isAnswering) {
    if (!answerDisplayed) {
      displayAnswer();
      answerDisplayed = true;
    }

    if (key == 'C') {  // cancel, this is to allow returning to question when answering
      isAnswering = false;
      inputString = "";  // clear any answer
      displayingQuestion = true;
      currentIndex = 0;
      displayQuestion();
    } else if (key == '#') {
      inputString = "";
      displayAnswer();
    } else if (key == 'D') {
      // check for the input;
      answer = inputString.toInt();

      Serial.println(String(answer));
      Serial.println(getCorrectAnswer());

      if (answer == getCorrectAnswer()) {
        correctAnswer();
      } else {
        incorrectAnswer();
      }
      answerDisplayed = false;
    } else if (isDigit(key)) {  // if 0-9
      inputString += key;
      Serial.print("Current input: ");
      Serial.println(inputString);
      answerDisplayed = false;  // to update answer
      beep();
    }  // end if isAnswering
  }    // end if box is not open
}  // end-loop

bool isInProd() {
  return (ENV != "dev");
}

void openBox() {
  lcd.clear();          // Clear the screen
  lcd.setCursor(0, 0);  // Start from the first position
  lcd.print("OPEN SESAME!");
  lcd.setCursor(0, 1);  // Start from the first position
  lcd.print("PRESS(*) TO EXIT");
  boxIsOpen = true;
  myServo.write(0);
}

void closeBox() {
  boxIsOpen = false;
  myServo.write(180);
  delay(200);
  resetFunc();
}

void resetAll() {
  isAnswering = false;
  currStage = 1;
  currLevel = 1;

  // Initialize the sets array only once when the system starts.
  if (setIndex == 0) {
    // Shuffle the setNumbers array once at the beginning
    shuffleArray(setNumbers, NUM_SETS);
  }

  // Get the next set without repetition
  currSet = setNumbers[setIndex];
  setIndex++;

  // Print the current set for debugging
  Serial.println("set: " + String(currSet));

  // If all sets have been used, reset setIndex to start over
  if (setIndex >= NUM_SETS) {
    setIndex = 0;                        // Reset to loop through the sets again
    shuffleArray(setNumbers, NUM_SETS);  // Optionally shuffle again when restarting
  }

  currentIndex = 0;
  errorCount = 0;
  hasDisplayed = false;
  isFinalStage = false;
  displayingQuestion = true;
}

// Function to shuffle the array randomly (Fisher-Yates shuffle)
void shuffleArray(int arr[], int len) {
  for (int i = len - 1; i > 0; i--) {
    int j = random(i + 1);  // Random index between 0 and i
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}

void beep() {
  digitalWrite(buzzerPin, HIGH);  // Turn the buzzer on
  delay(100);
  digitalWrite(buzzerPin, LOW);  // Turn the buzzer off
  delay(100);
}

void stopMelodyAndLEDs() {
  noTone(buzzerPin);
  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, LOW);
}

void congratulations() {
  lcd.clear();          // Clear the screen
  lcd.setCursor(0, 0);  // Start from the first position
  lcd.print("CONGRATULATIONS!");
  lcd.setCursor(0, 1);  // Start from the first position
  lcd.print("PRESS(*) TO OPEN");

  for (int thisNote = 0; thisNote < sizeof(melody) / sizeof(int); thisNote++) {
    if (keypad.getKey() == '*') {
      openBox();
      stopMelodyAndLEDs();
      return;  // Exit the melody playback function
    }

    int noteDuration = 1000 / noteDurations[thisNote];  // Calculate duration of the note
    tone(buzzerPin, melody[thisNote], noteDuration);    // Play the note

    if (thisNote % 2 == 0) {
      digitalWrite(greenLed, HIGH);
      digitalWrite(redLed, LOW);
    } else {
      digitalWrite(greenLed, LOW);
      digitalWrite(redLed, HIGH);
    }

    // Pause between notes to distinguish them
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // Stop the tone
    noTone(buzzerPin);
  }
  delay(200);
  resetAll();
}

void showCurrentLevel() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STAGE " + String(currStage));
  lcd.setCursor(0, 1);
  lcd.print("QUESTION " + String(currLevel));
  delay(2000);
}

String arrangeNumbers(int a, int b, int c) {
  // Place the numbers into an array
  int numbers[3] = { a, b, c };

  // Sort the array
  for (int i = 0; i < 2; i++) {
    for (int j = i + 1; j < 3; j++) {
      if (numbers[i] > numbers[j]) {
        int temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
      }
    }
  }

  // numbers[0] = smallest, numbers[1] = second largest, numbers[2] = largest
  return String(numbers[0]) + String(numbers[2]) + String(numbers[1]);
}

// get the current question being displayed or answered
String getQuestion() {
  int idx = getIndex();
  Serial.println("Question: " + String(idx));

  strcpy_P(buffer, (char*)pgm_read_word(&(questions[idx])));
  return buffer;
}

int getCorrectAnswer() {
  return answers[getIndex()];
}

int getIndex() {  // get the index of the current question being process
  Serial.println("Stage: " + String(currLevel));
  Serial.println("Level: " + String(currLevel));
  Serial.println("Set: " + String(currSet));

  return ((currStage - 1) * 9) + ((currSet - 1) * 3) + (currLevel - 1);
}

void correctBeep() {
  digitalWrite(greenLed, HIGH);
  tone(buzzerPin, 3500, 100);  // Start buzzer tone
  delay(100);                  // Wait for the buzzer tone duration
  tone(buzzerPin, 3500, 100);  // Start the second buzzer tone
  delay(100);                  // Wait for the second buzzer tone duration
  // Let the LED stay on for an additional time
  delay(700);                   // LED stays on longer (adjust as needed)
  digitalWrite(greenLed, LOW);  // Turn off the LED
}

void incorrectBeep() {
  digitalWrite(redLed, HIGH);
  tone(buzzerPin, 1000, 500);  // Start buzzer tone
  delay(500);                  // Wait for the buzzer tone duration
  // Let the LED stay on for an additional time
  delay(700);                 // LED stays on longer (adjust as needed)
  digitalWrite(redLed, LOW);  // Turn off the LED
}


void correctAnswer() {
  errorCount = 0;  // reset any error
  correctBeep();
  inputString = "";

  sumAnswer += answer;

  if ((currStage == 3) && (currLevel == 3)) {
    mean3 = (sumAnswer / 3);  // get the last mean
    isFinalStage = true;
    if (!isInProd()) {
      // for development purpose only
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("FINAL STAGE");
      lcd.setCursor(0, 1);
      lcd.print("CODE IS: " + arrangeNumbers(mean1, mean2, mean3));
      delay(2000);
    }
    displayFinal();
    // congratulations();
  } else {
    if (currLevel == 3) {
      if (currStage == 1)
        mean1 = (sumAnswer / 3);
      else
        mean2 = (sumAnswer / 3);

      sumAnswer = 0;
      currSet = random(1, 4);
      currLevel = 1;
      currStage++;
    } else {
      currLevel++;  // increase level
    }

    // After answering correctly, show the current level
    showCurrentLevel();

    isAnswering = false;
    displayingQuestion = true;
    currentIndex = 0;
    displayQuestion();
  }
}


void incorrectAnswer() {
  incorrectBeep();

  inputString = "";  // reset answer
  errorCount++;

  if (errorCount == CHANCESCOUNT) {
    lcd.clear();          // Clear the screen
    lcd.setCursor(0, 0);  // Start from the first position
    lcd.print("SORRY!");
    delay(2500);
    resetFunc();
  }
}

void nextSegment() {
  // Move to the next segment if possible
  if (currentIndex + displayLength < question.length()) {
    currentIndex += displayLength;
    displayQuestion();
    askAnswer = false;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);  // Start from the first position
    lcd.print("Answer? (B)");
    lcd.setCursor(0, 1);
    lcd.print("Return? (C)");
    askAnswer = true;
  }
}

void prevSegment() {
  askAnswer = false;
  // Move to the previous segment if possible
  if (currentIndex - displayLength >= 0) {
    currentIndex -= displayLength;
    displayQuestion();
  }
}

void displayQuestion() {
  question = getQuestion();
  Serial.println(question);

  String displayText = question.substring(currentIndex, currentIndex + 32);

  if (isInProd()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(displayText.substring(0, 16));  // First row
    if (displayText.length() > 16) {
      lcd.setCursor(0, 1);
      lcd.print(displayText.substring(16));  // Second row, if there's more text
    }
  } else {
    // for development purposes only
    // this block can be deleted or commented before uploading final code
    question = "";        // make it blank
    lcd.clear();          // Clear the screen
    lcd.setCursor(0, 0);  // Start from the first position
    lcd.print("STAGE " + String(currStage) + "-" + String(currLevel));
    lcd.setCursor(0, 1);                                    // Start from the first position
    lcd.print("ANSWER IS: " + String(getCorrectAnswer()));  // Show THE ANSWER
    delay(2000);
    askAnswer = true;
    isAnswering = true;
    displayingQuestion = false;
    // end of development code
  }
}

void displayFinal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FINAL STAGE (" + String((3 - errorCount)) + ")");
  lcd.setCursor(0, 1);
  lcd.print("ENTER CODE: " + inputString);
}

void displayAnswer() {
  String s = "S";
  lcd.clear();
  lcd.setCursor(0, 0);
  if ((CHANCESCOUNT - errorCount) < 2)
    s = "";
  lcd.print("CHANCE" + s + ": " + String(CHANCESCOUNT - errorCount));
  lcd.setCursor(0, 1);
  lcd.print("ANSWER: " + inputString);
}
