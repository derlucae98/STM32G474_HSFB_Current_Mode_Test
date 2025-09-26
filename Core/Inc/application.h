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
extern HRTIM_HandleTypeDef hhrtim1;
extern OPAMP_HandleTypeDef hopamp6;

// Start parameters from WDS:
#define U_B0 (+0.314417183586f)
#define U_B1 (+0.000626689610f)
#define U_B2 (-0.313790493976f)
#define U_A1 (+1.500000000000f)
#define U_A2 (-0.500000000000f)
//#define K (+10.821607546241f)
#define U_K (1.0f)
#define REF (2048)
#define DUTY_TICKS_MIN (0)
#define DUTY_TICKS_MAX (3686)
#define SLOPE_VPP (0.4556)
#define DECVAL (1.6627)

typedef struct ctrl_2p2z {
    float ctrl_2p2z_B0;
    float ctrl_2p2z_B1;
    float ctrl_2p2z_B2;
    float ctrl_2p2z_A1;
    float ctrl_2p2z_A2;
    float ctrl_2p2z_K;
    float ctrl_2p2z_x[3];
    float ctrl_2p2z_y[3];
} ctrl_2p2z_t;

void ctrl_2p2z_init(ctrl_2p2z_t *ctrl, float B0, float B1, float B2, float A1, float A2, float K);
uint16_t ctrl_2p2z_update(ctrl_2p2z_t *ctrl, uint16_t input, uint16_t ref);

void app_init(void);



void app_loop(void);

#endif /* INC_APPLICATION_H_ */
