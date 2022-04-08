/**
 * ov2640.c
 *
 * 	Created on: 23.02.2019
 *	Modified on:  23.02.2021
 *
 *	Copyright 2021 SimpleMethod
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *THE SOFTWARE.
 ******************************************************************************
 */

#include "TEST_ov2640.h"

// Other
#include "debug.h"

/**
 * Code debugging option
 */
//#define DEBUG

I2C_HandleTypeDef* phi2c;
DCMI_HandleTypeDef* phdcmi;
UART_HandleTypeDef* phuart;

const unsigned char OV2640_JPEG_INIT[][2] = {
    {0xff, 0x00}, {0x2c, 0xff}, {0x2e, 0xdf}, {0xff, 0x01}, {0x3c, 0x32},
    {0x11, 0x00}, {0x09, 0x02}, {0x04, 0x28}, {0x13, 0xe5}, {0x14, 0x48},
    {0x2c, 0x0c}, {0x33, 0x78}, {0x3a, 0x33}, {0x3b, 0xfB}, {0x3e, 0x00},
    {0x43, 0x11}, {0x16, 0x10}, {0x39, 0x92}, {0x35, 0xda}, {0x22, 0x1a},
    {0x37, 0xc3}, {0x23, 0x00}, {0x34, 0xc0}, {0x36, 0x1a}, {0x06, 0x88},
    {0x07, 0xc0}, {0x0d, 0x87}, {0x0e, 0x41}, {0x4c, 0x00}, {0x48, 0x00},
    {0x5B, 0x00}, {0x42, 0x03}, {0x4a, 0x81}, {0x21, 0x99}, {0x24, 0x40},
    {0x25, 0x38}, {0x26, 0x82}, {0x5c, 0x00}, {0x63, 0x00}, {0x61, 0x70},
    {0x62, 0x80}, {0x7c, 0x05}, {0x20, 0x80}, {0x28, 0x30}, {0x6c, 0x00},
    {0x6d, 0x80}, {0x6e, 0x00}, {0x70, 0x02}, {0x71, 0x94}, {0x73, 0xc1},
    {0x12, 0x40}, {0x17, 0x11}, {0x18, 0x43}, {0x19, 0x00}, {0x1a, 0x4b},
    {0x32, 0x09}, {0x37, 0xc0}, {0x4f, 0x60}, {0x50, 0xa8}, {0x6d, 0x00},
    {0x3d, 0x38}, {0x46, 0x3f}, {0x4f, 0x60}, {0x0c, 0x3c}, {0xff, 0x00},
    {0xe5, 0x7f}, {0xf9, 0xc0}, {0x41, 0x24}, {0xe0, 0x14}, {0x76, 0xff},
    {0x33, 0xa0}, {0x42, 0x20}, {0x43, 0x18}, {0x4c, 0x00}, {0x87, 0xd5},
    {0x88, 0x3f}, {0xd7, 0x03}, {0xd9, 0x10}, {0xd3, 0x82}, {0xc8, 0x08},
    {0xc9, 0x80}, {0x7c, 0x00}, {0x7d, 0x00}, {0x7c, 0x03}, {0x7d, 0x48},
    {0x7d, 0x48}, {0x7c, 0x08}, {0x7d, 0x20}, {0x7d, 0x10}, {0x7d, 0x0e},
    {0x90, 0x00}, {0x91, 0x0e}, {0x91, 0x1a}, {0x91, 0x31}, {0x91, 0x5a},
    {0x91, 0x69}, {0x91, 0x75}, {0x91, 0x7e}, {0x91, 0x88}, {0x91, 0x8f},
    {0x91, 0x96}, {0x91, 0xa3}, {0x91, 0xaf}, {0x91, 0xc4}, {0x91, 0xd7},
    {0x91, 0xe8}, {0x91, 0x20}, {0x92, 0x00}, {0x93, 0x06}, {0x93, 0xe3},
    {0x93, 0x05}, {0x93, 0x05}, {0x93, 0x00}, {0x93, 0x04}, {0x93, 0x00},
    {0x93, 0x00}, {0x93, 0x00}, {0x93, 0x00}, {0x93, 0x00}, {0x93, 0x00},
    {0x93, 0x00}, {0x96, 0x00}, {0x97, 0x08}, {0x97, 0x19}, {0x97, 0x02},
    {0x97, 0x0c}, {0x97, 0x24}, {0x97, 0x30}, {0x97, 0x28}, {0x97, 0x26},
    {0x97, 0x02}, {0x97, 0x98}, {0x97, 0x80}, {0x97, 0x00}, {0x97, 0x00},
    {0xc3, 0xed}, {0xa4, 0x00}, {0xa8, 0x00}, {0xc5, 0x11}, {0xc6, 0x51},
    {0xbf, 0x80}, {0xc7, 0x10}, {0xb6, 0x66}, {0xb8, 0xA5}, {0xb7, 0x64},
    {0xb9, 0x7C}, {0xb3, 0xaf}, {0xb4, 0x97}, {0xb5, 0xFF}, {0xb0, 0xC5},
    {0xb1, 0x94}, {0xb2, 0x0f}, {0xc4, 0x5c}, {0xc0, 0x64}, {0xc1, 0x4B},
    {0x8c, 0x00}, {0x86, 0x3D}, {0x50, 0x00}, {0x51, 0xC8}, {0x52, 0x96},
    {0x53, 0x00}, {0x54, 0x00}, {0x55, 0x00}, {0x5a, 0xC8}, {0x5b, 0x96},
    {0x5c, 0x00}, {0xd3, 0x00}, {0xc3, 0xed}, {0x7f, 0x00}, {0xda, 0x00},
    {0xe5, 0x1f}, {0xe1, 0x67}, {0xe0, 0x00}, {0xdd, 0x7f}, {0x05, 0x00},
    {0x12, 0x40}, {0xd3, 0x04}, {0xc0, 0x16}, {0xC1, 0x12}, {0x8c, 0x00},
    {0x86, 0x3d}, {0x50, 0x00}, {0x51, 0x2C}, {0x52, 0x24}, {0x53, 0x00},
    {0x54, 0x00}, {0x55, 0x00}, {0x5A, 0x2c}, {0x5b, 0x24}, {0x5c, 0x00},
    {0xff, 0xff},
};

const unsigned char OV2640_YUV422[][2] = {
    {0xFF, 0x00}, {0x05, 0x00}, {0xDA, 0x10}, {0xD7, 0x03}, {0xDF, 0x00},
    {0x33, 0x80}, {0x3C, 0x40}, {0xe1, 0x77}, {0x00, 0x00}, {0xff, 0xff},
};

const unsigned char OV2640_JPEG[][2] = {
    {0xe0, 0x14}, {0xe1, 0x77}, {0xe5, 0x1f}, {0xd7, 0x03}, {0xda, 0x10},
    {0xe0, 0x00}, {0xFF, 0x01}, {0x04, 0x08}, {0xff, 0xff},
};

const unsigned char OV2640_160x120_JPEG[][2] = {
    {0xFF, 0x01}, {0x12, 0x40}, {0x17, 0x11}, {0x18, 0x43}, {0x19, 0x00},
    {0x1a, 0x4b}, {0x32, 0x09}, {0x4f, 0xca}, {0x50, 0xa8}, {0x5a, 0x23},
    {0x6d, 0x00}, {0x39, 0x12}, {0x35, 0xda}, {0x22, 0x1a}, {0x37, 0xc3},
    {0x23, 0x00}, {0x34, 0xc0}, {0x36, 0x1a}, {0x06, 0x88}, {0x07, 0xc0},
    {0x0d, 0x87}, {0x0e, 0x41}, {0x4c, 0x00}, {0xFF, 0x00}, {0xe0, 0x04},
    {0xc0, 0x64}, {0xc1, 0x4b}, {0x86, 0x35}, {0x50, 0x92}, {0x51, 0xc8},
    {0x52, 0x96}, {0x53, 0x00}, {0x54, 0x00}, {0x55, 0x00}, {0x57, 0x00},
    {0x5a, 0x2c}, {0x5b, 0x24}, {0x5c, 0x00}, {0xe0, 0x00}, {0xff, 0xff}};

const unsigned char OV2640_320x240_JPEG[][2] = {
    {0xff, 0x01}, {0x12, 0x40}, {0x17, 0x11}, {0x18, 0x43}, {0x19, 0x00},
    {0x1a, 0x4b}, {0x32, 0x09}, {0x4f, 0xca}, {0x50, 0xa8}, {0x5a, 0x23},
    {0x6d, 0x00}, {0x39, 0x12}, {0x35, 0xda}, {0x22, 0x1a}, {0x37, 0xc3},
    {0x23, 0x00}, {0x34, 0xc0}, {0x36, 0x1a}, {0x06, 0x88}, {0x07, 0xc0},
    {0x0d, 0x87}, {0x0e, 0x41}, {0x4c, 0x00}, {0xff, 0x00}, {0xe0, 0x04},
    {0xc0, 0x64}, {0xc1, 0x4b}, {0x86, 0x35}, {0x50, 0x89}, {0x51, 0xc8},
    {0x52, 0x96}, {0x53, 0x00}, {0x54, 0x00}, {0x55, 0x00}, {0x57, 0x00},
    {0x5a, 0x50}, {0x5b, 0x3c}, {0x5c, 0x00}, {0xe0, 0x00}, {0xff, 0xff},
};

const unsigned char OV2640_640x480_JPEG[][2] = {
    {0xff, 0x01}, {0x11, 0x01}, {0x12, 0x00}, {0x17, 0x11}, {0x18, 0x75},
    {0x32, 0x36}, {0x19, 0x01}, {0x1a, 0x97}, {0x03, 0x0f}, {0x37, 0x40},
    {0x4f, 0xbb}, {0x50, 0x9c}, {0x5a, 0x57}, {0x6d, 0x80}, {0x3d, 0x34},
    {0x39, 0x02}, {0x35, 0x88}, {0x22, 0x0a}, {0x37, 0x40}, {0x34, 0xa0},
    {0x06, 0x02}, {0x0d, 0xb7}, {0x0e, 0x01}, {0xff, 0x00}, {0xe0, 0x04},
    {0xc0, 0xc8}, {0xc1, 0x96}, {0x86, 0x3d}, {0x50, 0x89}, {0x51, 0x90},
    {0x52, 0x2c}, {0x53, 0x00}, {0x54, 0x00}, {0x55, 0x88}, {0x57, 0x00},
    {0x5a, 0xa0}, {0x5b, 0x78}, {0x5c, 0x00}, {0xd3, 0x04}, {0xe0, 0x00},
    {0xff, 0xff},
};

const unsigned char OV2640_800x600_JPEG[][2] = {
    {0xFF, 0x01}, {0x11, 0x01}, {0x12, 0x00}, {0x17, 0x11}, {0x18, 0x75},
    {0x32, 0x36}, {0x19, 0x01}, {0x1a, 0x97}, {0x03, 0x0f}, {0x37, 0x40},
    {0x4f, 0xbb}, {0x50, 0x9c}, {0x5a, 0x57}, {0x6d, 0x80}, {0x3d, 0x34},
    {0x39, 0x02}, {0x35, 0x88}, {0x22, 0x0a}, {0x37, 0x40}, {0x34, 0xa0},
    {0x06, 0x02}, {0x0d, 0xb7}, {0x0e, 0x01}, {0xFF, 0x00}, {0xe0, 0x04},
    {0xc0, 0xc8}, {0xc1, 0x96}, {0x86, 0x35}, {0x50, 0x89}, {0x51, 0x90},
    {0x52, 0x2c}, {0x53, 0x00}, {0x54, 0x00}, {0x55, 0x88}, {0x57, 0x00},
    {0x5a, 0xc8}, {0x5b, 0x96}, {0x5c, 0x00}, {0xd3, 0x02}, {0xe0, 0x00},
    {0xff, 0xff}};

const unsigned char OV2640_1024x768_JPEG[][2] = {
    {0xFF, 0x01}, {0x11, 0x01}, {0x12, 0x00}, {0x17, 0x11}, {0x18, 0x75},
    {0x32, 0x36}, {0x19, 0x01}, {0x1a, 0x97}, {0x03, 0x0f}, {0x37, 0x40},
    {0x4f, 0xbb}, {0x50, 0x9c}, {0x5a, 0x57}, {0x6d, 0x80}, {0x3d, 0x34},
    {0x39, 0x02}, {0x35, 0x88}, {0x22, 0x0a}, {0x37, 0x40}, {0x34, 0xa0},
    {0x06, 0x02}, {0x0d, 0xb7}, {0x0e, 0x01}, {0xFF, 0x00}, {0xc0, 0xC8},
    {0xc1, 0x96}, {0x8c, 0x00}, {0x86, 0x3D}, {0x50, 0x00}, {0x51, 0x90},
    {0x52, 0x2C}, {0x53, 0x00}, {0x54, 0x00}, {0x55, 0x88}, {0x5a, 0x00},
    {0x5b, 0xC0}, {0x5c, 0x01}, {0xd3, 0x02}, {0xff, 0xff}};

const unsigned char OV2640_1280x960_JPEG[][2] = {
    {0xFF, 0x01}, {0x11, 0x01}, {0x12, 0x00}, {0x17, 0x11}, {0x18, 0x75},
    {0x32, 0x36}, {0x19, 0x01}, {0x1a, 0x97}, {0x03, 0x0f}, {0x37, 0x40},
    {0x4f, 0xbb}, {0x50, 0x9c}, {0x5a, 0x57}, {0x6d, 0x80}, {0x3d, 0x34},
    {0x39, 0x02}, {0x35, 0x88}, {0x22, 0x0a}, {0x37, 0x40}, {0x34, 0xa0},
    {0x06, 0x02}, {0x0d, 0xb7}, {0x0e, 0x01}, {0xFF, 0x00}, {0xe0, 0x04},
    {0xc0, 0xc8}, {0xc1, 0x96}, {0x86, 0x3d}, {0x50, 0x00}, {0x51, 0x90},
    {0x52, 0x2c}, {0x53, 0x00}, {0x54, 0x00}, {0x55, 0x88}, {0x57, 0x00},
    {0x5a, 0x40}, {0x5b, 0xf0}, {0x5c, 0x01}, {0xd3, 0x02}, {0xe0, 0x00},
    {0xff, 0xff}};

const unsigned char OV2640_CONTRAST2[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x04}, {0x7c, 0x07}, {0x7d, 0x20},
    {0x7d, 0x28}, {0x7d, 0x0c}, {0x7d, 0x06}, {0xff, 0xff}};

const unsigned char OV2640_CONTRAST1[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x04}, {0x7c, 0x07}, {0x7d, 0x20},
    {0x7d, 0x24}, {0x7d, 0x16}, {0x7d, 0x06}, {0xff, 0xff}};

const unsigned char OV2640_CONTRAST0[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x04}, {0x7c, 0x07}, {0x7d, 0x20},
    {0x7d, 0x20}, {0x7d, 0x20}, {0x7d, 0x06}, {0xff, 0xff}};

const unsigned char OV2640_CONTRAST_1[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x04}, {0x7c, 0x07}, {0x7d, 0x20},
    {0x7d, 0x1c}, {0x7d, 0x2a}, {0x7d, 0x06}, {0xff, 0xff}};

const unsigned char OV2640_CONTRAST_2[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x04}, {0x7c, 0x07}, {0x7d, 0x20},
    {0x7d, 0x18}, {0x7d, 0x34}, {0x7d, 0x06}, {0xff, 0xff}};

const unsigned char OV2640_SATURATION2[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x02}, {0x7c, 0x03},
    {0x7d, 0x68}, {0x7d, 0x68}, {0xff, 0xff}};

const unsigned char OV2640_SATURATION1[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x02}, {0x7c, 0x03},
    {0x7d, 0x58}, {0x7d, 0x68}, {0xff, 0xff}};

const unsigned char OV2640_SATURATION0[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x02}, {0x7c, 0x03},
    {0x7d, 0x48}, {0x7d, 0x48}, {0xff, 0xff}};

const unsigned char OV2640_SATURATION_1[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x02}, {0x7c, 0x03},
    {0x7d, 0x38}, {0x7d, 0x38}, {0xff, 0xff}};

const unsigned char OV2640_SATURATION_2[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x02}, {0x7c, 0x03},
    {0x7d, 0x28}, {0x7d, 0x28}, {0xff, 0xff}};

const unsigned char OV2640_BRIGHTNESS2[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x04}, {0x7c, 0x09},
    {0x7d, 0x40}, {0x7d, 0x00}, {0xff, 0xff}};

const unsigned char OV2640_BRIGHTNESS1[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x04}, {0x7c, 0x09},
    {0x7d, 0x30}, {0x7d, 0x00}, {0xff, 0xff}};

const unsigned char OV2640_BRIGHTNESS0[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x04}, {0x7c, 0x09},
    {0x7d, 0x20}, {0x7d, 0x00}, {0xff, 0xff}};

const unsigned char OV2640_BRIGHTNESS_1[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x04}, {0x7c, 0x09},
    {0x7d, 0x10}, {0x7d, 0x00}, {0xff, 0xff}};

const unsigned char OV2640_BRIGHTNESS_2[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x04}, {0x7c, 0x09},
    {0x7d, 0x00}, {0x7d, 0x00}, {0xff, 0xff}};

const unsigned char OV2640_SPECIAL_EFFECTS_NORMAL[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x00}, {0x7c, 0x05},
    {0x7d, 0x80}, {0x7d, 0x80}, {0xff, 0xff}};

const unsigned char OV2640_SPECIAL_EFFECTS_ANTIQUE[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x18}, {0x7c, 0x05},
    {0x7d, 0x40}, {0x7d, 0xa6}, {0xff, 0xff}};

const unsigned char OV2640_SPECIAL_EFFECTS_BLACK_NEGATIVE[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x58}, {0x7c, 0x05},
    {0x7d, 0x80}, {0x7d, 0x80}, {0xff, 0xff}};

const unsigned char OV2640_SPECIAL_EFFECTS_BLUISH[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x18}, {0x7c, 0x05},
    {0x7d, 0xa0}, {0x7d, 0x40}, {0xff, 0xff}};

const unsigned char OV2640_SPECIAL_EFFECTS_BLACK[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x18}, {0x7c, 0x05},
    {0x7d, 0x80}, {0x7d, 0x80}, {0xff, 0xff}};

const unsigned char OV2640_SPECIAL_EFFECTS_NEGATIVE[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x40}, {0x7c, 0x05},
    {0x7d, 0x80}, {0x7d, 0x80}, {0xff, 0xff}};

const unsigned char OV2640_SPECIAL_EFFECTS_GREENISH[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x18}, {0x7c, 0x05},
    {0x7d, 0x40}, {0x7d, 0x40}, {0xff, 0xff}};

const unsigned char OV2640_SPECIAL_EFFECTS_REDDISH[][2] = {
    {0xff, 0x00}, {0x7c, 0x00}, {0x7d, 0x18}, {0x7c, 0x05},
    {0x7d, 0x40}, {0x7d, 0xc0}, {0xff, 0xff}};

const unsigned char OV2640_LIGHT_MODE_AUTO[][2] = {
    {0xff, 0x00}, {0xc7, 0x00}, {0xff, 0xff}};

const unsigned char OV2640_LIGHT_MODE_SUNNY[][2] = {{0xff, 0x00}, {0xc7, 0x40},
                                                    {0xcc, 0x5e}, {0xcd, 0x41},
                                                    {0xce, 0x54}, {0xff, 0xff}};

const unsigned char OV2640_LIGHT_MODE_CLOUDY[][2] = {
    {0xff, 0x00}, {0xc7, 0x40}, {0xcc, 0x65},
    {0xcd, 0x41}, {0xce, 0x4f}, {0xff, 0xff}};

const unsigned char OV2640_LIGHT_MODE_OFFICE[][2] = {
    {0xff, 0x00}, {0xc7, 0x40}, {0xcc, 0x52},
    {0xcd, 0x41}, {0xce, 0x66}, {0xff, 0xff}};

const unsigned char OV2640_LIGHT_MODE_HOME[][2] = {{0xff, 0x00}, {0xc7, 0x40},
                                                   {0xcc, 0x42}, {0xcd, 0x3f},
                                                   {0xce, 0x71}, {0xff, 0xff}};

/**
 * Camera initialization.
 * @param p_hi2c Pointer to I2C interface.
 * @param p_hdcmi Pointer to DCMI interface.
 */
void OV2640_Init(I2C_HandleTypeDef* p_hi2c, DCMI_HandleTypeDef* p_hdcmi)
{
    phi2c  = p_hi2c;
    phdcmi = p_hdcmi;

    // Hardware reset
    HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

    // Software reset: reset all registers to default values
    SCCB_Write(0xff, 0x01);
    SCCB_Write(0x12, 0x80);
    HAL_Delay(100);

#ifdef DEBUG
    uint8_t pid;
    uint8_t ver;
    SCCB_Read(0x0a, &pid); // pid value is 0x26
    SCCB_Read(0x0b, &ver); // ver value is 0x42
    printfInfo("PID: 0x%x, VER: 0x%x\r\n", pid, ver);
#endif

    // Stop DCMI clear buffer
    OV2640_StopDCMI();
}

/**
 * Camera resolution selection.
 * @param opt Resolution option.
 */
void OV2640_ResolutionOptions(uint16_t opt)
{
    switch (opt)
    {
        case 15533:
            OV2640_ResolutionConfiguration(0);
            break;
        case 15534:
            OV2640_ResolutionConfiguration(1);
            break;
        case 15535:
            OV2640_ResolutionConfiguration(2);
            break;
        case 25535:
            OV2640_ResolutionConfiguration(3);
            break;
        case 45535:
            OV2640_ResolutionConfiguration(4);
            break;
        case 65535:
            OV2640_ResolutionConfiguration(5);
            break;
        default:
            OV2640_ResolutionConfiguration(1);
            break;
    }
}

/**
 * Camera resolution selection.
 * @param opt Resolution option.
 */
void OV2640_ResolutionConfiguration(short opt)
{
#ifdef DEBUG
    printInfo("Starting resolution choice \r\n");
#endif
    OV2640_Configuration(OV2640_JPEG_INIT);
    OV2640_Configuration(OV2640_YUV422);
    OV2640_Configuration(OV2640_JPEG);
    HAL_Delay(10);
    SCCB_Write(0xff, 0x01);
    HAL_Delay(10);
    SCCB_Write(0x15, 0x00);

    switch (opt)
    {
        case 0:
            OV2640_Configuration(OV2640_160x120_JPEG);
            break;
        case 1:
            OV2640_Configuration(OV2640_320x240_JPEG);
            break;
        case 2:
            OV2640_Configuration(OV2640_640x480_JPEG);
            break;
        case 3:
            OV2640_Configuration(OV2640_800x600_JPEG);
            break;
        case 4:
            OV2640_Configuration(OV2640_1024x768_JPEG);
            break;
        case 5:
            OV2640_Configuration(OV2640_1280x960_JPEG);
            break;
        default:
            OV2640_Configuration(OV2640_320x240_JPEG);
            break;
    }

#ifdef DEBUG
    printInfo("Finalize configuration \r\n");
#endif
}

/**
 * Configure camera registers.
 * @param arr Array with addresses and values using to overwrite camera
 * registers.
 */
void OV2640_Configuration(const unsigned char arr[][2])
{
    unsigned short i = 0;
    uint8_t reg_addr, data, data_read;
    while (1)
    {
        reg_addr = arr[i][0];
        data     = arr[i][1];
        if (reg_addr == 0xff && data == 0xff)
        {
            break;
        }
        SCCB_Read(reg_addr, &data_read);
        SCCB_Write(reg_addr, data);
        HAL_Delay(10);
        SCCB_Read(reg_addr, &data_read);
        if (data != data_read)
        {
            // printfInfo("data != read data on addr: 0x%x", reg_addr);
        }
        i++;
    }
}

/**
 *  Changing the special effect applied to a photo.
 * @param specialEffect Name or value of the special effect.
 */
void OV2640_SpecialEffect(short specialEffect)
{
    printfInfo("Special effect value:%d\r\n", specialEffect);
    if (specialEffect == 0)
    {
        OV2640_Configuration(OV2640_SPECIAL_EFFECTS_ANTIQUE);
    }
    else if (specialEffect == 1)
    {
        OV2640_Configuration(OV2640_SPECIAL_EFFECTS_BLUISH);
    }
    else if (specialEffect == 2)
    {
        OV2640_Configuration(OV2640_SPECIAL_EFFECTS_GREENISH);
    }
    else if (specialEffect == 3)
    {
        OV2640_Configuration(OV2640_SPECIAL_EFFECTS_REDDISH);
    }
    else if (specialEffect == 4)
    {
        OV2640_Configuration(OV2640_SPECIAL_EFFECTS_BLACK);
    }
    else if (specialEffect == 5)
    {
        OV2640_Configuration(OV2640_SPECIAL_EFFECTS_NEGATIVE);
    }
    else if (specialEffect == 6)
    {
        OV2640_Configuration(OV2640_SPECIAL_EFFECTS_BLACK_NEGATIVE);
    }
    else if (specialEffect == 7)
    {
        OV2640_Configuration(OV2640_SPECIAL_EFFECTS_NORMAL);
    }
}

/**
 * Activation of simple white balance.
 */
void OV2640_AdvancedWhiteBalance()
{
    printInfo("Enable simple white balance mode\r\n");
    SCCB_Write(0xff, 0x00);
    HAL_Delay(1);
    SCCB_Write(0xc7, 0x00);
}

/**
 * Activation of simple white balance.
 */
void OV2640_SimpleWhiteBalance()
{
    printInfo("Enable simple white balance mode\r\n");
    SCCB_Write(0xff, 0x00);
    HAL_Delay(1);
    SCCB_Write(0xc7, 0x10);
}

/**
 * Changing image brightness.
 * @param brightness Name or value of the brightness.
 */
void OV2640_Brightness(short brightness)
{
    printfInfo("Brightness value:%d\r\n", brightness);

    if (brightness == 0)
    {
        OV2640_Configuration(OV2640_BRIGHTNESS0);
    }
    else if (brightness == 1)
    {
        OV2640_Configuration(OV2640_BRIGHTNESS1);
    }
    else if (brightness == 2)
    {
        OV2640_Configuration(OV2640_BRIGHTNESS2);
    }
    else if (brightness == 3)
    {
        OV2640_Configuration(OV2640_BRIGHTNESS_1);
    }
    else if (brightness == 4)
    {
        OV2640_Configuration(OV2640_BRIGHTNESS_2);
    }
}

/**
 * Changing image light mode.
 * @param lightMode Name or value of the light mode.
 */
void OV2640_LightMode(short lightMode)
{
    printfInfo("Light mode value:%d\r\n", lightMode);

    if (lightMode == 0)
    {
        OV2640_AdvancedWhiteBalance();
    }
    else if (lightMode == 1)
    {
        OV2640_Configuration(OV2640_LIGHT_MODE_SUNNY);
    }
    else if (lightMode == 2)
    {
        OV2640_Configuration(OV2640_LIGHT_MODE_CLOUDY);
    }
    else if (lightMode == 3)
    {
        OV2640_Configuration(OV2640_LIGHT_MODE_OFFICE);
    }
    else if (lightMode == 4)
    {
        OV2640_Configuration(OV2640_LIGHT_MODE_HOME);
    }
}
/**
 *  Changing image saturation.
 * @param saturation  Name or value of the saturation.
 */
void OV2640_Saturation(short saturation)
{
    printfInfo("Saturation value:%d\r\n", saturation);

    if (saturation == 0)
    {
        OV2640_Configuration(OV2640_SATURATION0);
    }
    else if (saturation == 1)
    {
        OV2640_Configuration(OV2640_SATURATION1);
    }
    else if (saturation == 2)
    {
        OV2640_Configuration(OV2640_SATURATION2);
    }
    else if (saturation == 3)
    {
        OV2640_Configuration(OV2640_SATURATION_1);
    }
    else if (saturation == 4)
    {
        OV2640_Configuration(OV2640_SATURATION_2);
    }
}

/**
 * Changing image contrast.
 * @param contrast Name or value of the contrast.
 */
void OV2640_Contrast(short contrast)
{
    printfInfo("Contrast value:%d\r\n", contrast);

    if (contrast == 0)
    {
        OV2640_Configuration(OV2640_CONTRAST0);
    }
    else if (contrast == 1)
    {
        OV2640_Configuration(OV2640_CONTRAST1);
    }
    else if (contrast == 2)
    {
        OV2640_Configuration(OV2640_CONTRAST2);
    }
    else if (contrast == 3)
    {
        OV2640_Configuration(OV2640_CONTRAST_1);
    }
    else if (contrast == 4)
    {
        OV2640_Configuration(OV2640_CONTRAST_2);
    }
}

/**
 * Stop DCMI (Clear  memory buffer)
 */
void OV2640_StopDCMI(void)
{
    printInfo("DCMI has been stopped \r\n");
    HAL_DCMI_Stop(phdcmi);
    HAL_Delay(10); // If you get a DCMI error (data is not received), increase
                   // value to 30.
}

/**
 * Executes a single reading from DCMI and returns  data as an image.
 * @param frameBuffer Table with data.
 * @param length Length of capture to be transferred.
 */
void OV2640_CaptureSnapshot(uint32_t frameBuffer, int length)
{
    HAL_DCMI_Start_DMA(phdcmi, DCMI_MODE_SNAPSHOT, frameBuffer, length);
    HAL_Delay(2000);
    HAL_DCMI_Suspend(phdcmi);
    HAL_DCMI_Stop(phdcmi);
}

/**
 * Write value to camera register.
 * @param reg_addr Address of register.
 * @param data New value.
 * @return  Operation status.
 */
short SCCB_Write(uint8_t reg_addr, uint8_t data)
{
    short opertionStatus = 0;
    uint8_t buffer[2]    = {0};
    HAL_StatusTypeDef connectionStatus;
    buffer[0] = reg_addr;
    buffer[1] = data;
    __disable_irq();
    connectionStatus =
        HAL_I2C_Master_Transmit(phi2c, (uint16_t)0x60, buffer, 2, 100);
    if (connectionStatus == HAL_OK)
    {
        opertionStatus = 1;
    }
    else
    {
        opertionStatus = 0;
    }
    __enable_irq();
    return opertionStatus;
}

/**
 * Reading data from camera registers.
 * @param reg_addr Address of register.
 * @param pdata Value read from register.
 * @return Operation status.
 */
short SCCB_Read(uint8_t reg_addr, uint8_t* pdata)
{
    short opertionStatus = 0;
    HAL_StatusTypeDef connectionStatus;
    __disable_irq();
    connectionStatus =
        HAL_I2C_Master_Transmit(phi2c, (uint16_t)0x60, &reg_addr, 1, 100);
    if (connectionStatus == HAL_OK)
    {
        connectionStatus =
            HAL_I2C_Master_Receive(phi2c, (uint16_t)0x61, pdata, 1, 100);
        if (connectionStatus == HAL_OK)
        {
            opertionStatus = 0;
        }
        else
        {
            opertionStatus = 1;
        }
    }
    else
    {
        opertionStatus = 2;
    }
    __enable_irq();
    return opertionStatus;
}
