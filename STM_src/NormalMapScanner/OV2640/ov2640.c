/*
 * ov2640.c
 *
 *  Copied from previous author on: 19. 03. 2022
 *      Previous author: hjh
 *      Current author: https://github.com/LigasN
 */
/*
 * my_ov2640.c
 *
 *  Created on: 2. okt. 2020
 *      Author: hjh
 */
/*
 ******************************************************************************
 * @file    ov2640.c
 * @author  MCD Application Team
 * @version V1.0.2
 * @date    02-December-2014
 * @brief   This file provides the OV2640 camera driver
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *notice, this list of conditions and the following disclaimer in the
 *documentation and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "ov2640.h"

static uint32_t ov2640ConvertValue(uint32_t feature, uint32_t value);

/* Initialization sequence */
const unsigned char OV2640_myinit[][2] = {
    {0xff, 0x00}, /* Device control register list Table 12 */
    {0x2c, 0xff}, /* Reserved                              */
    {0x2e, 0xdf}, /* Reserved                              */
    {0xff, 0x01}, /* Device control register list Table 13 */
    {0x3c, 0x32}, /* Reserved                              */
    {0x11, 0x00}, /* Clock Rate Control                    */
    {0x09, 0x02}, /* Common control 2                      */
    {0x04, 0xA8}, /* Mirror                                */
    {0x13, 0xe5}, /* Common control 8                      */
    {0x14, 0x48}, /* Common control 9                      */
    {0x2c, 0x0c}, /* Reserved                              */
    {0x33, 0x78}, /* Reserved                              */
    {0x3a, 0x33}, /* Reserved                              */
    {0x3b, 0xfB}, /* Reserved                              */
    {0x3e, 0x00}, /* Reserved                              */
    {0x43, 0x11}, /* Reserved                              */
    {0x16, 0x10}, /* Reserved                              */
    {0x39, 0x02}, /* Reserved                              */
    {0x35, 0x88}, /* Reserved                              */
    {0x22, 0x0A}, /* Reserved                              */
    {0x37, 0x40}, /* Reserved                              */
    {0x23, 0x00}, /* Reserved                              */
    {0x34, 0xa0}, /* Reserved                              */
    {0x06, 0x02}, /* Reserved                              */
    {0x07, 0xc0}, /* Reserved                              */
    {0x0d, 0xb7}, /* COM4 Data out pin hold at last state  */
    {0x0e, 0x01}, /* Reserved                              */
    {0x4c, 0x00}, /* Reserved                              */
    {0x4a, 0x81}, /* Reserved                              */
    {0x21, 0x99}, /* Reserved                              */
    {0x24, 0x40}, /* Luminance signal High range           */
    {0x25, 0x38}, /* Luminance signal low range            */
    {0x26, 0x82}, /* Fast Mode Large Step Range Threshold  */
    {0x5c, 0x00}, /* Reserved                              */
    {0x63, 0x00}, /* Reserved                              */
    {0x46, 0x22}, /* Frame length adjustment               */
    {0x0c, 0x3A}, /* Common control 3                      */
    {0x5d, 0x55}, /* 16-zone average weight option         */
    {0x5e, 0x7d}, /* 16-zone average weight option         */
    {0x5f, 0x7d}, /* 16-zone average weight option         */
    {0x60, 0x55}, /* 16-zone average weight option         */
    {0x61, 0x70}, /* Histogram algo low level              */
    {0x62, 0x80}, /* Histogram algo high level             */
    {0x7c, 0x05}, /* Reserved                              */
    {0x20, 0x80}, /* Reserved                              */
    {0x28, 0x30}, /* Reserved                              */
    {0x6c, 0x00}, /* Reserved                              */
    {0x6d, 0x80}, /* Reserved                              */
    {0x6e, 0x00}, /* Reserved                              */
    {0x70, 0x02}, /* Reserved                              */
    {0x71, 0x94}, /* Reserved                              */
    {0x73, 0xc1}, /* Reserved                              */
    {0x3d, 0x34}, /* Reserved                              */
    {0x12, 0x04}, /* Common control 7                      */
    {0x5a, 0x57}, /* Reserved                              */
    {0x4f, 0xbb}, {0x50, 0x9c}, {0xff, 0x0},
    {0xe5, 0x7f}, {0xf9, 0xc0}, {0x41, 0x24},
    {0xe0, 0x14}, {0x76, 0xff}, {0x33, 0xa0},
    {0x42, 0x20}, {0x43, 0x18}, {0x4c, 0x0},
    {0x87, 0xd0}, {0x88, 0x3f}, {0xd7, 0x3},
    {0xd9, 0x10}, {0xd3, 0x82}, {0xc8, 0x8},
    {0xc9, 0x80}, {0x7c, 0x0},  {0x7d, 0x0},
    {0x7c, 0x3},  {0x7d, 0x48}, {0x7d, 0x48},
    {0x7c, 0x8},  {0x7d, 0x20}, {0x7d, 0x10},
    {0x7d, 0xe},  {0x90, 0x00}, {0x91, 0x0e},
    {0x91, 0x1a}, {0x91, 0x31}, {0x91, 0x5a},
    {0x91, 0x69}, {0x91, 0x75}, {0x91, 0x7e},
    {0x91, 0x88}, {0x91, 0x8f}, {0x91, 0x96},
    {0x91, 0xa3}, {0x91, 0xaf}, {0x91, 0xc4},
    {0x91, 0xd7}, {0x91, 0xe8}, {0x91, 0x20},
    {0x92, 0x00}, {0x93, 0x06}, {0x93, 0xe3},
    {0x93, 0x05}, {0x93, 0x05}, {0x93, 0x00},
    {0x93, 0x04}, {0x93, 0x00}, {0x93, 0x00},
    {0x93, 0x00}, {0x93, 0x00}, {0x93, 0x00},
    {0x93, 0x00}, {0x93, 0x00}, {0x96, 0x00},
    {0x97, 0x08}, {0x97, 0x19}, {0x97, 0x02},
    {0x97, 0x0c}, {0x97, 0x24}, {0x97, 0x30},
    {0x97, 0x28}, {0x97, 0x26}, {0x97, 0x02},
    {0x97, 0x98}, {0x97, 0x80}, {0x97, 0x00},
    {0x97, 0x00}, {0xc3, 0xed}, {0xa4, 0x00},
    {0xa8, 0x00}, {0xc5, 0x11}, {0xc6, 0x51},
    {0xbf, 0x80}, {0xc7, 0x10}, {0xb6, 0x66},
    {0xb8, 0xA5}, {0xb7, 0x64}, {0xb9, 0x7C},
    {0xb3, 0xaf}, {0xb4, 0x97}, {0xb5, 0xFF},
    {0xb0, 0xC5}, {0xb1, 0x94}, {0xb2, 0x0f},
    {0xc4, 0x5c}, {0xc0, 0xc8}, // hsize  200 // Arducam has 0x64
    {0xc1, 0x96},               // vsice  150 // Arducam has 0x4B
    {0x8c, 0x00}, {0x86, 0x3d}, {0x50, 0x92},
    {0x51, 0x90}, {0x52, 0x2c}, {0x53, 0x00},
    {0x54, 0x00}, {0x55, 0x88}, {0x5a, 0x50},
    {0x5b, 0x3c}, {0x5c, 0x00}, {0xd3, 0x00}, //{ 0xd3, 0x7f },
    {0xc3, 0xed}, {0x7f, 0x00}, {0xda, 0x00},
    {0xe5, 0x1f}, {0xe1, 0x67}, {0xe0, 0x00},
    {0xdd, 0x7f}, {0x05, 0x00}, {0x12, 0x40},
    {0xd3, 0x04}, //{ 0xd3, 0x7f },
    {0xc0, 0xc8}, // hsize  200 // Arducam has 0x64
    {0xC1, 0x96}, // vsice  150 // Arducam has 0x4B
    {0x8c, 0x00}, {0x86, 0x3d}, {0x50, 0x80},
    {0x51, 0x90}, {0x52, 0x2c}, {0x53, 0x00}, /// offset x
    {0x54, 0x00},                             /// offset y
    {0x55, 0x88},                             ///  //{0x57, 0x00},
    {0x5a, 0x5A},                             /// outw   0x78   0x5A
    {0x5b, 0x5A},                             /// outh   0x44   0X5A
    {0x5c, 0x00}};

/**
 * @brief  Configures the OV2640 camera feature.
 * @param  feature: Camera feature to be configured
 * @param  value: Value to be configured
 * @param  brightness_value: Brightness value to be configured
 * @retval None
 */
uint8_t cameraRead(uint8_t Reg)
{
    uint8_t value;
    HAL_I2C_Mem_Read(&hi2c2, CAMERA_I2C_ADDRESS, (uint16_t)Reg,
                     I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
    return value;
}

/**
 * @brief  Configures the OV2640 camera feature.
 * @param  DeviceAddr: Device address on communication Bus.
 * @param  feature: Camera feature to be configured
 * @param  value: Value to be configured
 * @param  brightness_value: Brightness value to be configured
 * @retval None
 */
void cameraWrite(uint8_t Reg, uint8_t Value)
{
    HAL_I2C_Mem_Write(&hi2c2, CAMERA_I2C_ADDRESS, (uint16_t)Reg,
                      I2C_MEMADD_SIZE_8BIT, &Value, 1, 100);
}

void ov2640Init()
{
    uint32_t index;
    // Prepare the camera to be configured

    // Read camera ID
    printfInfo("Camera ID(OV2640_SENSOR_PIDH): %u\n", ov2640ReadID());

    // Table13 addresses chosen of OV2640
    cameraWrite(OV2640_DSP_RA_DLMT, CAMERA_SENSOR_ADDRESS_BANK);

    // Initiates system reset. All registers are set to factory
    // default values after which the chip resumes normal
    // operation.
    cameraWrite(OV2640_SENSOR_COM7, 0x80);

    HAL_Delay(100);

    printfInfo("OV2640_DSP_RA_DLMT: %u\n", cameraRead(OV2640_DSP_RA_DLMT));
    printfInfo("OV2640_SENSOR_COM7: %u\n", cameraRead(OV2640_SENSOR_COM7));

    for (index = 0; index < (sizeof(OV2640_myinit) / 2); index++)
    {
        cameraWrite(OV2640_myinit[index][0], OV2640_myinit[index][1]);
        HAL_Delay(2);
    }

    printfInfo("OV2640_SENSOR_COM4: %u\n",
               cameraRead(OV2640_SENSOR_COM4)); // 0xb7
}

void ov2640Config(uint32_t feature, uint32_t value, uint32_t brightness_value)
{
    uint8_t value1, value2;
    uint32_t value_tmp;
    uint32_t br_value;

    /* Convert the input value into ov2640 parameters */
    value_tmp = ov2640ConvertValue(feature, value);
    br_value = ov2640ConvertValue(CAMERA_CONTRAST_BRIGHTNESS, brightness_value);

    switch (feature)
    {
        case CAMERA_BLACK_WHITE:
        {
            cameraWrite(0xff, 0x00);
            cameraWrite(0x7c, 0x00);
            cameraWrite(0x7d, value_tmp);
            cameraWrite(0x7c, 0x05);
            cameraWrite(0x7d, 0x80);
            cameraWrite(0x7d, 0x80);
            break;
        }
        case CAMERA_CONTRAST_BRIGHTNESS:
        {
            value1 = (uint8_t)(value_tmp);
            value2 = (uint8_t)(value_tmp >> 8);
            cameraWrite(0xff, 0x00);
            cameraWrite(0x7c, 0x00);
            cameraWrite(0x7d, 0x04);
            cameraWrite(0x7c, 0x07);
            cameraWrite(0x7d, br_value);
            cameraWrite(0x7d, value1);
            cameraWrite(0x7d, value2);
            cameraWrite(0x7d, 0x06);
            break;
        }
        case CAMERA_COLOR_EFFECT:
        {
            value1 = (uint8_t)(value_tmp);
            value2 = (uint8_t)(value_tmp >> 8);
            cameraWrite(0xff, 0x00);
            cameraWrite(0x7c, 0x00);
            cameraWrite(0x7d, 0x18);
            cameraWrite(0x7c, 0x05);
            cameraWrite(0x7d, value1);
            cameraWrite(0x7d, value2);
            break;
        }
        default:
        {
            break;
        }
    }
}

/**
 * @brief  Read the OV2640 Camera identity.
 * @param  DeviceAddr: Device address on communication Bus.
 * @retval the OV2640 ID
 */
uint16_t ov2640ReadID()
{
    uint16_t id;
    /* Initialize I2C */
    // cameraInit();

    /* Prepare the sensor to read the Camera ID */
    cameraWrite(OV2640_DSP_RA_DLMT, 0x01);

    /* Get the camera ID */
    id = cameraRead(OV2640_SENSOR_PIDH) << 8;
    id += cameraRead(OV2640_SENSOR_PIDL);

    return id;
}

/******************************************************************************
                            Static Functions
*******************************************************************************/
/**
 * @brief  Convert input values into ov2640 parameters.
 * @param  feature: Camera feature to be configured
 * @param  value: Value to be configured
 * @retval The converted value
 */
static uint32_t ov2640ConvertValue(uint32_t feature, uint32_t value)
{
    uint32_t ret = 0;

    switch (feature)
    {
        case CAMERA_BLACK_WHITE:
        {
            switch (value)
            {
                case CAMERA_BLACK_WHITE_BW:
                {
                    ret = OV2640_BLACK_WHITE_BW;
                    break;
                }
                case CAMERA_BLACK_WHITE_NEGATIVE:
                {
                    ret = OV2640_BLACK_WHITE_NEGATIVE;
                    break;
                }
                case CAMERA_BLACK_WHITE_BW_NEGATIVE:
                {
                    ret = OV2640_BLACK_WHITE_BW_NEGATIVE;
                    break;
                }
                case CAMERA_BLACK_WHITE_NORMAL:
                {
                    ret = OV2640_BLACK_WHITE_NORMAL;
                    break;
                }
                default:
                {
                    ret = OV2640_BLACK_WHITE_NORMAL;
                    break;
                }
            }
            break;
        }
        case CAMERA_CONTRAST_BRIGHTNESS:
        {
            switch (value)
            {
                case CAMERA_BRIGHTNESS_LEVEL0:
                {
                    ret = OV2640_BRIGHTNESS_LEVEL0;
                    break;
                }
                case CAMERA_BRIGHTNESS_LEVEL1:
                {
                    ret = OV2640_BRIGHTNESS_LEVEL1;
                    break;
                }
                case CAMERA_BRIGHTNESS_LEVEL2:
                {
                    ret = OV2640_BRIGHTNESS_LEVEL2;
                    break;
                }
                case CAMERA_BRIGHTNESS_LEVEL3:
                {
                    ret = OV2640_BRIGHTNESS_LEVEL3;
                    break;
                }
                case CAMERA_BRIGHTNESS_LEVEL4:
                {
                    ret = OV2640_BRIGHTNESS_LEVEL4;
                    break;
                }
                case CAMERA_CONTRAST_LEVEL0:
                {
                    ret = OV2640_CONTRAST_LEVEL0;
                    break;
                }
                case CAMERA_CONTRAST_LEVEL1:
                {
                    ret = OV2640_CONTRAST_LEVEL1;
                    break;
                }
                case CAMERA_CONTRAST_LEVEL2:
                {
                    ret = OV2640_CONTRAST_LEVEL2;
                    break;
                }
                case CAMERA_CONTRAST_LEVEL3:
                {
                    ret = OV2640_CONTRAST_LEVEL3;
                    break;
                }
                case CAMERA_CONTRAST_LEVEL4:
                {
                    ret = OV2640_CONTRAST_LEVEL4;
                    break;
                }
                default:
                {
                    ret = OV2640_CONTRAST_LEVEL0;
                    break;
                }
            }
            break;
        }
        case CAMERA_COLOR_EFFECT:
        {
            switch (value)
            {
                case CAMERA_COLOR_EFFECT_ANTIQUE:
                {
                    ret = OV2640_COLOR_EFFECT_ANTIQUE;
                    break;
                }
                case CAMERA_COLOR_EFFECT_BLUE:
                {
                    ret = OV2640_COLOR_EFFECT_BLUE;
                    break;
                }
                case CAMERA_COLOR_EFFECT_GREEN:
                {
                    ret = OV2640_COLOR_EFFECT_GREEN;
                    break;
                }
                case CAMERA_COLOR_EFFECT_RED:
                {
                    ret = OV2640_COLOR_EFFECT_RED;
                    break;
                }
                default:
                {
                    ret = OV2640_COLOR_EFFECT_RED;
                    break;
                }
            }
            break;
            default:
            {
                ret = 0;
                break;
            }
        }
    }

    return ret;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
