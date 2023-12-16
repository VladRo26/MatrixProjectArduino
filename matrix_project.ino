#include "LedControl.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

const int dinPin = 12;
const int clockPin = 13;
const int loadPin = 10;
const int xPin = A0;
const int yPin = A1;
const int swPin = 2;
const int buzzerPin = 11;
const int lcdBrightnessPin = 5; // PWM pin connected to LCD A pin

bool introDisplayed = false;
unsigned long introStartTime = 0;
const unsigned long introDuration = 3000; 

const int totalMenuItems = 5; // Total number of items in the menu
int currentMenuItem = 0; // Current selected menu item
int lastMenuItem = -1; // Initialize with -1 to ensure the menu is drawn the first time

unsigned long lastJoystickMove = 0;
const unsigned int joystickDebounceTime = 300;
const unsigned int joystickDebounceTimeName = 500;

unsigned long lastUpdateTime = 0; // For LCD update interval
const unsigned long updateInterval = 500; // Update interval for Matrix

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 3;
const byte d7 = 4;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte swState = HIGH;
unsigned int lastDebounceTime = 0;
unsigned int debounceDelay = 200;
int playerVisible = true;
const unsigned long playerBlinkInterval = 700;
unsigned long lastPlayerBlinkTime = 0;
int wallCount = 0;  
bool isShooting = false;
bool firstGameRun = true; 
unsigned long shootingStartTime = 0;
const unsigned long shootingDuration = 1000; 
const unsigned long shootingBlinkInterval = 100;
unsigned long highscore = 0;


int currentLevel = 1; // Current game level
const int maxLevel = 3; // Maximum number of levels

unsigned long gameStartTime = 0;

unsigned long lastMenuEnterTime = 0;
const unsigned long menuEnterDelay = 500; 
bool displayLink = false; // Global variable to control display of GitHub link

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

const byte matrixSize = 8;

byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;


const int minThreshold = 200;
const int maxThreshold = 600;
const byte moveInterval = 100; 
unsigned long long lastMoved = 0;
bool matrixChanged = true;

char playerName[4] = "AAA"; 
int playerNamePosition = 0; // Position within the name (0, 1, or 2)

int currentHighScoreIndex = 0; // Global variable to track the current displayed high score


byte matrix[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

byte playIcon[8] = {
  B00001,
  B00111,
  B00100,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110
};

byte settingsIcon[8] = {
  B01010,
  B10001,
  B01010,
  B01110,
  B00100,
  B00100,
  B00100,
  B01110
};


byte highscoreIcon[8] = {
  B01110,
  B11011,
  B11011,
  B01110,
  B00100,
  B00100,
  B01110,
  B11111
};

byte aboutIcon[8] = {
  B00000,
  B00100,
  B00000,
  B00100,
  B00100,
  B00100,
  B00000,
  B00000
};

byte howToPlayIcon[8] = {
  B00000,
  B01110,
  B00010,
  B01110,
  B01000,
  B01000,
  B00000,
  B01000
};

const byte heartPattern[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  B00000000
};


byte upArrow[8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00000
};

byte downArrow[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100,
  B00000
};


enum GameState {
  INTRO,
  MENU,
  PLAYING,
  LEVEL_COMPLETE,
  WAIT_AFTER_LEVEL,
  ADJUST_LCD_BRIGHTNESS,
  ADJUST_MATRIX_BRIGHTNESS,
  RETURN_TO_SETTINGS,
  DISPLAY_HIGHSCORE,
  DISPLAY_SCORE,
  ALERT_HIGHSCORE,
  SETTINGS,
  ADJUST_WALL_RANDOMNESS,
  HIGHSCORE,
  CHANGE_NAME,
  CHANGE_SOUND,
  ABOUT,
  RESET_HIGHSCORES_CONFIRM,
  HOW_TO_PLAY
};

enum MenuSelection {
  START_GAME,
  SETTINGS_MENU,
  HIGHSCORE_MENU,
  ABOUT_MENU,
  HOW_TO_PLAY_MENU 
};

enum EndMenuSelection {
  NEXT_LEVEL,
  MAIN_MENU
};

enum SettingsMenuOption {
    LCD_BRIGHTNESS,
    MATRIX_BRIGHTNESS,
    SET_NAME, 
    WALL_RANDOMNESS,
    SOUND_SETTING,
    RESET_HIGHSCORES,
    SETTINGS_EXIT
};

int currentSettingsMenu = 0; // Current selected settings option
const int totalSettingsOptions = 7; // Total number of options in the settings menu
EndMenuSelection endMenuSelection = NEXT_LEVEL; // Current selection in the end-of-level menu
GameState currentState = INTRO;
int menuPosition = 0; // Position in the menu
unsigned long timeTakenForLevel[3] = {0, 0, 0}; // Array to store time taken for each level


unsigned long lastMatrixBrightnessAdjustTime = 0;
const unsigned long matrixBrightnessAdjustInterval = 100; 
const int maxMatrixBrightness = 15; 
int matrixBrightness = 8; 

unsigned long lastBrightnessAdjustTime = 0;
const unsigned long brightnessAdjustInterval = 100; 
int playerIntensity = 0;
int wallIntensity  = 0;
int bombIntensity = 0;
int lcdBrightness = 128; // Default brightness (0-255)
const int EEPROM_LCD_BRIGHTNESS_ADDR = 0; // Address to store LCD brightness in EEPROM
const int EEPROM_MATRIX_BRIGHTNESS_ADDR = sizeof(int); // EEPROM address for matrix brightness
int wallRandomness = 50;
const int EEPROM_HIGHSCORES_ADDR = EEPROM_MATRIX_BRIGHTNESS_ADDR + sizeof(int); // Starting address for high scores
int howToPlayMenuPosition = 0; // 0 for "Go Back", 1 for "Rules"


struct HighScore {
    unsigned long score;
    char name[4]; // 3 characters + null terminator
};

const int highScoresCount = 3;
HighScore highScores[highScoresCount];


// Default high scores and names
const unsigned long defaultHighScores[highScoresCount] = {0, 0, 0};
const char defaultNames[highScoresCount][4] = {"AAA", "AAA", "AAA"};

int betterThanPeople = 0;

const int EEPROM_WALL_RANDOMNESS_ADDR = EEPROM_HIGHSCORES_ADDR + sizeof(HighScore) * highScoresCount; // EEPROM address for saving the top three highscores

bool soundEnabled = true; // Default sound setting
const int EEPROM_SOUND_ADDR = EEPROM_WALL_RANDOMNESS_ADDR + sizeof(int); // EEPROM address for settings the wall randomness

bool resetHighscores = false; // Default is 'no'
const int EEPROM_RESET_HIGHSCORES_ADDR = EEPROM_SOUND_ADDR + sizeof(bool); // EEPROM address for reset highscores setting


void setup() {
  lcd.begin(16, 2);
  lc.shutdown(0, false);
  lc.setIntensity(0, 4);  
  lc.clearDisplay(0);
  pinMode(swPin, INPUT_PULLUP);
  pinMode(lcdBrightnessPin, OUTPUT);
  EEPROM.get(EEPROM_LCD_BRIGHTNESS_ADDR, lcdBrightness);
  analogWrite(lcdBrightnessPin, lcdBrightness); 
  EEPROM.get(EEPROM_MATRIX_BRIGHTNESS_ADDR, matrixBrightness);
  loadHighScores(); 
  lc.setIntensity(0, matrixBrightness); 
  EEPROM.get(EEPROM_WALL_RANDOMNESS_ADDR, wallRandomness);
  EEPROM.get(EEPROM_SOUND_ADDR,soundEnabled);
  introDisplayed = false; 
  lcd.createChar(0, playIcon);
  lcd.createChar(1, settingsIcon);
  lcd.createChar(2, highscoreIcon);
  lcd.createChar(3, aboutIcon);
  lcd.createChar(4, howToPlayIcon);
  lcd.createChar(5, upArrow);
  lcd.createChar(6, downArrow);
}

//This set the wall, player and bombs intensity based on the matrix brightness that it's set from the game options
void setGameIntensity(){
  if(matrixBrightness >= 14){
    playerIntensity = 13;
  }
  else if(matrixBrightness ==1){
    playerIntensity = 2;
  }
  else{
      playerIntensity = matrixBrightness;
  }
  wallIntensity = playerIntensity - 1;
  bombIntensity = playerIntensity +2;
}

void loadHighScores() {
    for (int i = 0; i < highScoresCount; i++) {
        EEPROM.get(EEPROM_HIGHSCORES_ADDR + sizeof(HighScore) * i, highScores[i]);
    }
}


void resetDefaultHighscore() {
    for (int i = 0; i < highScoresCount; i++) {
        highScores[i].score = 0; 
        strncpy(highScores[i].name, "AAA", 3); 
        highScores[i].name[3] = '\0'; 
        EEPROM.put(EEPROM_HIGHSCORES_ADDR + sizeof(HighScore) * i, highScores[i]);
    }
}


void displayCurrentHighScore() {

    if (currentHighScoreIndex < 0) currentHighScoreIndex = 0;
    if (currentHighScoreIndex > highScoresCount - 1) currentHighScoreIndex = highScoresCount - 1;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(currentHighScoreIndex + 1);
    lcd.print(F(". "));
    lcd.print(highScores[currentHighScoreIndex].name);
    lcd.print(F(" "));
    lcd.print(highScores[currentHighScoreIndex].score);
}

void displayHighScoreAlert() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("New Highscore!"));
    lcd.setCursor(0, 1);
    lcd.print(highScores[0].score);
}


void selectMenuItem() {
    switch (currentMenuItem) {
        case START_GAME:
            currentState = PLAYING;
            break;
        case SETTINGS_MENU:
            currentState = SETTINGS;
            break;
        case ABOUT_MENU:
            currentState = ABOUT;
            break;
        case HIGHSCORE_MENU:
            currentState = DISPLAY_HIGHSCORE;
            currentHighScoreIndex = 0; 
            break;
         case HOW_TO_PLAY_MENU:
            currentState = HOW_TO_PLAY;
            break;
    }
}

//here are the states handled
void loop() {
    switch (currentState) {
        case INTRO:
            if (!introDisplayed) {
                lcd.clear();
                lcd.print(F("Welcome to"));
                lcd.setCursor(0, 1);
                lcd.print(F("Turis Game!"));
                introDisplayed = true;
                introStartTime = millis();
                playThemeSong();
            } else if (millis() - introStartTime > introDuration) {
                lcd.clear();
                introDisplayed = false;
                currentState = MENU;
            }
            break;
        case MENU:
            displayMenu();
            handleJoystickMenu();
            break;
        case PLAYING:
            game(); 
            break;
        case WAIT_AFTER_LEVEL:
            if (millis() - gameStartTime > 1000) { 
                currentState = LEVEL_COMPLETE;
                displayEndLevelMenu();
            }
            break;
        case LEVEL_COMPLETE:
            if (currentLevel == maxLevel) {
              unsigned long score = calculateTotalScore();
              checkAndUpdateHighScore(score, playerName); 
              displayEnd(score);
              currentState = DISPLAY_SCORE;
            } else {
              handleJoystickEndLevelMenu();
            }
            break;
        case DISPLAY_HIGHSCORE:
          handleHighScoreJoystick();
          break;
        case HOW_TO_PLAY:
          handleHowToPlayJoystick();
          break;
        case SETTINGS:
            displaySettingsMenu();
            handleJoystickSettings();
            break;
        case ADJUST_LCD_BRIGHTNESS:
            adjustLCDBrightness();
            break;
         case ADJUST_MATRIX_BRIGHTNESS:
            adjustMatrixBrightness();
            break;
        case ABOUT:
        handleAboutJoystick(); 
        displayAboutScreen(); 
        break;
        case DISPLAY_SCORE:
            buttonPress();
            break;
        case ALERT_HIGHSCORE:
            playAlertHighScoreSong();
            buttonPress();
            break;
        case CHANGE_NAME:
          handleNameChange();
          break;
        case ADJUST_WALL_RANDOMNESS:
        handleWallsJoystick();
        break;
        case CHANGE_SOUND:
        handleJoystickSound();
        break;
        case RESET_HIGHSCORES_CONFIRM:
        handleResetHighscoresConfirm();
        break;
    }
}


void displayEnd(int score){
  lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(F("Score: "));
                lcd.print(score);
                lcd.setCursor(0, 1);
                lcd.print(F("Better than "));
                lcd.print(betterThanPeople);
}


void displayHowToPlay() {
    lcd.clear();
    if (howToPlayMenuPosition == 0) {
        lcd.setCursor(0, 0);
        lcd.print(F("Press joy button"));
        lcd.setCursor(0, 1);
        lcd.print(F("to go back"));
    } else {
        lcd.setCursor(0, 0);
        lcd.print(F("Rules: Destroy"));
        lcd.setCursor(0, 1);
        lcd.print(F("walls quickly!"));
    }
}

void displayResetHighscoresConfirm() {
    lcd.clear();
    lcd.print(F("Reset Highscores?"));
    lcd.setCursor(0, 1);
    lcd.print(resetHighscores ? "Yes" : "No");
}


void handleResetHighscoresConfirm() {
    int xValue = analogRead(xPin);
    if (millis() - lastJoystickMove > joystickDebounceTime) {
        if (xValue < minThreshold || xValue > maxThreshold) {
            resetHighscores = !resetHighscores; 
            displayResetHighscoresConfirm();
            lastJoystickMove = millis();
        }
    }
    buttonPress();
}

void handleHowToPlayJoystick() {
    int xValue = analogRead(xPin);

    if (xValue < minThreshold && howToPlayMenuPosition != 0) {
        howToPlayMenuPosition = 0; // Set to "Go Back"
        playMenuNavigationSound();
        displayHowToPlay(); 
    } else if (xValue > maxThreshold && howToPlayMenuPosition != 1) { 
        howToPlayMenuPosition = 1; // Set to "Rules"
        playMenuNavigationSound();
        displayHowToPlay(); 
    }
    buttonPress();
}

void handleWallsJoystick() {
    int yValue = analogRead(yPin);
    unsigned long currentMillis = millis();

    if (currentMillis - lastJoystickMove > joystickDebounceTime) {
        if (yValue > maxThreshold && wallRandomness > 30) {
            wallRandomness--;
            displayRandomness();
            lastJoystickMove = currentMillis;
        } else if (yValue < minThreshold && wallRandomness < 60) {
            wallRandomness++;
            displayRandomness();
            lastJoystickMove = currentMillis;
        }
    }

    buttonPress();
}

void displayRandomness() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Wall Randomness: "));
    lcd.setCursor(0, 1);
    lcd.print(wallRandomness);
    lcd.print(F("%"));
}

void playBombSound() {
    if(soundEnabled){
      tone(buzzerPin, 1000, 200); 
    }
}

void playLevelCompleteSound() {
   if(soundEnabled){
        tone(buzzerPin, 2000, 500); 
    }
}

void playMenuNavigationSound() {
   if(soundEnabled){
      tone(buzzerPin, 500, 100);
    }
}

void playSettingsNavigationSound() {
   if(soundEnabled){
      tone(buzzerPin, 300, 100); 
    }
}

void playButtonPressSound() {
   if(soundEnabled){
      tone(buzzerPin, 800, 100); 
    }
}

//THEME SONG LOGIC IMPLEMENTED WITH CHATGPT!!!
void playThemeSong() {
   if(soundEnabled){
     int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
    int noteDurations[] = {4, 4, 4, 4, 4, 4, 4, 4};

    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {
        // to calculate the note duration, take one second divided by the note type.
        // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(buzzerPin, melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        
        // stop the tone playing:
        noTone(buzzerPin);
    }
  }   
}

//THEME SONG LOGIC IMPLEMENTED WITH CHATGPT!!!
void playAlertHighScoreSong() {
   if(soundEnabled){
        // Define the melody and note durations
    int melody[] = {523, 523, 440, 523, 659, 784, 659, 523}; // Example notes in Hz
    int noteDurations[] = {4, 4, 4, 4, 4, 4, 4, 4}; // Duration of each note

    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {
        // to calculate the note duration, take one second divided by the note type.
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(buzzerPin, melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.
        buttonPress();
        int pauseBetweenNotes = noteDuration * 0.40;
        delay(pauseBetweenNotes);
        
        // stop the tone playing:
        noTone(buzzerPin);
    }
  }
}

void displayMenu() {
    // Check if the menu item has changed
    byte customIcons[] = {0, 1, 2, 3, 4}; // Array of custom character indices
    if (lastMenuItem != currentMenuItem) {
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print(F("Main Menu"));

        if (currentMenuItem > 0) {
            lcd.setCursor(15, 0); // Position cursor at the end of the first line
            lcd.write(byte(5)); // Custom character for up arrow
        }

        String menuItems[] = {"Play Game", "Settings", "Highscore", "About", "How to Play"};
        lcd.setCursor(0, 1);
        lcd.print("> "); // Arrow to indicate the current selection
        
        // Display the custom icon for the current menu item
        lcd.write(customIcons[currentMenuItem]); 

    
        lcd.setCursor(3, 1);
        lcd.print(menuItems[currentMenuItem]); // Print the menu item name

        // Display the down arrow at the end of the second line if not the last item
        if (currentMenuItem < totalMenuItems - 1) {
            lcd.setCursor(15, 1); // Position cursor at the end of the second line
            lcd.write(byte(6)); // Custom character for down arrow
        }

        lastMenuItem = currentMenuItem;
    }
}

void checkEnd() {
    wallCount = 0;
    for (int col = 0; col < matrixSize; col++) {
        for (int row = 0; row < matrixSize; row++) {
            if (matrix[row][col] == wallIntensity) {
                wallCount++;
            }
        }
    }

    if (wallCount == 0 && currentState == PLAYING) {
        lcd.clear();
        currentState = WAIT_AFTER_LEVEL;
        timeTakenForLevel[currentLevel - 1] = millis() - gameStartTime; // Store time taken for the level
        gameStartTime = millis(); // Reset gameStartTime to use as timer
        playLevelCompleteSound();
    }
}


//this function is used to see if my game score it's good enough to be in the top 3 higscores

void checkAndUpdateHighScore(unsigned long score, const char* name) {
    betterThanPeople = 0;  // Reset betterThanPeople

    for (int i = 0; i < highScoresCount; i++) {
        if (score > highScores[i].score) {
            // Shift down the high scores below the new high score
            for (int j = highScoresCount - 1; j > i; j--) {
                highScores[j] = highScores[j - 1];
            }

            // Update the new high score
            highScores[i].score = score;
            strncpy(highScores[i].name, name, 3);
            highScores[i].name[3] = '\0'; // Ensure null termination

            // Update EEPROM
            for (int k = 0; k < highScoresCount; k++) {
                EEPROM.put(EEPROM_HIGHSCORES_ADDR + sizeof(HighScore) * k, highScores[k]);
            }

            // Update betterThanPeople based on the position where the new score is inserted
            betterThanPeople = 3 - i; // 3 for the top score, 2 for the second, 1 for the third
            break; 
        }
    }
}


void displaySettingsMenu() {
    static int lastSettingsMenu = -1; // Keep track of the last settings menu item

    if (lastSettingsMenu != currentSettingsMenu) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Settings"));

        const char* settingsOptions[] = {"LCD Bright", "Matrix Bright", "Set Name", "Wall Density", "Sound", "Reset Scores", "Exit"};
        lcd.setCursor(0, 1); 
        lcd.print("> "); // Arrow to indicate the current selection
        lcd.print(settingsOptions[currentSettingsMenu]);

        // Display the up arrow at the end of the first line if not the first item
        if (currentSettingsMenu > 0) {
            lcd.setCursor(15, 0); 
            lcd.write(byte(5)); // Custom character for up arrow
        }

        // Display the down arrow at the end of the second line if not the last item
        if (currentSettingsMenu < totalSettingsOptions - 1) {
            lcd.setCursor(15, 1); 
            lcd.write(byte(6)); // Custom character for down arrow
        }
        lastSettingsMenu = currentSettingsMenu;
    }
}

void displaySoundSettings() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Sound"));
    lcd.setCursor(0, 1);
    if (soundEnabled) {
        lcd.print(F("ON"));
    } else {
        lcd.print(F("OFF"));
    }
}

void displayEndLevelMenu() {
    clearMatrix();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("End Level Menu"));

        String endMenuItems[] = {"Next Level", "Main Menu"};
        lcd.setCursor(0, 1);
        lcd.print("> ");
        lcd.print(endMenuItems[endMenuSelection]);
}

void handleAboutJoystick() {
    int xValue = analogRead(xPin); 

    if (xValue > maxThreshold) { // Joystick moved down
        lcd.setCursor(0, 1);
        lcd.print(F("                ")); // Clear the second line to not have two things displayed on the same time
        displayLink = true;
        playMenuNavigationSound();
    } else if (xValue < minThreshold) { 
        lcd.setCursor(0, 1);
        lcd.print(F("                ")); // Clear the second line two things displayed on the same time
        displayLink = false;
        playMenuNavigationSound();
    }
}

void displayAboutScreen() {
    const char* gameName = "Turis Game"; 
    const char* name = "Turis Vlad";
    const char* githubLink = "https://github.com/VladRo26/IntroductionToRobotics";
    static int githubLinkPosition = 0;
    static unsigned long lastScrollTime = 0;
    const int scrollInterval = 300;  // Time in milliseconds between scrolls

    lcd.setCursor(0, 0);
    lcd.print(gameName);

    // Decide what to display on the second line
    if (displayLink) {
        // Handle the scrolling GitHub link
        if (millis() - lastScrollTime > scrollInterval) {
            lcd.setCursor(0, 1);
            int linkLength = strlen(githubLink);
            // Create a substring of the link that fits the LCD's width (16 characters)
            // It s start at githubLinkPosition and goes either at the final, or either at the position plus 16 to fit on the LCD are 16 characters that can be displayed
            String toDisplay = String(githubLink).substring(githubLinkPosition, min(githubLinkPosition + 16, linkLength));
            lcd.print(toDisplay);

            githubLinkPosition++;
            if (githubLinkPosition > linkLength - 16) {
                githubLinkPosition = 0; // Reset scrolling for GitHub link
            }
            lastScrollTime = millis();
        }
    } else {
        // Clear and display the name on the second line
        lcd.setCursor(0, 1);
        lcd.print(name);
    }

    buttonPress();
}


void resetGame() {
  betterThanPeople= 0;
  clearMatrix();
  wallCount = 0;
  isShooting = false;
  firstGameRun = true; // Reset for next game start
}

/*
Calculates the total score based on level completion times.
Begins with a base score of 10000.
Subtracts a time penalty, calculated as 30 points for each second spent on a level.
Ensures the final score does not drop below zero.
Returns the calculated total score.
 */
unsigned long calculateTotalScore() {
    const unsigned long baseScore = 10000; 
    unsigned long timePenalty = 0;

    for (int i = 0; i < maxLevel; i++) {
        timePenalty += (timeTakenForLevel[i] / 1000) * 30; 
    }

    unsigned long totalScore = (baseScore > timePenalty) ? (baseScore - timePenalty) : 0;

    return totalScore;
}


//Clear the entire Matrix
void clearMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      matrix[row][col] = 0;
    }
  }
  updateMatrix();
}


void handleJoystickMenu() {
    int xValue = analogRead(xPin);
    unsigned long currentMillis = millis();

    if (currentMillis - lastJoystickMove > joystickDebounceTime) {
        if (xValue < minThreshold && currentMenuItem > 0) {
            currentMenuItem--;
            playMenuNavigationSound();
            displayMenu();
            lastJoystickMove = currentMillis;
        } else if (xValue > maxThreshold && currentMenuItem < totalMenuItems - 1) {
            currentMenuItem++;
            playMenuNavigationSound();
            displayMenu();
            lastJoystickMove = currentMillis;
        }
        buttonPress();
    }
}

void handleJoystickSound() {
    int xValue = analogRead(xPin);
    unsigned long currentMillis = millis();

    if (currentMillis - lastJoystickMove > joystickDebounceTime) {
          if (xValue < minThreshold || xValue > maxThreshold) {
        soundEnabled = !soundEnabled; // Toggle sound setting
        displaySoundSettings();
    }
  }
    buttonPress();
}


void handleJoystickEndLevelMenu() {
    int xValue = analogRead(xPin); 
    unsigned long currentMillis = millis();

    if (currentMillis - lastJoystickMove > joystickDebounceTime) {
        if (xValue < minThreshold && endMenuSelection != NEXT_LEVEL) {
            endMenuSelection = NEXT_LEVEL;
            lastJoystickMove = currentMillis;
            playMenuNavigationSound();
            displayEndLevelMenu(); 
        } else if (xValue > maxThreshold && endMenuSelection != MAIN_MENU) {
            endMenuSelection = MAIN_MENU;
            lastJoystickMove = currentMillis;
            playMenuNavigationSound();
            displayEndLevelMenu(); 
        }
        buttonPress();
    }

}

void handleHighScoreJoystick() {
    int xValue = analogRead(xPin); 
    unsigned long currentMillis = millis();

    if (currentMillis - lastJoystickMove > joystickDebounceTime) {
        if (xValue < minThreshold && currentHighScoreIndex > 0) {
            currentHighScoreIndex--;
            displayCurrentHighScore();
            playMenuNavigationSound();
            lastJoystickMove = currentMillis;
        } else if (xValue > maxThreshold && currentHighScoreIndex < highScoresCount - 1) {
            currentHighScoreIndex++;
            displayCurrentHighScore();
            playMenuNavigationSound();
            lastJoystickMove = currentMillis;
        }
    }
    buttonPress();
}


void handleNameChange() {
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);
    unsigned long currentMillis = millis();

    if (currentMillis - lastJoystickMove > joystickDebounceTimeName) {
        if (xValue < minThreshold) {
            playerName[playerNamePosition]++;
            if (playerName[playerNamePosition] > 'Z') {
                playerName[playerNamePosition] = 'A';
            }
            lcd.print(playerName[playerNamePosition]);
            lcd.setCursor(playerNamePosition,1);
            lastJoystickMove = currentMillis;
        } else if (xValue > maxThreshold) {
            playerName[playerNamePosition]--;
            if (playerName[playerNamePosition] < 'A') {
                playerName[playerNamePosition] = 'Z';
            }
            lcd.print(playerName[playerNamePosition]);
            lcd.setCursor(playerNamePosition,1);
            lastJoystickMove = currentMillis;
        }

    if (yValue < minThreshold) {
        playerNamePosition++;
        if (playerNamePosition > 2) {
            playerNamePosition = 0; 
        }
        lcd.setCursor(playerNamePosition,1);
        lastJoystickMove = currentMillis;
    } else if (yValue > maxThreshold) {
        playerNamePosition--;
        if (playerNamePosition < 0) {
            playerNamePosition = 2;
        }
        lcd.setCursor(playerNamePosition,1);
        lastJoystickMove = currentMillis;
    }
  }
    buttonPress();
}

void displayName(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Set Name: "));
    lcd.setCursor(0, 1);
    for (int i = 0; i < 3; i++) {
        lcd.print(playerName[i]);
    }

    // Place the cursor under the current character
    lcd.setCursor(playerNamePosition, 1);
    lcd.blink(); // Blink the cursor at the current position
}


void handleJoystickSettings() {
    int xValue = analogRead(xPin); 
    unsigned long currentMillis = millis();

    if (currentMillis - lastJoystickMove > joystickDebounceTime) {
        if (xValue < minThreshold && currentSettingsMenu > 0) {
            currentSettingsMenu--;
            displaySettingsMenu();
            playSettingsNavigationSound();
            lastJoystickMove = currentMillis;
        } else if (xValue > maxThreshold && currentSettingsMenu < totalSettingsOptions - 1) {
            currentSettingsMenu++;
            displaySettingsMenu();
            playSettingsNavigationSound();
            lastJoystickMove = currentMillis;
        }
    }
     buttonPress();
}

void game(){

   setGameIntensity();
  if (firstGameRun) {
    generateWalls();
    spawnPlayer();
    firstGameRun = false;
    gameStartTime = millis(); 
  }

  if (gameStartTime == 0) { 
    gameStartTime = millis();
  }

    if (millis() - lastMoved > moveInterval) {
    updatePositions();
    lastMoved = millis();
  }

  //Here it s handled the player blinking logic  
  if (millis() - lastPlayerBlinkTime > playerBlinkInterval) {
  playerVisible = !playerVisible;
  lastPlayerBlinkTime = millis();
  matrix[xPos][yPos] = playerVisible ? playerIntensity : 0; 
  matrixChanged = true;
  }
    
  buttonPress();


  if (isShooting) {
    unsigned long currentTime = millis();
    if (currentTime - shootingStartTime < shootingDuration) {
      /*This if it's for bomb blinking, when the time difference it's even
       it's shooting and when not it's not shooting to create the blink effect*/
      if ((currentTime - shootingStartTime) / shootingBlinkInterval % 2 == 0) {
        setBombs(bombIntensity);
      } else {
        setBombs(0); 
      }
    } else {
      setBombs(0); 
      isShooting = false;
      clearBombs(); 
    }
  }

  if (matrixChanged) {
    updateMatrix();
    matrixChanged = false;
  }

  if (millis() - lastUpdateTime > updateInterval) {
    unsigned long currentTime = millis();
    unsigned long timeElapsed = currentTime - gameStartTime;

    // Display time and level on the LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    lcd.print(timeElapsed / 1000); // Display time in seconds
    lcd.setCursor(0, 1);
    lcd.print("Level: ");
    lcd.print(currentLevel);
    lcd.print(" ");
    lcd.print(playerName);

    lastUpdateTime = millis(); 
  }
  checkEnd();
}


void clearBombs() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      if (matrix[row][col] == bombIntensity) {
        matrix[row][col] = 0; 
      }
    }
  }
  matrixChanged = true;
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col] > 0);
      lc.setIntensity(0, matrix[row][col]);  
    }
  }
}

void generateWalls(){
  randomSeed(analogRead(0));

  for(int row = 0; row < matrixSize; row++){
    for(int col = 0; col < matrixSize; col++){
      if(random(100) < wallRandomness){
        matrix[row][col] = wallIntensity;
      }
    }
  }

}

void buttonPress() {
    byte newSwState = digitalRead(swPin);
     
    if((newSwState != swState) && (millis() - lastDebounceTime) > debounceDelay){
      lastDebounceTime = millis();
      swState = newSwState;
      if(swState == LOW ){
         switch (currentState) {
                case PLAYING:
                    shoot();
                    break;
                case MENU:
                    playButtonPressSound();
                    if (currentMenuItem == START_GAME) {
                        currentState = PLAYING; 
                    } else if (currentMenuItem == SETTINGS_MENU) {
                        currentState = SETTINGS;
                    } else if (currentMenuItem == ABOUT_MENU) {
                        currentState = ABOUT;
                    } else if (currentMenuItem == HIGHSCORE_MENU){
                        currentState = DISPLAY_HIGHSCORE;
                        currentHighScoreIndex=0;
                        displayCurrentHighScore();
                    } else if (currentMenuItem == HOW_TO_PLAY_MENU){
                        currentState = HOW_TO_PLAY;
                        displayHowToPlay();
                    }
                    break;
                 case DISPLAY_HIGHSCORE:
                  playButtonPressSound();
                  currentState = MENU; // Navigate back to the main menu
                  currentMenuItem = 0;
                  lastMenuItem = -1;
                  lastMenuEnterTime = millis(); // Update the time when entering the menu
                  break;
                 case SETTINGS:
                  playButtonPressSound();
                    if(currentSettingsMenu == LCD_BRIGHTNESS){
                            currentState = ADJUST_LCD_BRIGHTNESS;
                    } else if(currentSettingsMenu == MATRIX_BRIGHTNESS){
                            currentState = ADJUST_MATRIX_BRIGHTNESS;
                    } else if(currentSettingsMenu == SET_NAME){
                            currentState = CHANGE_NAME;
                            displayName();
                    } else if (currentSettingsMenu == SETTINGS_EXIT){
                            currentState = MENU; // Navigate back to the main menu
                            currentMenuItem = 0; 
                            currentSettingsMenu = 0;
                            lastMenuItem = -1; 
                            lastMenuEnterTime = millis(); // Update the time when entering the menu
                    } else if(currentSettingsMenu == WALL_RANDOMNESS){
                            currentState = ADJUST_WALL_RANDOMNESS;
                            displayRandomness();
                    } else if(currentSettingsMenu == SOUND_SETTING){
                            currentState = CHANGE_SOUND;
                             displaySoundSettings();
                    } else if(currentSettingsMenu == RESET_HIGHSCORES){
                            currentState = RESET_HIGHSCORES_CONFIRM;
                            displayResetHighscoresConfirm();
                    }
                    break;
                  case ADJUST_LCD_BRIGHTNESS:
                    playButtonPressSound();
                    EEPROM.put(EEPROM_LCD_BRIGHTNESS_ADDR, lcdBrightness); // Save the new brightness value
                    currentState = SETTINGS; // Return to settings menu
                    currentSettingsMenu = 1;
                    break;
                  case ADJUST_MATRIX_BRIGHTNESS:
                    playButtonPressSound();
                    EEPROM.put(EEPROM_MATRIX_BRIGHTNESS_ADDR, matrixBrightness); // Save the new brightness value
                    clearMatrix();
                    currentState = SETTINGS; // Return to settings menu
                    currentSettingsMenu= 0;
                    break;
                  case ADJUST_WALL_RANDOMNESS:
                    playButtonPressSound();
                    EEPROM.put(EEPROM_WALL_RANDOMNESS_ADDR,wallRandomness);
                    currentState = SETTINGS; // Return to settings menu
                    currentSettingsMenu= 0;
                    break;
                  case ABOUT:
                     playButtonPressSound();
                     currentState = MENU; // Navigate back to the main menu
                     currentMenuItem = 0;
                     lastMenuItem = -1;
                     lastMenuEnterTime = millis(); // Update the time when entering the menu
                     break;
                  case LEVEL_COMPLETE:
                    playButtonPressSound();
                    if (endMenuSelection == NEXT_LEVEL) {
                        if (currentLevel < maxLevel) {
                            currentLevel++;
                            resetGame();
                            currentState = PLAYING;
                        } else {
                            currentState = MENU;
                        }
                    } else if (endMenuSelection == MAIN_MENU) {
                        resetGame();
                        currentState = MENU; // Transition to MENU state
                        lastMenuEnterTime = millis(); // Update the time when entering the menu
                        currentMenuItem = 0; // Reset the current menu item
                        lastMenuItem = -1;
                    }
                    break;
                     case DISPLAY_SCORE:
                     playButtonPressSound();
                      if (betterThanPeople == 3) { // Player has the top high score
                        displayHighScoreAlert();
                        currentState = ALERT_HIGHSCORE; // Go to alert high score state
                      } else {
                          resetGame();
                          currentState = MENU;
                          currentLevel = 1;
                          lastMenuEnterTime = millis(); // Update the time when entering the menu
                          currentMenuItem = 0; // Reset the current menu item
                          lastMenuItem = -1; 
                      }
                  break;
                  case ALERT_HIGHSCORE:
                    playButtonPressSound();
                    resetGame();
                     currentState = MENU;
                     currentLevel = 1;
                     lastMenuEnterTime = millis(); // Update the time when entering the menu
                     currentMenuItem = 0; // Reset the current menu item
                     lastMenuItem = -1; 
                  break;
                  case CHANGE_NAME:
                    playButtonPressSound();
                    currentState = SETTINGS;
                    currentSettingsMenu = SETTINGS_EXIT; // Return to a default settings menu item
                    lcd.noBlink(); // Turn off blinking cursor
                  break;
                  case CHANGE_SOUND:
                    EEPROM.put(EEPROM_SOUND_ADDR, soundEnabled); // Save sound setting to EEPROM
                    currentState = SETTINGS; // Return to settings menu
                    currentSettingsMenu= 0;
                  break;
                  case HOW_TO_PLAY:
                    currentState = MENU; // Navigate back to the main menu
                    currentMenuItem = 0; 
                    lastMenuItem = -1; 
                    lastMenuEnterTime = millis(); // Update the time when entering the menu
                  break;
                  case RESET_HIGHSCORES_CONFIRM:
                    playButtonPressSound();
                    if (resetHighscores) {
                        resetDefaultHighscore(); // Function to reset highscores
                    }
                    currentState = SETTINGS; // Return to settings menu
                    currentSettingsMenu = SETTINGS_EXIT; // Default to a settings menu item
                  break;
                }
      }
    }
}


void shoot() {
  if (!isShooting) { 
    isShooting = true;
    shootingStartTime = millis();

    setBombs(bombIntensity);
    playBombSound();
  }
}

void adjustLCDBrightness() {
    unsigned long currentMillis = millis();
    
    if (currentMillis - lastBrightnessAdjustTime >= brightnessAdjustInterval) {
        int yValue = analogRead(yPin);

        // Adjust brightness level based on joystick position
        if (yValue < minThreshold && lcdBrightness > 0) {
            lcdBrightness = max(0, lcdBrightness - 25); // Increase the decrement step to 25
        } else if (yValue > maxThreshold && lcdBrightness < 255) {
            lcdBrightness = min(255, lcdBrightness + 25); // Increase the increment step to 25
        }

        // Apply the brightness setting
        analogWrite(lcdBrightnessPin, lcdBrightness);

        // Calculate the brightness level for display (1 to 12)
        int brightnessLevel = map(lcdBrightness, 0, 255, 1, 12);

        // Display the current brightness level on the LCD
        lcd.clear();
        lcd.print("Brightness: ");
        lcd.setCursor(0, 1);
        lcd.print(brightnessLevel);

        // Update the last adjustment time
        lastBrightnessAdjustTime = currentMillis;
    }

    buttonPress(); 
}


void adjustMatrixBrightness() {
    unsigned long currentMillis = millis();
  
    if (currentMillis - lastMatrixBrightnessAdjustTime >= matrixBrightnessAdjustInterval) {
        int yValue = analogRead(yPin); // Reading Y-axis value of the joystick

        // Adjust brightness level based on joystick position
        if (yValue < minThreshold && matrixBrightness > 1) {
            matrixBrightness--;
        } else if (yValue > maxThreshold && matrixBrightness < maxMatrixBrightness) {
            matrixBrightness++;
        }

        // Apply the brightness setting (adjusting for 0-based index)
        lc.setIntensity(0, matrixBrightness);

        // Display heart pattern on the matrix with the new brightness
        for (int row = 0; row < 8; row++) {
            lc.setRow(0, row, heartPattern[row]);
        }

        // Display the current matrix brightness on the LCD
        lcd.clear();
        lcd.print("Matrix Brightness: ");
        lcd.setCursor(0, 1);
        lcd.print(matrixBrightness);

        // Update the last adjustment time
        lastMatrixBrightnessAdjustTime = currentMillis;
    }

    // Call to buttonPress() to handle any button actions
    buttonPress();
}


void setBombs(byte state) {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      if (matrix[row][col] == playerIntensity) { // If player is present
        // Set bombs based on the level
        int bombRange = 5 - currentLevel; // 3 bombs in level 1, 2 in level 2, and 1 in level 3
        for (int i = 1; i < bombRange; i++) {
          if (row >= i) matrix[row - i][col] = state;
          if (col >= i) matrix[row][col - i] = state;
          if (row < matrixSize - i) matrix[row + i][col] = state;
          if (col < matrixSize - i) matrix[row][col + i] = state;
        }
      }
    }
  }
  matrixChanged = true;
}


void spawnPlayer() {
  bool playerSpawned = false;
  randomSeed(analogRead(0)); 

  while (!playerSpawned) {
    byte randomX = random(matrixSize);
    byte randomY = random(matrixSize);

    if (matrix[randomX][randomY] == 0) {
      xPos = randomX;
      yPos = randomY;
      matrix[xPos][yPos] = playerIntensity; 
      playerSpawned = true;
    }
  }
}

void updatePositions() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

   if (isShooting) {
    return; 
  }

  xLastPos = xPos;
  yLastPos = yPos;

  if (xValue < minThreshold) {
        if (xPos > 0) {
      --xPos;
    } else {
      xPos = matrixSize - 1;
    }
  }

  if (xValue > maxThreshold) {
      if (xPos < matrixSize - 1) {
      ++xPos;
    } else {
      xPos = 0;
    }
  }

  if (yValue < minThreshold) {
    if (yPos < matrixSize - 1) {
      ++yPos;
    } else {
      yPos = 0;
    }
  }

  if (yValue > maxThreshold) {
    if (yPos > 0) {
      --yPos;
    } else {
      yPos = matrixSize - 1;
    }
  }
  if(matrix[xPos][yPos] == wallIntensity){

    xPos = xLastPos;
    yPos = yLastPos;

  }else{
      if (xPos != xLastPos || yPos != yLastPos) {
    matrixChanged = true;
    matrix[xLastPos][yLastPos] = 0;
    matrix[xPos][yPos] = playerIntensity;  // Set the brightness for the LED at the new position
  }
  }
}
