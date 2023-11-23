################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

-include makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include vehicle/transport/core/source/subdir.mk
-include vehicle/security/crypto_spake2/source/subdir.mk
-include vehicle/security/crypto_dk_creation/source/subdir.mk
-include vehicle/security/crypto_digitalkey_applet/source/subdir.mk
-include vehicle/security/crypto_data_access/source/subdir.mk
-include vehicle/security/crypto_common/source/subdir.mk
-include vehicle/security/crypto_abs/source/subdir.mk
-include vehicle/ranging/source/subdir.mk
-include vehicle/framework/standard_transaction/source/subdir.mk
-include vehicle/framework/passive_entry/source/subdir.mk
-include vehicle/framework/parse_format_commands/source/subdir.mk
-include vehicle/framework/owner_pairing/source/subdir.mk
-include vehicle/framework/fw_controller/source/subdir.mk
-include vehicle/framework/fast_transaction/source/subdir.mk
-include vehicle/framework/dk_message_format/source/subdir.mk
-include vehicle/application/tab_app/source/subdir.mk
-include vehicle/application/source/subdir.mk
-include vehicle/application/murata_custom/subdir.mk
-include src/SEGGER_RTT/subdir.mk
-include src/subdir.mk
-include ra_gen/subdir.mk
-include ra/fsp/src/rm_freertos_port/subdir.mk
-include ra/fsp/src/r_spi/subdir.mk
-include ra/fsp/src/r_ioport/subdir.mk
-include ra/fsp/src/r_gpt/subdir.mk
-include ra/fsp/src/bsp/mcu/all/subdir.mk
-include ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/subdir.mk
-include ra/board/ra6m5_ek/subdir.mk
-include ra/aws/FreeRTOS/FreeRTOS/Source/portable/MemMang/subdir.mk
-include ra/aws/FreeRTOS/FreeRTOS/Source/subdir.mk
-include platform/thinkey_transport_al/source/subdir.mk
-include platform/thinkey_transport_al/PTX/RF_TEST/subdir.mk
-include platform/thinkey_transport_al/PTX/PLAT/type2ab/subdir.mk
-include platform/thinkey_transport_al/PTX/NSC/NSC_RD/subdir.mk
-include platform/thinkey_transport_al/PTX/NSC/NSC_IOTRD/subdir.mk
-include platform/thinkey_transport_al/PTX/NSC/NSC_CE/subdir.mk
-include platform/thinkey_transport_al/PTX/NSC/subdir.mk
-include platform/thinkey_transport_al/PTX/NDEF/subdir.mk
-include platform/thinkey_transport_al/PTX/NATIVE_TAG/subdir.mk
-include platform/thinkey_transport_al/PTX/IOT_READER/subdir.mk
-include platform/thinkey_transport_al/PTX/HCE/subdir.mk
-include platform/thinkey_transport_al/PTX/GPIO/subdir.mk
-include platform/thinkey_transport_al/PTX/FELICA_DTE/subdir.mk
-include platform/thinkey_transport_al/PTX/COMMON/subdir.mk
-include platform/thinkey_transport_al/PTX/subdir.mk
-include platform/thinkey_storage_al/source/subdir.mk
-include platform/thinkey_security_al/mbedtls/source/subdir.mk
-include platform/thinkey_security_al/subdir.mk
-include platform/thinkey_ranging_al/source/subdir.mk
-include platform/thinkey_debug_al/source/subdir.mk
-include platform/thinkey_bsp_al/source/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(JMP_DEPS)),)
-include $(JMP_DEPS)
endif
ifneq ($(strip $(FSY_DEPS)),)
-include $(FSY_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(SRC_UPPER_DEPS)),)
-include $(SRC_UPPER_DEPS)
endif
ifneq ($(strip $(P_UPPER_DEPS)),)
-include $(P_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(PP_UPPER_DEPS)),)
-include $(PP_UPPER_DEPS)
endif
ifneq ($(strip $(P_DEPS)),)
-include $(P_DEPS)
endif
ifneq ($(strip $(FSY_UPPER_DEPS)),)
-include $(FSY_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(SRC_DEPS)),)
-include $(SRC_DEPS)
endif
ifneq ($(strip $(JMP_UPPER_DEPS)),)
-include $(JMP_UPPER_DEPS)
endif
ifneq ($(strip $(PP_DEPS)),)
-include $(PP_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(SX_UPPER_DEPS)),)
-include $(SX_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
THINKEY_RENESAS_DEMO_PROJECT.hex \

SECONDARY_SIZE += \
THINKEY_RENESAS_DEMO_PROJECT.siz \


# All Target
all: THINKEY_RENESAS_DEMO_PROJECT.elf secondary-outputs

# Tool invocations
THINKEY_RENESAS_DEMO_PROJECT.elf: $(OBJS) $(USER_OBJS) $(LINKER_SCRIPT)
	$(file > THINKEY_RENESAS_DEMO_PROJECT.elf.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -T "fsp.ld" -Xlinker --gc-sections -L "C:\\Users\\jeeva\\OneDrive\\Desktop\\Thinkseed\\vehicle-gits2\\platform\\renesas\\THINKEY_RENESAS_DEMO_PROJECT/script" -Wl,-Map,"THINKEY_RENESAS_DEMO_PROJECT.map" --specs=nano.specs --specs=rdimon.specs -o "THINKEY_RENESAS_DEMO_PROJECT.elf" -Wl,--start-group $(OBJS) $(USER_OBJS) $(LIBS) -Wl,--end-group)
	@echo Building target: $@ && arm-none-eabi-gcc @"THINKEY_RENESAS_DEMO_PROJECT.elf.in"

THINKEY_RENESAS_DEMO_PROJECT.hex: THINKEY_RENESAS_DEMO_PROJECT.elf
	arm-none-eabi-objcopy -O ihex "THINKEY_RENESAS_DEMO_PROJECT.elf"  "THINKEY_RENESAS_DEMO_PROJECT.hex"

THINKEY_RENESAS_DEMO_PROJECT.siz: THINKEY_RENESAS_DEMO_PROJECT.elf
	arm-none-eabi-size --format=berkeley "THINKEY_RENESAS_DEMO_PROJECT.elf"

# Other Targets
clean:
	-$(file > clean_file.tmp,  $(JMP_DEPS)  $(FSY_DEPS)  $(C_UPPER_DEPS)  $(SECONDARY_SIZE)  $(SRC_UPPER_DEPS)  $(P_UPPER_DEPS)  $(S_DEPS)  $(PP_UPPER_DEPS)  $(P_DEPS)  $(FSY_UPPER_DEPS)  $(C_DEPS)  $(SRC_DEPS)  $(JMP_UPPER_DEPS)  $(PP_DEPS)  $(ASM_UPPER_DEPS)  $(SX_DEPS)  $(SX_UPPER_DEPS)  $(OBJS)  $(SECONDARY_FLASH)  $(ASM_DEPS)  $(SREC)  $(S_UPPER_DEPS)  $(MAP) )
	-@ xargs -P 2 -s 32000 -t -a clean_file.tmp $(RM)
	-@$(RM) clean_file.tmp
	-$(RM)  THINKEY_RENESAS_DEMO_PROJECT.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
