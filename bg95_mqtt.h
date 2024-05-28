/*
 * bg95_mqtt.h
 *
 * Created: 14/05/2024 01:58:44 p. m.
 *  Author: JPMB
 */ 


#ifndef BG95_MQTT_H_
#define BG95_MQTT_H_

void mqtt_init(void);
void mqtt_connect(void);
void mqtt_subscribe(const char *topic);
void mqtt_read(void);
void mqtt_pub_str(const char *topic, const char *message);
void mqtt_pub_int(const char *topic, const int message);
void mqtt_pub_char(const char *topic, const char message);
void mqtt_pub_unsigned_short(const char *topic, unsigned short message);
void mqtt_pub_float(const char *topic, const float message);
void mqtt_pub_float_buffer(const char *topic, float *message, size_t buffersize);
void mqtt_disconnect(void);

#endif /* BG95_MQTT_H_ */