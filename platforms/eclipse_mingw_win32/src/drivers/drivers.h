/*
 * drivers.h
 *
 *  Created on: 02/09/2016
 *      Author: Avell
 */

#ifndef DRIVERS_DRIVERS_H_
#define DRIVERS_DRIVERS_H_


#define INTERRUPT_UART  2
#define INTERRUPT_ADC   3
#define INTERRUPT_DAC	4


typedef enum{
	ADC_POLLING,
	ADC_IRQ,
}adc_irq_t;

typedef enum{
	ADC_RES_8 = 8,
	ADC_RES_10 = 10,
	ADC_RES_12 = 12,
	ADC_RES_16 = 16
}adc_res_t;


typedef enum
{
	ADC_SAMPLERATE,
	ADC_RESOLUTION,
	ADC_QUEUE_SIZE,
	ADC_TIMEOUT
}adc_request_t;

typedef struct adc_config_t_{
	int 		samplerate;
	adc_res_t   resolution;
	adc_irq_t 	polling_irq;
	int 		queue_size;
	ostick_t 	timeout;
	bool		read_mutex;
}adc_config_t;

typedef enum{
	DAC_POLLING,
	DAC_IRQ,
}dac_irq_t;

typedef enum{
	DAC_RES_8 = 8,
	DAC_RES_10 = 10,
	DAC_RES_12 = 12,
	DAC_RES_16 = 16
}dac_res_t;


typedef enum
{
	DAC_SAMPLERATE,
	DAC_RESOLUTION,
	DAC_QUEUE_SIZE,
	DAC_TIMEOUT
}dac_request_t;

typedef struct dac_config_t_
{
	int 		samplerate;
	dac_res_t   resolution;
	dac_irq_t 	polling_irq;
	int 		queue_size;
	ostick_t 	timeout;
	bool		write_mutex;
}dac_config_t;


#endif /* DRIVERS_DRIVERS_H_ */
