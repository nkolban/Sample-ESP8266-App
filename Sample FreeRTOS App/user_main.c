#include "esp_common.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"

#include "uart.h"

void setupListener();

static void wifiEventCallback(System_Event_t *pEvent) {
  printf("Caught an event!\n");
  printf("Free heap size: %d\n", system_get_free_heap_size());
  switch(pEvent->event_id) {
      case EVENT_STAMODE_CONNECTED:
        os_printf("Event: EVENT_STAMODE_CONNECTED\n");
        break;
      case EVENT_STAMODE_DISCONNECTED:
        os_printf("Event: EVENT_STAMODE_DISCONNECTED\n");
        break;
      case EVENT_STAMODE_AUTHMODE_CHANGE:
        os_printf("Event: EVENT_STAMODE_AUTHMODE_CHANGE\n");
        break;
      case EVENT_STAMODE_GOT_IP:
        os_printf("Event: EVENT_STAMODE_GOT_IP\n");
        xTaskCreate(setupListener, "setupListener", 200, NULL, 3, NULL);
        break;
      case EVENT_SOFTAPMODE_STACONNECTED:
        os_printf("Event: EVENT_SOFTAPMODE_STACONNECTED\n");
        break;
      case EVENT_SOFTAPMODE_STADISCONNECTED:
        os_printf("Event: EVENT_SOFTAPMODE_STADISCONNECTED\n");
        break;
      default:
        os_printf("Unexpected event: %d\n", pEvent->event_id);
      break;
    }
}

void setupListener() {
  printf("setupListener started\n");
  int serverSocket;
  serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverSocket < 0) {
    printf("Error from socket!\n");
    return;
  }
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(80);

  int rc = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  if (rc < 0) {
    printf("Error from bind\n");
    return;
  }
  printf("All ok\n");
  rc = listen(serverSocket, 5);
  if (rc < 0) {
    printf("Error from listen\n");
    return;
  }
  while(1) {
    struct sockaddr_in addr;
    socklen_t length = sizeof(addr);
    int newSocket = accept(serverSocket, (struct sockaddr *)&addr, &length);
    if (newSocket < 0) {
      printf("Error with accept: %d, errn0=%d\n", newSocket, errno);
      return;
    }
    printf("We have received a new client connection!");
    close(newSocket);
  }
  // Let us now watch for new connections
  vTaskDelete(NULL);
}

void taskConnect(void *pData) {
  wifi_set_event_handler_cb(wifiEventCallback);
  wifi_set_opmode_current(STATION_MODE);
  struct station_config stationConfig;
  strncpy(stationConfig.ssid, "sweetie", 32);
  strncpy(stationConfig.password, "kolbanpassword", 64);
  wifi_station_set_config(&stationConfig);
  wifi_station_connect();
  printf("Hello from taskConnect!\n");
  int count = 0;
  while(1) {
    count++;
    printf("count - %d\n", count);
    vTaskDelay(100);
  }
}

void user_init(void) {
  uart_init_new();
  UART_SetBaudrate(UART0, 115200);
  printf("SDK version:%s\n", system_get_sdk_version());
  printf("Free heap size: %d\n", system_get_free_heap_size());
  printf("user_init running ...\n");
  wifi_set_opmode(NULL_MODE);
  xTaskCreate(taskConnect, "taskConnect", 200, NULL, 3, NULL);
}
