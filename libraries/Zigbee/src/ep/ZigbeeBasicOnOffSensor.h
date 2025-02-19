/* Class of Zigbee Pressure sensor endpoint inherited from common EP class */

#pragma once

#include "soc/soc_caps.h"
#include "sdkconfig.h"
#if SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED

#include "ZigbeeEP.h"
#include "ha/esp_zigbee_ha_standard.h"

// clang-format off
#define ZIGBEE_DEFAULT_ON_OFF_CONFIG()                                                       \
  {                                                                                          \
    .basic_cfg =                                                                             \
      {                                                                                      \
        .zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,                           \
        .power_source = ESP_ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE,                         \
      },                                                                                     \
    .identify_cfg =                                                                          \
      {                                                                                      \
        .identify_time = ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE,                    \
      },                                                                                     \
    .state_cfg = {                                                                           \
      .on_off = ESP_ZB_ZCL_ON_OFF_ON_OFF_DEFAULT_VALUE,                                      \
      }                                                                                      \
  }                                                                                          \
// clang-format on

typedef struct esp_zb_on_off_cfg_s {
    esp_zb_basic_cluster_cfg_t basic_cfg;
    esp_zb_identify_cluster_cfg_t identify_cfg;
    esp_zb_on_off_cluster_cfg_t state_cfg;
} esp_zb_on_off_cfg_t;

class ZigbeeBasicOnOffSensor : public ZigbeeEP {
public:
  ZigbeeBasicOnOffSensor(uint8_t endpoint);
  ~ZigbeeBasicOnOffSensor();

  // Set the occupancy value. True for occupied, false for unoccupied
  void setState(bool state);

  // Report the occupancy value
  void report();
};

#endif  //SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED
