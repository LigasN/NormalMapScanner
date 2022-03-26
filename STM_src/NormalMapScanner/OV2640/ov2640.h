/*
 * ov2640.c
 *
 *  Copied from previous author on: 19. 03. 2022
 *      Previous author: hjh
 *      Current author: https://github.com/LigasN
 */
/*
 * my_ov2640.h
 *
 *  Created on: 2. okt. 2020
 *      Author: hjh
 */
#ifndef __OV2640_H__
#define __OV2640_H__

#include "stm32f4xx_hal.h"

// User settings

#define IMAGE_RESOLUTION_WIDTH 400 // Actual display is 480x320
#define IMAGE_RESOLUTION_HEIGHT 300

//------------------------------------------------------------------------------

#define CAMERA_R160x120 0x00 /* QQVGA Resolution                     */
#define CAMERA_R320x240 0x01 /* QVGA Resolution                      */
#define CAMERA_R480x272 0x02 /* 480x272 Resolution                   */
#define CAMERA_R640x480 0x03 /* VGA Resolution                       */

#define CAMERA_CONTRAST_BRIGHTNESS                                             \
    0x00                         /* Camera contrast brightness features  */
#define CAMERA_BLACK_WHITE 0x01  /* Camera black white feature           */
#define CAMERA_COLOR_EFFECT 0x03 /* Camera color effect feature          */

#define CAMERA_BRIGHTNESS_LEVEL0 0x00 /* Brightness level -2         */
#define CAMERA_BRIGHTNESS_LEVEL1 0x01 /* Brightness level -1         */
#define CAMERA_BRIGHTNESS_LEVEL2 0x02 /* Brightness level 0          */
#define CAMERA_BRIGHTNESS_LEVEL3 0x03 /* Brightness level +1         */
#define CAMERA_BRIGHTNESS_LEVEL4 0x04 /* Brightness level +2         */

#define CAMERA_CONTRAST_LEVEL0 0x05 /* Contrast level -2           */
#define CAMERA_CONTRAST_LEVEL1 0x06 /* Contrast level -1           */
#define CAMERA_CONTRAST_LEVEL2 0x07 /* Contrast level  0           */
#define CAMERA_CONTRAST_LEVEL3 0x08 /* Contrast level +1           */
#define CAMERA_CONTRAST_LEVEL4 0x09 /* Contrast level +2           */

#define CAMERA_BLACK_WHITE_BW 0x00          /* Black and white effect      */
#define CAMERA_BLACK_WHITE_NEGATIVE 0x01    /* Negative effect             */
#define CAMERA_BLACK_WHITE_BW_NEGATIVE 0x02 /* BW and Negative effect      */
#define CAMERA_BLACK_WHITE_NORMAL 0x03      /* Normal effect               */

#define CAMERA_COLOR_EFFECT_NONE 0x00    /* No effects                  */
#define CAMERA_COLOR_EFFECT_BLUE 0x01    /* Blue effect                 */
#define CAMERA_COLOR_EFFECT_GREEN 0x02   /* Green effect                */
#define CAMERA_COLOR_EFFECT_RED 0x03     /* Red effect                  */
#define CAMERA_COLOR_EFFECT_ANTIQUE 0x04 /* Antique effect              */
/** @defgroup OV2640_Exported_Types
 * @{
 */
typedef enum
{
    BMP_QQVGA    = 0x00, /* BMP Image QQVGA 160x120 Size */
    BMP_QVGA     = 0x01, /* BMP Image QVGA 320x240 Size */
    JPEG_160x120 = 0x02, /* JPEG Image 160x120 Size */
    JPEG_176x144 = 0x03, /* JPEG Image 176x144 Size */
    JPEG_320x240 = 0x04, /* JPEG Image 320x240 Size */
    JPEG_352x288 = 0x05  /* JPEG Image 352x288 Size */

} ImageFormat_TypeDef;

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    uint8_t Manufacturer_ID1;
    uint8_t Manufacturer_ID2;
    uint8_t Version;
    uint8_t PID;
} OV2640_IDTypeDef;

#define CAMERA_I2C_ADDRESS 0x60

// Address bank Table12 -- 0x00 or Table 13 -- 0x01
#define CAMERA_DSP_ADDRESS_BANK 0x00
#define CAMERA_SENSOR_ADDRESS_BANK 0x01
#define OV2640_ID 0x2642

/* OV2640 Registers definition when DSP bank selected (0xFF = 0x00) */
#define OV2640_DSP_R_BYPASS 0x05
#define OV2640_DSP_Qs 0x44
#define OV2640_DSP_CTRL 0x50
#define OV2640_DSP_HSIZE1 0x51
#define OV2640_DSP_VSIZE1 0x52
#define OV2640_DSP_XOFFL 0x53
#define OV2640_DSP_YOFFL 0x54
#define OV2640_DSP_VHYX 0x55
#define OV2640_DSP_DPRP 0x56
#define OV2640_DSP_TEST 0x57
#define OV2640_DSP_ZMOW 0x5A
#define OV2640_DSP_ZMOH 0x5B
#define OV2640_DSP_ZMHH 0x5C
#define OV2640_DSP_BPADDR 0x7C
#define OV2640_DSP_BPDATA 0x7D
#define OV2640_DSP_CTRL2 0x86
#define OV2640_DSP_CTRL3 0x87
#define OV2640_DSP_SIZEL 0x8C
#define OV2640_DSP_HSIZE2 0xC0
#define OV2640_DSP_VSIZE2 0xC1
#define OV2640_DSP_CTRL0 0xC2
#define OV2640_DSP_CTRL1 0xC3
#define OV2640_DSP_R_DVP_SP 0xD3
#define OV2640_DSP_IMAGE_MODE 0xDA
#define OV2640_DSP_RESET 0xE0
#define OV2640_DSP_MS_SP 0xF0
#define OV2640_DSP_SS_ID 0x7F
#define OV2640_DSP_SS_CTRL 0xF8
#define OV2640_DSP_MC_BIST 0xF9
#define OV2640_DSP_MC_AL 0xFA
#define OV2640_DSP_MC_AH 0xFB
#define OV2640_DSP_MC_D 0xFC
#define OV2640_DSP_P_STATUS 0xFE
#define OV2640_DSP_RA_DLMT 0xFF

/* OV2640 Registers definition when sensor bank selected (0xFF = 0x01) */
#define OV2640_SENSOR_GAIN 0x00
#define OV2640_SENSOR_COM1 0x03
#define OV2640_SENSOR_REG04 0x04
#define OV2640_SENSOR_REG08 0x08
#define OV2640_SENSOR_COM2 0x09
#define OV2640_SENSOR_PIDH 0x0A
#define OV2640_SENSOR_PIDL 0x0B
#define OV2640_SENSOR_COM3 0x0C
#define OV2640_SENSOR_COM4 0x0D
#define OV2640_SENSOR_AEC 0x10
#define OV2640_SENSOR_CLKRC 0x11
#define OV2640_SENSOR_COM7 0x12
#define OV2640_SENSOR_COM8 0x13
#define OV2640_SENSOR_COM9 0x14
#define OV2640_SENSOR_COM10 0x15
#define OV2640_SENSOR_HREFST 0x17
#define OV2640_SENSOR_HREFEND 0x18
#define OV2640_SENSOR_VSTART 0x19
#define OV2640_SENSOR_VEND 0x1A
#define OV2640_SENSOR_MIDH 0x1C
#define OV2640_SENSOR_MIDL 0x1D
#define OV2640_SENSOR_AEW 0x24
#define OV2640_SENSOR_AEB 0x25
#define OV2640_SENSOR_W 0x26
#define OV2640_SENSOR_REG2A 0x2A
#define OV2640_SENSOR_FRARL 0x2B
#define OV2640_SENSOR_ADDVSL 0x2D
#define OV2640_SENSOR_ADDVHS 0x2E
#define OV2640_SENSOR_YAVG 0x2F
#define OV2640_SENSOR_REG32 0x32
#define OV2640_SENSOR_ARCOM2 0x34
#define OV2640_SENSOR_REG45 0x45
#define OV2640_SENSOR_FLL 0x46
#define OV2640_SENSOR_FLH 0x47
#define OV2640_SENSOR_COM19 0x48
#define OV2640_SENSOR_ZOOMS 0x49
#define OV2640_SENSOR_COM22 0x4B
#define OV2640_SENSOR_COM25 0x4E
#define OV2640_SENSOR_BD50 0x4F
#define OV2640_SENSOR_BD60 0x50
#define OV2640_SENSOR_REG5D 0x5D
#define OV2640_SENSOR_REG5E 0x5E
#define OV2640_SENSOR_REG5F 0x5F
#define OV2640_SENSOR_REG60 0x60
#define OV2640_SENSOR_HISTO_LOW 0x61
#define OV2640_SENSOR_HISTO_HIGH 0x62

/**
 * @brief  OV2640 Features Parameters
 */
#define OV2640_BRIGHTNESS_LEVEL0 0x40 /* Brightness level -2         */
#define OV2640_BRIGHTNESS_LEVEL1 0x30 /* Brightness level -1         */
#define OV2640_BRIGHTNESS_LEVEL2 0x20 /* Brightness level 0          */
#define OV2640_BRIGHTNESS_LEVEL3 0x10 /* Brightness level +1         */
#define OV2640_BRIGHTNESS_LEVEL4 0x00 /* Brightness level +2         */

#define OV2640_BLACK_WHITE_BW 0x18          /* Black and white effect      */
#define OV2640_BLACK_WHITE_NEGATIVE 0x40    /* Negative effect             */
#define OV2640_BLACK_WHITE_BW_NEGATIVE 0x58 /* BW and Negative effect      */
#define OV2640_BLACK_WHITE_NORMAL 0x00      /* Normal effect               */

#define OV2640_CONTRAST_LEVEL0 0x3418 /* Contrast level -2           */
#define OV2640_CONTRAST_LEVEL1 0x2A1C /* Contrast level -2           */
#define OV2640_CONTRAST_LEVEL2 0x2020 /* Contrast level -2           */
#define OV2640_CONTRAST_LEVEL3 0x1624 /* Contrast level -2           */
#define OV2640_CONTRAST_LEVEL4 0x0C28 /* Contrast level -2           */

#define OV2640_COLOR_EFFECT_ANTIQUE 0xA640 /* Antique effect              */
#define OV2640_COLOR_EFFECT_BLUE 0x40A0    /* Blue effect                 */
#define OV2640_COLOR_EFFECT_GREEN 0x4040   /* Green effect                */
#define OV2640_COLOR_EFFECT_RED 0xC040     /* Red effect                  */

void ov2640Init();
void ov2640Config(uint32_t feature, uint32_t value, uint32_t BR_value);
void ov2640JPEGConfig(ImageFormat_TypeDef ImageFormat);
void ov2640BrightnessConfig(uint8_t Brightness);
void ov2640AutoExposure(uint8_t level);
uint16_t ov2640ReadID();

/* I2C_HandleTypeDef structure */
extern I2C_HandleTypeDef hi2c2;

#endif // __OV2640_H__
