/* Class of Zigbee Light Sensor sensor endpoint inherited from common EP class */

#pragma once

#include "soc/soc_caps.h"
#include "sdkconfig.h"
#if SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED

#include "ZigbeeEP.h"
#include "ha/esp_zigbee_ha_standard.h"

#define ESP_ZB_DEFAULT_LIGHT_SENSOR_CONFIG()                                          \
    {                                                                                               \
        .basic_cfg =                                                                                \
            {                                                                                       \
                .zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,                          \
                .power_source = ESP_ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE,                        \
            },                                                                                      \
        .identify_cfg =                                                                             \
            {                                                                                       \
                .identify_time = ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE,                   \
            },                                                                                      \
        .illuminance_cfg =                                                                              \
            {                                                                                           \
                .measured_value = ESP_ZB_ZCL_ILLUMINANCE_MEASUREMENT_LIGHT_SENSOR_TYPE_DEFAULT_VALUE,   \
                .min_value = ESP_ZB_ZCL_ILLUMINANCE_MEASUREMENT_LIGHT_SENSOR_TYPE_DEFAULT_VALUE,        \
                .max_value = ESP_ZB_ZCL_ILLUMINANCE_MEASUREMENT_LIGHT_SENSOR_TYPE_DEFAULT_VALUE,        \
            },                                                                                          \
    }                                                                                               

class ZigbeeLightSensor : public ZigbeeEP {
public:
	ZigbeeLightSensor(uint8_t endpoint);
  ~ZigbeeLightSensor();

  // Set the light value in lux
  void setIlluminance(uint16_t value);

  // Set the min and max value for the light sensor in lux
  void setMinMaxValue(uint16_t min, uint16_t max);

  // Set the tolerance value of the light sensor
  void setTolerance(uint16_t zb_tolerance);
  
  // Set the reporting interval for light measurement and delta in lux
  void setReporting(uint16_t min_interval, uint16_t max_interval, float delta);

  // Report the light value
  void reportIlluminance();

};

#endif  //SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED
