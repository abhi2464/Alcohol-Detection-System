#include <ESP8266WiFi.h>
#include <espnow.h>

#define SIGNAL_TIMEOUT 1000  // This is signal timeout in milli seconds. We will reset the data if no signal
unsigned long lastRecvTime = 0;
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int b;
    String d;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  if(len==0)
  {
    return;
  }
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  // Serial.print("Char: ");
  // Serial.println(myData.a);
  Serial.print("Int: ");
  Serial.println(myData.b);
  // Serial.print("Float: ");
  // Serial.println(myData.c);
  Serial.print("String: ");
  Serial.println(myData.d);
  // Serial.print("Bool: ");
  // Serial.println(myData.e);
  if(myData.b==1)
  {
    digitalWrite(D4,HIGH);
  }
  else if(myData.b==0)
  {
    digitalWrite(D4,LOW);
  }
  Serial.println();
  lastRecvTime = millis();
}
 
void setup() {
  pinMode(D4,OUTPUT);
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  unsigned long now = millis();
  if ( now - lastRecvTime > SIGNAL_TIMEOUT ) 
  {
    // rotateMotor(0, 0);
    Serial.println("Signal lost");
  }
}