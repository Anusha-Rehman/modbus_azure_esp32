void Azuretojson()
{
  WiFiCheck();
  Esp32MQTTClient_Check();
}
void callingMain()
{

      EVENT_INSTANCE* message = Esp32MQTTClient_Event_Generate(JSONMessage, MESSAGE);
      Esp32MQTTClient_SendEventInstance(message);
    
   
}

float transmission(byte slave_add, byte func_code, byte add_hi, byte add_lo, byte point_hi, byte point_lo) {

  byte Modbus_req1[] = {slave_add, func_code, add_hi, add_lo, point_hi, point_lo};
  unsigned short value = crc.Modbus(Modbus_req1, 0, 6);
  byte input_c1 = value & 0xFF ;
  byte input_c2 = value >> 8;
  //Serial.println(c2,HEX);
  byte Modbus_req[] = {slave_add, func_code, add_hi, add_lo, point_hi, point_lo, input_c1, input_c2};
  Serial.println("");
  Serial2.write(Modbus_req, sizeof(Modbus_req));
  Serial2.flush();
  Serial2.readBytes(Modbus_buf, sizeof(Modbus_buf));
  int sizee = Modbus_buf[2] + 5;
  byte output_c1 = Modbus_buf[sizee - 1];
  byte output_c2 = Modbus_buf[sizee - 2];
  byte neww[20];
  for (int i = 0; i < sizee - 2; i++) {
    neww[i] = Modbus_buf[i];
  }
  unsigned short value1 = crc.Modbus(neww, 0, sizee - 2);
  byte generated_c1 = value1 & 0xFF;
  byte generated_c2 = value1 >> 8;
  float float_num;
  if (output_c1 == generated_c2 && output_c2 == generated_c1) {

    x = (Modbus_buf[3] << 24) | (Modbus_buf[4] << 16) | (Modbus_buf[5] << 8) | Modbus_buf[6];
    float_num = *(float*)&x;
  }
  else {

    Serial.print("Incorrect");
  }

  return float_num;

}

void json(float v, float c, float p, float pf) {
  doc["Voltage (V)"] = v;
  doc["Current (A)"] = c;
  doc ["Power (W)"] = p;
  doc ["Power Factor"] = pf;
  serializeJson(doc, Serial);
  Serial.println();
  serializeJsonPretty(doc, JSONMessage);

}
