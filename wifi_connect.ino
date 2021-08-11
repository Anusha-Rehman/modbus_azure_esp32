static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    Serial.println("Send Confirmation Callback finished.");
  }
}

static void MessageCallback(const char* payLoad, int size)
{
  Serial.println("Message callback:");
  Serial.println(payLoad);
}

static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
{
  char *temp = (char *)malloc(size + 1);
  if (temp == NULL)
  {
    return;
  }
  memcpy(temp, payLoad, size);
  temp[size] = '\0';
  // Display Twin message.
  Serial.println(temp);
  free(temp);
}

static int  DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
  LogInfo("Try to invoke method %s", methodName);
  const char *responseMessage = "\"Successfully invoke device method\"";
  int result = 200;

  if (strcmp(methodName, "start") == 0)
  {
    LogInfo("Start sending temperature and humidity data");
    messageSending = true;
  }
  else if (strcmp(methodName, "stop") == 0)
  {
    LogInfo("Stop sending temperature and humidity data");
    messageSending = false;
  }
  else
  {
    LogInfo("No method %s found", methodName);
    responseMessage = "\"No method found\"";
    result = 404;
  }

  *response_size = strlen(responseMessage) + 1;
  *response = (unsigned char *)strdup(responseMessage);

  return result;
}

void wifiConnect() {

  for (int i = 0; i < 30; i++)                      //Delay for 40 Sec in start
  {
    printf("Delay for WiFi %d sec\n", 30 - i);
    delay(1000);
  }
  wifiManager.setTimeout(120);                      //Timeout for hotspot

  //callback when entering AP configuration mode
  wifiManager.setAPCallback(configModeCallback);

  //callback for when you connect to a network
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //creates a network named ESP32 with password 12345678
  if (!wifiManager.autoConnect("ESP32", "12345678")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again
    ESP.restart();
    delay(5000);
  }
}

void wifiDelete() {
  //using this command, the settings are cleared from memory
  //if you have saved a network to connect automatically, it is deleted.
  wifiManager.resetSettings();
}

//callback indicating that ESP has entered AP mode
void configModeCallback (WiFiManager *myWiFiManager) {
  //  Serial.println("Entered config mode");
  Serial.println("Entered configuration mode");
  Serial.println(WiFi.softAPIP()); //prints the IP of the AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //prints the SSID created from the network </p> <p>} </p> <p> // callback that indicates that we have saved a new network to connect to (station mode)

}

void saveConfigCallback () {
  //  Serial.println("Should save config");
  Serial.println("Configuration saved");
  Serial.println(WiFi.softAPIP()); //prints the IP of the AP
}

void WiFiCheck()
{

  Serial.print("Connected to WiFi: ");
  Serial.println(wifiManager.getSSID()); //prints the SSID of the network to which it is connected
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  while (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i < 30; i++)
    {
      printf("Trying to connect with WiFi %d\n", 30 - i);
      delay(1000);
    }
    ESP.restart();
    delay(500);
  }
}

void forceHotspot() {
  //if the button was pressed
  if ( digitalRead(2) == HIGH) {
    Serial.println("reset"); // try to open the portal
    wifiManager.resetSettings();
    if (!wifiManager.startConfigPortal("ESP_AP", "12345678") ) {
      Serial.println("Failed to connect");
      delay(2000);
      ESP.restart();
      delay(1000);
    }
    Serial.println("Connected ESP_AP !!!");
  }
}
