# This file is a makefile included from the top level makefile which
# defines the sources built for the target.

# Define the prefix to this directory. 
# Note: The name must be unique within this build and should be
#       based on the root of the project
TARGET_SRC_PATH = src

# Add include to all objects built for this target
INCLUDE_DIRS += inc

# C source files included in this build.
CSRC += $(TARGET_SRC_PATH)/usb_bsp.c
CSRC += $(TARGET_SRC_PATH)/usbd_desc.c
CSRC += $(TARGET_SRC_PATH)/usbd_usr.c
CSRC += $(TARGET_SRC_PATH)/syscalls.c
CSRC += $(TARGET_SRC_PATH)/outlet_control.c

# C++ source files included in this build.
CPPSRC += $(TARGET_SRC_PATH)/application.cpp
CPPSRC += $(TARGET_SRC_PATH)/main.cpp
CPPSRC += $(TARGET_SRC_PATH)/newlib_stubs.cpp
CPPSRC += $(TARGET_SRC_PATH)/spark_utilities.cpp
CPPSRC += $(TARGET_SRC_PATH)/spark_wiring_limited.cpp
CPPSRC += $(TARGET_SRC_PATH)/spark_wiring_ipaddress.cpp
CPPSRC += $(TARGET_SRC_PATH)/spark_wiring_print.cpp
CPPSRC += $(TARGET_SRC_PATH)/spark_wiring_string.cpp
CPPSRC += $(TARGET_SRC_PATH)/spark_wiring_time.cpp
CPPSRC += $(TARGET_SRC_PATH)/spark_wiring_usbserial.cpp
CPPSRC += $(TARGET_SRC_PATH)/spark_wiring_wifi.cpp
CPPSRC += $(TARGET_SRC_PATH)/spark_wlan.cpp
CPPSRC += $(TARGET_SRC_PATH)/stm32_it.cpp
CPPSRC += $(TARGET_SRC_PATH)/wifi_credentials_reader.cpp

# ASM source files included in this build.
ASRC +=

