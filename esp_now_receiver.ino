#include "ReelTwo.h"
#include "ESP_NOW_receiver.h"

// ------------------------------
// ESP-NOW, communication with dome

ESP_NOW espNOW;                         // Create an ESP-NOW receiver
ESP_NOW* ESP_NOW::anchor = { NULL };    // This is needed to help with the callback functions

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
}


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

void loop()
{
    AnimatedEvent::process();
}
