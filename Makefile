PROJECT_NAME := Objective-Bucket
IDF_PATH := esp8266/ESP8266_RTOS_SDK
export PATH := $(PATH):$(CURDIR)/esp8266/xtensa-lx106-elf/bin

CFLAGS += -Og
include esp8266/ESP8266_RTOS_SDK/make/project.mk

