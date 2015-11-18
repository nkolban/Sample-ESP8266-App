#include "osapi.h"
#include "user_interface.h"
#include "pwm.h"
#include "uart.h"
#include "osapi.h"
#include "mem.h"
#include "espmissingincludes.h"
#include "wificonnect.h"
#include "startListening.h"
#include "getdata.h"

static void setDuty(int value);

uint32 g_frequency = 50;
#define USECS_IN_HZ (1000000)

void uart_init_2(UartBautRate uart0_br, UartBautRate uart1_br);

os_timer_t myTimer;

void timerCallback(void *pArg) {
	os_printf("Tick!");
} // End of timerCallback

void newData(char *pData, uint16 length) {
  os_printf("New data!\n");
  char *pString = os_malloc(length+1);
  os_memcpy(pString, pData, length);
  pString[length] = '\0';
  int value = atoi(pString);
  setDuty(value);
  os_free(pString);
  os_printf("Value: %d\n", value);
}

static void newConnection(struct espconn *pEspconn) {
  os_printf("New connection!\n");
  getData(pEspconn, newData);
}

static void gotIpCB() {
  os_printf("We got an IP!\n");
  startListening(80, newConnection);
}

static void setDuty(int value) {
  // if value = -100 -> 1.0 msec
  // if value = 0 -> 1.5 msec
  // if value = 100 -> 2.0 msec
  if (value < -100 || value > 100) {
    return;
  }
  value = (value + 100) / 2; // Value now from 0->100

  uint32 duty = (1000 + value * 10) / 0.045;
  os_printf("Setting new duty to be: %d\n", duty);
  pwm_set_duty(duty, 0);
  pwm_start();
}

void systemInitDoneCB() {
	os_timer_setfn(&myTimer, timerCallback, NULL);
	//os_timer_arm(&myTimer, 1000, 1);

	os_printf("Hello World\n");
	// 1Hz = 1000msecs = 1000000usecs
	// 1KHz = 1000Hz =
	double ratio = 0.5;
	uint32 duty = ratio * USECS_IN_HZ / 0.045 / g_frequency;
	duty = 22222;
	uint32 period;
	//period = USECS_IN_HZ / g_frequency;
	period = 20 *1000;
	duty = 2000 / 0.045;
	os_printf("duty = %d\n", duty);
	uint32 pinInfoList[][3] = {   {PERIPHS_IO_MUX_MTDI_U,FUNC_GPIO12,12},
	                          {PERIPHS_IO_MUX_MTDO_U,FUNC_GPIO15,15},
	                          {PERIPHS_IO_MUX_MTCK_U,FUNC_GPIO13,13},
	                          };
	os_printf("About to call init\n");
	pwm_init(USECS_IN_HZ / g_frequency, &duty, 1, pinInfoList);
	os_printf("About to call start\n");
	pwm_start();
	os_printf("Start completed\n");
	wifiConnect("sweetie", "kolbanpassword", gotIpCB);
}

void user_init() {
	uart_init_2(115200, 115200);
	system_init_done_cb(systemInitDoneCB);
}
