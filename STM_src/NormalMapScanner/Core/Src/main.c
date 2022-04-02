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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dcmi.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// LCD
#include "LCD_Driver.h"
#include "LCD_GUI.h"
#include "LCD_Touch.h"

// Camera
#include "ov2640.h"

// Other
#include "debug.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//---------------          GUI          -------------------------

GUI_Window g_window;
/// Declared textboxes
typedef enum
{
    InfoText,
    StatusText,
    StatusValueText,
} TextBoxes;

/// Default GUI with informations about camera status and errors
void initGUI(GUI_Window* w)
{
    w->background              = WHITE;
    GUI_TextBox defaultTextBox = {LCD_X + Font12.Height,
                                  LCD_Y_MAXPIXEL / 5,
                                  LCD_X_MAXPIXEL - Font12.Height,
                                  LCD_Y_MAXPIXEL - Font12.Height,
                                  WHITE,
                                  BLACK,
                                  &Font12,
                                  '\0'};
    uint16_t fontHeight        = defaultTextBox.font->Height;

    // Console
    w->textboxes[InfoText]      = defaultTextBox;
    w->textboxes[InfoText].xEnd = LCD_X_MAXPIXEL / 5;
    w->textboxes[InfoText].yEnd = w->textboxes[InfoText].yPos + fontHeight;
    w->textboxes[InfoText].text = "Console:";

    w->consoles[0].xPos            = w->textboxes[InfoText].xEnd + fontHeight;
    w->consoles[0].yPos            = w->textboxes[InfoText].yPos;
    w->consoles[0].xEnd            = defaultTextBox.xEnd;
    w->consoles[0].yEnd            = w->consoles[0].yPos + fontHeight * 10;
    w->consoles[0].font            = w->textboxes[InfoText].font;
    w->consoles[0].backgroundColor = w->textboxes[InfoText].backgroundColor;
    w->consoles[0].foregroundColor = w->textboxes[InfoText].foregroundColor;

    char* text  = "Console initialized.\n";
    size_t size = strnlen(text, CONSOLE_TEXT_SIZE);
    strncpy(w->consoles->text, text, size);

    // DCMI status
    w->textboxes[StatusText]      = w->textboxes[InfoText];
    w->textboxes[StatusText].yPos = w->consoles[0].yEnd + fontHeight;
    w->textboxes[StatusText].yEnd = w->textboxes[StatusText].yPos + fontHeight;
    w->textboxes[StatusText].text = "DCMI status:";

    w->textboxes[StatusValueText] = w->textboxes[StatusText];
    w->textboxes[StatusValueText].xPos =
        w->textboxes[StatusText].xEnd + fontHeight;
    w->textboxes[StatusValueText].xEnd = defaultTextBox.xEnd;
    w->textboxes[StatusValueText].text = "No status info received yet";
}

//-------------------          Camera          -------------------
uint8_t FRAME_BUFFER[IMAGE_RESOLUTION_WIDTH * IMAGE_RESOLUTION_HEIGHT];
#define FRAME_BUFFER_SIZE (sizeof(FRAME_BUFFER) / sizeof(char))

/// Refreshes DCMI status information on display
void refreshStatusInfo()
{
    switch (HAL_DCMI_GetState(&hdcmi))
    {
        case HAL_DCMI_STATE_RESET:
        {
            g_window.textboxes[StatusValueText].text =
                "DCMI not yet initialized or disabled.";
            break;
        }
        case HAL_DCMI_STATE_READY:
        {
            g_window.textboxes[StatusValueText].text =
                "DCMI initialized and ready for use.";
            break;
        }
        case HAL_DCMI_STATE_BUSY:
        {
            g_window.textboxes[StatusValueText].text =
                "DCMI internal processing is ongoing.";
            break;
        }
        case HAL_DCMI_STATE_TIMEOUT:
            g_window.textboxes[StatusValueText].text = "DCMI timeout state.";
            break;
        case HAL_DCMI_STATE_ERROR:
            g_window.textboxes[StatusValueText].text = "DCMI error state.";
            break;
        case HAL_DCMI_STATE_SUSPENDED:
            g_window.textboxes[StatusValueText].text = "DCMI suspend state.";
            break;
        default:
            g_window.textboxes[StatusValueText].text = "Unknown state.";
    }
    GUI_RefreshTextBox(&g_window.textboxes[StatusValueText]);
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
    printfInfo("%x  %x  %x  %x\r\n", FRAME_BUFFER[0], FRAME_BUFFER[1],
               FRAME_BUFFER[2], FRAME_BUFFER[3]);
    if (FRAME_BUFFER[0] == 0)
    {
        FRAME_BUFFER[0] = 0xFF;
        FRAME_BUFFER[1] = 0xD8;
        FRAME_BUFFER[2] = 0xFF;
        FRAME_BUFFER[3] = 0xE0;
    }

    int32_t index = firstNonZeroValue(FRAME_BUFFER, FRAME_BUFFER_SIZE);
    if (index != -1)
        printInfo("Success\n");

    GUI_DrawImage(LCD_X, LCD_Y, FRAME_BUFFER);
}

void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef* hdcmi)
{
    /* NOTE : This function Should not be modified, when the callback
     is needed, the HAL_DCMI_ErrorCallback could be implemented in
     the user file

     This is the user implementation.
     */
    char* text = "Unknown error";
    switch (HAL_DCMI_GetError(hdcmi))
    {
        case HAL_DCMI_ERROR_NONE:
            text = "No error";
            break;
        case HAL_DCMI_ERROR_OVR:
            text = "Overrun error";
            break;
        case HAL_DCMI_ERROR_SYNC:
            text = "Synchronization error";
            break;
        case HAL_DCMI_ERROR_TIMEOUT:
            text = "Timeout error";
            break;
        case HAL_DCMI_ERROR_DMA:
            text = "DMA error";
        default:
            break;
    }

    printfInfo("%s.\n", text);
    refreshStatusInfo();
}

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
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

    /* Reset of all peripherals, Initializes the Flash interface and the
     * Systick. */
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
    MX_TIM1_Init();
    MX_USART3_UART_Init();
    /* USER CODE BEGIN 2 */

    // Display setup
    LCD_SCAN_DIR Lcd_ScanDir = SCAN_DIR_DFT; // SCAN_DIR_DFT = D2U_L2R
    LCD_Init(Lcd_ScanDir, 1000);
    initGUI(&g_window);
    GUI_DrawGUI(&g_window);

    // Touch screen setup
    TP_Init(Lcd_ScanDir);
    TP_GetAdFac();

    // Camera setup
    ov2640Init();
    HAL_Delay(100);
    // 	ov2640_Config(CAMERA_I2C_ADDRESS, CAMERA_CONTRAST_BRIGHTNESS,
    // CAMERA_CONTRAST_LEVEL2, CAMERA_BRIGHTNESS_LEVEL4);
    HAL_Delay(100);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    printInfo("Initialization done.\n");
    printInfo("Push Blue Button to make a picture.\n");
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin) == SET)
        {
            int32_t index = firstNonZeroValue(FRAME_BUFFER, FRAME_BUFFER_SIZE);
            if (index != -1)
                printInfo("Success\n");
            if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin) == SET)
            {
                printInfo("Button pushed.\n");
                refreshStatusInfo();
                HAL_Delay(1000);
                HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

                HAL_DCMI_Stop(&hdcmi);
                MX_DCMI_Init();
                __HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);
                HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT,
                                   (uint32_t)&FRAME_BUFFER,
                                   FRAME_BUFFER_SIZE); // Buffer size
                printInfo("DCMI DMA started.\n");
            }
            while (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin) == SET)
                HAL_Delay(10);
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
            printInfo("Button released, push again to make another shot.\n");
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

    /** Configure LSE Drive Capability
     */
    HAL_PWR_EnableBkUpAccess();

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 4;
    RCC_OscInitStruct.PLL.PLLN       = 96;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 4;
    RCC_OscInitStruct.PLL.PLLR       = 2;
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
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
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
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
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
