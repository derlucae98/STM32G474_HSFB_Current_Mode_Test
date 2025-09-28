/*
 * application.c
 *
 *  Created on: Sep 23, 2025
 *      Author: luca
 */

#include "application.h"

static ctrl_2p2z_t ctrl_u;
static ctrl_2p2z_t ctrl_i;


void BSP_PB_Callback(Button_TypeDef Button) {
    if (Button == BUTTON_USER) {

    }
}

volatile uint32_t adc_val = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {


    HAL_GPIO_WritePin(ADC_ISR_DBG_GPIO_Port, ADC_ISR_DBG_Pin, 1);

    uint16_t dac_val = ctrl_2p2z_update(&ctrl_u, adc_val, REF);

    LL_DAC_SetWaveSawtoothResetData(DAC3, LL_DAC_CHANNEL_1, dac_val);

    HAL_GPIO_WritePin(ADC_ISR_DBG_GPIO_Port, ADC_ISR_DBG_Pin, 0);
}

void app_init(void) {

    ctrl_2p2z_init(&ctrl_u, U_B0, U_B1, U_B2, U_A1, U_A2, U_K, 0, 4095);

    HAL_DAC_Start(&hdac3, DAC_CHANNEL_1);
    HAL_OPAMP_Start(&hopamp6);
    HAL_COMP_Start(&hcomp3);

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &adc_val, 1);
    __HAL_DMA_DISABLE_IT(&hdma_adc1, DMA_IT_HT); // Disable DMA half-transfer IRQ
    __HAL_DMA_ENABLE_IT(&hdma_adc1, DMA_IT_TC); //Enable DMA transfer complete IRQ. This will call HAL_ADC_ConvCpltCallback()

    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA2);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB1);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB2);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC2);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TE1);

    HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1,
            HRTIM_TIMERID_MASTER | HRTIM_TIMERID_TIMER_A | HRTIM_TIMERID_TIMER_B
                    | HRTIM_TIMERID_TIMER_C | HRTIM_TIMERID_TIMER_D
                    | HRTIM_TIMERID_TIMER_E);

}

uint16_t ctrl_2p2z_update(ctrl_2p2z_t *ctrl, uint16_t input, uint16_t ref) {
    int32_t err = ref - input;

    // Shift new sample into sample buffer
    ctrl->ctrl_2p2z_x[0] = ctrl->ctrl_2p2z_x[1];
    ctrl->ctrl_2p2z_x[1] = ctrl->ctrl_2p2z_x[2];
    ctrl->ctrl_2p2z_x[2] = (float)err;

    // Shift output of previous calculation into sample buffer
    ctrl->ctrl_2p2z_y[0] = ctrl->ctrl_2p2z_y[1];
    ctrl->ctrl_2p2z_y[1] = ctrl->ctrl_2p2z_y[2];

    float yb0 = ctrl->ctrl_2p2z_B0 * ctrl->ctrl_2p2z_x[2];
    float yb1 = ctrl->ctrl_2p2z_B1 * ctrl->ctrl_2p2z_x[1];
    float yb2 = ctrl->ctrl_2p2z_B2 * ctrl->ctrl_2p2z_x[0];
    float ya1 = ctrl->ctrl_2p2z_A1 * ctrl->ctrl_2p2z_y[1];
    float ya2 = ctrl->ctrl_2p2z_A2 * ctrl->ctrl_2p2z_y[0];

    ctrl->ctrl_2p2z_y[2] = yb0 + yb1 + yb2 + ya1 + ya2;

    // Clamp output to max and min value
    if (ctrl->ctrl_2p2z_y[2] >= ctrl->ctrl_2p2z_sat_max) {
        ctrl->ctrl_2p2z_y[2] = ctrl->ctrl_2p2z_sat_max;
    }
    if (ctrl->ctrl_2p2z_y[2] < ctrl->ctrl_2p2z_sat_min) {
        ctrl->ctrl_2p2z_y[2] = ctrl->ctrl_2p2z_sat_min;
    }

    return (uint16_t) ctrl->ctrl_2p2z_y[2];
}

void ctrl_2p2z_init(ctrl_2p2z_t *ctrl, float B0, float B1, float B2, float A1, float A2, float K, uint32_t sat_min, uint32_t sat_max) {
    ctrl->ctrl_2p2z_x[0] = 0.0f;
    ctrl->ctrl_2p2z_x[1] = 0.0f;
    ctrl->ctrl_2p2z_x[2] = 0.0f;
    ctrl->ctrl_2p2z_y[0] = 0.0f;
    ctrl->ctrl_2p2z_y[1] = 0.0f;
    ctrl->ctrl_2p2z_y[2] = 0.0f;
    ctrl->ctrl_2p2z_B0 = B0 * K;
    ctrl->ctrl_2p2z_B1 = B1 * K;
    ctrl->ctrl_2p2z_B2 = B2 * K;
    ctrl->ctrl_2p2z_A1 = A1;
    ctrl->ctrl_2p2z_A2 = A2;
    ctrl->ctrl_2p2z_sat_min = sat_min;
    ctrl->ctrl_2p2z_sat_max = sat_max;
}

void app_loop(void) {

}
