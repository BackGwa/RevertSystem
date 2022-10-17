
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include "Adafruit_TCS34725.h"

#define BUZZER 4
#define IR_SENSOR 7
#define A1_MOTOR A0
#define A2_MOTOR A1
#define RELAY 12
#define LED_PIN 9
#define NORMAL_ANGLE 120
#define LED_NUM 1
#define BOARD_RATE 9600

void SetText(char *TEXT, int COLUMNS = 0, int ROW = 0);
void Resetlcd();
bool isIR(int IR_SENSOR_PIN = IR_SENSOR);
void TurnMotor(uint8_t SPEED = 0);
void Buzzer(int TONE, int TIME);
void StopMotor(int DELAY);
void LED(bool SWITCH, int R = 0, int G = 0, int B = 0);
char Receiver();

LiquidCrystal_I2C lcd(0x27,20,4);
Servo SERVO;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

char setcolor = ' ';
int Sound = 700;
bool isTurn = false;
bool isTurnAlarm = true;
int looptime = 0;
char DATA = ' ';
uint16_t clear, red, green, blue;
byte gammatable[256];

void setup() {
  
  Serial.begin(BOARD_RATE);

  lcd.init();
  lcd.backlight();
  Resetlcd();
  strip.begin();
  strip.setBrightness(100);
  LED(false);

  Buzzer(300, 150);
  SetText("CHECKING...", 3, 0);

  if(!tcs.begin()){
    error("SE1");
  }

  delay(1000);
  digitalWrite(RELAY, LOW);

  Resetlcd();

  pinMode(IR_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  SetText("RevertSystem", 2, 0);
  SetText("Team : ArtMega", 1, 1);
  Buzzer(300, 100);
  Buzzer(600, 175);
  
  SERVO.attach(6);
  SERVO.write(NORMAL_ANGLE);

  delay(400);
  SERVO.detach();

  SERVO.attach(5);
  SERVO.write(NORMAL_ANGLE);

  delay(400);
  SERVO.detach();

  delay(1250);

  Buzzer(550, 200);
  Resetlcd();

}

void loop() {
  if(isIR() && isTurnAlarm){

    LED(false);
    TurnMotor(150);
    Sound += 100;
    delay(50);
    Buzzer(Sound, 25);

    isTurn = true;
    
  }
  else{
    Sound = 750;
  }

  if(!isIR() && !isTurn){
    Sound = 750;
    digitalWrite(RELAY, HIGH);
    LED(true, 255, 127, 0);
    SetText("WAITING", 4, 0);
    delay(125);
    SetText("WAITING.", 4, 0);
    delay(125);
    SetText("WAITING..", 4, 0);
    delay(125);
    SetText("WAITING...", 4, 0);
    delay(125);
    digitalWrite(RELAY, LOW);
    LED(false);
    delay(500);
    Resetlcd();
  }

  if(isTurn){

    if(looptime == 1){
      Resetlcd();
      SetText("!Detected!", 3, 0);
      LED(true, 255, 255, 255);
    }

    if(looptime == 200){
      isTurnAlarm = false;
    }

    if(looptime > 5500){
      StopMotor(375);
      looptime = 0;
      isTurn = false;
      isTurnAlarm = true;

      Resetlcd();
      SetText("WAITING...", 4, 0);

      SERVO.attach(5);
      SERVO.write(NORMAL_ANGLE);
      delay(500);
      SERVO.attach(6);
      SERVO.write(NORMAL_ANGLE);
    }
    delay(1);
    looptime++;
  }

  if(looptime > 200 && looptime < 1000){

    StopMotor(0);
    tcs.setInterrupt(false);
    delay(25);
    looptime += 25;

    tcs.getRawData(&red, &green, &blue, &clear);
    tcs.setInterrupt(true);

    uint32_t sum = clear;
    float r, g, b;

    r = red; r /= sum;
    g = green; g /= sum;
    b = blue; b /= sum;
    r *= 10; g *= 10; b *= 10;

    Serial.print(r);
    Serial.print(" ");
    Serial.print(g);
    Serial.print(" ");
    Serial.print(b);
    Serial.println();

    if(r < 4 && g < 4 && b < 4){
      LED(true, 255, 255, 255);
      setcolor = 's';
    }
    else if((r > g) && (r > b)){
      LED(true, 255, 0, 0);
      setcolor = 'r';
    }
    else if((g > r) && (g > b)){
      LED(true, 0, 255, 0);
      setcolor = 'g';
    }
    else if((b > r) && (b > g)){
      LED(true, 0, 0, 255);
      setcolor = 'b';
    }
    else if((r < g) && (r < b)){
      LED(true, 0, 255, 255);
      setcolor = 'c';
    }
    else if((g < r) && (g < b)){
      LED(true, 255, 0, 255);
      setcolor = 'm';
    }
    else if((b < r) && (b < g)){
      LED(true, 255, 255, 0);
      setcolor = 'y';
    }
    else{
      LED(false);
    }
    }

    if(looptime > 1000 && looptime < 1010){

      TurnMotor(250);

      if(setcolor == 'r'){
        SetText("DEBUGCOLOR R", 0, 1);
        SERVO.attach(6);
        SERVO.write(140);
      }

      else if(setcolor == 'b'){
        SetText("DEBUGCOLOR B", 0, 1);
        SERVO.attach(5);
        SERVO.write(140);
      }
      else if(setcolor == 's'){
        SetText("SKIP COLOR", 0, 1);
      }

      }
}

void SetText(char *TEXT, int COLUMNS = 0, int ROW = 0){
  lcd.setCursor(COLUMNS,ROW);
  lcd.print(TEXT);
}

void Resetlcd(){
  lcd.backlight();
  lcd.clear();
}

bool isIR(int IR_SENSOR_PIN = IR_SENSOR){
  return (digitalRead(IR_SENSOR_PIN) == 1) ? true : false;
}

void TurnMotor(uint8_t SPEED = 0){
  analogWrite(A1_MOTOR, SPEED);
  analogWrite(A2_MOTOR, 0);
}

void Buzzer(int TONE, int TIME){
  tone(BUZZER, TONE);
  delay(TIME);
  noTone(BUZZER);
}

void StopMotor(int DELAY){
  delay(DELAY);
  TurnMotor();
}

void LED(bool SWITCH, int R = 0, int G = 0, int B = 0){
  if(SWITCH){
    strip.setPixelColor(0, R, G, B);
  }
  else{
    strip.setPixelColor(0, 0, 0, 0);
  }
  
  strip.show();
}

char Receiver(){
  if(Serial.available() > 0)
  {
    DATA = Serial.read();
    return DATA;
  }

  return 'X';
}

// SE : SENSOR ERROR
//  1 : COLOR
//  2 : RFID
//  3 : IR

// ME : MODULE ERROR
//  1 : LED
//  2 : RELAY
//  3 : BUZZER
//  4 : LCD
//  5 : MOTOR CONTROLLER A
//  6 : MOTOR CONTROLLER B
//  7 : SERVO A
//  8 : SERVO B
//  9 : UNKNOWN MODULE

void error(char *errcode){
  delay(1000);
  Resetlcd();
  SetText("[ ERROR! ]", 3, 0);
  SetText("Reporter >> ", 0, 1);
  SetText(errcode, 12, 1);

  strip.setBrightness(100);
  LED(true, 255, 0, 0);

  Buzzer(150, 200);
  Buzzer(175, 150);
  Buzzer(200, 100);

  while(true){
    LED(true, 255, 0, 0);
    Buzzer(100, 50);
    delay(1500);
    LED(false);
    Buzzer(200, 50);
    delay(1500);
  }
}