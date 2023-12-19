// LD19 PWM_FREQ MIN:20kHz TYP:30kHz 50kHz
// PWM 0% 40% 100% 40%=10HZ
// LD19: 230400 bit/s
//
// LD14: 115200 bit/s
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

int LD_TYPE   = 0;
int LD14_FAST = 0;
int LD19_FAST = 0;
int WIRE_CTRL = 1;

#define POINT_PER_PACK 12
#define HEADER 0x54

#define SWITCH_PIN 12
#define LD14_PIN   13
#define LD19_PIN   14

#define START_PIN  5

#define PWM_PIN  4

typedef struct __attribute__((packed)) {
  uint16_t distance;
  uint8_t intensity;
} LidarPointStructDef;

typedef struct __attribute__((packed)) {
  uint8_t header;
  uint8_t ver_len;
  uint16_t speed;
  uint16_t start_angle;
  LidarPointStructDef point[POINT_PER_PACK];
  uint16_t end_angle;
  uint16_t timestamp;
  uint8_t crc8;
} LiDARFrameTypeDef;

const char *ssid = "TwojaSiecWiFi";
const char *password = "TwojeHasloWiFi";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

LiDARFrameTypeDef lidarFrame;

void setup() {
  // put your setup code here, to run once:
  pinMode(SWITCH_PIN,INPUT);
  pinMode(LD14_PIN,INPUT);
  pinMode(LD19_PIN,INPUT);

  pinMode(START_PIN,OUTPUT);
  
  digitalWrite(START_PIN, HIGH);

  Serial.begin(115200);

   // Łączenie z Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Łączenie z Wi-Fi...");
  }
  Serial.println("Połączono z Wi-Fi");

  // Obsługa WebSocket
  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      Serial.println("Nowe połączenie WebSocket");
    } else if (type == WS_EVT_DISCONNECT) {
      Serial.println("Rozłączono z WebSocket");
    } else if (type == WS_EVT_DATA) {
      handleWebSocketMessage(arg, data, len);
    }
  });

  server.addHandler(&ws);

  server.begin();

}


void loop() {
  // put your main code here, to run repeatedly:
  if(WIRE_CTRL == 1){
    switchCheck();
  }
  
  if (Serial.available() > 0) {
    char state = Serial.read();
    if (state == '1') {
      digitalWrite(START_PIN, HIGH);
      WIRE_CTRL = 0;
    }
    else if (state == '0') {
      digitalWrite(START_PIN, LOW);
      WIRE_CTRL = 0;
    }
    else if (state == '2') {
      WIRE_CTRL = 1;
    }
  }

   // Odbierz dane z UART
  if (Serial.available() >= sizeof(LiDARFrameTypeDef)) {
    Serial.readBytes((uint8_t *)&lidarFrame, sizeof(LiDARFrameTypeDef));

    // Wyślij dane do wszystkich klientów przez WebSocket co 10 sekund
    static unsigned long lastWebSocketSendTime = 0;
    if (millis() - lastWebSocketSendTime >= 10000) {
      sendLiDARData();
      lastWebSocketSendTime = millis();
    }
  }

}
void switchCheck(){
  if(digitalRead(SWITCH_PIN) == LOW){
    digitalWrite(START_PIN, LOW);
    WIRE_CTRL = 1;
  }
  else{
    digitalWrite(START_PIN, HIGH);
  }
}


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  // Tutaj możesz przetworzyć otrzymane dane z WebSocket
  // W przypadku tej funkcji przyjmuje ona dane, ale nie robi z nimi niczego konkretnego
}

void sendLiDARData() {
  // Wysłanie struktury do wszystkich klientów przez WebSocket
  ws.textAll((uint8_t *)&lidarFrame, sizeof(LiDARFrameTypeDef));
}