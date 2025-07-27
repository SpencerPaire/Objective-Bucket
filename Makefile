PROJECT_NAME := Objective-Bucket
export IDF_PATH := esp8266/ESP8266_RTOS_SDK
export PATH := $(PATH):$(CURDIR)/esp8266/xtensa-lx106-elf/bin

ifdef DEBUG
	CXXFLAGS += -Og
else
	CXXFLAGS += -O3
endif
CXXFLAGS += -std=c++17
include esp8266/ESP8266_RTOS_SDK/make/project.mk
