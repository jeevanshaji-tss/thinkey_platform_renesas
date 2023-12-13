/*
 * Copyright (c) 2019-2022 Mauna Kea Semiconductor Holdings.
 * All rights reserved.
 *
 */

#include "stdlib.h"
#include "uci_common.h"
#include "r_ioport.h"
#include "common_data.h"
#include "thinkey_debug.h"
/****************************************************************************//**
 *
 *
 *
 *******************************************************************************/


volatile uint32_t tick_count = 0;
uci_rx_buf_t uci_rx_buf[MAX_UCI_RX_BUF_NUM];

/****************************************************************************//**
 *
 *                                 Time section
 *
 *******************************************************************************/
/*
 * SysTick callback
 *
 * Note: this function is called every 1 ms
 *
 */
//void SysTick_Handler(void)
//{
//	tick_count += 1;
//}

uint32_t hal_get_tick(void)
{
	return (tick_count);
}

/*******************************************************************************
* Function Name: handle_error
********************************************************************************
* Summary:
* User defined error handling function.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void handle_error(void)
{
     /* Disable all interrupts. */
    __disable_irq();

    CY_ASSERT(0);
}

void delay_ms(uint32_t ms)
{

	R_BSP_SoftwareDelay(ms, BSP_DELAY_UNITS_MILLISECONDS);
}

void delay_us(uint32_t us)
{
	cyhal_system_delay_us(us);
}

void uwb_reset(void)
{
    /* MK8000 Reset Pin Active Low */
	gpio_write(GPIO_MK8000_RESET_PIN, 0);
	delay_ms(100);
    /* MK8000 Reset Pin Active High */
	gpio_write(GPIO_MK8000_RESET_PIN, 1);
	delay_ms(300);

	return;
}

void uci_reset_rx_buf(void)
{
	THINKEY_DEBUG_INFO("uci_reset_rx_buf called\r\n");
	for(int i = 0; i <MAX_UCI_RX_BUF_NUM;i++)
	{
		uci_rx_buf[i].occupied = false;
	}
	uci_rx_buf[0].occupied = true;
}

int uci_get_free_rx_buf(void)
{
	int i;
	for(i = 0; i <MAX_UCI_RX_BUF_NUM;i++)
	{
	    THINKEY_DEBUG_INFO("uci_rx_buf[%d].occupied:%d\r\n", i, uci_rx_buf[i].occupied );
		if( uci_rx_buf[i].occupied == false ) {
			break;
		}
	}
	if(i >= MAX_UCI_RX_BUF_NUM) {
	    THINKEY_DEBUG_INFO("run out of rx buffer, increase MAX_UCI_RX_BUF_NUM\r\n");
		assert(0);
	}

	uci_rx_buf[i].occupied = true;
	THINKEY_DEBUG_INFO("got free buffer:%d\r\n", i);
	return i;
}

void uci_buf_release(uint8_t bus_id, uint8_t uwbs_id, uint8_t *ptr)
{
	for(int i = 0; i <MAX_UCI_RX_BUF_NUM;i++)
	{
	    THINKEY_DEBUG_INFO("uci_rx_buf[%d].buf:%d\r\n" , i, uci_rx_buf[i].buf);
		if( uci_rx_buf[i].buf == ptr )
		{
			uci_rx_buf[i].occupied = false;
			break;
		}
	}
	return;
}

void *uci_malloc(uint16_t len)
{
	return malloc(len);
}

void uci_free(void* ptr)
{
	free(ptr);
	return;
}

uint32_t uci_enter_critical(void)
{
	//return cyhal_system_critical_section_enter();
}

void uci_exit_critical(uint32_t key)
{
	//cyhal_system_critical_section_exit(key);
}

uint32_t uci_get_tick(void)
{
	//return hal_get_tick();
    return xTaskGetTickCount();  //return freeRtos tick count
}

/*
 * hal init
 *
 *
 */
void hal_init(void)
{
    //cy_rslt_t result;
    fsp_err_t result = FSP_SUCCESS;
    /* config gpio for MK8000 Reset Pin control */
//    result = cyhal_gpio_init( GPIO_MK8000_RESET_PIN, CYHAL_GPIO_DIR_OUTPUT,
//    						CYHAL_GPIO_DRIVE_PULLUP, true);
    //GPIO_MK800_RESET_PIN configured as output (pull up mode) at port 6

//    CY_ASSERT(result == CY_RSLT_SUCCESS);
    /* MK8000 Reset Pin Active Low */
	//gpio_write(GPIO_MK8000_RESET_PIN, 1);
    result = R_IOPORT_PinWrite(&g_ioport_ctrl, GPIO_MK8000_RESET_PIN, BSP_IO_LEVEL_HIGH);

    /* config gpio for UWB_ROLE */
//    result = cyhal_gpio_init( GPIO_RANGING_ROLE, CYHAL_GPIO_DIR_INPUT,
//    						CYHAL_GPIO_DRIVE_PULLUP, true);
//    result = cyhal_gpio_init( GPIO_RESPONER_ID_PIN_0, CYHAL_GPIO_DIR_INPUT,
//    						CYHAL_GPIO_DRIVE_PULLUP, true);
//    result = cyhal_gpio_init( GPIO_RESPONER_ID_PIN_1, CYHAL_GPIO_DIR_INPUT,
//    						CYHAL_GPIO_DRIVE_PULLUP, true);
    // above 3 gpio pins are initialized at port 6

   // CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Enable interrupts */
    __enable_irq();

    /*Init the SysTick */
    // Clock source set to the CLK_LF system clock which is actually the WCO (32768 Hz) source.
    //The closest to 1msec is 1.024msec for 32 SysTick counts.

    // TODO:Below 3 commented functions we should implement
    //Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_LF, 32);
  //  Cy_SysTick_SetCallback(0, SysTick_Handler);
    /* Enable the SysTick resource*/
  //  Cy_SysTick_Enable();

    return;
}


