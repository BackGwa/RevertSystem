
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define A1_MOTOR 5
#define A2_MOTOR 6
#define BOARD_RATE 9600

MFRC522 RFID(SS_PIN, RST_PIN);

String RFID_VALUE = "00 00 00 00";

char isRFID();
void TurnMotor(uint8_t SPEED = 0);
void Send(int DATA);

void setup(){
  Serial.begin(BOARD_RATE);
  SPI.begin();
  RFID.PCD_Init();
}


void loop(){
}


char isRFID(){
  String content= "";
  byte letter;

  if (!rfid.PICC_IsNewCardPresent()){
    return 'N';
  }
  
  if (!rfid.PICC_ReadCardSerial()){
    return 'N';
  }

  Serial.print("RFID VALUE >> ");

  for (byte i = 0; i < rfid.uid.size; i++){
     Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(rfid.uid.uidByte[i], HEX);
     content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(rfid.uid.uidByte[i], HEX));
  }

  content.toUpperCase();

  if (content.substring(1) == RFID_VALUE){
    return 'T'; 
  }
  
  else{ 
    return 'F';
  }
}

void TurnMotor(uint8_t SPEED = 0){
  analogWrite(A1_MOTOR, SPEED);
  analogWrite(A2_MOTOR, 0);
}

void Send(int DATA){
  Serial.print(DATA);
}
