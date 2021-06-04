#ifndef _ESP_NOW_H_
#define _ESP_NOW_H_

#include <esp_now.h>
#include <WiFi.h>
#include "JoystickController.h"

extern void interpretController(void);

class ESP_NOW
{
public:

	// ------------------------------
	// Constructors for Sender and Receiver modes

    ESP_NOW(void)
    	: fRole(Receiver)
    	, fAvailable(false) 
    {}

    ESP_NOW(uint8_t receiverAddress[])
    	: fRole(Sender)
    	, fAvailable(false)
    {
    	memcpy(fReceiverAddress, receiverAddress, sizeof(fReceiverAddress));
    }

	// ------------------------------
	// Public members

    static ESP_NOW* anchor;
    JoystickController::State* buffer;

	// ------------------------------
	// Public functions

    void begin(void)
    {
        anchor = this;
      
        // Set device as a Wi-Fi Station
        WiFi.mode(WIFI_STA);

        // Init ESP-NOW
        if (esp_now_init() != ESP_OK) {
            Serial.println("Error initializing ESP-NOW");
            return;
        }

      	if (fRole == Sender)
      	{
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

		else if (fRole == Receiver)
      	{
        	// Register for recv callback to get recv packer info
        	esp_now_register_recv_cb(OnDataRecv);
		}
    }

    void send(JoystickController::State* outgoingData)
    {
        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(fReceiverAddress, (uint8_t *) outgoingData, sizeof(outgoingData));
        if (result != ESP_OK) {
          Serial.println("Error sending the data");
        }
    }

    bool available(void)
    {
    	return fAvailable;
    }

    bool flush(void)
    {
    	fAvailable = false;
    	memcpy(buffer, 0, sizeof(buffer));
    }

	// ------------------------------
	// Callback functions

    static void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
        Serial.print("\r\nLast Packet Send Status:\t");
        Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    }

    static void OnDataRecv(const uint8_t* mac_addr, const uint8_t* incomingData, int len) {
        if (anchor != NULL) {
            anchor->unloadPacket(incomingData);	// This gets us out of the static function so we can update local variables
        }
    }

private:
	// ------------------------------
	// Private members

	enum esp_now_role_e
	{
		Sender,
		Receiver
	};
	esp_now_role_e fRole;

	bool fAvailable;
  	uint8_t fReceiverAddress[6] = { NULL };

	// ------------------------------
	// Private functions

	// The receiver board should use loop() to look for available data in the buffer.
	// It should then use flush() after processing the data to clear the buffer.
	// This will only populate the buffer when it is empty (i.e. data is not available).
    void unloadPacket(const uint8_t* incomingData)
    {
    	if (!available()) {
        	memcpy(buffer, incomingData, sizeof(buffer));
        	fAvailable = true;
        }
    }
};

#endif
