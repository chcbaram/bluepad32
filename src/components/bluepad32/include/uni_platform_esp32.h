/****************************************************************************
http://retro.moe/unijoysticle2

Copyright 2021 Ricardo Quesada

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
****************************************************************************/

#ifndef UNI_PLATFORM_ESP32_H
#define UNI_PLATFORM_ESP32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "uni_common.h"
#include "uni_gamepad.h"
#include "uni_platform.h"

enum {
    UNI_ESP32_OK = 0,
    UNI_ESP32_ERROR = -1,
};

enum {
    UNI_ESP32_GAMEPAD_INVALID = -1,
};

enum {
    ESP32_PROPERTY_FLAG_RUMBLE = BIT(0),
    ESP32_PROPERTY_FLAG_PLAYER_LEDS = BIT(1),
    ESP32_PROPERTY_FLAG_PLAYER_LIGHTBAR = BIT(2),
};

typedef uni_gamepad_t esp32_gamepad_data_t;

typedef struct {
    uint8_t btaddr[6];    // BT Addr
    uint8_t type;         // Gamepad model: PS3, PS4, Switch, etc.
    uint8_t subtype;      // subtype. E.g: Wii Remote 2nd version
    uint16_t vendor_id;   // VID
    uint16_t product_id;  // PID
    uint16_t flags;       // Features like Rumble, LEDs, etc.
} esp32_gamepad_properties_t;

typedef struct {
    int8_t idx;  // Gamepad index
    esp32_gamepad_data_t data;

    // TODO: To reduce RAM, the properties should be calculated at "request time", and
    // not store them "forever".
    esp32_gamepad_properties_t properties;
} esp32_gamepad_t;

struct uni_platform* uni_platform_esp32_create(void);

int esp32_get_gamepad_data(int idx, esp32_gamepad_data_t* out_data);
int esp32_get_gamepad_properties(int idx, esp32_gamepad_properties_t* out_properties);
int esp32_set_player_leds(int idx, uint8_t leds);
int esp32_set_lightbar_color(int idx, uint8_t r, uint8_t g, uint8_t b);
int esp32_set_rumble(int idx, uint8_t force, uint8_t duration);
int esp32_forget_bluetooth_keys(void);

#ifdef __cplusplus
}
#endif

#endif  // UNI_PLATFORM_ESP32_H
