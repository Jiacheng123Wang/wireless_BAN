#/* 1: sensor side, 0: smart phone side */
SENSOR      :=  0

#/* NRF52, NRF51 */
TARGET_CHIP := NRF52
##########################################################################
##########################################################################
##########################################################################
SDK_ROOT := ../..
WIRELESS_SIM_DIR := .

ifeq ($(TARGET_CHIP), NRF52)
ifeq ($(SENSOR), 1)
  OUTPUT_FILENAME := wireless_sim_nrf52832_sensor
  LIB_FILENAME := lib/wireless_sim_nrf52832_sensor.a
else
  OUTPUT_FILENAME := wireless_sim_nrf52832_phone
  LIB_FILENAME := lib/wireless_sim_nrf52832_phone.a
endif 
else
ifeq ($(SENSOR), 1)
  OUTPUT_FILENAME := wireless_sim_nrf51822_sensor
  LIB_FILENAME := lib/wireless_sim_nrf51822_sensor.a
else
  OUTPUT_FILENAME := wireless_sim_nrf51822_phone
  LIB_FILENAME := lib/wireless_sim_nrf51822_phone.a
endif 
endif


#source common to all targets
ifeq ($(TARGET_CHIP), NRF52)
  C_SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/hal/nrf_saadc.c
  C_SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/saadc/nrf_drv_saadc.c 
  C_SOURCE_FILES += $(SDK_ROOT)/components/toolchain/system_nrf52.c 
else
  C_SOURCE_FILES += $(SDK_ROOT)/components/toolchain/system_nrf51.c 
endif
C_SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c 
C_SOURCE_FILES += main.c  
C_SOURCE_FILES += sensor_data_generation.c  
C_SOURCE_FILES += \
  $(WIRELESS_SIM_DIR)/wireless_uicc_terminal/comm_UICC_terminal_interface.c \
  $(WIRELESS_SIM_DIR)/wireless_uicc_terminal/comm_phone_command.c \
  $(WIRELESS_SIM_DIR)/wireless_uicc_terminal/comm_wireless_interface.c \
  $(WIRELESS_SIM_DIR)/wireless_uicc_terminal/comm_misc.c \
  $(WIRELESS_SIM_DIR)/wireless_uicc_terminal/watch_wireless_interface.c  \
  $(WIRELESS_SIM_DIR)/wireless_uicc_terminal/wireless_sim_phone.c \
  $(WIRELESS_SIM_DIR)/wireless_uicc_terminal/phone_wireless_interface.c \
  $(WIRELESS_SIM_DIR)/wireless_uicc_terminal/phone_command_sim.c \
  $(WIRELESS_SIM_DIR)/wireless_uicc_terminal/radio_config_update.c \

#lib file
# LIBS += $(LIB_FILENAME)

export OUTPUT_FILENAME
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_DIR := $(dir $(MAKEFILE_NAME) ) 

TEMPLATE_PATH = $(SDK_ROOT)/components/toolchain/gcc
ifeq ($(OS), Windows_NT)
  include $(TEMPLATE_PATH)/Makefile.windows
else
  include $(TEMPLATE_PATH)/Makefile.posix
endif

MK := mkdir
RM := rm -rf

#echo suspend
ifeq ("$(VERBOSE)","1")
  NO_ECHO := 
else
  NO_ECHO := @
endif

# Toolchain commands
CC              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc'
AS              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as'
AR              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar' -r
LD              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld'
NM              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm'
OBJDUMP         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump'
OBJCOPY         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy'
SIZE            := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size'

#function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

#assembly files common to all targets
ifeq ($(TARGET_CHIP), NRF52)
  ASM_SOURCE_FILES  = $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S
else
  ASM_SOURCE_FILES  = $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf51.S
endif

#includes common to all targets
INC_PATHS += -I$(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd
INC_PATHS += -I$(SDK_ROOT)/components/device
INC_PATHS += -I$(SDK_ROOT)/components/drivers_nrf/hal
INC_PATHS += -I$(SDK_ROOT)/components/drivers_nrf/delay
INC_PATHS += -I$(SDK_ROOT)/components/drivers_nrf/common
INC_PATHS += -I$(SDK_ROOT)/components/drivers_nrf/config
INC_PATHS += -I$(SDK_ROOT)/components/drivers_nrf/saadc
INC_PATHS += -I$(SDK_ROOT)/components/toolchain
INC_PATHS += -I$(SDK_ROOT)/components/toolchain/gcc
INC_PATHS += -I$(SDK_ROOT)/components/libraries/util
INC_PATHS += -I$(SDK_ROOT)/components/libraries/button
INC_PATHS += -I$(WIRELESS_SIM_DIR)/wireless_uicc_terminal
INC_PATHS += -I$(WIRELESS_SIM_DIR)/config
INC_PATHS += -I$(SDK_ROOT)/components/drivers_nrf/delay
INC_PATHS += -I$(SDK_ROOT)/components/toolchain/cmsis/include
INC_PATHS += -I$(SDK_ROOT)/components/drivers_nrf/nrf_uart

OBJECT_DIRECTORY = _build
LISTING_DIRECTORY = $(OBJECT_DIRECTORY)
OUTPUT_BINARY_DIRECTORY = $(OBJECT_DIRECTORY)

# OBJ files
OBJECT_FILES = $(OBJECT_DIRECTORY)/comm_UICC_terminal_interface.o  \
               $(OBJECT_DIRECTORY)/comm_phone_command.o  \
               $(OBJECT_DIRECTORY)/comm_wireless_interface.o  \
               $(OBJECT_DIRECTORY)/comm_misc.o  \
               $(OBJECT_DIRECTORY)/watch_wireless_interface.o  \
               $(OBJECT_DIRECTORY)/wireless_sim_phone.o  \
               $(OBJECT_DIRECTORY)/phone_wireless_interface.o  \
               $(OBJECT_DIRECTORY)/phone_command_sim.o  \
               $(OBJECT_DIRECTORY)/radio_config_update.o  \

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )

#flags common to all targets
ifeq ($(TARGET_CHIP), NRF52)
  CFLAGS += -DNRF52
  CFLAGS += -DBOARD_PCA10040
  CFLAGS += -DNRF52832
  CFLAGS += -DNRF52_PAN_64
  CFLAGS += -DNRF52_PAN_12
  CFLAGS += -DNRF52_PAN_15
  CFLAGS += -DNRF52_PAN_58
  CFLAGS += -DNRF52_PAN_55
  CFLAGS += -DNRF52_PAN_54
  CFLAGS += -DNRF52_PAN_31
  CFLAGS += -DNRF52_PAN_30
  CFLAGS += -DNRF52_PAN_51
  CFLAGS += -DNRF52_PAN_36
  CFLAGS += -DNRF52_PAN_53
  CFLAGS += -DS132
  CFLAGS += -DCONFIG_GPIO_AS_PINRESET
  CFLAGS += -DBLE_STACK_SUPPORT_REQD
  CFLAGS += -DNRF_SD_BLE_API_VERSION=3
  CFLAGS += -DSWI_DISABLE0
  CFLAGS += -DNRF52_PAN_20
  CFLAGS += -DSOFTDEVICE_PRESENT
  CFLAGS += -DNRF52_PAN_62
  CFLAGS += -DNRF52_PAN_63
  CFLAGS += -mcpu=cortex-m4
  CFLAGS += -mthumb -mabi=aapcs
  CFLAGS +=  -Wall -Werror -O3 -g3
  CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
  # keep every function in separate section, this allows linker to discard unused ones
  CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
  CFLAGS += -fno-builtin --short-enums 
  # generate dependency output file
  CFLAGS += -MP -MD
else
  CFLAGS  = -DNRF51
#  CFLAGS += -DBOARD_PCA10001
#  CFLAGS += -DBSP_DEFINES_ONLY
  CFLAGS += -mcpu=cortex-m0
  CFLAGS += -mthumb -mabi=aapcs --std=gnu99
  CFLAGS += -Wall -Werror -O3
  CFLAGS += -mfloat-abi=soft
  CFLAGS += -flto -fno-builtin
  CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
endif

# keep every function in separate section. This will allow linker to dump unused functions
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
ifeq ($(TARGET_CHIP), NRF52)
  LDFLAGS += -mcpu=cortex-m4
  LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
else
  LDFLAGS += -mcpu=cortex-m0
endif

# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

# Assembler flags
ASMFLAGS += -x assembler-with-cpp
ifeq ($(TARGET_CHIP), NRF52)
  # Assembler flags common to all targets
  ASMFLAGS += -x assembler-with-cpp
  ASMFLAGS += -DNRF52
  ASMFLAGS += -DBOARD_PCA10040
  ASMFLAGS += -DNRF52832
  ASMFLAGS += -DNRF52_PAN_64
  ASMFLAGS += -DNRF52_PAN_12
  ASMFLAGS += -DNRF52_PAN_15
  ASMFLAGS += -DNRF52_PAN_58
  ASMFLAGS += -DNRF52_PAN_55
  ASMFLAGS += -DNRF52_PAN_54
  ASMFLAGS += -DNRF52_PAN_31
  ASMFLAGS += -DNRF52_PAN_30
  ASMFLAGS += -DNRF52_PAN_51
  ASMFLAGS += -DNRF52_PAN_36
  ASMFLAGS += -DNRF52_PAN_53
  ASMFLAGS += -DS132
  ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
  ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
  ASMFLAGS += -DNRF_SD_BLE_API_VERSION=3
  ASMFLAGS += -DSWI_DISABLE0
  ASMFLAGS += -DNRF52_PAN_20
  ASMFLAGS += -DSOFTDEVICE_PRESENT
  ASMFLAGS += -DNRF52_PAN_62
  ASMFLAGS += -DNRF52_PAN_63
else
  ASMFLAGS += -DNRF51
  ASMFLAGS += -DBOARD_PCA10028
  ASMFLAGS += -DBSP_DEFINES_ONLY
endif


#default target - first one defined
default: clean $(OUTPUT_FILENAME)

#building all targets
all: clean
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e cleanobj
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e $(OUTPUT_FILENAME) 

#target for printing all targets
help:
	@echo following targets are available:
	@echo 	$(OUTPUT_FILENAME)
	
C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_SOURCE_FILE_NAMES:.S=.o) )

vpath %.c $(C_PATHS)
vpath %.S $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)
ifeq ($(TARGET_CHIP), NRF52)
  LINKER_SCRIPT=$(WIRELESS_SIM_DIR)/config/gcc_nrf52.ld
else
  LINKER_SCRIPT=$(WIRELESS_SIM_DIR)/config/gcc_nrf51.ld
endif

$(OUTPUT_FILENAME): $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e finalize

## Create build directories
$(BUILD_DIRECTORIES):
	echo $(MAKEFILE_NAME)
	$(MK) $@

# Create objects from C SRC files
$(OBJECT_DIRECTORY)/%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

# Assemble files
$(OBJECT_DIRECTORY)/%.o: %.S
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<


# Link
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out


## Create binary .bin file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

finalize: genbin genhex echosize

genbin:
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
genhex: 
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

echosize:
	-@echo ''
	$(NO_ECHO)$(SIZE) $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	-@echo ''

clean:
ifeq ($(OS), Windows_NT)
	$(RM) $(BUILD_DIRECTORIES)
else
	$(RM) $(BUILD_DIRECTORIES)
endif

cleanobj:
	$(RM) $(BUILD_DIRECTORIES)/*.o

# flash program in windows
flash: $(OBJECT_DIRECTORY)/$(OUTPUT_FILENAME).hex
	@echo Flashing: $<
ifeq ($(TARGET_CHIP), NRF52)
	nrfjprog --program $< -f nrf52 --sectorerase
	nrfjprog --reset -f nrf52
else
	nrfjprog --program $< --sectorerase
	nrfjprog --reset
endif
	
recover:
ifeq ($(TARGET_CHIP), NRF52)
	nrfjprog --recover -f nrf52
else
	nrfjprog --recover
endif

reset: 
ifeq ($(TARGET_CHIP), NRF52)
	nrfjprog --reset -f nrf52
else
	nrfjprog --reset
endif
	
archieve: 
	$(NO_ECHO)$(AR) $(LIB_FILENAME) $(OBJECT_FILES)
	

