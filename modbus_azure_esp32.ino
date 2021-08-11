
#include <Crc16.h>
#include <ArduinoJson.h>
#include "AzureIotHub.h"
#include "Esp32MQTTClient.h"
#include "config.h"
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <WebServer.h> //Local DNS Server used for redirecting all requests to the configuration portal (  https://github.com/zhouhan0126/DNSServer---esp32  )
#include <DNSServer.h> //Local WebServer used to serve the configuration portal (  https://github.com/zhouhan0126/DNSServer---esp32  )
#include <WiFiManager.h>   // WiFi Configuration Magic (  https://github.com/zhouhan0126/DNSServer---esp32  ) >>  https://github.com/zhouhan0126/DNSServer---esp32  (ORIGINAL)

StaticJsonDocument<800> doc;

char JSONMessage[500];
#include <SoftwareSerial.h>

#include <EEPROM.h>
#include <NTPClient.h>

SoftwareSerial RS485 (2, 4);  //RX, TX //Correct

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
WiFiManager wifiManager;


#define INTERVAL 10000
#define MESSAGE_MAX_LEN 256
// Please input the SSID and password of WiFi
const char* ssid     = "StormFiber-2.4G";
const char* password = "40842585";
Crc16 crc;

uint32_t x;
byte Modbus_buf[50];

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
//static const char* connectionString = "HostName=new-esp32s.azure-devices.net;DeviceId=new-esp32s;SharedAccessKey=jzhtdeToszKrmDJljzhbA6usLmYDw/JvFD24Trms2+Y=";
static const char* connectionString = "HostName=new-esp32s.azure-devices.net;DeviceId=esp32ss;SharedAccessKey=K71pPlQttYTpEtYIuXFspMjRag6n9Q+rdyAJ2QWDN5A=";

//const char *messageData = "{\"messageId\":%d, \"Temperature\":%f, \"Humidity\":%f}";
static bool hasIoTHub = false;
static bool hasWifi = false;
int messageCount = 1;
static bool messageSending = true;
static uint64_t send_interval_ms;


void setup() {
  
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial.println("ESP32 Device");
  Serial.println("Initializing...");
  Serial.println(" > WiFi");
  Serial.println("Starting connecting WiFi.");
  wifiConnect();

    delay(10);
    WiFi.mode(WIFI_AP);
    WiFi.enableSTA(true);
  //
    WiFi.begin(ssid, password);
  //
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
    }
  //
    Serial.println("Connected to the WiFi network");
 
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println(" > IoT Hub");

 
  Esp32MQTTClient_SetOption(OPTION_MINI_SOLUTION_NAME, "GetStarted");
  Esp32MQTTClient_Init((const uint8_t*)connectionString, true);

  hasIoTHub = true;
  Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
  Esp32MQTTClient_SetMessageCallback(MessageCallback);
  Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
  Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);
  Serial.println("Start sending events.");
  randomSeed(analogRead(0));
  send_interval_ms = millis();

}

void loop() // run over and over
{

  float voltage = transmission(0x01, 0x04, 0x00, 0x00, 0x00, 0x02);
  delay(200);
  float current = transmission(0x01, 0x04, 0x00, 0x06, 0x00, 0x02);
  delay(200);
  float power = transmission(0x01, 0x04, 0x00, 0x0C, 0x00, 0x02);
  delay(200);
  float pf = transmission(0x01, 0x04, 0x00, 0x1E, 0x00, 0x02);
  delay(200);

  json(voltage, current, power, pf);
  callingMain();
  Azuretojson();   
  
  delay(5000);
}
