#ifndef __MAIN_H
#define __MAIN_H

/* Hardware Defines ----------------------------------------------------------*/
#define LED_G_Port          GPIOE
#define LED_G_Pin           GPIO_PIN_2
#define LED_Y_Port          GPIOE
#define LED_Y_Pin           GPIO_PIN_3
#define LED_R_Port          GPIOE
#define LED_R_Pin           GPIO_PIN_4

#define BTN_Port            GPIOD
#define BTN_Pin             GPIO_PIN_8
#define REED_Port           GPIOD
#define REED_Pin            GPIO_PIN_11

/* Hardware Macros -----------------------------------------------------------*/
#define LED_G_ON()          HAL_GPIO_WritePin(LED_G_Port, LED_G_Pin, GPIO_PIN_SET);
#define LED_G_OFF()         HAL_GPIO_WritePin(LED_G_Port, LED_G_Pin, GPIO_PIN_RESET);
#define LED_G_TG();         HAL_GPIO_TogglePin(LED_G_Port, LED_G_Pin);
#define LED_Y_ON()          HAL_GPIO_WritePin(LED_Y_Port, LED_Y_Pin, GPIO_PIN_SET);
#define LED_Y_OFF()         HAL_GPIO_WritePin(LED_Y_Port, LED_Y_Pin, GPIO_PIN_RESET);
#define LED_Y_TG();         HAL_GPIO_TogglePin(LED_Y_Port, LED_Y_Pin);
#define LED_R_ON()          HAL_GPIO_WritePin(LED_R_Port, LED_R_Pin, GPIO_PIN_SET);
#define LED_R_OFF()         HAL_GPIO_WritePin(LED_R_Port, LED_R_Pin, GPIO_PIN_RESET);
#define LED_R_TG();         HAL_GPIO_TogglePin(LED_R_Port, LED_R_Pin);

#define IS_BTN_PRESSED()    ((HAL_GPIO_ReadPin(BTN_Port, BTN_Pin) == GPIO_PIN_RESET) ? 1 : 0)

#endif /* __MAIN_H */