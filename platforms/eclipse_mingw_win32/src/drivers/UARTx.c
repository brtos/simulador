#include "BRTOS.h"
#include "device.h"
#include "drivers.h"

#include "stdio.h"

#define NUM_UART		1

static BRTOS_Sem   *SerialTX[NUM_UART];
static BRTOS_Queue *SerialQ[NUM_UART];

static BRTOS_Mutex *SerialRMutex[NUM_UART];
static BRTOS_Mutex *SerialWMutex[NUM_UART];


char UART_RX_buffer;
static uint32_t UARTHandler(void)
{
	char c = UART_RX_buffer;
	if(SerialQ[0] != NULL)
	{
		OSQueuePost(SerialQ[0], c);
	}
	return TRUE;
}


static void Init_UART(void *parameters)
{
	uart_config_t *uart_conf = (uart_config_t *)parameters;

	ConfiguraInterruptHandler( INTERRUPT_UART, UARTHandler);

	// todo: Configure UART Baud
	//UARTConfigSet(uart_conf->baudrate, config);

	ASSERT(OSSemCreate(0, &SerialTX[0]) == ALLOC_EVENT_OK);
	ASSERT(OSQueueCreate(uart_conf->queue_size, &SerialQ[0]) == ALLOC_EVENT_OK);

	if (uart_conf->write_mutex == true){
		OSMutexCreate (&SerialWMutex[0], 0);
	}

	if (uart_conf->read_mutex == true){
		OSMutexCreate (&SerialRMutex[0], 0);
	}
}


static size_t UART_Write(OS_Device_Control_t *dev, char *string, size_t size ){
	size_t nbytes = 0;

	while(size){
		putchar((char)*string);
		nbytes++;
		size--;
		string++;
	}
	return nbytes;
}

static size_t UART_Read(OS_Device_Control_t *dev, char *string, size_t size ){
	size_t nbytes = 0;
	uart_config_t *uart_conf = (uart_config_t *)dev->device->DriverData;
	while(nbytes < size)
	{
		if (OSQueuePend(SerialQ[dev->device_number], (uint8_t*)string, uart_conf->timeout) != READ_BUFFER_OK) goto failed_rx;
		string++;
		nbytes++;
	}
failed_rx:
	return nbytes;
}

static size_t UART_Set(OS_Device_Control_t *dev, uint32_t request, uint32_t value){
	unsigned long config = 0;
	size_t ret = 0;
	uart_config_t *uart_conf = (uart_config_t *)dev->device->DriverData;

	switch(request){
		case UART_BAUDRATE:
			uart_conf->baudrate = value;
			break;
		case UART_PARITY:
			uart_conf->parity = value;
			break;
		case UART_STOP_BITS:
			uart_conf->polling_irq = value;
			break;
		case UART_QUEUE_SIZE:
			/* somente pode ser alterado se filar dinâmicas forem utilizadas. */
			break;
		case UART_TIMEOUT:
			uart_conf->timeout = value;
			break;
		case CTRL_ACQUIRE_READ_MUTEX:
			if (SerialRMutex[dev->device_number] != NULL){
				ret = OSMutexAcquire(SerialRMutex[dev->device_number],value);
			}
			break;
		case CTRL_ACQUIRE_WRITE_MUTEX:
			if (SerialWMutex[dev->device_number] != NULL){
				ret = OSMutexAcquire(SerialWMutex[dev->device_number],value);
			}
			break;
		case CTRL_RELEASE_WRITE_MUTEX:
			if (SerialWMutex[dev->device_number] != NULL){
				ret = OSMutexRelease(SerialWMutex[dev->device_number]);
			}
			break;
		case CTRL_RELEASE_READ_MUTEX:
			if (SerialRMutex[dev->device_number] != NULL){
				ret = OSMutexRelease(SerialRMutex[dev->device_number]);
			}
			break;
		default:
			break;
	}

	return ret;
}

static size_t UART_Get(OS_Device_Control_t *dev, uint32_t request){
	uint32_t ret;
	uart_config_t *uart_conf = (uart_config_t *)dev->device->DriverData;

	switch(request){
		case UART_BAUDRATE:
			ret = uart_conf->baudrate;
			break;
		case UART_PARITY:
			ret = uart_conf->parity;
			break;
		case UART_STOP_BITS:
			ret = uart_conf->stop_bits;
			break;
		case UART_QUEUE_SIZE:
			ret = uart_conf->queue_size;
			break;
		case UART_TIMEOUT:
			ret = uart_conf->timeout;
			break;
		default:
			ret = 0;
			break;
	}
	return ret;
}

static const device_api_t UART_api ={
		.read = (Device_Control_read_t)UART_Read,
		.write = (Device_Control_write_t)UART_Write,
		.set = (Device_Control_set_t)UART_Set,
		.get = (Device_Control_get_t)UART_Get
};

void OSOpenUART(void *pdev, void *parameters){
	OS_Device_Control_t *dev = pdev;
	Init_UART(parameters);
	dev->api = &UART_api;
}



