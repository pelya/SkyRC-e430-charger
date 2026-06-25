
CC = sdcc
CFLAGS = -mstm8 -I stm8s-sdcc/inc -DSTM8S903=1
LDFLAGS = -mstm8 -lstm8 --out-fmt-ihx

SRCS = main.c
SRCS += stm8s-sdcc/src/stm8s_gpio.c
SRCS += stm8s-sdcc/src/stm8s_adc1.c

OBJS = $(foreach F, $(SRCS), out/$(basename $(F)).rel)

TARGET = out/main.ihx

all: $(TARGET) stm8flash/stm8flash

out/%.rel: %.c gpio_config.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

gpio_config.h: charger.ioc8
	./generate-header-from-ioc8.py $< > $@

clean:
	rm -rf out

flash: $(TARGET) stm8flash/stm8flash
	./write-flash.sh

stm8flash/stm8flash: stm8flash/Makefile
	$(MAKE) -C stm8flash

stm8flash/Makefile stm8s-sdcc/src/stm8s_gpio.c:
	git submodule update --init
