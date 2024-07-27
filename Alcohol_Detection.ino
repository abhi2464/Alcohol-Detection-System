#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xCC, 0x50, 0xE3, 0x3C, 0x16, 0x6E};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int b;
  String d;
} struct_message;

// Create a struct_message called myData
struct_message myData;

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

#define ir D4
#define alco A0
#define motor_pin D0
const int alco_limit=400;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void OnDataSent(uint8_t *mac_addr, uint8_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status);
  Serial.println(status == 0 ? "Message sent" : "Message failed");
}

void setup() {
  pinMode(ir,INPUT);
  pinMode(motor_pin,OUTPUT);

  Serial.begin(115200);

  Wire.begin();
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

}

void loop() {
  // lcd.clear();
  int ir_value=digitalRead(ir);

  if(ir_value==LOW)
  {
    Serial.println("Person is detected on the seat");
    delay(1000);

    int alco_level=analogRead(alco);
    Serial.print("Alcohol Level: ");
    Serial.println(alco_level);

    if(alco_level>=alco_limit)
    {
      Serial.println("Alcohol Detected");
      Serial.println("You Are Drunk");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alcohol detected: ");
      lcd.print(alco_level);

      lcd.setCursor(0, 1);
      lcd.print("You Are Drunk");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Car Stopped");

      myData.b=0;
      myData.d="Car Stopped";
      esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
      
    }

    else
    {
      Serial.println("No Alcohol Detected Save to Drive");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No Alcohol");
      lcd.setCursor(0, 1);
      lcd.print("Save Drive");

      delay(1000);
      myData.b=1;
      myData.d="Car Stopped";
      esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
      
    }

    delay(1000);
  }

  else
  {
    Serial.println("Person Not Detected on Seat");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No Person");
    myData.b=0;
    myData.d="Car Stopped";
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  }

  delay(1000);
}
