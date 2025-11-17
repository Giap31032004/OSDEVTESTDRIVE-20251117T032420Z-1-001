CROSS   = arm-none-eabi-
CC      = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy

BUILD   = build
TARGET  = kernel

CFLAGS  = -mcpu=cortex-m3 -mthumb -O0 -ffreestanding -nostdlib -I./driver
LDFLAGS = -T linker.ld -nostdlib

SRC_C   = main.c \
		  driver/driver_uart.c \
		  driver/driver_sysctl.c \
 		  driver/driver_gpio.c \
		  driver/driver_systick.c \
 		  os/os.c \
 		  task_run/task_run.c

SRC_S   = startup.s \
		  os/context_switch.s

OBJ     = $(SRC_C:%.c=$(BUILD)/%.o) $(SRC_S:%.s=$(BUILD)/%.o)

all: $(BUILD)/$(TARGET).bin

# build .c -> .o
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# build .s -> .o
$(BUILD)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# link ELF
$(BUILD)/$(TARGET).elf: $(OBJ) linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ)

# convert ELF -> BIN
$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# run QEMU
run: $(BUILD)/$(TARGET).bin
	qemu-system-arm -M lm3s6965evb -kernel $< -serial mon:stdio -nographic

# clean build
clean:
	rm -rf $(BUILD)
