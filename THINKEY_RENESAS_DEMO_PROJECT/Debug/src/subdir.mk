################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/gpt_timer_initialize.c \
../src/hal_entry.c \
../src/receiver_task_entry.c \
../src/semaphore_task_entry.c \
../src/sender_task_entry.c 

C_DEPS += \
./src/gpt_timer_initialize.d \
./src/hal_entry.d \
./src/receiver_task_entry.d \
./src/semaphore_task_entry.d \
./src/sender_task_entry.d 

OBJS += \
./src/gpt_timer_initialize.o \
./src/hal_entry.o \
./src/receiver_task_entry.o \
./src/semaphore_task_entry.o \
./src/sender_task_entry.o 

SREC += \
THINKEY_RENESAS_DEMO_PROJECT.srec 

MAP += \
THINKEY_RENESAS_DEMO_PROJECT.map 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -D_RENESAS_RA_ -D_RA_CORE=CM33 -DPTX_PRODUCT_TYPE_IOT_READER -DPTX_FEATURES_NSC_READER_ONLY -DPTX_FEATURES_HAL_SPI -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/src" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/ra/fsp/inc" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/ra/fsp/inc/api" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/ra/fsp/inc/instances" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/ra/fsp/src/rm_freertos_port" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/ra/arm/CMSIS_5/CMSIS/Core/Include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/ra_gen" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/ra_cfg/fsp_cfg/bsp" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/ra_cfg/fsp_cfg" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/ra_cfg/aws" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/ra/aws/FreeRTOS/FreeRTOS/Source/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_bsp_al/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_debug_al/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_security_al/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_security_al/mbedtls/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_transport_al/PTX/COMMON" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_transport_al/PTX/FELICA_DTE" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_ranging_al/deca_source/drivers/dwt_uwb_driver/Inc" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_ranging_al/deca_source/node/task_node" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_ranging_al/deca_source/node/Inc" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_ranging_al/deca_source/config/default_config" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_ranging_al/deca_source/node/srv/tag_list" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_ranging_al/deca_source/node/CMSIS_RTOS" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_ranging_al/deca_source/node/core/usb_uart_tx" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_ranging_al/deca_source/node/srv/common_n" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_ranging_al/deca_source/node/node" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_ranging_al/deca_source/node/srv/msg_time" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/platform/thinkey_ranging_al/deca_source/platform_nrf52840/port" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/application/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/application/tab_app/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/framework/common/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/framework/dk_message_format/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/framework/fast_transaction/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/framework/fw_controller/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/framework/owner_pairing/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/framework/parse_format_commands/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/framework/passive_entry/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/ranging/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/ranging/ral/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/security/crypto_abs/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/security/crypto_common/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/transport/core/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/security/crypto_data_access/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/security/crypto_digitalkey_applet/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/security/crypto_dk_creation/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/security/crypto_spake2/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/security/crypto_storage/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/transport/nal/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/framework/standard_transaction/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/transport/btal/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/transport/include" -I"C:/Users/jeeva/OneDrive/Desktop/Thinkseed/vehicle-gits2/platform/renesas/THINKEY_RENESAS_DEMO_PROJECT/../../../vehicle/include" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

