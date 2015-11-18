#include "osapi.h"
#include "user_interface.h"
#include "pwm.h"
#include "uart.h"
#include "osapi.h"
#include "mem.h"
#include "espmissingincludes.h"

void systemInitDoneCB() {
	os_printf("Hello!\n");
}

void user_init() {
	uart_init_2(115200, 115200);
	system_init_done_cb(systemInitDoneCB);
}
