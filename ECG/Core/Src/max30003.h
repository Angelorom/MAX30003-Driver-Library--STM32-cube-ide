/*
 * max30003.h
 *
 *  Created on: Dec 24, 2024
 *      Author: angelone
 */

#ifndef MAX30003_H
#define MAX30003_H

#include "stm32f4xx_hal.h" // Includere la libreria HAL specifica per STM32

// Definizioni dei registri
#define SW_RST 0x08
#define INFO 0x0F
#define STATUS 0x01
#define FIFO_RST 0x0A
#define CNFG_GEN 0x10
#define CNFG_CAL 0x12
#define CNFG_EMUX 0x14
#define CNFG_ECG 0x15
#define ECG_FIFO 0x20
#define MNGR_INT 0x04
#define EN_INT 0x02

// Funzioni pubbliche
void MAX30003_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_GPIO_Port, uint16_t CS_Pin);
void MAX30003_Reset(void);
void MAX30003_ReadECG(uint8_t *data_buffer);
void MAX30003_WriteRegister(uint8_t reg_address, uint8_t *data);
void MAX30003_ReadRegister(uint8_t reg_address, uint8_t *data);

#endif // MAX30003_H
