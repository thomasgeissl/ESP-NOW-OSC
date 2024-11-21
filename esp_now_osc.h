#pragma once
#include <esp_now.h>
#include <OSCMessage.h>

class esp_now_osc {
public:
  // Typedef for the callback function signature
  typedef void (*DataSentCallback)(const uint8_t *mac_addr, esp_now_send_status_t status);

  // callback when data is sent (must be static for esp_now_register_send_cb)
  static void DefaultOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  }

  void setup(const uint8_t broadcastAddress[6], DataSentCallback callback = DefaultOnDataSent) {
    // Copy the broadcast address into the class member variable
    memcpy(_broadcastAddress, broadcastAddress, sizeof(_broadcastAddress));

    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      return;
    }

    // Once ESPNow is successfully initialized, register Send callback to get the status of the transmitted packet
    esp_now_register_send_cb(callback);

    // Register peer
    memcpy(_peerInfo.peer_addr, _broadcastAddress, sizeof(_broadcastAddress));
    _peerInfo.channel = 0;
    _peerInfo.encrypt = false;

    // Add peer
    if (esp_now_add_peer(&_peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
    }
  }

  esp_err_t send(OSCMessage &msg) {
    midi_message message;
    message.channel = channel;
    message.status = MIDI_NOTE_ON;
    message.firstByte = note;
    message.secondByte = velocity;
    return esp_now_send(_broadcastAddress, (uint8_t *)&message, sizeof(message));
  }
  esp_err_t send(OSCBundle &bundle) {
    midi_message message;
    message.channel = channel;
    message.status = MIDI_NOTE_ON;
    message.firstByte = note;
    message.secondByte = velocity;
    return esp_now_send(_broadcastAddress, (uint8_t *)&message, sizeof(message));
  }

private:
  uint8_t _broadcastAddress[6];
  esp_now_peer_info_t _peerInfo;
};
