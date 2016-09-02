/*
 * GPIOx.c
 *
 *  Created on: 10 de ago de 2016
 *      Author: gustavo
 */

//#include "WinBase.h"
#include "BRTOS.h"
#include "device.h"

int ff1 (int x)
{
	int i=0;
	if(!x) return -1;
	while(!((x>>(i++)) & 1));
	return i;
}

static size_t GPIO_Write(OS_Device_Control_t *dev, void *string, size_t size)
{
	uint32_t mask = (uint32_t)string;
	if(size)
	{
		Beep(dev->device->base_address*mask/1000, 1000);
	}
	return 0;
}

static size_t GPIO_Read(OS_Device_Control_t *dev, char *string, size_t size ){
	uint32_t temp = (uint32_t)string;
	(void)size;
	return (size_t)(1);
}

static size_t GPIO_Set(OS_Device_Control_t *dev, uint32_t request, uint32_t value){

	/* todo */

	return 0;
}

static size_t GPIO_Get(OS_Device_Control_t *dev, uint32_t request){
	uint32_t ret;
	gpio_config_t *gpio_conf = (gpio_config_t *)dev->device->DriverData;
	switch(request){
		default:
			ret = 0;
			break;
	}
	return ret;
}

const device_api_t GPIO_api ={
		.read = (Device_Control_read_t)GPIO_Read,
		.write = (Device_Control_write_t)GPIO_Write,
		.set = (Device_Control_set_t)GPIO_Set,
		.get = (Device_Control_get_t)GPIO_Get
};

#define GPIOA_BASE  110000
#define GPIOB_BASE  123471
#define GPIOC_BASE  65406
#define GPIOD_BASE  73416
#define GPIOE_BASE  82407
#define GPIOF_BASE  87307
#define GPIOG_BASE  97999

void OSOpenGPIO(void *pdev, void *parameters){
	int i = 0;
	unsigned long pins;
	unsigned long mux = 0;
	gpio_config_t *gpio_conf = (gpio_config_t *)parameters;
	OS_Device_Control_t *dev = pdev;
	switch(dev->device_number)
	{
		case 'A':
			dev->device->base_address = GPIOA_BASE;
			break;
		case 'B':
			dev->device->base_address = GPIOB_BASE;
			break;
		case 'C':
			dev->device->base_address = GPIOC_BASE;
			break;
		case 'D':
			dev->device->base_address = GPIOD_BASE;
			break;
		case 'E':
			dev->device->base_address = GPIOE_BASE;
			break;
		case 'F':
			dev->device->base_address = GPIOF_BASE;
			break;
		case 'G':
			dev->device->base_address = GPIOG_BASE;
			break;
		default:
			break;
	}

	dev->api = &GPIO_api;

}
