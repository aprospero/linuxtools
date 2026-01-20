#ifndef _H_LINUXTOOLS_CTRL_COM_MQTT
#define _H_LINUXTOOLS_CTRL_COM_MQTT

#define LINUXTOOLS_MQTT_KEEPALIVE 10 // in seconds

struct mqtt_handle;

enum mqtt_retval
{
  MQTT_RET_OK,
  MQTT_RET_RETRY,
  MQTT_RET_FAILED
};

struct mqtt_sub {
  const char * topic;
  void (*cb)(const char * topic, const char * payload);
};


struct mqtt_config
{
  const char *      remote_address;
  int               remote_port;
  const char *      client_id;
  const char *      topic;
  int               qos;
  struct mqtt_sub * subs;
};


#ifdef __cplusplus
extern "C"
{
#endif

  enum mqtt_retval mqtt_init(struct mqtt_handle ** hnd, struct mqtt_config * cfg);

  void mqtt_publish(struct mqtt_handle * hnd, const char * type, const char * entity, int value);
  void mqtt_publish_formatted(struct mqtt_handle * hnd, const char * type, const char * entity, const char * fmt, ...);
  void mqtt_publish_raw(struct mqtt_handle * hnd, const char * topic, const char * payload);
  void mqtt_loop(struct mqtt_handle * hnd, int timeout);
  void mqtt_close(struct mqtt_handle * hnd);

#ifdef __cplusplus
}
#endif

#endif // _H_LINUXTOOLS_CTRL_COM_MQTT
