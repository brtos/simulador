/*
 * PWMx.c
 *
 *  Created on: 18 de ago de 2016
 *      Author: gustavo
 */


#include "BRTOS.h"
#include "device.h"

static size_t PWM_Set(OS_Device_Control_t *dev, uint32_t request, uint32_t value){

	size_t ret = 0;
	return ret;
}

static size_t PWM_Get(OS_Device_Control_t *dev, uint32_t request){
	uint32_t ret = 0;
	return ret;
}

static const device_api_t PWM_api ={
		.read = (Device_Control_read_t)NULL,
		.write = (Device_Control_write_t)NULL,
		.set = (Device_Control_set_t)PWM_Set,
		.get = (Device_Control_get_t)PWM_Get
};


void Init_PWM(unsigned long base, void *parameters)
{
	pwm_config_t *pwm_conf = (pwm_config_t *)parameters;
}


void OSOpenPWM(void *pdev, void *parameters)
{
	OS_Device_Control_t *dev = pdev;
	dev->api = &PWM_api;
}
