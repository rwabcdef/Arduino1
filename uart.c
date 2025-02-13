#include "uart.h"
#if defined(ENV_CONFIG__SYSTEM_ARDUINO_UNO_R3)
#include <avr/interrupt.h>
#include "EmbUtils.h"
#endif
#include<stdlib.h>
#include<string.h>


static volatile char g_rx[UART_BUFF_LEN];
static volatile char g_rxChar = 0;
static volatile uint8_t g_rxIndex = 0;
static volatile uint8_t g_rxStatus = 0;
static volatile bool g_eof = false;
static volatile uint8_t g_rxLen;
static volatile bool rxBusy = false;

static char* pRxFramebuffer;  // external buffer
static uint8_t rxFrameBufferLen; // external buffer length


static volatile char g_tx[UART_BUFF_LEN];
static uint8_t g_txLen = 0;
static uint8_t g_txIndex = 0;
static volatile bool txBusy = false;
static volatile char g_txChar = 0;

//--------------------------------------------------------------------------------
// Rx interrupt routine
ISR(USART_RX_vect)
{
  g_rxChar = UDR0;

  g_rxStatus = UCSR0A;

  if(UCSR0A & 0x10)
  {
    // Frame error
    return;
  }

  g_rx[g_rxIndex++] = g_rxChar;

  rxBusy = true;

  if('\n' == g_rxChar){
    g_eof = true;
    //cbi(UCSR0B, RXCIE0); // disable uart rx interrupt

    g_rxLen = g_rxIndex;

    memset(pRxFramebuffer, 0, rxFrameBufferLen); // clear external buffer

    strncpy(pRxFramebuffer, g_rx, g_rxLen);

    memset(g_rx, 0, UART_BUFF_LEN); // clear rx buffer

    g_rxIndex = 0; // reset rx buffer index
  }  
}
//--------------------------------------------------------------------------------
ISR(USART_UDRE_vect)
{
  g_txChar = g_tx[g_txIndex];
  //UDR0 = g_tx[g_txIndex]; // write current char
  UDR0 = g_txChar;
  g_txIndex++;
  //if(g_txIndex >= g_txLen)
  if(g_txChar == '\n')
  {
    // all chars have been written
    cbi(UCSR0B, UDRIE0);   // disable uart data register empty interrupt
    //sbi(UCSR0B, RXCIE0);   // enable uart rx interrupt
    txBusy = false;
  }
}
//--------------------------------------------------------------------------------
// Init uart for 19200 baud 8N1, rx & tx
void uart_init(char* pRxBuffer, uint8_t rxBufferLen)
{
  //-------------------------------
  // Initialise uart hardware
  UCSR0A = 0; // 1 << U2X0;
  UCSR0B = 0;

  // 19200 baud
  UBRR0H = 0;
  UBRR0L = 51;

  UCSR0C = 0x06;

  sbi(UCSR0B, RXEN0);
  sbi(UCSR0B, TXEN0);
  sbi(UCSR0B, RXCIE0);
  cbi(UCSR0B, UDRIE0); 
  //-------------------------------

  pRxFramebuffer = pRxBuffer;
  rxFrameBufferLen = rxBufferLen;
}

//--------------------------------------------------------------------------------
bool uart_checkFrameRx()
{
  bool b = g_eof;
  g_eof = false;
  return b;
}
//--------------------------------------------------------------------------------
uint8_t uart_getRxLenAndReset()
{
  rxBusy = false;
  return g_rxLen;
}
//--------------------------------------------------------------------------------

//uint8_t uart_write(char* buffer, uint8_t len)
uint8_t uart_write(char* buffer)
{
  if(rxBusy || txBusy)
  {
    return UART_STATUS_BUSY;
  }

  uint8_t len = 0;
  bool eofFound = false;
  for(len=0; len<UART_BUFF_LEN; len++)
  {
    if(buffer[len] == '\n'){
      break;
    }
  }
  buffer[UART_BUFF_LEN] = '\n';
  len++;
  strncpy(g_tx, buffer, len); // copy incoming buffer to tx buffer
  g_txLen = len;
  g_txIndex = 0;   // reset buffer index
  txBusy = true;
  sbi(UCSR0B, UDRIE0); // enable uart data register empty interrupt

  return UART_STATUS_OK;
}
//--------------------------------------------------------------------------------
bool uart_getTxBusy()
{
	return txBusy;
}
//--------------------------------------------------------------------------------
