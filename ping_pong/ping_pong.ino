/*********************************************************************
Written by Florian Wachter
florianwachter.com
UX Designer & Developer

*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "pitches.h"

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// -----------------------------------------
// Pong Global Variables - Game Physics
// -----------------------------------------
uint8_t color = WHITE;

// Brick controll
int poti = A0;
int potiValue = 0;

// Brick
int brickWidth = 20;
int brickHeight = 2;
int brickY = display.height() - 5;

// Ball
int ballX = 128/2;      
int ballY = 64/2;
int ballSpeedX = 2;
int ballSpeedY = 1;

// Sound
int speakerPin = 8;

int rectWidth = 10;
int rectHeight = 5;


void music(int melody[], int noteDurations[], int notesAmount){
  
  
  for (int thisNote = 0; thisNote < notesAmount; thisNote++) {
      int noteDuration = 1000/noteDurations[thisNote];
      tone(speakerPin, melody[thisNote],noteDuration);
      delay(noteDuration * 1.10);
    }  
    
}

// -----------------------------------------
// Pong Global Variables - Game engine
// -----------------------------------------
int lifes = 2;
bool gameIsRunning = true;

int lvl[] = {10,25,40,70,85,100 };
int lvlSize = 6; 
int score = 0;
bool win = false;


// -----------------------------------------
// Pong Setup
// -----------------------------------------
void setup()   {                
  Serial.begin(9600);
  pinMode(poti, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();  
  drawIntro();
  
}

// -----------------------------------------
// Pong Loop
// -----------------------------------------

 
void loop() {
  if(gameIsRunning) drawBrick( brickPosition() );
  collisionControl(brickPosition());
  drawBall();
  drawLife();
  
  drawObjects();
  objCollisionControl();

  if(win != true){
    checkForWin();
  }
  
  
    
  // ------- ENDE --------
  display.display();
  delay(5);
  if(gameIsRunning) {
    display.clearDisplay();
  }
} 

// -----------------------------------------
// Pong Functions
// -----------------------------------------

int brickPosition(){
  potiValue = analogRead(A0);
  return map(potiValue, 40, 1024, 0, display.width()-brickWidth); 
}

int drawBrick(int xpos){
  display.fillRect(xpos, brickY, brickWidth, brickHeight, color);
}

void drawBall() {
  display.fillRect(ballX, ballY, 2, 2, 1);
  ballX += ballSpeedX;
  ballY += ballSpeedY;
}

int collisionControl(int paddlePositionPlayer) {
  // bounce from Left and Right
  if (ballX >= display.width() - 2 || ballX <= 0) {
    ballSpeedX *= -1;
    tone(speakerPin, NOTE_F2, 100);
  }

  // bounce from the top
  if (ballY <= 0) {
    ballSpeedY *= -1;
    tone(speakerPin, NOTE_F2, 100);
  }
 
  // bounce from Brick
  if (ballY < brickY && ballY >= brickY - 4 ) {
    if(ballX > paddlePositionPlayer && ballX < paddlePositionPlayer + brickWidth){
       ballSpeedY *= -1;
       tone(speakerPin, NOTE_C2, 100);
       // to make it harder after every hit from the brick
       makeItHarder();
    }
  }

  // score points if ball hits wall behind player
  if (ballY >= display.height() - 2 && lifes >= 0) {
     // if missed the paddle
     lifes -= 1;
     ballX = display.width()/2;
     ballY = 10;

     int melody5[] = {NOTE_D4,NOTE_D2};
     int noteDurations5[] = {2, 4};
     music(melody5, noteDurations5, 2);
     
     if(lifes == 0){
      drawGameOver(); 
      stopGame();
     }
  } 
}

// -----------------------------------------
// Draw 
// -----------------------------------------
void drawLife(){
  // display the Amount of lifes left
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(display.width() - 10, 0);
  display.println(lifes);
 }

void stopGame() {
  ballSpeedY = 0;
  ballSpeedX = 0;
  gameIsRunning = false;
}

void drawGameOver(){
  drawText(2, "GAME OVER", 10, (display.height()/2)-6 );
}

void checkForWin() {
  if (score >= lvlSize ){
    stopGame();
    drawText(2, "YOU WON!", 18, (display.height()/2) - 6 );
    tone(speakerPin, NOTE_E6, 300);
    delay(500);
    tone(speakerPin, NOTE_FS6, 600);
    win = true;
    
  } 
}

void makeItHarder() {
  brickWidth--;;
}

void drawIntro(){
  // PING PONG
  drawText(2, "PING PONG", 10, (display.height()/2)-6 );
  int melody0[] = { NOTE_C6, NOTE_B6, NOTE_A6, 0};
  int noteDurations0[] = { 4, 2, 4, 0};
  music(melody0, noteDurations0, 3);
  clearDisplayCach(1000);
  
  // Game is starting
  drawText(1, "Game is starting", 10, (display.height()/2) );
  clearDisplayCach(1000);
  
  // ..3
  drawText(1, "..3", display.width()/2, display.height()/2 );
  int melody1[] = {NOTE_GS7};
  int noteDurations1[] = {2};
  music(melody1, noteDurations1, 1);
  clearDisplayCach(500);

  // ..2
  drawText(1, "..2", display.width()/2, display.height()/2 );
  int melody2[] = {NOTE_GS7};
  int noteDurations2[] = {2};
  music(melody2, noteDurations2, 1);
  clearDisplayCach(500);

  // ..1
  drawText(1, "..1", display.width()/2, display.height()/2 );
  int melody3[] = {NOTE_GS7};
  int noteDurations3[] = {2};
  music(melody3, noteDurations3, 1);
  clearDisplayCach(500);

  // GO
  drawText(1, "GO", display.width()/2, display.height()/2 );
  int melody4[] = {NOTE_B7};
  int noteDurations4[] = {1};
  music(melody4, noteDurations4, 1);
  clearDisplayCach(1000);
}

void drawText(int fontSize, String str, int xPos, int yPos ){
  display.setTextSize(fontSize);
  display.setTextColor(WHITE);
  display.setCursor(xPos, yPos);
  display.println(str);
  display.display();
}

void clearDisplayCach(int delayVal){
  delay(delayVal);
  display.clearDisplay();
}

// ----------------------------------------------
// Objects
// ----------------------------------------------

void drawObjects(){
  for (int i = 0; i < lvlSize; i++) {
   if( lvl[i] != 0 ){
    display.fillRect(lvl[i], display.height()/3, rectWidth, rectHeight, color);
   }
  }
}

void objCollisionControl() {
  // bounce from object
  for (int i = 0; i < lvlSize; i++ ){
    if(ballY > display.height()/3 && ballY < (display.height()/3) + rectHeight ){
      if (ballX > lvl[i] && ballX < lvl[i] + rectWidth && lvl[i] != 0) {
        ballSpeedY *= -1;
        digitalWrite(LED_BUILTIN, HIGH);
        tone(speakerPin, NOTE_A5, 100);
        score++;
        lvl[i] = 0;
        
      }      
    }else{
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}












