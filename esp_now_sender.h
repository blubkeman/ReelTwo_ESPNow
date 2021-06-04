#ifndef _ESP_NOW_H_
#define _ESP_NOW_H_

#include <esp_now.h>
#include <WiFi.h>
#include "core/SetupEvent.h"
#include "JoystickController.h"

extern void interpretController(void);

class ESP_NOW : public SetupEvent
{
public:

    /** \brief
      * Constructor
      */
    ESP_NOW(uint8_t receiverAddress[])
    {
    	memcpy(fReceiverAddress, receiverAddress, sizeof(fReceiverAddress));
    }

    /**	\brief
      * Public members
      */
    JoystickController::State* buffer;

    /** /brief
      * Perform any initialzation not possible in the constructor
      */
    virtual void setup() override
    {
        // Set device as a Wi-Fi Station
        WiFi.mode(WIFI_STA);

        // Init ESP-NOW
        if (esp_now_init() != ESP_OK) {
            Serial.println("Error initializing ESP-NOW");
            return;
        }

        // Register for send callback to get the status of trasnmitted packet
        esp_now_register_send_cb(OnDataSent);

        // Register peer
        esp_now_peer_info_t peerInfo;
        memcpy(peerInfo.peer_addr, fReceiverAddress, 6);
        peerInfo.channel = 0;  
        peerInfo.encrypt = false;

        // Add peer        
        if (esp_now_add_peer(&peerInfo) != ESP_OK){
            Serial.println("Failed to add peer");
            return;
        }
    }

    /** /brief
      * Send the joystick controller state data to the receiver ESP32 board
      */
    void send(JoystickController::State* outgoingData)
    {
        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(fReceiverAddress, (uint8_t *) outgoingData, sizeof(outgoingData));
        if (result != ESP_OK) {
          Serial.println("Error sending the data");
        }
    }

	/** /brief
	  * Callback function for error handling when data is sent
	  */
    static void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
        Serial.print("\r\nLast Packet Send Status:\t");
        Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    }

private:
  	uint8_t fReceiverAddress[6] = { NULL };
};

#endif
