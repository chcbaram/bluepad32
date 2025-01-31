#include "sdkconfig.h"
#include "Bluepad32.h"

extern "C"
{
#include <btstack_port_esp32.h>
#include <btstack_run_loop.h>
#include <hci_dump.h>
#include <hci_dump_embedded_stdout.h>

#include "uni_esp32.h"
#include "uni_main.h"
#include "uni_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
};



GamepadPtr myGamepads[BP32_MAX_GAMEPADS];


void setup(void);
void loop(void);



static void mainThread(void* params) 
{
  setup();
  for (;;) 
  {
    loop();
  }
}

extern "C" void esp32_main(void)
{
  uni_log("esp32_main()\n");
  xTaskCreatePinnedToCore(mainThread, "mainThread", 8*1024, NULL, 1, NULL, 1);  
}




// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedGamepad(GamepadPtr gp) 
{
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) 
  {
    if (myGamepads[i] == nullptr) 
    {
      uni_log("CALLBACK: Gamepad is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      GamepadProperties properties = gp->getProperties();
      //Console.printf("Gamepad model: %s, VID=0x%04x, PID=0x%04x\n", gp->getModelName(), properties.vendor_id,
      //                properties.product_id);
      myGamepads[i] = gp;
      foundEmptySlot = true;
      break;
    }
  }
  if (!foundEmptySlot) 
  {
    uni_log("CALLBACK: Gamepad connected, but could not found empty slot");
  }
}

void onDisconnectedGamepad(GamepadPtr gp) 
{
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) 
  {
    if (myGamepads[i] == gp) 
    {
      uni_log("CALLBACK: Gamepad is disconnected from index=%d\n", i);
      myGamepads[i] = nullptr;
      foundGamepad = true;
      break;
    }
  }

  if (!foundGamepad) 
  {
    uni_log("CALLBACK: Gamepad disconnected, but not found in myGamepads");
  }
}

// setup function. Runs in CPU 1
void setup() 
{
  uni_log("Firmware: %s\n", BP32.firmwareVersion());

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();
}

// loop function. Runs in CPU 1
void loop() 
{
  // This call fetches all the gamepad info from the NINA (ESP32) module.
  // Just call this function in your main loop.
  // The gamepads pointer (the ones received in the callbacks) gets updated
  // automatically.
  BP32.update();

  // It is safe to always do this before using the gamepad API.
  // This guarantees that the gamepad is valid and connected.
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) 
  {
    GamepadPtr myGamepad = myGamepads[i];

    if (myGamepad && myGamepad->isConnected()) 
    {
      // There are different ways to query whether a button is pressed.
      // By query each button individually:
      //  a(), b(), x(), y(), l1(), etc...
      if (myGamepad->a()) 
      {
        static int colorIdx = 0;
        // Some gamepads like DS4 and DualSense support changing the color LED.
        // It is possible to change it by calling:
        switch (colorIdx % 3) 
        {
          case 0:
            // Red
            myGamepad->setColorLED(255, 0, 0);
            break;
          case 1:
            // Green
            myGamepad->setColorLED(0, 255, 0);
            break;
          case 2:
            // Blue
            myGamepad->setColorLED(0, 0, 255);
            break;
        }
        colorIdx++;
      }

      if (myGamepad->b()) 
      {
        // Turn on the 4 LED. Each bit represents one LED.
        static int led = 0;
        led++;
        // Some gamepads like the DS3, DualSense, Nintendo Wii, Nintendo Switch
        // support changing the "Player LEDs": those 4 LEDs that usually indicate
        // the "gamepad seat".
        // It is possible to change them by calling:
        myGamepad->setPlayerLEDs(led & 0x0f);
      }

      if (myGamepad->x()) 
      {
        // Duration: 255 is ~2 seconds
        // force: intensity
        // Some gamepads like DS3, DS4, DualSense, Switch, Xbox One S support
        // rumble.
        // It is possible to set it by calling:
        myGamepad->setRumble(0xc0 /* force */, 0xc0 /* duration */);
      }

      // Another way to query the buttons, is by calling buttons(), or
      // miscButtons() which return a bitmask.
      // Some gamepads also have DPAD, axis and more.
      //Console.printf(
      uni_log(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, "
        "%4d, brake: %4d, throttle: %4d, misc: 0x%02x\n",
        i,                        // Gamepad Index
        myGamepad->dpad(),        // DPAD
        myGamepad->buttons(),     // bitmask of pressed buttons
        myGamepad->axisX(),       // (-511 - 512) left X Axis
        myGamepad->axisY(),       // (-511 - 512) left Y axis
        myGamepad->axisRX(),      // (-511 - 512) right X axis
        myGamepad->axisRY(),      // (-511 - 512) right Y axis
        myGamepad->brake(),       // (0 - 1023): brake button
        myGamepad->throttle(),    // (0 - 1023): throttle (AKA gas) button
        myGamepad->miscButtons()  // bitmak of pressed "misc" buttons
      );

      // You can query the axis and other properties as well. See Gamepad.h
      // For all the available functions.
    }
  }

  vTaskDelay(50 / portTICK_PERIOD_MS);
}
