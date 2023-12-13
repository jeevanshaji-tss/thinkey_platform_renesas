/*
 * Copyright (c) 2019-2022 Mauna Kea Semiconductor Holdings.
 * All rights reserved.
 *
 */

#include "stdlib.h"
#include "uci_port.h"
#include "uci_debug.h"
#include "uci_common.h"
#include "r_sci_uart.h"
#include "common_utils.h"
#include "thinkey_debug.h"
#ifdef UCI_INTF_UART

/****************************************************************************//**
 *
 *
 *
 *******************************************************************************/

extern int32_t uci_get_uwbs_id();
extern int32_t uci_get_bus_id();
extern uci_rx_buf_t uci_rx_buf[MAX_UCI_RX_BUF_NUM];
char testBuffer[100];
uint32_t testSize=50;

/****************************************************************************//**
 *
 *                                 MACRO function
 *
 *******************************************************************************/

#define UART_RX_BUF_SIZE 				(1024)


/****************************************************************************//**
 *
 *                                 APP global variables
 *
 *******************************************************************************/

/* Variable Declarations */
//cyhal_uart_t uart_obj=
//	{
//		.pin_rx=UCI_UART_RX_PIN,
//		.pin_tx=UCI_UART_TX_PIN,
//		.pin_cts=NC,
//		.pin_rts=NC,
//	};

uint8_t uart_rx_buf[UART_RX_BUF_SIZE];
static int active_rx = 0;
static char buffer[100];
static int indexVal=0;
/*********************************************************************************/
void uwb_uart_callback(uart_callback_args_t *p_args){
    THINKEY_DEBUG_INFO("UART CALLBACK EVENT %d",p_args->event);
    if(p_args->event== UART_EVENT_RX_CHAR){
        //THINKEY_DEBUG_INFO(".....RECEIVING SOMETHING FROM UART..: 0%x",p_args->data);
        buffer[indexVal++]=(char)p_args->data;
        THINKEY_DEBUG_INFO("0x%x",p_args->data);

    }
    else if(p_args->event==UART_EVENT_RX_COMPLETE){
        THINKEY_DEBUG_INFO("INSIDE RX COMPLETE EVENT");

    }
    else if(p_args->event==UART_EVENT_ERR_OVERFLOW){
        THINKEY_DEBUG_INFO("Overflow Occured");
    }

}
static void uart_event_callback(/*void *callback_arg, cyhal_uart_event_t event*/)
{
	static uint8_t state = 0;
	static int len, offset;
	size_t size;

	//uint32_t count = cyhal_uart_readable(&uart_obj);
	//TODO : BELOW COUNT IS HARDCODED BY REPLACING ABOVE LINE
	  uint32_t count=0;
	switch(state) {
	case 0:
		if(count >=5) {
			size = 5;
			//cyhal_uart_read(&uart_obj,uci_rx_buf[active_rx].buf, &size);
			R_SCI_UART_Read(&g_uart1_ctrl, uci_rx_buf[active_rx].buf, size);
			len = uci_rx_buf[active_rx].buf[3];
			if(len==1) { /* get packet */
				uci_process_recv_pkt(uci_get_bus_id(), uci_get_uwbs_id(), uci_rx_buf[active_rx].buf, 5);
				active_rx = uci_get_free_rx_buf();
			} else {
				len -= 1;
				offset = 0;
				state = 1;
			}
		}
		break;
	case 1:
		if(count > 0)
		{
			size = count>(len-offset)?(len-offset):count;
			//cyhal_uart_read(&uart_obj, &uci_rx_buf[active_rx].buf[5+offset], &size);
			R_SCI_UART_Read(&g_uart1_ctrl, &uci_rx_buf[active_rx].buf[5+offset], size);
			offset+=size;
			if(offset >= len)
			{
				uci_process_recv_pkt(uci_get_bus_id(), uci_get_uwbs_id(), uci_rx_buf[active_rx].buf, len+5);
				state = 0;
				active_rx = uci_get_free_rx_buf();
			}
		}
		break;
	default:
		handle_error();
		break;
	}
}


void uci_intf_open(uint8_t bus_id)
{
    THINKEY_DEBUG_INFO("Configuring UART...\r\n");

//    const cyhal_uart_cfg_t uart_config =
//    {
//          .data_bits = 8,
//          .stop_bits = 1,
//          .parity = CYHAL_UART_PARITY_NONE,
//		  .rx_buffer = uart_rx_buf,
//		  .rx_buffer_size = UART_RX_BUF_SIZE
//    };
    fsp_err_t err = FSP_SUCCESS;
   // cy_rslt_t result = cyhal_uart_init(&uart_obj, UCI_UART_TX_PIN, UCI_UART_RX_PIN, NULL, &uart_config);
    err = R_SCI_UART_Open (&g_uart1_ctrl, &g_uart1_cfg);
   // CY_ASSERT(result == CY_RSLT_SUCCESS);


    uint32_t actualbaud;
    /* set UART baud rate to 921600bps */
    //result = cyhal_uart_set_baud(&uart_obj, 921600, &actualbaud);
   // result = cyhal_uart_set_baud(&uart_obj, 115200, &actualbaud);
   // CY_ASSERT(result == CY_RSLT_SUCCESS);

    //TODO:BELOW TWO FUNCTIONS SHOULD BE IMPLEMENTED
   // cyhal_uart_register_callback(&uart_obj, &uart_event_callback, NULL);
   // cyhal_uart_enable_event(&uart_obj, CYHAL_UART_IRQ_RX_NOT_EMPTY, CYHAL_ISR_PRIORITY_DEFAULT, true);

    uci_reset_rx_buf();
    //TODO:following read statemen is for the test purpose should be removed
    //R_SCI_UART_Read(&g_uart1_ctrl, &testBuffer, 2);

}

int32_t uci_send(uint8_t bus_id, uint8_t uwbs_id, const uint8_t *buf, uint16_t len)
{
	size_t l = (size_t) len;
	fsp_err_t err =FSP_SUCCESS;
	//TODO:BELOW WRITE FUNCTION IS TO BE MODIFIED
	//return cyhal_uart_write(&uart_obj, (uint8_t *)buf, &l);
	err= R_SCI_UART_Write(&g_uart1_ctrl, (uint8_t *)buf, l);
	THINKEY_DEBUG_INFO("UART WRITE STATUS : %d ",err);

	//TODO:following uart read api is for test purpose and should be removed

	 R_SCI_UART_Read(&g_uart1_ctrl, &testBuffer, &testSize);
	return err;

}

void uci_recov_from_timeout(uint8_t bus_id, uint8_t uwbs_id, uint8_t *buf, uint16_t len)
{
    // do nothing
}

#endif //UCI_INTF_UART
/****************************************************************************//**
 *
 *                             End APP port section
 *
 *******************************************************************************/


