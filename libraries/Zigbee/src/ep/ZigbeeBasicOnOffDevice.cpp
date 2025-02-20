#include "ZigbeeBasicOnOffDevice.h"
#if SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED

esp_zb_cluster_list_t *zigbee_on_off_clusters_create(esp_zb_on_off_cfg_t *on_off_sensor) {
  esp_zb_basic_cluster_cfg_t *basic_cfg = on_off_sensor ? &(on_off_sensor->basic_cfg) : NULL;
  esp_zb_identify_cluster_cfg_t *identify_cfg = on_off_sensor ? &(on_off_sensor->identify_cfg) : NULL;
  esp_zb_on_off_cluster_cfg_t *on_off_cfg = on_off_sensor ? &(on_off_sensor->state_cfg) : NULL;
  esp_zb_cluster_list_t *cluster_list = esp_zb_zcl_cluster_list_create();
  esp_zb_cluster_list_add_basic_cluster(cluster_list, esp_zb_basic_cluster_create(basic_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_identify_cluster(cluster_list, esp_zb_identify_cluster_create(identify_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_on_off_cluster(cluster_list, esp_zb_on_off_cluster_create(on_off_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_identify_cluster(cluster_list, esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  return cluster_list;
}

ZigbeeBasicOnOffDevice::ZigbeeBasicOnOffDevice(uint8_t endpoint) : ZigbeeEP(endpoint) {
  _device_id = ESP_ZB_HA_SIMPLE_SENSOR_DEVICE_ID;

  //Create custom occupancy sensor configuration
  esp_zb_on_off_cfg_t on_off_cfg = ZIGBEE_DEFAULT_ON_OFF_CONFIG();
  _cluster_list = zigbee_on_off_clusters_create(&on_off_cfg);

  _ep_config = {.endpoint = _endpoint, .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID, .app_device_id = ESP_ZB_HA_SIMPLE_SENSOR_DEVICE_ID, .app_device_version = 0};
}

//set attribute method -> method overridden in child class
void ZigbeeBasicOnOffDevice::zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *message) {
  //check the data and call right method
  if (message->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) {
    if (message->attribute.id == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID && message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_BOOL) {
      _current_state = *(bool *)message->attribute.data.value;
      stateChanged();
    } else {
      log_w("Received message ignored. Attribute ID: %d not supported for On/Off Light", message->attribute.id);
    }
  } else {
    log_w("Received message ignored. Cluster ID: %d not supported for On/Off Light", message->info.cluster);
  }
}

void ZigbeeBasicOnOffDevice::stateChanged() {
  if (_on_state_change) {
    _on_state_change(_current_state);
  } else {
    log_w("No callback function set for state");
  } 
}

void ZigbeeBasicOnOffDevice::setState(bool state) {
  _current_state = state;
  stateChanged();
  log_v("Updating on off sensor value...");
  /* Update on off sensor value */
  log_d("Setting state to %d", state);
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_set_attribute_val(
    _endpoint, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, &state, false
  );
  esp_zb_lock_release();
}

void ZigbeeBasicOnOffDevice::report() {
  /* Send report attributes command */
  esp_zb_zcl_report_attr_cmd_t report_attr_cmd;
  report_attr_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT;
  report_attr_cmd.attributeID = ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID;
  report_attr_cmd.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI;
  report_attr_cmd.clusterID = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF;
  report_attr_cmd.zcl_basic_cmd.src_endpoint = _endpoint;

  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_report_attr_cmd_req(&report_attr_cmd);
  esp_zb_lock_release();
  log_v("State report sent");
}

#endif  //SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED
