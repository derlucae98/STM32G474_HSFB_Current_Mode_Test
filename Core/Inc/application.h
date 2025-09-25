/*
 * application.h
 *
 *  Created on: Sep 23, 2025
 *      Author: luca
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include "main.h"

extern COM_InitTypeDef BspCOMInit;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern DAC_HandleTypeDef hdac3;
extern FMAC_HandleTypeDef hfmac;
extern HRTIM_HandleTypeDef hhrtim1;
extern OPAMP_HandleTypeDef hopamp6;
extern DMA_HandleTypeDef hdma_fmac_read;

// Start parameters from WDS:
#define B0 (0x1B37)
#define B1 (0xE)
#define B2 (0xE4D7)
#define A1 (0x6000)
#define A2 (0xE000)
#define post_shift (+1)
#define REF (2048 << 3)

#define COEFF_VECTOR_B_SIZE     3
#define COEFF_VECTOR_A_SIZE     2
#define MEMORY_PARAMETER_D1     1
#define MEMORY_PARAMETER_D2     2
#define INPUT_THRESHOLD         FMAC_THRESHOLD_1
#define OUTPUT_THRESHOLD        FMAC_THRESHOLD_1
#define COEFFICIENT_BUFFER_BASE 0
#define COEFFICIENT_BUFFER_SIZE COEFF_VECTOR_B_SIZE + COEFF_VECTOR_A_SIZE
#define INPUT_BUFFER_BASE       COEFFICIENT_BUFFER_SIZE
#define INPUT_BUFFER_SIZE       COEFF_VECTOR_B_SIZE + MEMORY_PARAMETER_D1
#define OUTPUT_BUFFER_BASE      COEFFICIENT_BUFFER_SIZE + INPUT_BUFFER_SIZE
#define OUTPUT_BUFFER_SIZE      COEFF_VECTOR_A_SIZE + MEMORY_PARAMETER_D2
#define FMAC_WDATA (0x40021418)


void app_init(void);

void fmac_prepare_input(uint32_t input);

void app_loop(void);

#endif /* INC_APPLICATION_H_ */
