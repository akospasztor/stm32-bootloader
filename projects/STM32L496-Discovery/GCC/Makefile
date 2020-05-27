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
-I../include \
-I../../../lib/stm32-bootloader \
-I../../../lib/fatfs \
-I../../../drivers/CMSIS/Include \
-I../../../drivers/CMSIS/Device/ST/STM32L4xx/Include \
-I../../../drivers/STM32L4xx_HAL_Driver/Inc

# Source files #################################################################
# Assembly source files
ASM_SOURCES =  \
startup_stm32l496xx.s

# C source files
C_SOURCES =  \
$(wildcard ../source/*.c) \
$(wildcard ../../../lib/stm32-bootloader/*.c) \
$(wildcard ../../../lib/fatfs/*.c) \
../../../lib/fatfs/option/unicode.c \
$(wildcard ../../../drivers/STM32L4xx_HAL_Driver/Src/*.c)

# Toolchain ####################################################################
CUSTOMPATH = "C:/Program Files (x86)/GNU Tools ARM Embedded/8 2019-q3-update/bin"
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(CUSTOMPATH)/$(PREFIX)gcc
AS = $(CUSTOMPATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(CUSTOMPATH)/$(PREFIX)objcopy
SZ = $(CUSTOMPATH)/$(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

# Compiler flags ###############################################################
# CPU
CPU = -mcpu=cortex-m4
# FPU
FPU = -mfpu=fpv4-sp-d16
# Float-abi
FLOAT-ABI = -mfloat-abi=hard
# MCU
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

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
LDSCRIPT = stm32l496xx_flash.ld

# Libraries
LIBS = -lc -lm -lnosys
LIBDIR =

# Create linker flags
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

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
