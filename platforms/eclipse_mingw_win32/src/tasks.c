#include "BRTOS.h"
#include "stdio.h"
#include "device.h"

void exec(void)
{
  while(1)
  {

	  printf("Tick Count: %u\r\n", (uint32_t)OSGetTickCount());
	  DelayTask(100);
  }
}

void exec2(void)
{

  uint32_t run = 0;
  while(1)
  {
	//printf("Task 2, run: %u\r\n", (uint32_t)run++);
    DelayTask(100);
  }
}

void exec3(void)
{
  char c = 0;
  while(1)
  {

	  //printf("Task 3, input: %c\r\n", c);
	  DelayTask(200);


  }
}

#define INF_TIMEOUT 	0

void SerialTask(void)
{
	char data;
	OS_Device_Control_t *uart;
	uart_config_t uart0;

	uart0.baudrate = 9600;
	uart0.parity = UART_PAR_EVEN;
	uart0.polling_irq = UART_IRQ;
	uart0.queue_size = 128;
	uart0.stop_bits = UART_STOP_ONE;
	uart0.timeout = 10;
	uart0.read_mutex = false;
	uart0.write_mutex = true;
	uart = OSDevOpen("UART0", &uart0);

	OSDevWrite(uart,"Porta serial ativa!\n\r",21);

	while(1){
		if (OSDevRead(uart,&data,1) >= 1){
			if (OSDevSet(uart,CTRL_ACQUIRE_WRITE_MUTEX,50) == OK)
			{
				OSDevWrite(uart,&data,1);
				OSDevSet(uart,CTRL_RELEASE_WRITE_MUTEX,0);
			}
		}
	}
}


