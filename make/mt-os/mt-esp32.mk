# Copyright (c) 2003-2016, LogMeIn, Inc. All rights reserved.
#
# This is part of the Xively C Client library,
# it is licensed under the BSD 3-Clause license.

include make/mt-os/mt-os-common.mk

#$(IDF_PATH) This is exported in the shell as a requirement of the IDF SDK
XI_ESP_IDF_SDK_PATH          ?= $(IDF_PATH)
XI_GCC_XTENSA_TOOLCHAIN_PATH ?= $(HOME)/Work/Xively/esp32/toolchain/xtensa-esp32-elf

CC = $(XI_GCC_XTENSA_TOOLCHAIN_PATH)/bin/xtensa-esp32-elf-gcc
AR = $(XI_GCC_XTENSA_TOOLCHAIN_PATH)/bin/xtensa-esp32-elf-ar
XI_BUILD_PRECONDITIONS := $(CC)

##################
# Libxively Config
##################
XI_CONFIG_FLAGS += -DXI_CROSS_TARGET
XI_CONFIG_FLAGS += -DXI_EMBEDDED_TESTS

################
# WolfSSL Config
################
XI_CONFIG_FLAGS += -DNO_WRITEV
XI_COMPILER_FLAGS += -DSINGLE_THREADED

#########################
# ESP System Include Dirs
#########################
XI_COMPILER_FLAGS += -I$(XI_ESP_IDF_SDK_PATH)/components/driver/include
XI_COMPILER_FLAGS += -I$(XI_ESP_IDF_SDK_PATH)/components/soc/esp32/include
XI_COMPILER_FLAGS += -I$(XI_ESP_IDF_SDK_PATH)/components/esp32/include

###################
# LWIP Include Dirs
###################
XI_COMPILER_FLAGS += -I$(XI_ESP_IDF_SDK_PATH)/components/lwip/system
XI_COMPILER_FLAGS += -I$(XI_ESP_IDF_SDK_PATH)/components/lwip/include/lwip
XI_COMPILER_FLAGS += -I$(XI_ESP_IDF_SDK_PATH)/components/lwip/include/lwip/port

#######################
# FreeRTOS Include Dirs
#######################
XI_COMPILER_FLAGS += -I$(XI_ESP_IDF_SDK_PATH)/components/freertos/include/freertos
XI_COMPILER_FLAGS += -I$(XI_ESP_IDF_SDK_PATH)/components/freertos/include

#TODO: sdkconfig file was generated by the SDK's menuconfig GUI in $(XI_ESP_IDF_SDK_PATH)/make/project.mk
#      We may want to include a pre-built one and get rid of that GUI somehow
XI_COMPILER_FLAGS += -I$(LIBXIVELY)/build/include
XI_COMPILER_FLAGS += -I$(LIBXIVELY)/src/bsp/platform/esp32/include

####################
# Code configuration
####################
XI_COMPILER_FLAGS += -DSNTP_MAX_SERVERS=4 #if modified, update your app's component.mk and xi_bsp_time_esp32_sntp.c too

################################
# xtensa-esp32 toolchain options
################################
XI_COMPILER_FLAGS += -fstrict-volatile-bitfields
XI_COMPILER_FLAGS += -ffunction-sections
XI_COMPILER_FLAGS += -fdata-sections
XI_COMPILER_FLAGS += -mlongcalls
XI_COMPILER_FLAGS += -nostdlib
XI_COMPILER_FLAGS += -ggdb
XI_COMPILER_FLAGS += -Os
XI_COMPILER_FLAGS += -DNDEBUG
XI_COMPILER_FLAGS += -std=gnu99
XI_COMPILER_FLAGS += -Wno-old-style-declaration

XI_ARFLAGS += -rs -c $(XI)

ifeq ($(XI_BSP_TLS),wolfssl)
WOLFSSL_STATIC_LIB   = $(LIBXIVELY)/bin/esp32/libwolfssl.a
WOLFSSL_MAKEFILE_DIR = $(LIBXIVELY)/examples/esp32/xively_demo/wolfssl-make

XI_BUILD_PRECONDITIONS += WOLFSSL_STATIC_LIB
WOLFSSL_STATIC_LIB:
	@mkdir -p $(dir $(WOLFSSL_STATIC_LIB))
	@cd $(WOLFSSL_MAKEFILE_DIR) && \
     make GCC_XTENSA_TOOLCHAIN_PATH=$(realpath $(XI_GCC_XTENSA_TOOLCHAIN_PATH))
endif

#ifdef XI_TRAVIS_BUILD
#### TOOLCHAIN AUTODOWNLOAD SECTION --- BEGIN
#
#	XI_BUILD_PRECONDITIONS += ESP32_SDK
#.PHONY : ESP32_SDK
#ESP32_SDK:
#	git clone -b esp32 git@github.com:xively/xively-client-artifactory.git $(HOME)/Downloads/xively-client-artifactory
#
#### TOOLCHAIN AUTODOWNLOAD SECTION --- END
#endif

#endif

XI_POST_COMPILE_ACTION =
