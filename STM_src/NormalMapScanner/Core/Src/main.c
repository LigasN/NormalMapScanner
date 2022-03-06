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
typedef enum
{
    infoText,
    infoValueText,
    StatusText,
    StatusValueText,
    ErrorText,
    ErrorValueText
} TextBoxes;

/// Default GUI with informations about camera status and errors
void initGUI(GUI_Window* w)
{
    w->background              = WHITE;
    w->textboxesSize           = TEXTBOXES_NUMBER;
    GUI_TextBox defaultTextBox = {LCD_X + 10,
                                  LCD_Y + 10,
                                  LCD_X_MAXPIXEL - 10,
                                  LCD_Y_MAXPIXEL - 10,
                                  &Font12,
                                  WHITE,
                                  BLACK,
                                  '\0'};

    w->textboxes[infoText]      = defaultTextBox;
    w->textboxes[infoText].xEnd = LCD_X_MAXPIXEL / 5U;
    w->textboxes[infoText].yEnd =
        w->textboxes[infoText].yPos + w->textboxes[infoText].font->Height;
    w->textboxes[infoText].text = "Information:";

    w->textboxes[StatusText]      = w->textboxes[infoText];
    w->textboxes[StatusText].yPos = LCD_Y_MAXPIXEL / 3U;
    w->textboxes[StatusText].yEnd =
        w->textboxes[StatusText].yPos + w->textboxes[StatusText].font->Height;
    w->textboxes[StatusText].text = "DCMI status:";

    w->textboxes[ErrorText] = w->textboxes[StatusText];
    w->textboxes[ErrorText].yPos *= 2U;
    w->textboxes[ErrorText].yEnd =
        w->textboxes[ErrorText].yPos + w->textboxes[ErrorText].font->Height;
    w->textboxes[ErrorText].text = "DCMI error:";

    w->textboxes[infoValueText]      = w->textboxes[infoText];
    w->textboxes[infoValueText].xPos = w->textboxes[infoText].xEnd + 10;
    w->textboxes[infoValueText].xEnd = defaultTextBox.xEnd;
    w->textboxes[infoValueText].yEnd = w->textboxes[StatusText].yPos - 10;
    w->textboxes[infoValueText].text = "No log for now";

    w->textboxes[StatusValueText]      = w->textboxes[infoValueText];
    w->textboxes[StatusValueText].xPos = w->textboxes[StatusText].xEnd + 10;
    w->textboxes[StatusValueText].yPos = w->textboxes[StatusText].yPos;
    w->textboxes[StatusValueText].yEnd = w->textboxes[ErrorText].yPos - 10;
    w->textboxes[StatusValueText].text = "No status info received yet";

    w->textboxes[ErrorValueText]      = w->textboxes[infoValueText];
    w->textboxes[ErrorValueText].xPos = w->textboxes[ErrorText].xEnd + 10;
    w->textboxes[ErrorValueText].yPos = w->textboxes[ErrorText].yPos;
    w->textboxes[ErrorValueText].yEnd = defaultTextBox.yEnd;
    w->textboxes[ErrorValueText].text = "No error info for now";
}

/// Macros on the library methods to spare some code
#define printInfo(info) printOnTextBox(&window.textboxes[infoValueText], info)
#define printfInfo(format, ...)                                                \
    printfOnTextBox(&window.textboxes[infoValueText], format, __VA_ARGS__)

//-------------------          Camera          -------------------
uint8_t CAMERA_FRAME_BUFFER[1600 * 33];
#define BufferLen (sizeof(CAMERA_FRAME_BUFFER) / sizeof(char))

/// Refreshes DCMI status information on display
void refreshStatusInfo()
{
    switch (HAL_DCMI_GetState(&hdcmi))
    {
    case HAL_DCMI_STATE_RESET:
        window.textboxes[StatusValueText].text =
            "DCMI not yet initialized or disabled.";
        break;
    case HAL_DCMI_STATE_READY:
        window.textboxes[StatusValueText].text =
            "DCMI initialized and ready for use.";
        break;
    case HAL_DCMI_STATE_BUSY:
        window.textboxes[StatusValueText].text =
            "DCMI internal processing is ongoing.";
        break;
    case HAL_DCMI_STATE_TIMEOUT:
        window.textboxes[StatusValueText].text = "DCMI timeout state.";
        break;
    case HAL_DCMI_STATE_ERROR:
        window.textboxes[StatusValueText].text = "DCMI error state.";
        break;
    case HAL_DCMI_STATE_SUSPENDED:
        window.textboxes[StatusValueText].text = "DCMI suspend state.";
        break;
    default:
        window.textboxes[StatusValueText].text = "Unknown state.";
    }
    GUI_RefreshTextBox(&window.textboxes[StatusValueText]);
}

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

    printInfo("End of shooting\r\n");
    // HAL_UART_DMAStop(&huart1);FF D8 FF E0
    printfInfo("%x  %x  %x  %x\r\n", CAMERA_FRAME_BUFFER[0],
               CAMERA_FRAME_BUFFER[1], CAMERA_FRAME_BUFFER[2],
               CAMERA_FRAME_BUFFER[3]);
    if (CAMERA_FRAME_BUFFER[0] == 0)
    {
        CAMERA_FRAME_BUFFER[0] = 0xFF;
        CAMERA_FRAME_BUFFER[1] = 0xD8;
        CAMERA_FRAME_BUFFER[2] = 0xFF;
        CAMERA_FRAME_BUFFER[3] = 0xE0;
    }

    int32_t index = firstNonZeroValue(CAMERA_FRAME_BUFFER, BufferLen);
    if (index != -1)
        printInfo("Success");

    GUI_DrawImage(LCD_X, LCD_Y, CAMERA_FRAME_BUFFER);
}

void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef* hdcmi)
{
    /* NOTE : This function Should not be modified, when the callback
     is needed, the HAL_DCMI_ErrorCallback could be implemented in
     the user file

     This is the user implementation.
     */

    switch (HAL_DCMI_GetError(hdcmi))
    {
    case HAL_DCMI_ERROR_NONE:
        window.textboxes[ErrorValueText].text = "No error";
        break;
    case HAL_DCMI_ERROR_OVR:
        window.textboxes[ErrorValueText].text = "Overrun error";
        break;
    case HAL_DCMI_ERROR_SYNC:
        window.textboxes[ErrorValueText].text = "Synchronization error";
        break;
    case HAL_DCMI_ERROR_TIMEOUT:
        window.textboxes[ErrorValueText].text = "Timeout error";
        break;
    case HAL_DCMI_ERROR_DMA:
        window.textboxes[ErrorValueText].text = "DMA error";
        break;
    default:
        window.textboxes[ErrorValueText].text = "Unknown error";
    }

    GUI_RefreshTextBox(&window.textboxes[ErrorValueText]);
    refreshStatusInfo();
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
    printInfo("Initialization done.");
    printInfo("Push Blue Button to make a picture.");
    printfInfo("Make me count to %d. %d! %d! %d!...", 3, 3, 2, 1);
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == RESET)
        {
            HAL_Delay(10);
            if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == RESET)
            {
                printInfo("Button pushed");
                refreshStatusInfo();
                HAL_Delay(1000);
                HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

                HAL_DCMI_Stop(&hdcmi);
                MX_DCMI_Init();
                __HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);
                HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT,
                                   (uint32_t)&CAMERA_FRAME_BUFFER, 1600 * 8);
                printInfo("DCMI DMA started");
            }
            while (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == RESET)
                ;
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
            printInfo("Button released, push again to make another shot.");
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
