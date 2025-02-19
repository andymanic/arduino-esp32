// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @brief This example demonstrates a Zigbee light sensor.
 *
 * The example demonstrates how to use Zigbee library to create a end device light sensor.
 * The light sensor is a Zigbee end device, which is controlled by a Zigbee coordinator.
 *
 * Proper Zigbee mode must be selected in Tools->Zigbee mode
 * and also the correct partition scheme must be selected in Tools->Partition Scheme.
 *
 * Please check the README.md for instructions and more detailed description.
 *
 * Created by Andrew McDonald (https://youtube.com/techteamgb)
 * Modified from examples by Jan Procházka (https://github.com/P-R-O-C-H-Y/)
 */

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

/* Zigbee light sensor configuration */
#define LIGHT_SENSOR_ENDPOINT_NUMBER 10
uint8_t button = BOOT_PIN;

ZigbeeLightSensor zbLightSensor = ZigbeeLightSensor(LIGHT_SENSOR_ENDPOINT_NUMBER);

/************************ Light sensor *****************************/
static void light_sensor_value_update(void *arg) {
  for (;;) {
    // Read light sensor value
    uint16_t lsens_value = (uint16_t)temperatureRead();
    Serial.printf("Updated light sensor value to %d\r\n", lsens_value);
    // Update illuminance value in light sensor EP
    zbLightSensor.setIlluminance(lsens_value);
    delay(1000);
  }
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);

  // Init button switch
  pinMode(button, INPUT_PULLUP);

  // Optional: set Zigbee device name and model
  zbLightSensor.setManufacturerAndModel("Espressif", "ZigbeeLightSensor");

  // Set minimum and maximum light measurement value 
  zbLightSensor.setMinMaxValue(0, 100);

  // Optional: Set tolerance for light measurement 
  zbLightSensor.setTolerance(1);

  // Add endpoint to Zigbee Core
  Zigbee.addEndpoint(&zbLightSensor);

  Serial.println("Starting Zigbee...");
  // When all EPs are registered, start Zigbee in End Device mode
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  } else {
    Serial.println("Zigbee started successfully!");
  }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Start light sensor reading task
  xTaskCreate(light_sensor_value_update, "light_sensor_update", 2048, NULL, 10, NULL);

  // Set reporting interval for light measurement in seconds, must be called after Zigbee.begin()
  // min_interval and max_interval in seconds, delta
  // if min = 1 and max = 0, reporting is sent only when illuminance changes by delta
  // if min = 0 and max = 10, reporting is sent every 10 seconds or illuminance changes by delta
  // if min = 0, max = 10 and delta = 0, reporting is sent every 10 seconds regardless of illuminance change
  zbLightSensor.setReporting(0, 30, 0);
}

void loop() {
  // Checking button for factory reset
  if (digitalRead(button) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
    zbLightSensor.report();
  }
  delay(100);
}
