
#include <MFRC522.h>
#include <Servo.h>
#include <SPI.h>

#define NORMAL_ANGLE 0
#define SS_PIN 10
#define RST_PIN 9
#define A1_MOTOR 5
#define A2_MOTOR 6
#define BOARD_RATE 9600

MFRC522 RFID(SS_PIN, RST_PIN);
Servo SERVO;

String RFID_VALUE = "32 6D 56 D3";
int RFID_RETURN = 0;
int looptime = 0;

int isRFID();
void TurnMotor(uint8_t SPEED = 0);
void Send(int DATA);
void ServoUse(int SERVO_PIN, int ANGLE = NORMAL_ANGLE, int DELAY = 0);
bool loopturn = false;

void setup(){
  Serial.begin(BOARD_RATE);
  SPI.begin();
  RFID.PCD_Init();

  ServoUse(3, 70);
  TurnMotor(0);
}


void loop(){

  if(!loopturn){
    RFID_RETURN = isRFID();
  }

  if(RFID_RETURN == 1 && !loopturn){
    delay(500);
    ServoUse(3, 180);
    delay(1250);
    ServoUse(3, 70);
    TurnMotor(175);
    loopturn = true;
    Serial.println("LOOPING...");
  }

  else if(loopturn){
    if(looptime >= 4000){
      TurnMotor();
      looptime = 0;
      loopturn = false;
      return;
      }
      delay(1);
      looptime++;
  }
}

int isRFID(){
  String content= "";
  byte letter;

  if ( ! RFID.PICC_IsNewCardPresent()){
    return 0;
  }
  
  if ( ! RFID.PICC_ReadCardSerial()){
    return 0;
  }

  Serial.print("RFID VALUE >> ");

  for (byte i = 0; i < RFID.uid.size; i++){
     Serial.print(RFID.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(RFID.uid.uidByte[i], HEX);
     content.concat(String(RFID.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(RFID.uid.uidByte[i], HEX));
  }

  Serial.println();

  //content.toUpperCase();
  //content.substring(1) == RFID_VALUE

  return 1;
}

void TurnMotor(uint8_t SPEED = 0){
  analogWrite(A1_MOTOR, SPEED);
  analogWrite(A2_MOTOR, 0);
}

void Send(int DATA){
  Serial.print(DATA);
}

void ServoUse(int SERVO_PIN, int ANGLE = NORMAL_ANGLE, int DELAY = 0)
{
    SERVO.attach(SERVO_PIN);
    SERVO.write(ANGLE);

    delay(DELAY);
}
