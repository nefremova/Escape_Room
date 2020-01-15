#include <PololuLedStrip.h>

// Define buttons 
#define blueButton 3
#define yellowButton 4
#define redButton 5
#define greenButton 6

// Define color ranges
#define range1Begin 1
#define range1End 9
#define range2Begin 11
#define range2End 19
#define range3Begin 21
#define range3End 29
#define range4Begin 31
#define range4End 39

enum GameState {
  TOGGLE,
  HOLD,
  CYCLE,
  MEMORY,
  FINAL
};

GameState currGameState;

// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;


// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 40
#define BLUE 0
#define YELLOW 1
#define RED 2
#define GREEN 3
#define WHITE 4

rgb_color Colors[5];
rgb_color currColors[LED_COUNT];

const rgb_color AllOff[LED_COUNT];
const rgb_color key[LED_COUNT];

byte currLevelState = 1;
byte tailOfCycle = 0; //cannot be greater than 39
bool memoryAttempting = false;

void setup(){
  SetupConstants();
  SetupButtons();
  currGameState = TOGGLE;
  ChangeAnimation(0);
  Serial.begin(9600);
}

void SetupConstants(){

  Colors[BLUE] = rgb_color(0, 0, 10);
  Colors[YELLOW] = rgb_color(10, 10, 0);
  Colors[RED] = rgb_color(10, 0, 0);
  Colors[GREEN] = rgb_color(0, 10, 0);
  Colors[WHITE] = rgb_color(10, 10, 10);

  for(byte i = 0; i < LED_COUNT; i++){
    AllOff[i] = rgb_color(0,0,0);
    
    if (i % 10 == 0) key[i] = Colors[WHITE];
    else if (i <= range1End) key[i] = Colors[BLUE];
    else if (i <= range2End) key[i] = Colors[YELLOW];
    else if (i <= range3End) key[i] = Colors[RED];
    else key[i] = Colors[GREEN];
  }

}

void SetupButtons(){
  pinMode(redButton, INPUT);
  pinMode(blueButton, INPUT);
  pinMode(yellowButton, INPUT);
  pinMode(greenButton, INPUT);
}

void ChangeAnimation(byte GameNumber){
  for (byte i = 0; i < LED_COUNT; i++) {
    currColors[i] = key[i];
    ledStrip.write(currColors, LED_COUNT);
    delay(100);
  }

  resetCurrColors();
  
  for (byte i = 0; i <= GameNumber * 10; i+= 10){
    currColors[i] = Colors[RED];
  }

  for (byte i = GameNumber * 10; i < LED_COUNT; i += 10){
    currColors[i] = Colors[WHITE];
  }

  ledStrip.write(currColors, LED_COUNT);
  delay(200);
  ledStrip.write(AllOff, LED_COUNT);
  delay(200);
  ledStrip.write(currColors, LED_COUNT);
  delay(200);
  ledStrip.write(AllOff, LED_COUNT);
  delay(200);
  ledStrip.write(currColors, LED_COUNT);
  delay(200);
  ledStrip.write(AllOff, LED_COUNT);
  delay(1000);
  resetCurrColors();
}

void loop(){
  HandleWinCondition();
  HandleLEDs();
  HandleButtonPresses();

  //draw only once and at the end of the loop
  ledStrip.write(currColors, LED_COUNT);
}

void resetCurrColors(){
  for (byte i = 0; i < LED_COUNT; i++) {
    currColors[i] = rgb_color(0, 0, 0);
  }
}

void HandleWinCondition(){
  bool hasWon = true;

  for (byte i = 0; i < LED_COUNT; i++){
    if (currColors[i].red != key[i].red || currColors[i].blue != key[i].blue || currColors[i].green != key[i].green){
      hasWon = false;
    }
  }

  if (hasWon){
    switch (currGameState){
      case TOGGLE:
        delay(1000);
        currGameState = HOLD;
        resetCurrColors();
        ChangeAnimation(1);
        break;
      case HOLD:
        delay(1000);
        currGameState = CYCLE;
        resetCurrColors();
        ChangeAnimation(2);
        break;
      case CYCLE:
        delay(1000);
        currGameState = MEMORY;
        resetCurrColors();
        ChangeAnimation(3);
        break;
      case MEMORY:
        delay(1000);
        currGameState = FINAL;
        resetCurrColors();
        ChangeAnimation(4);
        break;
      default:
        resetCurrColors();
        break;
    }
  }
}

void HandleLEDs(){
  switch(currGameState){
    case TOGGLE:
      HandleGame1LEDs();
      break;
    case HOLD:
      HandleGame2LEDs();
      break;
    case CYCLE:
      HandleGame3LEDs();
      break;
    case MEMORY:
      HandleGame4LEDs();
      break;
    case FINAL:
      HandleFinalLEDs();
      break;
  }
}

void HandleGame1LEDs() {
  currColors[0] = Colors[WHITE];
  currColors[10] = Colors[WHITE];
  currColors[20] = Colors[WHITE];
  currColors[30] = Colors[WHITE];
}

void HandleGame2LEDs(){
  resetCurrColors();
  currColors[0] = Colors[WHITE];
  currColors[10] = Colors[WHITE];
  currColors[20] = Colors[WHITE];
  currColors[30] = Colors[WHITE];
}

void HandleGame3LEDs(){
  //2, 4, 6, 8 which is 2 * currLevelState
  resetCurrColors();
  currColors[0] = Colors[WHITE];
  currColors[10] = Colors[WHITE];
  currColors[20] = Colors[WHITE];
  currColors[30] = Colors[WHITE];
  rgb_color currCycleColor = Colors[currLevelState - 1];

  for (byte i = tailOfCycle ; i < (currLevelState * 2) + tailOfCycle; i++) {
    currColors[i % 40] = currCycleColor;
  }

  tailOfCycle = (tailOfCycle + 1) % 40;
  delay(100);
}

void HandleGame4LEDs(){
  resetCurrColors();
  
  if (!memoryAttempting) {
     ChangeSectionToColor(3, Colors[RED]);
     ledStrip.write(currColors, LED_COUNT);
     delay(500);
     ledStrip.write(AllOff, LED_COUNT);
     delay(500);
     ledStrip.write(currColors, LED_COUNT);
     delay(500);
     ledStrip.write(AllOff, LED_COUNT);
     delay(500);
     ledStrip.write(currColors, LED_COUNT);
     delay(500);
     ledStrip.write(AllOff, LED_COUNT);
     delay(500);
     resetCurrColors();
     
     ChangeSectionToColor(4, Colors[GREEN]);
     ledStrip.write(currColors, LED_COUNT);
     delay(500);
     ledStrip.write(AllOff, LED_COUNT);
     delay(500);
     resetCurrColors();
      
     ChangeSectionToColor(1, Colors[BLUE]);
     ledStrip.write(currColors, LED_COUNT);
     delay(500);
     ledStrip.write(AllOff, LED_COUNT);
     delay(500);
     resetCurrColors();
     
     ChangeSectionToColor(4, Colors[GREEN]);
     ledStrip.write(currColors, LED_COUNT);
     delay(500);
     ledStrip.write(AllOff, LED_COUNT);
     delay(500);
     ledStrip.write(currColors, LED_COUNT);
     delay(500);
     ledStrip.write(AllOff, LED_COUNT);
     delay(500);
     resetCurrColors();
     
     ChangeSectionToColor(2, Colors[YELLOW]);
     ledStrip.write(currColors, LED_COUNT);
     delay(500);
     ledStrip.write(AllOff, LED_COUNT);
     delay(500);
     resetCurrColors();
  }
  memoryAttempting = true;
}

void HandleFinalLEDs() {
  currColors[0] = Colors[WHITE];
  for (byte i = 1; i < 6; i++) {
    currColors[i] = Colors[BLUE];
  }
  currColors[10] = Colors[WHITE];
  for (byte i = 11; i < 13; i++) {
    currColors[i] = Colors[YELLOW];
  }
  currColors[20] = Colors[WHITE];
  for (byte i = 21; i < 28; i++) {
    currColors[i] = Colors[RED];
  }
  currColors[30] = Colors[WHITE];
  for (byte i = 31; i < 35; i++) {
    currColors[i] = Colors[GREEN];
  }
}

void HandleButtonPresses(){
  switch(currGameState){
    case TOGGLE:
      HandleGame1Buttons();
      break;
    case HOLD:
      HandleGame2Buttons();
      break;
    case CYCLE:
      HandleGame3Buttons();
      break;
    case MEMORY:
      HandleGame4Buttons();
      break;
  }
}

byte redIndex = 0;
byte blueIndex = 0;
byte greenIndex = 0;
byte yellowIndex = 0;

void HandleGame1Buttons(){
  //this is when the colors are flipping through the colors per button press
  
  
  if(digitalRead(redButton) == LOW){
    ChangeSectionToColor(3, Colors[redIndex % 4]);
    redIndex++;
    delay(200);
  }
  
  if(digitalRead(blueButton) == LOW){
    ChangeSectionToColor(1, Colors[blueIndex % 4]);
    blueIndex++;
    delay(200);
  } 
  if(digitalRead(greenButton) == LOW){
    ChangeSectionToColor(4, Colors[greenIndex % 4]);
    greenIndex++;
    delay(200);
  }
  if(digitalRead(yellowButton) == LOW){
    ChangeSectionToColor(2, Colors[yellowIndex % 4]);
    yellowIndex++;
    delay(200);
  }
}

void HandleGame2Buttons(){
  if (digitalRead(redButton) == LOW){
    ChangeSectionToColor(3, Colors[RED]);
    delay(200);
  }
  if (digitalRead(blueButton) == LOW){
    ChangeSectionToColor(1, Colors[BLUE]);
    delay(200);
  }
  if (digitalRead(yellowButton) == LOW){
    ChangeSectionToColor(2, Colors[YELLOW]);
    delay(200);
  }
  if (digitalRead(greenButton) == LOW){
    ChangeSectionToColor(4, Colors[GREEN]);
    delay(200);
  }
}

void HandleGame3Buttons(){
  if (digitalRead(greenButton) == LOW) {
    if (currLevelState == 4) {
      ledStrip.write(AllOff, LED_COUNT);
      delay(100);
      ledStrip.write(currColors, LED_COUNT);
      delay(100);
      ledStrip.write(AllOff, LED_COUNT);
      delay(100);
      ledStrip.write(currColors, LED_COUNT);
      delay(100);
  
      if (Game3Win()) {
        for (byte i = 0; i < LED_COUNT; i++) {
          currColors[i] = key[i];
        }
      }
      else {
        currLevelState = 1;
      }
    }
    else 
      currLevelState = 1;
  }

  if (digitalRead(redButton) == LOW) {
    if (currLevelState == 3) {
      ledStrip.write(AllOff, LED_COUNT);
      delay(100);
      ledStrip.write(currColors, LED_COUNT);
      delay(100);
      ledStrip.write(AllOff, LED_COUNT);
      delay(100);
      ledStrip.write(currColors, LED_COUNT);
      delay(100);
  
      if (Game3Win()) {
        currLevelState = 4;
      }
      else {
        currLevelState = 1;
      }
    }
    else 
      currLevelState = 1;
  }

  if (digitalRead(yellowButton) == LOW) {
    if (currLevelState == 2) {
      ledStrip.write(AllOff, LED_COUNT);
      delay(100);
      ledStrip.write(currColors, LED_COUNT);
      delay(100);
      ledStrip.write(AllOff, LED_COUNT);
      delay(100);
      ledStrip.write(currColors, LED_COUNT);
      delay(100);
  
      if (Game3Win()) {
        currLevelState = 3;
      }
      else {
        currLevelState = 1;
      }
    }
    else
      currLevelState = 1;
  }

  if (digitalRead(blueButton) == LOW) {
    if (currLevelState == 1) {
      ledStrip.write(AllOff, LED_COUNT);
      delay(100);
      ledStrip.write(currColors, LED_COUNT);
      delay(100);
      ledStrip.write(AllOff, LED_COUNT);
      delay(100);
      ledStrip.write(currColors, LED_COUNT);
      delay(100);
  
      if (Game3Win()) {
        currLevelState = 2;
      }
      else {
        currLevelState = 1;
      }
    }
    else 
      currLevelState = 1;
   }
}

bool Game3Win() {
  int levelSectionBegin = 0;
  int levelSectionEnd = 0;
  rgb_color levelColor = rgb_color(0, 0, 0);
  
  switch(currLevelState) {
    case 1: 
      levelSectionBegin = range1Begin;
      levelSectionEnd = range1End;
      levelColor = Colors[BLUE];
      break;
    case 2:
      levelSectionBegin = range2Begin;
      levelSectionEnd = range2End;
      levelColor = Colors[YELLOW];
      break;
    case 3:
      levelSectionBegin = range3Begin;
      levelSectionEnd = range3End;
      levelColor = Colors[RED];
      break;
    case 4:
      levelSectionBegin = range4Begin;
      levelSectionEnd = range4End;
      levelColor = Colors[GREEN];
      break;
  }
  
  for (byte i = levelSectionBegin; i < levelSectionEnd; i++) {
    if (currColors[i].red == levelColor.red && currColors[i].green == levelColor.green && currColors[i].blue == levelColor.blue
        && i + (currLevelState * 2) <= levelSectionEnd) {
      return true;
    }
  }

  return false;
}

byte memoryQueue = 1;
long userAttemptingMemory = 0;
#define MEMORY_KEY 24414333
//8 is the max number of color memory
void HandleGame4Buttons() {
  long j = 1;
  for (byte i = 0; i < memoryQueue - 1; i++){
    j *= 10;
  }
  
  if (digitalRead(blueButton) == LOW){
    ChangeSectionToColor(1, Colors[BLUE]);
    userAttemptingMemory += 1 * j;
    memoryQueue++;
    memoryAttempting = true;
    checkGame4Progress();
    delay(200);
  }
  if (digitalRead(yellowButton) == LOW){
    ChangeSectionToColor(2, Colors[YELLOW]);
    userAttemptingMemory += 2 * j;
    memoryQueue++;
    memoryAttempting = true;
    checkGame4Progress();
    delay(200);
  }
  if (digitalRead(redButton) == LOW){
    ChangeSectionToColor(3, Colors[RED]);
    userAttemptingMemory += 3 * j;
    memoryQueue++;
    memoryAttempting = true;
    checkGame4Progress();
    delay(200);
  }
  if (digitalRead(greenButton) == LOW){
    ChangeSectionToColor(4, Colors[GREEN]);
    userAttemptingMemory += 4 * j;
    memoryQueue++;
    memoryAttempting = true;
    checkGame4Progress();
    delay(200);
  }

  //checkGame4Progress();
}

void checkGame4Progress(){
  long j = 1;
  for (byte i = 0; i < memoryQueue - 1; i++){
    j *= 10;
  }
  
  Serial.println(userAttemptingMemory);
  Serial.println(round((pow(10, memoryQueue - 1))));
  Serial.println(MEMORY_KEY % j);

  
  if (userAttemptingMemory == 0) return;
  if (userAttemptingMemory == MEMORY_KEY % j){
    if (memoryQueue > 8) {
      for (byte i = 0; i < LED_COUNT; i++) {
        currColors[i] = key[i];
      }
    }
  } else {
    //fail, clean-up
      userAttemptingMemory = 0;
      memoryQueue = 1;
      memoryAttempting = false;
  }
}

rgb_color GetSectionColor(byte section) {
  return currColors[section * 10 - 5];
}

void ChangeSectionToColor(byte section, rgb_color changeTo){ ∑
  switch(section){
    case 1:
      for(int i = range1Begin; i <= range1End; i++)
        currColors[i] = changeTo;
      break;
    case 2:
      for(int i = range2Begin; i <= range2End; i++)
        currColors[i] = changeTo;
      break;
    case 3:
      for(int i = range3Begin; i <= range3End; i++)
        currColors[i] = changeTo;
      break;
    case 4:
      for(int i = range4Begin; i <= range4End; i++)
        currColors[i] = changeTo;
      break;
  } 
}
