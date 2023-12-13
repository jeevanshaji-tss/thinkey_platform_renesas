/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = gpt_counter_overflow_isr, /* GPT0 COUNTER OVERFLOW (Overflow) */
            [1] = spi_rxi_isr, /* SPI0 RXI (Receive buffer full) */
            [2] = spi_txi_isr, /* SPI0 TXI (Transmit buffer empty) */
            [3] = spi_tei_isr, /* SPI0 TEI (Transmission complete event) */
            [4] = spi_eri_isr, /* SPI0 ERI (Error) */
            [5] = fcu_frdyi_isr, /* FCU FRDYI (Flash ready interrupt) */
            [6] = fcu_fiferr_isr, /* FCU FIFERR (Flash access error interrupt) */
            [7] = sci_uart_rxi_isr, /* SCI5 RXI (Received data full) */
            [8] = sci_uart_txi_isr, /* SCI5 TXI (Transmit data empty) */
            [9] = sci_uart_tei_isr, /* SCI5 TEI (Transmit end) */
            [10] = sci_uart_eri_isr, /* SCI5 ERI (Receive error) */
        };
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [0] = BSP_PRV_IELS_ENUM(EVENT_GPT0_COUNTER_OVERFLOW), /* GPT0 COUNTER OVERFLOW (Overflow) */
            [1] = BSP_PRV_IELS_ENUM(EVENT_SPI0_RXI), /* SPI0 RXI (Receive buffer full) */
            [2] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TXI), /* SPI0 TXI (Transmit buffer empty) */
            [3] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TEI), /* SPI0 TEI (Transmission complete event) */
            [4] = BSP_PRV_IELS_ENUM(EVENT_SPI0_ERI), /* SPI0 ERI (Error) */
            [5] = BSP_PRV_IELS_ENUM(EVENT_FCU_FRDYI), /* FCU FRDYI (Flash ready interrupt) */
            [6] = BSP_PRV_IELS_ENUM(EVENT_FCU_FIFERR), /* FCU FIFERR (Flash access error interrupt) */
            [7] = BSP_PRV_IELS_ENUM(EVENT_SCI5_RXI), /* SCI5 RXI (Received data full) */
            [8] = BSP_PRV_IELS_ENUM(EVENT_SCI5_TXI), /* SCI5 TXI (Transmit data empty) */
            [9] = BSP_PRV_IELS_ENUM(EVENT_SCI5_TEI), /* SCI5 TEI (Transmit end) */
            [10] = BSP_PRV_IELS_ENUM(EVENT_SCI5_ERI), /* SCI5 ERI (Receive error) */
        };
        #endif
