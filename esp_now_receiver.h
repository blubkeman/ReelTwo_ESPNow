#ifndef _ESP_NOW_H_
#define _ESP_NOW_H_

#include <esp_now.h>
#include <WiFi.h>
#include "core/AnimatedEvent.h"
#include "core/SetupEvent.h"
#include "JoystickController.h"

extern void interpretController(JoystickController::State*);

class ESP_NOW : public SetupEvent, AnimatedEvent
{
public:

    /** \brief Constructor
      *		Sender and Receiver modes
      */
    ESP_NOW(void)
    	: fAvailable(false) 
    {}

    /**	\brief
      * Public members
      */
    static ESP_NOW* anchor;
    JoystickController::State* buffer;

    /** /brief
      * Perform any initialzation not possible in the constructor
      */
    virtual void setup() override
    {
        anchor = this;
      
        // Set device as a Wi-Fi Station
        WiFi.mode(WIFI_STA);

        // Init ESP-NOW
        if (esp_now_init() != ESP_OK) {
            Serial.println("Error initializing ESP-NOW");
            return;
        }

        // Register for recv callback to get recv packer info
        esp_now_register_recv_cb(OnDataRecv);
	}

    /** /brief
      * Process any received data.
      */
    virtual void animate() override
    {
    	if (available())
  		{
    		interpretController(buffer);	// Process the controller state data.
    		flush();						// Clear the buffer for the next cycle.
  		}
	}

	// ------------------------------
	// Callback functions

	/** /brief
	  * Handles the receiving of data.
	  */
    static void OnDataRecv(const uint8_t* mac_addr, const uint8_t* incomingData, int len) {
        if (anchor != NULL) {
            anchor->unloadPacket(incomingData);	// This gets us out of the static function so we can update local variables
        }
    }

private:
	bool fAvailable;

	/** /brief
	  * When the buffer is clear, get a copy of the incoming data and
	  *  indicate there is data available for processing.
	  */
    void unloadPacket(const uint8_t* incomingData)
    {
    	if (!available()) {
        	memcpy(buffer, incomingData, sizeof(buffer));
        	fAvailable = true;
        }
    }

    /** /brief
      * A flag indicating whether or not data is available to the receiver board
      */
    bool available(void)
    {
    	return fAvailable;
    }

    /** /brief
      * Toggle the available flag and clear the buffer.
      *   Makes the buffer ready to receive more data.
      */
    bool flush(void)
    {
    	fAvailable = false;
    	memcpy(buffer, 0, sizeof(buffer));
    }

};

#endif
