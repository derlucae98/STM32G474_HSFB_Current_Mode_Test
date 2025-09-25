/*
 * application.c
 *
 *  Created on: Sep 23, 2025
 *      Author: luca
 */

#include "application.h"

FMAC_FilterConfigTypeDef sFmacConfig;
/* Array of filter coefficients A (feedback coefficients) in Q1.15 format */
static int16_t aFilterCoeffA[COEFF_VECTOR_A_SIZE] = { A1, A2 };

/* Array of filter coefficients B (feed-forward taps) in Q1.15 format */
static int16_t aFilterCoeffB[COEFF_VECTOR_B_SIZE] = { (int16_t) B0,
        (int16_t) B1, (int16_t) B2 };

static uint16_t test_inputs[16] = {3721, 158, 4092, 2876, 941, 2045, 3782, 67, 3156, 2299, 124, 3998, 1777, 845, 2560, 310};
static uint8_t current_value = 0;

volatile static uint32_t fmac_status;

void HAL_FMAC_OutputDataReadyCallback(FMAC_HandleTypeDef* hfmac);

void BSP_PB_Callback(Button_TypeDef Button) {
    if (Button == BUTTON_USER) {
        fmac_prepare_input(test_inputs[current_value]);
        current_value++;
    }
}

/* Expected number of calculated samples */
uint16_t output_size = (uint16_t) 1;

volatile uint32_t adc_val = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    HAL_GPIO_WritePin(ADC_ISR_DBG_GPIO_Port, ADC_ISR_DBG_Pin, 1);
    // 12 Bit conversion result is converted into a 15 bit left aligned value
    uint32_t err = adc_val << 3;
    err = err - REF;
    HAL_GPIO_WritePin(ADC_ISR_DBG_GPIO_Port, ADC_ISR_DBG_Pin, 0);
}

static int16_t fmac_input[3] = {0}; // fmac_input[3] contains the new sample
static int16_t fmac_output[3] = {0}; // fmac_output[3] contains the new sample

void app_init(void) {

//    HAL_DAC_Start(&hdac3, DAC_CHANNEL_1);
//    HAL_OPAMP_Start(&hopamp6);
//
//    HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &adc_val, 1);
//    __HAL_DMA_DISABLE_IT(&hdma_adc1, DMA_IT_HT); // Disable DMA half-transfer IRQ
//    __HAL_DMA_ENABLE_IT(&hdma_adc1, DMA_IT_TC); //Enable DMA transfer complete IRQ. This will call HAL_ADC_ConvCpltCallback()
//
//    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1);
//    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA2);
//    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB1);
//    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB2);
//    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1);
//    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC2);
//    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TE1);
//
//    HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1,
//            HRTIM_TIMERID_MASTER | HRTIM_TIMERID_TIMER_A | HRTIM_TIMERID_TIMER_B
//                    | HRTIM_TIMERID_TIMER_C | HRTIM_TIMERID_TIMER_D
//                    | HRTIM_TIMERID_TIMER_E);

    /* Configure the FMAC peripheral */
    sFmacConfig.InputBaseAddress = INPUT_BUFFER_BASE;
    sFmacConfig.InputBufferSize = INPUT_BUFFER_SIZE;
    sFmacConfig.InputThreshold = INPUT_THRESHOLD;
    sFmacConfig.CoeffBaseAddress = COEFFICIENT_BUFFER_BASE;
    sFmacConfig.CoeffBufferSize = COEFFICIENT_BUFFER_SIZE;
    sFmacConfig.OutputBaseAddress = OUTPUT_BUFFER_BASE;
    sFmacConfig.OutputBufferSize = OUTPUT_BUFFER_SIZE;
    sFmacConfig.OutputThreshold = OUTPUT_THRESHOLD;
    sFmacConfig.pCoeffA = aFilterCoeffA;
    sFmacConfig.CoeffASize = COEFF_VECTOR_A_SIZE;
    sFmacConfig.pCoeffB = aFilterCoeffB;
    sFmacConfig.CoeffBSize = COEFF_VECTOR_B_SIZE;
    sFmacConfig.Filter = FMAC_FUNC_IIR_DIRECT_FORM_1;
    sFmacConfig.InputAccess = FMAC_BUFFER_ACCESS_POLLING;
    sFmacConfig.OutputAccess = FMAC_BUFFER_ACCESS_DMA;
    sFmacConfig.Clip = FMAC_CLIP_ENABLED;
    sFmacConfig.P = COEFF_VECTOR_B_SIZE;
    sFmacConfig.Q = COEFF_VECTOR_A_SIZE;
    sFmacConfig.R = post_shift;

    if (HAL_FMAC_FilterConfig(&hfmac, &sFmacConfig) != HAL_OK) {
        /* Configuration Error */
        Error_Handler();
    }

    /* Preload the input and output buffers */
    if (HAL_FMAC_FilterPreload(&hfmac, NULL, INPUT_BUFFER_SIZE, fmac_output, COEFF_VECTOR_A_SIZE) != HAL_OK) {
        /* Configuration Error */
        Error_Handler();
    }


    __HAL_FMAC_DISABLE_IT(&hfmac, FMAC_IT_OVFLIEN  | FMAC_IT_UNFLIEN);
    __HAL_FMAC_ENABLE_IT(&hfmac, FMAC_IT_RIEN);
}

void fmac_prepare_input(uint32_t input) {

    //HAL_GPIO_WritePin(ADC_ISR_DBG_GPIO_Port, ADC_ISR_DBG_Pin, 1);
    // 12 Bit conversion result is converted into a 15 bit left aligned value
    uint32_t err = adc_val << 3;
    //err = err - REF;
    //HAL_GPIO_WritePin(ADC_ISR_DBG_GPIO_Port, ADC_ISR_DBG_Pin, 0);

    // Shift new sample into sample buffer
    fmac_input[0] = fmac_input[1];
    fmac_input[1] = fmac_input[2];
    fmac_input[2] = input;


    if (HAL_FMAC_FilterPreload(&hfmac, fmac_input, 3, fmac_output, 2) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_FMAC_FilterStart(&hfmac, &fmac_output[2], &output_size) != HAL_OK) {
        Error_Handler();
    }
}

void HAL_FMAC_OutputDataReadyCallback(FMAC_HandleTypeDef* hfmac) {
    // Shift new output into output buffer
    fmac_output[0] = fmac_output[1];
    fmac_output[1] = fmac_output[2];
    HAL_FMAC_FilterStop(hfmac);

}


void app_loop(void) {

}
