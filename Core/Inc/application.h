/*
 * application.h
 *
 *  Created on: Sep 23, 2025
 *      Author: luca
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include "main.h"
#include "stm32g4xx_ll_dac.h"
#include "stm32g4xx_ll_hrtim.h"

extern COM_InitTypeDef BspCOMInit;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern DAC_HandleTypeDef hdac3;
extern HRTIM_HandleTypeDef hhrtim1;
extern OPAMP_HandleTypeDef hopamp6;
extern COMP_HandleTypeDef hcomp3;

// Start parameters from WDS:
//#define U_K (1.0f)
#define U_K (+10.989011)
#define U_B0 (+0.395070356916)
#define U_B1 (+0.000393919184)
#define U_B2 (-0.394676437733)
#define U_A1 (+1.714285714286)
#define U_A2 (-0.714285714286)

#define REF (3614)
#define DUTY_TICKS_MIN (0)
#define DUTY_TICKS_MAX (3686)
#define SLOPE_VPP (0.4556)
#define DECVAL (1.6627)


// HRTIM equivalent frequency in Hz
#define HRTIM_EQUIVALENT_FREQ 5.44e9

// PWM Frequency (For full bridge = double) in Hz
#define PWM_FREQ 1e6

// Maximum duty cycle (For full bridge = double)
#define DUTY_MAX (2 * 0.40f)

// Primary falling edge to secondary rising edge in nanoseconds
#define DELAY_PRI_TO_SEC_NS 200

// Secondary falling edge to primary rising edge in nanoseconds
#define DELAY_SEC_TO_PRI_NS 200

// Leading edge blanking in ns
#define LEADING_EDGE_BLANKING_NS 25

// DAC step trigger (fixed to 15 MS/s)
#define DAC_STEP_TICKS 359

// Minimum on-time of PWM signal (see reference manual). Leading edge blanking must be longer than that
#define PWM_MINIMUM_ON_TIME_TICKS 98

#define PWM_PERIOD_TICKS (HRTIM_EQUIVALENT_FREQ/PWM_FREQ)

#define DUTY_TICKS (DUTY_MAX * PWM_PERIOD_TICKS)

#define NS_TO_TICKS(x) (x * 5.44e9/1e9)

// Ensure that maximum period ticks are smaller than (PWM_PERIOD_TICKS-DELAY_PRI_TO_SEC_NS)
#define PWM_MAX_DUTY_PERIOD_TICKS (PWM_PERIOD_TICKS-DELAY_PRI_TO_SEC_NS)

typedef struct ctrl_2p2z {
    float ctrl_2p2z_B0;
    float ctrl_2p2z_B1;
    float ctrl_2p2z_B2;
    float ctrl_2p2z_A1;
    float ctrl_2p2z_A2;
    float ctrl_2p2z_x[3];
    float ctrl_2p2z_y[3];
    uint32_t ctrl_2p2z_sat_min;
    uint32_t ctrl_2p2z_sat_max;
} ctrl_2p2z_t;

void ctrl_2p2z_init(ctrl_2p2z_t *ctrl, float B0, float B1, float B2, float A1, float A2, float K, uint32_t sat_min, uint32_t sat_max);
uint16_t ctrl_2p2z_update(ctrl_2p2z_t *ctrl, uint16_t input, uint16_t ref);

void init_pwm(void);


void app_init(void);



void app_loop(void);

#endif /* INC_APPLICATION_H_ */
