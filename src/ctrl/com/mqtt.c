#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <mosquitto.h>

#include "mqtt.h"
#include "../logger.h"

struct mqtt_handle
{
  struct mosquitto * mosq;
  struct mqtt_config * cfg;
};

#pragma GCC diagnostic ignored "-Wunused-parameter"

void on_connect(struct mosquitto * mosq, void * userdata, int mid)
{
  struct mqtt_handle * hnd = (struct mqtt_handle *) userdata;
  LG_INFO("MQTT - Connection to broker established.");
  if (hnd->cfg->subs) {
    struct mqtt_sub * sub = hnd->cfg->subs;
    while (sub && sub->topic) {
      switch (mosquitto_subscribe(mosq, NULL, sub->topic, hnd->cfg->qos)) {
        case MOSQ_ERR_INVAL:           LG_ERROR("Could not subscribe %s - invalid params.", sub->topic);   break;
        case MOSQ_ERR_NOMEM:           LG_ERROR("Could not subscribe %s - out of memory.", sub->topic);    break;
        case MOSQ_ERR_NO_CONN:         LG_ERROR("Could not subscribe %s - no connection.", sub->topic);    break;
        case MOSQ_ERR_MALFORMED_UTF8:  LG_ERROR("Could not subscribe %s - no valid utf-8.", sub->topic);   break;
        case MOSQ_ERR_OVERSIZE_PACKET: LG_ERROR("Could not subscribe %s - oversized packet.", sub->topic); break;
        case MOSQ_ERR_SUCCESS:         LG_INFO("Subscribed '%s'.", sub->topic);                            break;
        default:                       break;
      }
      ++sub;
    }
  }

}

void on_publish(struct mosquitto *mosq, void * userdata, int mid)
{
//  LG_DEBUG("MQTT - Value published.");
}

void on_message(struct mosquitto *mosq, void * userdata, const struct mosquitto_message * msg) {
  struct mqtt_handle * hnd = (struct mqtt_handle *) userdata;
  LG_DEBUG("Received message on topic %s (id:%d): %s.", msg->topic, msg->mid, (char *) msg->payload);
  for (struct mqtt_sub * sub = hnd->cfg->subs; sub && sub->topic; ++sub) {
    if (sub->cb) {
      sub->cb(msg->topic, (char *) msg->payload);
      break;
    }
  }
}

void on_disconnect(struct mosquitto *mosq, void *userdata, int mid)
{
  LG_ERROR("MQTT - Connection to broker disconnected!");
}

#pragma GCC diagnostic warning "-Wunused-parameter"


enum mqtt_retval mqtt_init(struct mqtt_handle ** hnd, struct mqtt_config * cfg)
{
  int result;

  if (*hnd == NULL)
  {
    LG_DEBUG("Initializing connection to MQTT broker.");

    *hnd = calloc(sizeof(struct mqtt_handle), 1);
    if (*hnd == NULL)
    {
      LG_CRITICAL("Could not allocate resources for MQTT Connection!");
      goto init_mqtt_fail;
    }

    (*hnd)->cfg = cfg;
    mosquitto_lib_init();
    LG_DEBUG("MQTT library initialized.");

    (*hnd)->mosq = mosquitto_new((*hnd)->cfg->client_id, TRUE, *hnd);
    if ((*hnd)->mosq == NULL)
    {
      LG_CRITICAL("MQTT - Could not instantiate a broker socket.");
      goto init_mqtt_fail;
    }
    LG_DEBUG("MQTT broker socket instantiated.");

    result = mosquitto_username_pw_set((*hnd)->mosq, (*hnd)->cfg->client_id, (*hnd)->cfg->client_id);
    if (result != MOSQ_ERR_SUCCESS)
    {
      LG_CRITICAL("Could not set MQTT broker user: %d", result);
      goto init_mqtt_fail;
    }
    LG_DEBUG("MQTT broker user set.");

    mosquitto_publish_callback_set((*hnd)->mosq, on_publish);
    mosquitto_connect_callback_set((*hnd)->mosq, on_connect);
    mosquitto_disconnect_callback_set((*hnd)->mosq, on_disconnect);
    mosquitto_message_callback_set((*hnd)->mosq, on_message);
    LG_DEBUG("MQTT broker callbacks set.");
  }

  if ((result = mosquitto_connect((*hnd)->mosq, (*hnd)->cfg->remote_address, (*hnd)->cfg->remote_port, 10)) != MOSQ_ERR_SUCCESS)
  {
    if (result == MOSQ_ERR_ERRNO)
      return MQTT_RET_RETRY;
    LG_CRITICAL("MQTT - Could not connect to broker. Connect returned: %u", result);
    goto init_mqtt_fail;
  }
  LG_DEBUG("Success - MQTT broker connected.");
  return MQTT_RET_OK;

init_mqtt_fail:
  if (*hnd) {
    free(*hnd);
    *hnd = NULL;
  }
  return MQTT_RET_FAILED;
}


void mqtt_publish_formatted(struct mqtt_handle * hnd, const char * type, const char * entity, const char * fmt, ...)
{
  static char tmp_val[64];
  static char tmp_msg[255];
  va_list ap;
  int result;

  va_start(ap, fmt);
  result = vsnprintf(tmp_val, sizeof(tmp_val), fmt, ap);
  va_end(ap);

  if (result >= ssizeof(tmp_val))
  {
    LG_ERROR("MQTT - publishing for %s:%s failed - format string exceeded size of %d chars.", type, entity, sizeof(tmp_val));
    return;
  }

  result = snprintf(tmp_msg, sizeof(tmp_msg), "%s,type=%s value=%s", entity, type, tmp_val);

  if (result >= ssizeof(tmp_msg))
  {
    LG_ERROR("MQTT - publishing for %s:%s failed - output string exceeded size of %d chars.", type, entity, sizeof(tmp_msg));
    return;
  }

  LG_DEBUG("MQTT - publishing in topic %s: %s.", hnd->cfg->topic, tmp_msg);
  result = mosquitto_publish(hnd->mosq, NULL, hnd->cfg->topic, strlen(tmp_msg), tmp_msg, hnd->cfg->qos, FALSE);

  switch (result)
  {
    case MOSQ_ERR_SUCCESS            : break;
    case MOSQ_ERR_INVAL              :
    case MOSQ_ERR_NOMEM              :
    case MOSQ_ERR_NO_CONN            :
    case MOSQ_ERR_PROTOCOL           :
    case MOSQ_ERR_PAYLOAD_SIZE       :
    case MOSQ_ERR_MALFORMED_UTF8     :
    case MOSQ_ERR_QOS_NOT_SUPPORTED  :
    case MOSQ_ERR_OVERSIZE_PACKET    :
      LG_ERROR("MQTT - Could not publish to broker. Error returned: %u", result);
      break;
  }
}

void mqtt_publish(struct mqtt_handle * hnd, const char * type, const char * entity, int value)
{
  mqtt_publish_formatted(hnd, type, entity, "%d", value);
}


void mqtt_loop(struct mqtt_handle * hnd, int timeout)
{
  int result;
  result = mosquitto_loop(hnd->mosq, timeout, 1);  // this calls mosquitto_loop() in a loop, it will exit once the client disconnects cleanly
  switch (result)
  {
    case MOSQ_ERR_SUCCESS   : break;
    case MOSQ_ERR_NO_CONN   :
      result = mosquitto_reconnect(hnd->mosq);
      LG_INFO("MQTT - disconnected. Reconnect returns %d.", result);
      break;
    case MOSQ_ERR_INVAL     :
    case MOSQ_ERR_NOMEM     :
    case MOSQ_ERR_CONN_LOST :
    case MOSQ_ERR_PROTOCOL  :
      LG_CRITICAL("MQTT - Could not process broker. Error returned: %u", result);
      break;
    case MOSQ_ERR_ERRNO     :
      LG_CRITICAL("MQTT - Could not process broker. Syscall returned %s", strerror(errno));
      break;
  }
}


void mqtt_close(struct mqtt_handle * hnd)
{
  mosquitto_disconnect(hnd->mosq);
  mosquitto_destroy(hnd->mosq);

}

