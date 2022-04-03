/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    dcmi.c
 * @brief   This file provides code for the configuration
 *          of the DCMI instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found
 *in the LICENSE file in the root directory of this software
 *component. If no LICENSE file comes with this software, it
 *is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "dcmi.h"

/* USER CODE BEGIN 0 */
/*
#define DCMI_DATn 8
typedef enum
{
    DCMI_DAT0 = 0,
    DCMI_DAT1 = 1,
    DCMI_DAT2 = 2,
    DCMI_DAT3 = 3,
    DCMI_DAT4 = 4,
    DCMI_DAT5 = 5,
    DCMI_DAT6 = 6,
    DCMI_DAT7 = 7
} DCMI_DAT_TypeDef;

// Data line 0
#define DCMI_DAT0_PORT GPIOC
#define DCMI_DAT0_CLK RCC_AHB1Periph_GPIOC
#define DCMI_DAT0_PIN GPIO_PIN_6
#define DCMI_DAT0_PIN_SOURCE GPIO_PinSource6

// Data line 1
#define DCMI_DAT1_PORT GPIOC
#define DCMI_DAT1_CLK RCC_AHB1Periph_GPIOC
#define DCMI_DAT1_PIN GPIO_PIN_7
#define DCMI_DAT1_PIN_SOURCE GPIO_PinSource7

// Data line 2
#define DCMI_DAT2_PORT GPIOC
#define DCMI_DAT2_CLK RCC_AHB1Periph_GPIOC
#define DCMI_DAT2_PIN GPIO_PIN_8
#define DCMI_DAT2_PIN_SOURCE GPIO_PinSource8

// Data line 3
#define DCMI_DAT3_PORT GPIOC
#define DCMI_DAT3_CLK RCC_AHB1Periph_GPIOC
#define DCMI_DAT3_PIN GPIO_PIN_9
#define DCMI_DAT3_PIN_SOURCE GPIO_PinSource9

// Data line 4
#define DCMI_DAT4_PORT GPIOC
#define DCMI_DAT4_CLK RCC_AHB1Periph_GPIOC
#define DCMI_DAT4_PIN GPIO_PIN_11
#define DCMI_DAT4_PIN_SOURCE GPIO_PinSource11

// Data line 5
#define DCMI_DAT5_PORT GPIOB
#define DCMI_DAT5_CLK RCC_AHB1Periph_GPIOB
#define DCMI_DAT5_PIN GPIO_PIN_6
#define DCMI_DAT5_PIN_SOURCE GPIO_PinSource6

// Data line 6
#define DCMI_DAT6_PORT GPIOB
#define DCMI_DAT6_CLK RCC_AHB1Periph_GPIOB
#define DCMI_DAT6_PIN GPIO_PIN_8
#define DCMI_DAT6_PIN_SOURCE GPIO_PinSource8

// Data line 7
#define DCMI_DAT7_PORT GPIOB
#define DCMI_DAT7_CLK RCC_AHB1Periph_GPIOB
#define DCMI_DAT7_PIN GPIO_PIN_9
#define DCMI_DAT7_PIN_SOURCE GPIO_PinSource9

// Data clocks
const uint32_t GPIO_DCMI_DAT_CLK[DCMI_DATn] = {
    DCMI_DAT0_CLK, DCMI_DAT1_CLK, DCMI_DAT2_CLK, DCMI_DAT3_CLK,
    DCMI_DAT4_CLK, DCMI_DAT5_CLK, DCMI_DAT6_CLK, DCMI_DAT7_CLK};

// Data pins
const uint16_t GPIO_DCMI_DAT_PIN[DCMI_DATn] = {
    DCMI_DAT0_PIN, DCMI_DAT1_PIN, DCMI_DAT2_PIN, DCMI_DAT3_PIN,
    DCMI_DAT4_PIN, DCMI_DAT5_PIN, DCMI_DAT6_PIN, DCMI_DAT7_PIN};

// Data ports
GPIO_TypeDef* GPIO_DCMI_DAT_PORT[DCMI_DATn] = {
    DCMI_DAT0_PORT, DCMI_DAT1_PORT, DCMI_DAT2_PORT, DCMI_DAT3_PORT,
    DCMI_DAT4_PORT, DCMI_DAT5_PORT, DCMI_DAT6_PORT, DCMI_DAT7_PORT};

// Line clocks
const uint32_t GPIO_DCMI_LINE_CLK[DCMI_IOn] = {
    DCMI_HSYNC_CLK, DCMI_VSYNC_CLK, DCMI_PCLK_CLK};

// Line pins
const uint16_t GPIO_DCMI_LINE_PIN[DCMI_IOn] = {
    DCMI_HSYNC_PIN, DCMI_VSYNC_PIN, DCMI_PCLK_PIN};

// Line ports
GPIO_TypeDef* GPIO_DCMI_LINE_PORT[DCMI_IOn] = {
    DCMI_HSYNC_PORT, DCMI_VSYNC_PORT, DCMI_PCLK_PORT};*/

/* USER CODE END 0 */

DCMI_HandleTypeDef hdcmi;
DMA_HandleTypeDef hdma_dcmi;

/* DCMI init function */
void MX_DCMI_Init(void)
{

  /* USER CODE BEGIN DCMI_Init 0 */

  /* USER CODE END DCMI_Init 0 */

  /* USER CODE BEGIN DCMI_Init 1 */

  /* USER CODE END DCMI_Init 1 */
  hdcmi.Instance = DCMI;
  hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
  hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
  hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
  hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
  hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
  hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hdcmi.Init.JPEGMode = DCMI_JPEG_ENABLE;
  hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
  hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
  hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
  hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
  if (HAL_DCMI_Init(&hdcmi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DCMI_Init 2 */

  /* USER CODE END DCMI_Init 2 */

}

void HAL_DCMI_MspInit(DCMI_HandleTypeDef* dcmiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(dcmiHandle->Instance==DCMI)
  {
  /* USER CODE BEGIN DCMI_MspInit 0 */

  /* USER CODE END DCMI_MspInit 0 */
    /* DCMI clock enable */
    __HAL_RCC_DCMI_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**DCMI GPIO Configuration
    PE4     ------> DCMI_D4
    PE5     ------> DCMI_D6
    PE6     ------> DCMI_D7
    PA4     ------> DCMI_HSYNC
    PA6     ------> DCMI_PIXCLK
    PC6     ------> DCMI_D0
    PC7     ------> DCMI_D1
    PC8     ------> DCMI_D2
    PC9     ------> DCMI_D3
    PD3     ------> DCMI_D5
    PG9     ------> DCMI_VSYNC
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* DCMI DMA Init */
    /* DCMI Init */
    hdma_dcmi.Instance = DMA2_Stream1;
    hdma_dcmi.Init.Channel = DMA_CHANNEL_1;
    hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dcmi.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_dcmi.Init.Mode = DMA_CIRCULAR;
    hdma_dcmi.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_dcmi.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_dcmi.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_dcmi.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_dcmi) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(dcmiHandle,DMA_Handle,hdma_dcmi);

    /* DCMI interrupt Init */
    HAL_NVIC_SetPriority(DCMI_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DCMI_IRQn);
  /* USER CODE BEGIN DCMI_MspInit 1 */

  /* USER CODE END DCMI_MspInit 1 */
  }
}

void HAL_DCMI_MspDeInit(DCMI_HandleTypeDef* dcmiHandle)
{

  if(dcmiHandle->Instance==DCMI)
  {
  /* USER CODE BEGIN DCMI_MspDeInit 0 */

  /* USER CODE END DCMI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DCMI_CLK_DISABLE();

    /**DCMI GPIO Configuration
    PE4     ------> DCMI_D4
    PE5     ------> DCMI_D6
    PE6     ------> DCMI_D7
    PA4     ------> DCMI_HSYNC
    PA6     ------> DCMI_PIXCLK
    PC6     ------> DCMI_D0
    PC7     ------> DCMI_D1
    PC8     ------> DCMI_D2
    PC9     ------> DCMI_D3
    PD3     ------> DCMI_D5
    PG9     ------> DCMI_VSYNC
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_9);

    /* DCMI DMA DeInit */
    HAL_DMA_DeInit(dcmiHandle->DMA_Handle);

    /* DCMI interrupt Deinit */
    HAL_NVIC_DisableIRQ(DCMI_IRQn);
  /* USER CODE BEGIN DCMI_MspDeInit 1 */

  /* USER CODE END DCMI_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
