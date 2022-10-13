
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#define BUZZER 4
#define IR_SENSOR 7
#define A1_MOTOR 10
#define A2_MOTOR 11
#define RELAY 12
#define LED_PIN 9
#define NORMAL_ANGLE 0
#define LED_NUM 1
#define BOARD_RATE 9600

void SetText(char *TEXT, int COLUMNS = 0, int ROW = 0);
void Resetlcd();
bool isIR(int IR_SENSOR_PIN = IR_SENSOR);
void TurnMotor(uint8_t SPEED = 0);
void Buzzer(int TONE, int TIME);
void StopMotor(int DELAY);
void ServoUse(int SERVO_PIN, int ANGLE = NORMAL_ANGLE, int DELAY = 0);
void LED(bool SWITCH, int R = 0, int G = 0, int B = 0);
char Receiver();

LiquidCrystal_I2C lcd(0x27,20,4);
Servo myservo;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);

int Sound = 700;
bool isTurn = false;
bool isTurnAlarm = true;
int looptime = 0;
char DATA = ' ';

void setup() {
  
  Serial.begin(BOARD_RATE);

  lcd.init();
  lcd.backlight();
  Resetlcd();
  strip.begin();
  strip.setBrightness(50);
  LED(false);

  SetText("RevertSystem", 2, 0);
  SetText("Team : ArtMega", 1, 1);

  pinMode(IR_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  Buzzer(300, 100);
  Buzzer(600, 175);

  delay(1250);

  Buzzer(550, 200);

  Resetlcd();

}

void loop() {
  if(isIR() && isTurnAlarm){

    LED(false);
    TurnMotor(200);
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
    LED(true, 255, 0, 0);
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
  else{
    LED(true, 255, 255, 255);
  }

  if(isTurn){

    if(looptime == 1){
      Resetlcd();
      SetText("!Detected!", 3, 0);
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
    }
    delay(1);
    looptime++;
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

void ServoUse(int SERVO_PIN, int ANGLE = NORMAL_ANGLE, int DELAY = 0){
    myservo.attach(SERVO_PIN);
    myservo.write(ANGLE);

    delay(DELAY);
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