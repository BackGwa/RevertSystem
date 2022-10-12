
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUZZER 4
#define IR_SENSOR 7
#define A1_MOTOR 10
#define A2_MOTOR 11

void SetText(char *TEXT, int COLUMNS = 0, int ROW = 0);
void Resetlcd();
bool isIR(int IR_SENSOR_PIN = IR_SENSOR);
void TurnMotor(uint8_t SPEED = 0);
void Buzzer(int TONE, int TIME);
void StopMotor(int DELAY);

LiquidCrystal_I2C lcd(0x27,20,4);

int Sound = 700;
bool isTurn = false;
bool isTurnAlarm = true;
int looptime = 0;

void setup() {
  
  lcd.init();
  lcd.backlight();

  SetText("RevertSystem", 2, 0);
  SetText("Team : ArtMega", 1, 1);

  pinMode(IR_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);

  Buzzer(300, 100);
  Buzzer(600, 175);

}

void loop() {

  if(isIR() && isTurnAlarm){

    TurnMotor(200);
    Sound += 100;
    delay(50);
    Buzzer(Sound, 25);

    isTurn = true;
    
  }
  else{
    Sound = 750;
  }

  if(isTurn){

    if(looptime == 200){
      isTurnAlarm = false;
      Resetlcd();
      SetText("!Detected!", 3, 0);
    }

    if(looptime > 5500){
      StopMotor(250);
      looptime = 0;
      isTurn = false;
      isTurnAlarm = true;
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