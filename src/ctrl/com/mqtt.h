#ifndef _H_LINUXTOOLS_CTRL_COM_MQTT
#define _H_LINUXTOOLS_CTRL_COM_MQTT

struct mqtt_handle;

#ifdef __cplusplus
extern "C"
{
#endif

  struct mqtt_handle * mqtt_init(const char * client_id, const char * topic, int qos);

  void mqtt_publish(struct mqtt_handle * hnd, const char * type, const char * entity, int value);
  void mqtt_publish_formatted(struct mqtt_handle * hnd, const char * type, const char * entity, const char * fmt, ...);
  void mqtt_loop(struct mqtt_handle * hnd, int timeout);
  void mqtt_close(struct mqtt_handle * hnd);

#ifdef __cplusplus
}
#endif

#endif // _H_LINUXTOOLS_CTRL_COM_MQTT