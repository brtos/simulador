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

#include "drivers/drivers.h"
void exec2(void)
{

  OS_Device_Control_t *adc;
  adc_config_t adc0;
  uint8_t buf[2];

  adc0.polling_irq = ADC_POLLING;
  adc0.resolution = ADC_RES_16;
  adc0.samplerate = 1000;
  adc0.timeout = 0;
  adc = OSDevOpen("ADC0", &adc0);

  while(1)
  {

	OSDevRead(adc,buf,2);
    DelayTask(100);
  }
}

void exec3(void)
{
	OS_Device_Control_t *dev_gpiob;
	gpio_config_t gpiob;

	OS_Device_Control_t *dev_gpiod;
	gpio_config_t gpiod;

	gpiob.used_pins_out = GPIO_PIN_8 | GPIO_PIN_7;
	gpiob.used_pins_in = 0;
	gpiob.irq_pins = 0;
	dev_gpiob = OSDevOpen("GPIOB", &gpiob);

	gpiod.used_pins_out = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8;
	gpiod.used_pins_in = 0;
	gpiod.irq_pins = 0;
	dev_gpiod = OSDevOpen("GPIOD", &gpiod);

	OSGPIOWrite(dev_gpiob,GPIO_PIN_8 | GPIO_PIN_7,1);
	OSGPIOWrite(dev_gpiod,GPIO_PIN_1,1);

  while(1)
  {
	  //printf("Task 3, input: %c\r\n", c);
	  DelayTask(200);
	  OSGPIOWrite(dev_gpiob,GPIO_PIN_8,1);

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


