/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (11)
#endif
/* ISR prototypes */
void gpt_counter_overflow_isr(void);
void spi_rxi_isr(void);
void spi_txi_isr(void);
void spi_tei_isr(void);
void spi_eri_isr(void);
void fcu_frdyi_isr(void);
void fcu_fiferr_isr(void);
void sci_uart_rxi_isr(void);
void sci_uart_txi_isr(void);
void sci_uart_tei_isr(void);
void sci_uart_eri_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_GPT0_COUNTER_OVERFLOW ((IRQn_Type) 0) /* GPT0 COUNTER OVERFLOW (Overflow) */
#define GPT0_COUNTER_OVERFLOW_IRQn          ((IRQn_Type) 0) /* GPT0 COUNTER OVERFLOW (Overflow) */
#define VECTOR_NUMBER_SPI0_RXI ((IRQn_Type) 1) /* SPI0 RXI (Receive buffer full) */
#define SPI0_RXI_IRQn          ((IRQn_Type) 1) /* SPI0 RXI (Receive buffer full) */
#define VECTOR_NUMBER_SPI0_TXI ((IRQn_Type) 2) /* SPI0 TXI (Transmit buffer empty) */
#define SPI0_TXI_IRQn          ((IRQn_Type) 2) /* SPI0 TXI (Transmit buffer empty) */
#define VECTOR_NUMBER_SPI0_TEI ((IRQn_Type) 3) /* SPI0 TEI (Transmission complete event) */
#define SPI0_TEI_IRQn          ((IRQn_Type) 3) /* SPI0 TEI (Transmission complete event) */
#define VECTOR_NUMBER_SPI0_ERI ((IRQn_Type) 4) /* SPI0 ERI (Error) */
#define SPI0_ERI_IRQn          ((IRQn_Type) 4) /* SPI0 ERI (Error) */
#define VECTOR_NUMBER_FCU_FRDYI ((IRQn_Type) 5) /* FCU FRDYI (Flash ready interrupt) */
#define FCU_FRDYI_IRQn          ((IRQn_Type) 5) /* FCU FRDYI (Flash ready interrupt) */
#define VECTOR_NUMBER_FCU_FIFERR ((IRQn_Type) 6) /* FCU FIFERR (Flash access error interrupt) */
#define FCU_FIFERR_IRQn          ((IRQn_Type) 6) /* FCU FIFERR (Flash access error interrupt) */
#define VECTOR_NUMBER_SCI5_RXI ((IRQn_Type) 7) /* SCI5 RXI (Received data full) */
#define SCI5_RXI_IRQn          ((IRQn_Type) 7) /* SCI5 RXI (Received data full) */
#define VECTOR_NUMBER_SCI5_TXI ((IRQn_Type) 8) /* SCI5 TXI (Transmit data empty) */
#define SCI5_TXI_IRQn          ((IRQn_Type) 8) /* SCI5 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI5_TEI ((IRQn_Type) 9) /* SCI5 TEI (Transmit end) */
#define SCI5_TEI_IRQn          ((IRQn_Type) 9) /* SCI5 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI5_ERI ((IRQn_Type) 10) /* SCI5 ERI (Receive error) */
#define SCI5_ERI_IRQn          ((IRQn_Type) 10) /* SCI5 ERI (Receive error) */
#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
