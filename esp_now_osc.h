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


    // TODO: take code from cnmat/oscmessage send function to serialize the message

//     OSCMessage& OSCMessage::send(Print &p){
//     //don't send a message with errors
//     if (hasError()){
//         return *this;
//     }
//     uint8_t nullChar = '\0';
//     //send the address
//     int addrLen = strlen(address) + 1;
//     //padding amount
//     int addrPad = padSize(addrLen);
//     //write it to the stream
//     p.write((uint8_t *) address, addrLen);
//     //add the padding
//     while(addrPad--){
//         p.write(nullChar);
//     }
//     //add the comma separator
//     p.write((uint8_t) ',');
//     //add the types
// #ifdef PAULSSUGGESTION
//     // Paul suggested buffering on the stack
//     // to improve performance. The problem is this could exhaust the stack
//     // for long complex messages
//     {
//         uint8_t typstr[dataCount];

//         for (int i = 0; i < dataCount; i++){
//             typstr[i] =  getType(i);
//         }
//         p.write(typstr,dataCount);
//     }
// #else
//     for (int i = 0; i < dataCount; i++){
//         p.write((uint8_t) getType(i));
//     }
// #endif
//     //pad the types
//     int typePad = padSize(dataCount + 1); // 1 is for the comma
//     if (typePad == 0){
//             typePad = 4;  // This is because the type string has to be null terminated
//     }
//     while(typePad--){
//         p.write(nullChar);
//     }
//     //write the data
//     for (int i = 0; i < dataCount; i++){
//         const auto datum = getOSCData(i);
//         if ((datum->type == 's') || (datum->type == 'b')){
//             p.write(datum->data.b, datum->bytes);
//             int dataPad = padSize(datum->bytes);
//             while(dataPad--){
//                 p.write(nullChar);
//             }
//         } else if (datum->type == 'd'){
//             double d = BigEndian(datum->data.d);
//             uint8_t * ptr = (uint8_t *) &d;
//             p.write(ptr, 8);
//         } else if (datum->type == 't'){
//             osctime_t time =  datum->data.time;
//             uint32_t d = BigEndian(time.seconds);
//             uint8_t * ptr = (uint8_t *)    &d;
//             p.write(ptr, 4);
//             d = BigEndian(time.fractionofseconds);
//             ptr = (uint8_t *)    &d;
//             p.write(ptr, 4);

//         } else if (datum->type == 'T' || datum->type == 'F')
//                     { }
//         else { // float or int
//             uint32_t i = BigEndian(datum->data.i);
//             uint8_t * ptr = (uint8_t *) &i;
//             p.write(ptr, datum->bytes);
//         }
//     }
//     return *this;
// }
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
