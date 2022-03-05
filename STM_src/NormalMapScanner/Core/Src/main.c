/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the
 *LICENSE file in the root directory of this software component. If no
 *LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes
 * ------------------------------------------------------------------*/
#include "main.h"
#include "dcmi.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"

/* Private includes
 * ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// LCD
#include "LCD_Driver.h"
#include "LCD_GUI.h"
#include "LCD_Touch.h"

// Camera
#include "ov2640.h"

/* USER CODE END Includes */

/* Private typedef
 * -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define
 * ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//---------------          GUI          -------------------------
GUI_Window window;

/// Declared textboxes
enum TextBoxes
{
    StatusText,
    ErrorText,
    StatusValueText,
    ErrorValueText
};

/// Default GUI with informations about camera status and errors
void initGUI(GUI_Window* w)
{
    w->background                   = WHITE;
    w->textboxesSize                = TEXTBOXES_NUMBER;
    GUI_TextBox GUI_TextBox_default = {LCD_X + 10,
                                       LCD_Y + 10,
                                       LCD_X_MAXPIXEL - 10,
                                       LCD_Y_MAXPIXEL - 10,
                                       &Font12,
                                       WHITE,
                                       BLACK,
                                       '\0'};

    w->textboxes[StatusText]      = GUI_TextBox_default;
    w->textboxes[StatusText].xEnd = LCD_X_MAXPIXEL / 4U;
    w->textboxes[StatusText].yEnd = 30U;
    w->textboxes[StatusText].text = "Status:";

    w->textboxes[ErrorText]      = w->textboxes[StatusText];
    w->textboxes[ErrorText].yPos = LCD_Y_MAXPIXEL / 2U;
    w->textboxes[ErrorText].yEnd = w->textboxes[ErrorText].yPos + 20U;
    w->textboxes[ErrorText].text = "Error:";

    w->textboxes[StatusValueText]      = GUI_TextBox_default;
    w->textboxes[StatusValueText].xPos = w->textboxes[StatusText].xEnd + 10;
    w->textboxes[StatusValueText].yPos = w->textboxes[StatusText].yPos;
    w->textboxes[StatusValueText].yEnd = w->textboxes[ErrorText].yPos - 10;
    w->textboxes[StatusValueText].text = "No status info received yet";

    w->textboxes[ErrorValueText]      = GUI_TextBox_default;
    w->textboxes[ErrorValueText].xPos = w->textboxes[ErrorText].xEnd + 10;
    w->textboxes[ErrorValueText].yPos = w->textboxes[ErrorText].yPos;
    w->textboxes[ErrorValueText].xEnd = w->textboxes[StatusValueText].xEnd;
    w->textboxes[ErrorValueText].text = "No error info for now";
}

//-------------------          Camera          -------------------
uint8_t CAMERA_FRAME_BUFFER[1600 * 33];
#define BufferLen (sizeof(CAMERA_FRAME_BUFFER) / sizeof(char))

int32_t firstNonZeroValue(const uint8_t* array, int32_t size)
{
    for (int32_t i = 0; i < size; ++i)
    {
        if (array[i] != 0U)
            return i;
    }
    return -1;
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef* hdcmi)
{
    /* NOTE : This function Should not be modified, when the callback
     is needed, the HAL_DCMI_ErrorCallback could be implemented in
     the user file

     This is the user implementation.
     */

    printf("End of shooting\r\n");
    // HAL_UART_DMAStop(&huart1);FF D8 FF E0
    printf("%x  %x  %x  %x\r\n", CAMERA_FRAME_BUFFER[0], CAMERA_FRAME_BUFFER[1],
           CAMERA_FRAME_BUFFER[2], CAMERA_FRAME_BUFFER[3]);
    if (CAMERA_FRAME_BUFFER[0] == 0)
    {
        CAMERA_FRAME_BUFFER[0] = 0xFF;
        CAMERA_FRAME_BUFFER[1] = 0xD8;
        CAMERA_FRAME_BUFFER[2] = 0xFF;
        CAMERA_FRAME_BUFFER[3] = 0xE0;
    }

    int32_t index = firstNonZeroValue(CAMERA_FRAME_BUFFER, BufferLen);
    if (index != -1)
        printf("Success");

    GUI_DrawImage(LCD_X, LCD_Y, CAMERA_FRAME_BUFFER);
}

void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef* hdcmi)
{
    /* NOTE : This function Should not be modified, when the callback
     is needed, the HAL_DCMI_ErrorCallback could be implemented in
     the user file

     This is the user implementation.
     */
    uint32_t error              = HAL_DCMI_GetError(hdcmi);
    HAL_DCMI_StateTypeDef state = HAL_DCMI_GetState(hdcmi);
    switch (state)
    {
    case HAL_DCMI_STATE_RESET:
    case HAL_DCMI_STATE_READY:
    case HAL_DCMI_STATE_BUSY:
    case HAL_DCMI_STATE_TIMEOUT:
    case HAL_DCMI_STATE_ERROR:
    case HAL_DCMI_STATE_SUSPENDED:
        window.textboxes[StatusValueText].text =
            "Some status stuff right there.";
        window.textboxes[ErrorValueText].text = "Some error stuff right there.";
        GUI_RefreshTextBox(&window.textboxes[StatusValueText]);
        GUI_RefreshTextBox(&window.textboxes[ErrorValueText]);
        break;
    }
}

/* USER CODE END PD */

/* Private macro
 * -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables
 * ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes
 * -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code
 * ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU
     * Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and
     * the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DCMI_Init();
    MX_I2C2_Init();
    MX_SPI2_Init();
    MX_DMA_Init();
    MX_TIM2_Init();
    /* USER CODE BEGIN 2 */

    // Display setup
    LCD_SCAN_DIR Lcd_ScanDir = SCAN_DIR_DFT; // SCAN_DIR_DFT = D2U_L2R
    LCD_Init(Lcd_ScanDir, 1000);
    initGUI(&window);
    GUI_DrawGUI(&window);

    // Touch screen setup
    TP_Init(Lcd_ScanDir);
    TP_GetAdFac();

    // Camera setup
    OV2640_JPEGConfig(JPEG_320x240);
    OV2640_BrightnessConfig(0x20);
    OV2640_AutoExposure(5);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == RESET)
        {
            HAL_Delay(10);
            if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == RESET)
            {
                printf("Start shooting   \r\n");
                HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

                HAL_DCMI_Stop(&hdcmi);
                MX_DCMI_Init();
                __HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);
                HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT,
                                   (uint32_t)&CAMERA_FRAME_BUFFER, 1600 * 8);
            }
            while (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == RESET)
                ;
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        }
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified
     * parameters in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = 8;
    RCC_OscInitStruct.PLL.PLLN            = 180;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ            = 2;
    RCC_OscInitStruct.PLL.PLLR            = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Activate the Over-Drive mode
     */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_1);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error
     * return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line
 * number where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and
       line number, ex: printf("Wrong parameters value: file %s on
       line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
