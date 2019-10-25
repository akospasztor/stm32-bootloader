#ifndef __MAIN_H
#define __MAIN_H

/*** Application-Specific Configuration ***************************************/
#define USE_SWO_TRACE       1               /* For development/debugging: stdout/stderr via SWO trace */
/******************************************************************************/

/* Hardware Defines ----------------------------------------------------------*/
#define LED_G1_Port         GPIOA
#define LED_G1_Pin          GPIO_PIN_5
#define LED_G2_Port         GPIOB
#define LED_G2_Pin          GPIO_PIN_13

/* Hardware Macros -----------------------------------------------------------*/
#define LED_G1_ON()          HAL_GPIO_WritePin(LED_G1_Port, LED_G1_Pin, GPIO_PIN_SET)
#define LED_G1_OFF()         HAL_GPIO_WritePin(LED_G1_Port, LED_G1_Pin, GPIO_PIN_RESET)
#define LED_G1_TG()          HAL_GPIO_TogglePin(LED_G1_Port, LED_G1_Pin)
#define LED_G2_ON()          HAL_GPIO_WritePin(LED_G2_Port, LED_G2_Pin, GPIO_PIN_RESET)
#define LED_G2_OFF()         HAL_GPIO_WritePin(LED_G2_Port, LED_G2_Pin, GPIO_PIN_SET)
#define LED_G2_TG()          HAL_GPIO_TogglePin(LED_G2_Port, LED_G2_Pin)

#define LED_ALL_ON()        do { LED_G1_ON(); LED_G2_ON(); } while(0)
#define LED_ALL_OFF()       do { LED_G1_OFF(); LED_G2_OFF(); } while(0)


#endif /* __MAIN_H */
