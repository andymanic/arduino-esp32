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
 * @brief This example demonstrates Zigbee occupancy sensor.
 *
 * The example demonstrates how to use Zigbee library to create a end device occupancy sensor.
 * The occupancy sensor is a Zigbee end device, which is reporting data to the Zigbee network.
 * Tested with PIR sensor HC-SR501 connected to GPIO4.
 *
 * Proper Zigbee mode must be selected in Tools->Zigbee mode
 * and also the correct partition scheme must be selected in Tools->Partition Scheme.
 *
 * Please check the README.md for instructions and more detailed description.
 *
 * Created by Jan Procházka (https://github.com/P-R-O-C-H-Y/)
 */

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

/* Zigbee on off sensor configuration */
#define ON_OFF_SENSOR_ENDPOINT_NUMBER 10
uint8_t button = BOOT_PIN;
uint8_t sensor_pin = 4;

ZigbeeBasicOnOffSensor zbOnOffSensor = ZigbeeBasicOnOffSensor(ON_OFF_SENSOR_ENDPOINT_NUMBER);

void setup() {
  Serial.begin(115200);

  // Init button + PIR sensor
  pinMode(button, INPUT_PULLUP);
  pinMode(sensor_pin, INPUT);

  // Optional: set Zigbee device name and model
  zbOnOffSensor.setManufacturerAndModel("Espressif", "ZigbeeOnOffSensor");

  // Add endpoint to Zigbee Core
  Zigbee.addEndpoint(&zbOnOffSensor);

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
}

void loop() {
  // Check input state
  static bool state = false;
  if (digitalRead(sensor_pin) == HIGH && !state) {
    // Update state value
    zbOnOffSensor.setState(true);
    zbOnOffSensor.report();
    state = true;
  } else if (digitalRead(sensor_pin) == LOW && state) {
    zbOnOffSensor.setState(false);
    zbOnOffSensor.report();
    state = false;
  }

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
  }
  delay(100);
}
