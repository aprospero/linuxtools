#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "ctrl/com/mqtt.h"
#include "ctrl/logger.h"
#include "version.h"

#define DEFAULT_LOG_FAC LF_STDOUT
#define DEFAULT_LOG_LEVEL LL_INFO

void receive_test(const char * topic, const char * payload) {
  LG_INFO("received %s -> %s.", topic, payload);
}

struct mqtt_sub subs[] = {
                           { "MTDC"            , receive_test},
                           { "grafana/circ1_on", receive_test},
                           { NULL              , NULL        }
                         };


struct mqtt_config cfg = {
    "localhost",
    1883,
    "linuxtools",
    "test",
    2,
    subs
};


int abort_rx_loop = FALSE;

int read_loop()
{
  struct mqtt_handle * mqtt = NULL;
  int do_log = TRUE;

  while(!abort_rx_loop && mqtt_init(&mqtt, &cfg) == MQTT_RET_RETRY)
  {
    if (do_log)
    {
      LG_WARN("MQTT - Could not connect to broker. Syscall returned '%s'. Retry every 5 sec.", strerror(errno));
      do_log = FALSE;
    }
    sleep(5);
  }
  if (mqtt == NULL)
  {
    LG_CRITICAL("Could not initialize mqtt API.");
    goto END;
  }
  LG_INFO("MQTT API Initialized.");

  LG_INFO("Starting test.");

  while(!abort_rx_loop)
     mqtt_loop(mqtt, 0);

END:
  if (mqtt) {
    mqtt_close(mqtt);
    return 0;
  }
  return -1;
}

void sig_stop() {
  abort_rx_loop = TRUE;
}

int main(int argc, char *argv[]) {

  struct sigaction signal_action;

  log_init("ems_serio", DEFAULT_LOG_FAC , DEFAULT_LOG_LEVEL);

  log_push(LL_NONE, "############################################################################################");
  log_push(LL_NONE, "Starting %s "APP_VERSION" - LogFacility:%s, Level:%s.",
           argv[0], log_get_facility_name(DEFAULT_LOG_FAC), log_get_level_name(DEFAULT_LOG_LEVEL, TRUE));
  log_push(LL_NONE, "############################################################################################");

  if (argc > 1)
    LG_WARN("Ignoring exceding cli parameters.");
  // Set signal handler
  signal_action.sa_handler = sig_stop;
  sigemptyset(&signal_action.sa_mask);
  signal_action.sa_flags = 0;
  sigaction(SIGINT, &signal_action, NULL);
  sigaction(SIGHUP, &signal_action, NULL);
  sigaction(SIGTERM, &signal_action, NULL);

  return read_loop();
}
