#pragma once
#include <esp_now.h>
#include <OSCMessage.h>

class esp_now_osc {
public:
  typedef void (*DataSentCallback)(const uint8_t *mac_addr, esp_now_send_status_t status);

  static void DefaultOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  }

  void setup(const uint8_t broadcastAddress[6], DataSentCallback callback = DefaultOnDataSent) {
    memcpy(_broadcastAddress, broadcastAddress, sizeof(_broadcastAddress));

    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      return;
    }

    esp_now_register_send_cb(callback);

    memcpy(_peerInfo.peer_addr, _broadcastAddress, sizeof(_broadcastAddress));
    _peerInfo.channel = 0;
    _peerInfo.encrypt = false;

    if (esp_now_add_peer(&_peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
    }
  }

  esp_err_t send(OSCMessage &msg) {
    // Determine the size of the serialized message
    size_t bufferSize = calculateSerializedSize(msg);
    uint8_t buffer[bufferSize];

    // Serialize the OSCMessage into the buffer
    serializeOSCMessage(msg, buffer, bufferSize);

    // Send the buffer using ESP-NOW
    return esp_now_send(_broadcastAddress, buffer, bufferSize);
  }

  esp_err_t send(OSCBundle &bundle) {
    // Determine the size of the serialized bundle
    size_t bufferSize = calculateSerializedSize(bundle);
    uint8_t buffer[bufferSize];

    // Serialize the OSCBundle into the buffer
    serializeOSCBundle(bundle, buffer, bufferSize);

    // Send the buffer using ESP-NOW
    return esp_now_send(_broadcastAddress, buffer, bufferSize);
  }

private:
  uint8_t _broadcastAddress[6];
  esp_now_peer_info_t _peerInfo;

  // Calculate the size of the serialized OSC message or bundle
  size_t calculateSerializedSize(OSCMessage &msg) {
    size_t size = 0;

    // Address length and padding
    int addrLen = strlen(msg.getAddress()) + 1;
    size += addrLen + padSize(addrLen);

    // Type tags length and padding
    size_t typeTagLen = msg.dataCount() + 1; // Including the ',' prefix
    size += typeTagLen + padSize(typeTagLen);

    // Data length and padding
    for (int i = 0; i < msg.dataCount(); i++) {
      size += msg.getOSCData(i)->bytes + padSize(msg.getOSCData(i)->bytes);
    }

    return size;
  }

  // Helper function to calculate padding for 4-byte alignment
  size_t padSize(size_t size) {
    return (4 - (size % 4)) % 4;
  }

  // Serialize OSCMessage into a buffer
  void serializeOSCMessage(OSCMessage &msg, uint8_t *buffer, size_t bufferSize) {
    uint8_t *ptr = buffer;

    // Serialize the address
    const char *address = msg.getAddress();
    int addrLen = strlen(address) + 1;
    memcpy(ptr, address, addrLen);
    ptr += addrLen;

    // Add padding for address
    int addrPad = padSize(addrLen);
    memset(ptr, '\0', addrPad);
    ptr += addrPad;

    // Serialize the type tag string
    *ptr++ = ','; // Type tag prefix
    for (int i = 0; i < msg.dataCount(); i++) {
      *ptr++ = msg.getType(i);
    }

    // Add padding for type tags
    int typePad = padSize(msg.dataCount() + 1);
    memset(ptr, '\0', typePad);
    ptr += typePad;

    // Serialize the data
    for (int i = 0; i < msg.dataCount(); i++) {
      const auto datum = msg.getOSCData(i);
      memcpy(ptr, datum->data.b, datum->bytes);
      ptr += datum->bytes;

      // Add padding for data
      int dataPad = padSize(datum->bytes);
      memset(ptr, '\0', dataPad);
      ptr += dataPad;
    }
  }

  // Serialize OSCBundle into a buffer (similar logic applies)
  void serializeOSCBundle(OSCBundle &bundle, uint8_t *buffer, size_t bufferSize) {
    // Implementation follows the same structure as serializeOSCMessage
    // Serialize each message within the bundle
    uint8_t *ptr = buffer;

    for (int i = 0; i < bundle.size(); i++) {
      OSCMessage *msg = bundle.getMessage(i);
      size_t msgSize = calculateSerializedSize(*msg);

      serializeOSCMessage(*msg, ptr, msgSize);
      ptr += msgSize;
    }
  }
};
