#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <esp_now_midi.h>
#include <WiFi.h>
#include <Adafruit_TinyUSB.h>

// Adafruit_USBD_CDC_ECM usb_ethernet;
IPAddress gateway;
void setup()
{
  Serial.begin(115200);

  // Initialize USB Ethernet
  // usb_ethernet.begin("ESP32 USB Network", "ESP32", "123456");
  Serial.println("USB Network Device Started");

  // Wait for the network to be ready
  delay(2000);

  // Get the gateway IP (typically the host's IP)
  gateway = WiFi.gatewayIP();
  Serial.print("Host (Gateway) IP: ");
  Serial.println(gateway);

  WiFi.mode(WIFI_STA);

  // readMacAddress();

  if (TinyUSBDevice.mounted())
  {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop()
{
#ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
#endif
}

void readMacAddress()
{
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK)
  {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  }
  else
  {
    Serial.println("Failed to read MAC address");
  }
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  OSCMessage message;
  message.fill(incomingData, len);
  // TODO: send osc message to host (should hopefully be gateway) via udp
  // TODO: needs a mechanism for larger messages than the max OSC NOW package size
}
