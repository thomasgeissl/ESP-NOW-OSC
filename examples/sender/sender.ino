#include <esp_now.h>
#include <WiFi.h>
#include <OSCMessage.h>
#include "esp_now_osc.h"


// run the print_mac firmware and adjust the mac address
uint8_t broadcastAddress[6] = { 0xCC, 0x8D, 0xA2, 0x8B, 0x85, 0x1C };

esp_now_osc ESP_NOW_OSC;
void customOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Custom Callback - Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failure");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ESP_NOW_OSC.setup(broadcastAddress, customOnDataSent);
}

void loop() {
  OSCMessage msg("/test/message/via/esp_now");
  msg.add((intOSC_t)(0));
  esp_err_t result = ESP_NOW_OSC.send(msg);

  if (result != ESP_OK) {
    Serial.println("Error sending the data");
  }
  
  delay(2000);
}