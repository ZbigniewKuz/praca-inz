#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include "HardwareSerial.h"
#include "lipkg.h"

#define POINT_PER_PACK 12
#define HEADER 0x54


HardwareSerial SerialPort(2);

const char* ssid     = "FunBox2-0755";
const char* password = "6F31A7CDFFA3A25DED94FDC69F";
const char* data;
size_t data_len = strlen(data);
 
using namespace websockets;
WebsocketsClient socket;
const char* websocketServer = "ws://192.168.1.25:8080";
boolean connected = false;
 
WiFiClient client;

boolean assembled_packet;

ldlidar::LiPkg lidarPackage;

void setup() {
  Serial.begin(115200);
  SerialPort.begin(921600, SERIAL_8N1, 16, 17);

  connectToWiFi();

  connectToWebSocket();

  lidarPackage.RegisterTimestampGetFunctional(getTimestamp);

  // Set product type (choose the appropriate type)
  lidarPackage.SetProductType(ldlidar::LDType::STL_27L);

  // Enable LiDAR data filter (optional)
  lidarPackage.EnableFilter(true);
  

  socket.onMessage(handleMessage);
  socket.onEvent(handleEvent);
}
 
void loop() {
  if(!connected) { 
    Serial.println("Connecting to WebSocket server");
    connectToWebSocket();
  }
  socket.poll();
  

 // Read LiDAR data from UART
  if (SerialPort.available() >= sizeof(ldlidar::LiDARFrameTypeDef)) {
    ldlidar::LiDARFrameTypeDef lidarFrame;
    SerialPort.readBytes(reinterpret_cast<uint8_t *>(&lidarFrame), sizeof(ldlidar::LiDARFrameTypeDef));

    // Process LiDAR data
    lidarPackage.CommReadCallback(reinterpret_cast<const char *>(&lidarFrame), sizeof(ldlidar::LiDARFrameTypeDef));

    // Check if a LiDAR frame is ready
    if (lidarPackage.IsLiDARFrameReady()) {
      // Get the LiDAR data
      ldlidar::Points2D lidarData;
      if (lidarPackage.GetLaserScanData(lidarData)) {
        // Convert LiDAR data to a JSON string (you may need to customize this part based on your data format)
        // String jsonData = "{";
        // for (const auto &point : lidarData) {
        //   jsonData += "\"" + String(point.angle) + "\": " + String(point.distance) + ",";
        // }
        // jsonData.remove(jsonData.length() - 1); // Remove the trailing comma
        // jsonData += "}";

        // Send LiDAR data via WebSocket
        socket.send(jsonData);
      }
    }
  }
  delay(1000);
}


void handleMessage(WebsocketsMessage message){
  Serial.println(message.data());
}

void handleEvent(WebsocketsEvent event, WSInterfaceString data) { 
  switch (event) {
    case WebsocketsEvent::ConnectionOpened:
      Serial.println("WebSocket connection established");
      //socket.send("Hello, server!");
      connected = true;
      break;
    case WebsocketsEvent::ConnectionClosed:
      Serial.println("WebSocket connection closed");
      connected = false;
      break;
    case WebsocketsEvent::GotPing:
      Serial.println("Received ping");
      break;
    case WebsocketsEvent::GotPong:
      Serial.println("Received pong");
      break;
    default:
      break;
  }
}

void connectToWiFi(){
    WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
}

uint64_t getTimestamp() {
  return millis();
}

void connectToWebSocket(){
  connected = socket.connect(websocketServer);

  if(connected){
    Serial.println("Connected");

  }
  else{
    Serial.println("connection failed.");
  }
}
