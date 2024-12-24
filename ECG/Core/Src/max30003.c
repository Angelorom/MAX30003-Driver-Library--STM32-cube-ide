/*
 * max30003.c
 *
 *  Created on: Dec 24, 2024
 *      Author: angelone
 */

#include "max30003.h"
#include <string.h> // Per memcpy

// Variabili statiche per gestire l'SPI e il pin CS
static SPI_HandleTypeDef *MAX30003_SPI;
static GPIO_TypeDef *MAX30003_CS_GPIO_Port;
static uint16_t MAX30003_CS_Pin;

static void SendAndReceive(uint8_t mode, uint8_t *TX_Buffer, uint8_t *RX_Buffer);

/**
 * @brief Inizializza il MAX30003 con la configurazione di base.
 * @param hspi: Handle SPI utilizzato per comunicare con il MAX30003.
 * @param CS_GPIO_Port: Porta GPIO per il Chip Select.
 * @param CS_Pin: Pin GPIO per il Chip Select.
 */
void MAX30003_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_GPIO_Port, uint16_t CS_Pin) {
    MAX30003_SPI = hspi;
    MAX30003_CS_GPIO_Port = CS_GPIO_Port;
    MAX30003_CS_Pin = CS_Pin;

    uint8_t RX_Buffer[4] = {0};
    uint8_t init_sequences[][4] = {
        {SW_RST, 0x00, 0x00, 0x00},   // Software reset
        {CNFG_GEN, 0x08, 0x10, 0x07}, // General configuration
        {CNFG_CAL, 0x70, 0x00, 0x00}, // Calibration configuration
        {CNFG_EMUX, 0x0B, 0x00, 0x00}, // EMUX configuration
        {CNFG_ECG, 0x00, 0x50, 0x00}, // ECG configuration
        {EN_INT, 0x80, 0x00, 0x01},   // Enable interrupt
        {MNGR_INT, 0x78, 0x00, 0x04}, // Interrupt manager
        {FIFO_RST, 0x00, 0x00, 0x00}, // FIFO reset
        {0x09, 0x00, 0x00, 0x00}      // Sync reset
    };

    for (int i = 0; i < sizeof(init_sequences) / sizeof(init_sequences[0]); i++) {
        SendAndReceive(2, init_sequences[i], RX_Buffer);
    }
}

/**
 * @brief Esegue un reset del MAX30003.
 */
void MAX30003_Reset(void) {
    uint8_t cmd[] = {SW_RST, 0x00, 0x00, 0x00};
    uint8_t RX_Buffer[4] = {0};
    SendAndReceive(2, cmd, RX_Buffer);
}

/**
 * @brief Legge i dati ECG dal FIFO.
 * @param data_buffer: Buffer per memorizzare i dati ECG letti.
 */
void MAX30003_ReadECG(uint8_t *data_buffer) {
    uint8_t TX_Buffer[49] = {ECG_FIFO};
    SendAndReceive(4, TX_Buffer, data_buffer);
}

/**
 * @brief Scrive un valore in un registro del MAX30003.
 * @param reg_address: Indirizzo del registro.
 * @param data: Dati da scrivere (3 byte).
 */
void MAX30003_WriteRegister(uint8_t reg_address, uint8_t *data) {
    uint8_t TX_Buffer[4] = {reg_address, data[0], data[1], data[2]};
    uint8_t RX_Buffer[4] = {0};
    SendAndReceive(2, TX_Buffer, RX_Buffer);
}

/**
 * @brief Legge un valore da un registro del MAX30003.
 * @param reg_address: Indirizzo del registro.
 * @param data: Buffer per memorizzare i dati letti (3 byte).
 */
void MAX30003_ReadRegister(uint8_t reg_address, uint8_t *data) {
    uint8_t TX_Buffer[4] = {reg_address, 0x00, 0x00, 0x00};
    uint8_t RX_Buffer[4] = {0};
    SendAndReceive(1, TX_Buffer, RX_Buffer);
    memcpy(data, &RX_Buffer[1], 3); // I dati utili sono nei byte 1-3
}

/**
 * @brief Sends and receives data over SPI based on the specified mode.
 * @param mode: Operation mode (0 = Write & Read, 1 = Read, 2 = Write, 4 = Burst Read from FIFO buffer)
 * @param TX_Buffer: Transmit buffer
 * @param RX_Buffer: Receive buffer
 */
static void SendAndReceive(uint8_t mode, uint8_t *TX_Buffer, uint8_t *RX_Buffer) {
    uint8_t TX_Buffer_Cplt[49] = {0};

    // Imposta il primo byte in base al mode
    if (mode == 0 || mode == 2) {
        TX_Buffer[0] = (TX_Buffer[0] << 1) & 0xFE;  // Write
    } else {
        TX_Buffer[0] = (TX_Buffer[0] << 1) | 0x01;  // Read
    }

    // Copia i dati in TX_Buffer_Cplt
    memcpy(TX_Buffer_Cplt, TX_Buffer, (mode == 4) ? 49 : 4);

    // Chip Select Low
    HAL_GPIO_WritePin(MAX30003_CS_GPIO_Port, MAX30003_CS_Pin, GPIO_PIN_RESET);

    // Transazione SPI
    if (mode == 4) {
        HAL_SPI_TransmitReceive(MAX30003_SPI, TX_Buffer_Cplt, RX_Buffer, 49, 1000);
    } else if (mode == 2) {
        HAL_SPI_Transmit(MAX30003_SPI, TX_Buffer_Cplt, 4, 1000);
    } else {
        HAL_SPI_TransmitReceive(MAX30003_SPI, TX_Buffer_Cplt, RX_Buffer, 4, 1000);
    }

    // Chip Select High
    HAL_GPIO_WritePin(MAX30003_CS_GPIO_Port, MAX30003_CS_Pin, GPIO_PIN_SET);
}

