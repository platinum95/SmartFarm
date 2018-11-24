#ifndef __MQTT_PUBLISHER_H__
#define __MQTT_PUBLISHER_H__
#ifdef __cplusplus
extern "C"{
#endif
void tb_pubsub_start();
void tb_publish_attributes(const char * attr_payload);
void tb_publish_telemetry(const char * tlmtry_payload);
#ifdef __cplusplus
}
#endif
#endif
