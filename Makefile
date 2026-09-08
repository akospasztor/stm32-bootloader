# Configuration ################################################################
# Target name
TARGET = stm32-bootloader
# Debug configuration selector
DEBUG = 1
# Optimization level
OPT = -Og
# Build path
BUILD_DIR = build

# Defines ######################################################################
# Assembly defines
ASM_DEFS =

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32L496xx

# Includes #####################################################################
# Assembly includes
ASM_INCLUDES =

# C includes
C_INCLUDES =  \
-Ibootloader/include \
-Ibsp/stm32l496-discovery/include \
-Ilib/FatFs \
-Ilib/CMSIS/Include \
-Ilib/CMSIS/Device/ST/STM32L4xx/Include \
-Ilib/STM32L4xx_HAL_Driver/Inc \

# Source files #################################################################
# Assembly source files
ASM_SOURCES =  \
bsp/stm32l496-discovery/gcc-arm/startup_stm32l496xx.s

# C source files
C_SOURCES =  \
$(wildcard bootloader/source/*.c) \
$(wildcard bsp/stm32l496-discovery/source/*.c) \
$(wildcard lib/FatFs/*.c) \
lib/FatFs/option/unicode.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc_ex.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd_ex.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
lib/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_sdmmc.c

# Toolchain ####################################################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

# Compiler flags ###############################################################
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# Create compiler flags
ASFLAGS = $(MCU) $(ASM_DEFS) $(ASM_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

# Generate debug information
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

# Linker flags #################################################################
# Linker script
LDSCRIPT = bsp/stm32l496-discovery/gcc-arm/stm32l496xx_flash.ld

# Libraries
LIBS = \
-specs=nano.specs \
-specs=nosys.specs \
-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref \
-Wl,--gc-sections \
-Wl,--start-group -lc -lm -lnosys -Wl,--end-group \
-Wl,--print-memory-usage \

# Create linker flags
LDFLAGS = $(MCU) -T$(LDSCRIPT) $(LIBS)

# Build target #################################################################
# Default: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# List of C objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# List of ASM objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

# Clean
clean:
	-rm -rf $(BUILD_DIR)

# Dependencies
-include $(wildcard $(BUILD_DIR)/*.d)
