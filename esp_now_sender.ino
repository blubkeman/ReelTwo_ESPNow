#include "ReelTwo.h"
#include "ESP_NOW_sender.h"
#include "bt/PSController/PSController.h"

// ------------------------------
// ESP-NOW, communication with dome

// Put the WiFi MAC address of the EPS32 receiver board here
uint8_t espnowReceiverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
ESP_NOW espNOW(espnowReceiverAddress);  // Create an ESP-NOW sender

// ------------------------------
// Foward declarations

void enableController();
void disableController();
void emergencyStop();

// ------------------------------
// Controller interpretation

void interpretController(JoystickController::State* pState)
{
  if (pState->button.up > 0) {
    // take some action
  } else if (pState->button.right > 0) {
    // take some action
  } else if (pState->button.down > 0) {
    // take some action
  } else if (pState->button.left > 0) {
    // take some action
  }
};

// ------------------------------
// Class definitions

class DriveController : public PSController
{
public:
    DriveController(const char* mac = nullptr)
        : PSController(mac) {}

    void notify() override
    {
        uint32_t currentTime = millis();
        uint32_t lagTime = (currentTime > _lastTime) ? currentTime - _lastTime : 0;
  
        if (lagTime > 5000) {
          DEBUG_PRINTLN("More than 5 seconds. Disconnect");
          emergencyStop();
          disconnect();
  
        } else if (lagTime > 500) {
          DEBUG_PRINTLN("It has been 500ms. Shutdown drive motors");
          emergencyStop();
  
        } else {
  
          if (state.button.ps >0) {
            if (state.button.share > 0 || state.button.select > 0) {
              disableController();
            } else if (state.button.options > 0 || state.button.start > 0) {
              enableController();
            }
  
          } else {
  
              // Send the joystick controller state data to the dome
              espNOW.send(&state);

              // Do something with the controller input
              interpretController(&state);
          }
        }
        _lastTime = currentTime;
    }

    void onConnect() override
    {
        DEBUG_PRINTLN("Drive Stick Connected");
        setPlayer(1);
        enableController();
        _lastTime = millis();
    }

    void onDisconnect() override
    {
        DEBUG_PRINTLN("Drive Stick Disconnected");
        disableController();
    }

private:
    uint32_t _lastTime = 0;
};


// ================================
//           Main Program
// ================================

void setup()
{
    REELTWO_READY();

    Serial.begin(115200);
    while (!Serial) { delay(500); }  // Give Serial time to start.

    SetupEvent::ready();
}

void loop() {}
