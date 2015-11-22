#
# Sample FreeRTOS App
#
PROJ_NAME=sampleESP8266App
COMPORT=/dev/ttyUSB0
OBJS=user_main.o uart.o
#
CC=xtensa-lx106-elf-gcc
APP=a.out
ESPTOOL_CK=esptool
# esptool.py is distributed with the pfalcon/esp-open-sdk in the xtensa-lx106 bin.
ESPTOOL=esptool.py
CCFLAGS= -Wimplicit-function-declaration -fno-inline-functions -mlongcalls -mtext-section-literals \
-mno-serialize-volatile -I$(ESP8266_SDK_ROOT)/include/espressif -I$(ESP8266_SDK_ROOT)/include/lwip -I$(ESP8266_SDK_ROOT)/include/lwip/ipv4 -I$(ESP8266_SDK_ROOT)/include/lwip/ipv6 \
-I$(ESP8266_SDK_ROOT)/include -I$(ESP8266_SDK_ROOT)/extra_include -I. -D__ETS__ -DICACHE_FLASH -DXTENSA -DUSE_US_TIMER

LDFLAGS=-nostdlib \
-L$(ESP8266_SDK_ROOT)/lib -L$(ESP8266_SDK_ROOT)/ld -T$(ESP8266_SDK_ROOT)/ld/eagle.app.v6.ld \
-Wl,--no-check-sections -u call_user_start -Wl,-static -Wl,--start-group \
-lminic -lgcc -lhal -lphy -lpp -lnet80211 -lwpa -lcrypto -lmain -lfreertos -llwip $(OBJS) -Wl,--end-group

all: $(PROJ_NAME)_0x00000.bin $(PROJ_NAME)_0x40000.bin

a.out: $(OBJS)
	$(CC) -o a.out $(LDFLAGS)

$(PROJ_NAME)_0x00000.bin: a.out
	$(ESPTOOL_CK) -eo $< -bo $@ -bs .text -bs .data -bs .rodata -bs .iram0.text -bc -ec || true

$(PROJ_NAME)_0x40000.bin: a.out
	$(ESPTOOL_CK) -eo $< -es .irom0.text $@ -ec || true
	
.c.o:
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f a.out *.o *.bin
	
flash: all
	#$(ESPTOOL_CK) -cp $(COMPORT) -bm qio -bz 4M -bf 40 -cd nodemcu -cb 115200 -ca 0x00000 -cf $(PROJ_NAME)_0x00000.bin -ca 0x40000 -cf $(PROJ_NAME)_0x40000.bin
	$(ESPTOOL) --port $(COMPORT) --baud 230400 write_flash --flash_freq 40m --flash_mode qio --flash_size 4m 0x00000 $(PROJ_NAME)_0x00000.bin 0x40000 $(PROJ_NAME)_0x40000.bin