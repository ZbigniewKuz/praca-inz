#include "BluetoothSerial.h"
#include "HardwareSerial.h"
#include "WiFi.h"

HardwareSerial SerialPort(2);
BluetoothSerial SerialBT;
char data;
char dataToStm;
int intToStm;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  SerialPort.begin(115200, SERIAL_8N1, 16, 17);
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {

  }
  if (SerialBT.available()) {
    data = SerialBT.read();
      if(data == 'f'){
      SerialPort.print('f');
      Serial.print('f');
      }
      else if(data == 'b'){
      SerialPort.print('b');
      Serial.print('b');
      }
      else if(data == 'l'){
      SerialPort.print('l');
      Serial.print('l');
      }
      else if(data == 'r'){
      SerialPort.print('r');
      Serial.print('r');
      }
       else if(data == 's'){
      SerialPort.print('s');
      Serial.print('s');
      }
      else if(data >= '0' && data <= '9'){
        intToStm = data - '0';
        SerialPort.print(intToStm);
      }
  }
 if(SerialPort.available()){
  String buffer = SerialPort.readString();
  Serial.print(buffer);
 }
}
