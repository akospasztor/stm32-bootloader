#ifndef __MAIN_H
#define __MAIN_H

/*** Application-Specific Configuration ***************************************/
/* File name of application located on SD card */
#define CONF_FILENAME "app-demo.bin"
/* For development/debugging: print messages to ST-LINK VCP */
#define USE_VCP 1
/******************************************************************************/

/* Hardware Defines ----------------------------------------------------------*/
/* Joystick center button */
#define BTN_Port GPIOC
#define BTN_Pin  GPIO_PIN_13

/* LD2 */
#define LED_G1_Port GPIOB
#define LED_G1_Pin  GPIO_PIN_13

/* LD3 */
#define LED_G2_Port GPIOA
#define LED_G2_Pin  GPIO_PIN_5

/* Enumerations --------------------------------------------------------------*/
/* Error codes */
enum eApplicationErrorCodes
{
    ERR_OK = 0,
    ERR_WRP_ACTIVE,
    ERR_SD_INIT,
    ERR_SD_MOUNT,
    ERR_SD_FILE,
    ERR_APP_LARGE,
    ERR_FLASH,
    ERR_VERIFY,
    ERR_OBP,
};

/* Hardware Macros -----------------------------------------------------------*/
#define LED_G1_ON()  HAL_GPIO_WritePin(LED_G1_Port, LED_G1_Pin, GPIO_PIN_RESET)
#define LED_G1_OFF() HAL_GPIO_WritePin(LED_G1_Port, LED_G1_Pin, GPIO_PIN_SET)
#define LED_G1_TG()  HAL_GPIO_TogglePin(LED_G1_Port, LED_G1_Pin)
#define LED_G2_ON()  HAL_GPIO_WritePin(LED_G2_Port, LED_G2_Pin, GPIO_PIN_SET)
#define LED_G2_OFF() HAL_GPIO_WritePin(LED_G2_Port, LED_G2_Pin, GPIO_PIN_RESET)
#define LED_G2_TG()  HAL_GPIO_TogglePin(LED_G2_Port, LED_G2_Pin)

#define LED_ALL_ON() \
    do               \
    {                \
        LED_G1_ON(); \
        LED_G2_ON(); \
    } while(0)
#define LED_ALL_OFF() \
    do                \
    {                 \
        LED_G1_OFF(); \
        LED_G2_OFF(); \
    } while(0)
#define LED_ALL_TG() \
    do               \
    {                \
        LED_G1_TG(); \
        LED_G2_TG(); \
    } while(0)

#define IS_BTN_PRESSED() \
    ((HAL_GPIO_ReadPin(BTN_Port, BTN_Pin) == GPIO_PIN_SET) ? 1 : 0)

#endif /* __MAIN_H */
