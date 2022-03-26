/*
 * ov2640.c
 *
 *      Modeled on ArduCAM and http://iotbyskovholm.dk
 *      date: 19. 03. 2022
 *      Author: https://github.com/LigasN
 */

/* Includes ------------------------------------------------------------------*/
#include "ov2640.h"

// Other
#include "debug.h"

static uint32_t ov2640ConvertValue(uint32_t feature, uint32_t value);

/* Initialization sequence */
const unsigned char InitializationSequence[][2] = {
    {0xff, 0x00},       /* Switch to device control register list Table 12    */
    {0x2c, 0xff},       /* Reserved                                           */
    {0x2e, 0xdf},       /* Reserved                                           */
    {0xff, 0x01},       /* Switch to device control register list Table 13    */
    {0x3c, 0x32},       /* Reserved                                           */
    {0x11, 0x00},       /* CLKRC - Clock Rate Control  RW
                               Bit[7]: Internal frequency doublers
                                   0: OFF
                                   1: ON
                               Bit[6]: Reserved
                               Bit[5:0]: Clock divider

                           CLK = XVCLK/(decimal value of CLKRC[5:0] + 1)
                                                                              */
    {0x09, 0x02},       /* COM2 - Common control 2  RW
                               Bit[7:5]: Reserved
                               Bit[4]:   Standby mode enable
                                   0: Normal mode
                                   1: Standby mode
                               Bit[3]:   Reserved
                               Bit[2]:   Pin PWDN/RESETB used as SLVS/SLHS
                               Bit[1:0]: Output drive select
                                   00: 1x capability
                                   01: 3x capability
                                   10: 2x capability
                                   11: 4x capability
                                                                              */
    {0x04, 0b10101000}, /* REGO4 - Register 04  RW
                               Bit[7]:   Horizontal mirror
                               Bit[6]:   Vertical flip
                               Bit[5]:   ??? no documentation here
                               Bit[4]:   VREF bit[0]
                               Bit[3]:   HREF bit[0]
                               Bit[2]:   Reserved
                               Bit[1:0]: AEC[1:0]

                           Info: AEC[15:10] is in register REG45[5:0] (0x45),
                                 AEC[9:2] is in register AEC[7:0] (0x10))
                                                                              */
    {0x13, 0b11100101}, /* COM8 - Common control 8  RW
                               Bit[7:6]: Reserved  --->
                                   from VV register Bit[7] = AEC/AGC fast mode
                               Bit[5]: Banding filter selection
                                   0: OFF
                                   1: ON, set minimum exposure time to 1/120s
                               Bit[4:3]: Reserved
                               Bit[2]: | AGC auto/manual control selection
                                   0: Manual
                                   1: Auto
                               Bit[1]: Reserved
                               Bit[O]: Exposure control
                                   0: Manual
                                   1: Auto
                                                                              */
    {0x14, 0b01001000}, /* COM9 - Common control 9  RW
                               Bit[7:5]: AGC gain ceiling, GH[2:0]
                                   000: 2x
                                   001: 4x
                                   010: 8x
                                   011: 16x
                                   100: 32x
                                   101: 64x
                                   11x: 128x
                               Bit[4:0]: Reserved
                                                                              */
    {0x2c, 0x0c},       /* Reserved                                           */
    {0x33, 0x78},       /* Reserved                                           */
    {0x3a, 0x33},       /* Reserved                                           */
    {0x3b, 0xfB},       /* Reserved                                           */
    {0x3e, 0x00},       /* Reserved                                           */
    {0x43, 0x11},       /* Reserved                                           */
    {0x16, 0x10},       /* Reserved                                           */
    {0x39, 0x02},       /* Reserved                                           */
    {0x35, 0x88},       /* Reserved                                           */
    {0x22, 0x0A},       /* Reserved                                           */
    {0x37, 0x40},       /* Reserved                                           */
    {0x23, 0x00},       /* Reserved                                           */
    {0x34, 0xa0},       /* ARCOM2 20  RW
                               Bit[7:3]: Reserved
                               Bit[2]:   Zoom window horizontal start point
                               Bit[1:0]: Reserved
                                                                              */
    {0x06, 0x02},       /* Reserved                                           */
    {0x07, 0xc0},       /* Reserved                                           */
    {0x0d, 0b10110111}, /* COM4 - Common Control 4  RW
                               Bit[7:3]: Reserved
                               Bit[2]: Clock output power-down pin status
                                   0: Tri-state data output pin upon power-down
                                   1: Data output pin hold at last state before
                                      power-down
                               Bit[1:0]: Reserved
                                                                              */
    {0x0e, 0x01},       /* Reserved                                           */
    {0x4c, 0x00},       /* Reserved                                           */
    {0x4a, 0x81},       /* Reserved                                           */
    {0x21, 0x99},       /* Reserved                                           */
    {0x24, 0x40},       /* AEW  RW   value = 64(DEC)  default = 0x78 = 120
                           Luminance Signal High Range for AEC/AGC Operation
                           AEC/AGC values will decrease in auto mode when
                           average luminance is greater than AEV[7:0]
                                                                              */
    {0x25, 0x38},       /* AEB  RW   value = 56(DEC)  default = 0x68 = 104
                           Luminance Signal Low Range for AEC/AGC Operation
                           AEC/AGC values will increase in auto mode when
                           average luminance is less than AEB[7:0]
                                                                              */
    {0x26, 0x82},       /* VV  RW   value = [High ->8, Low -> 2]
                           Fast Mode Large Step Range Threshold - effective
                           only in AEC/AGC fast mode (COM8[7] = 1)
                               Bit[7:4]: High threshold
                               Bit[3:0]: Low threshold

                           Note: AEC/AGC may change in larger steps when
                                 luminance average is greater than VV[7:4]
                                 or less than VV[3:0].
                                                                              */
    {0x5c, 0x00},       /* Reserved                                           */
    {0x63, 0x00},       /* Reserved                                           */
    {0x46, 0x22},       /* FLL - Frame length adjustment  RW
                           Frame Length Adjustment LSBs
                           Each bit will add 1 horizontal line timing in frame
                                                                              */
    {0x0c, 0b00111011}, /* COM3 - Common control 3  RW  (before was: 00111010)
                               Bit[7:3]: Reserved
                               Bit[2]: Set banding manually
                                   0: 60 Hz
                                   1: 50 Hz
                               Bit[1]: Auto set banding
                               Bit[O]: Snapshot option
                                   0: Enable live video output after
                                      snapshot sequence
                                   1: Output single frame only
                                                                              */
    {0x5d, 0x55},       /* REGSD - Register 5D  RW
                           Bit[7:0]: AVGsel[7:0], 16-zone average weight option
                                                                              */
    {0x5e, 0x7d},       /* REGSD - Register 5E  RW
                           Bit[7:0]: AVGsel[15:8], 16-zone average weight
                                     option
                                                                              */
    {0x5f, 0x7d},       /* REGSD - Register 5F  RW
                           Bit[7:0]: AVGsel[23:16], 16-zone average weight
                                     option
                                                                              */
    {0x60, 0x55},       /* REGSD - Register 60  RW
                           Bit[7:0]: AVGsel[31:24], 16-zone average weight
                                     option
                                                                              */
    {0x61, 0x70},       /* HISTO_LOW  RW             default = 0x80
                           Histogram Algorithm Low Level
                                                                              */
    {0x62, 0x80},       /* HISTO_LOW  RW             default = 0x90
                           Histogram Algorithm High Level
                                                                              */
    {0x7c, 0x05},       /* Reserved                                           */
    {0x20, 0x80},       /* Reserved                                           */
    {0x28, 0x30},       /* Reserved                                           */
    {0x6c, 0x00},       /* Reserved                                           */
    {0x6d, 0x80},       /* Reserved                                           */
    {0x6e, 0x00},       /* Reserved                                           */
    {0x70, 0x02},       /* Reserved                                           */
    {0x71, 0x94},       /* Reserved                                           */
    {0x73, 0xc1},       /* Reserved                                           */
    {0x3d, 0x34},       /* Reserved                                           */
    {0x12, 0b00000100}, /* COM7 - Common Control 7  RW
                               Bit[7]: SRST
                                   1: Initiates system reset. All registers are
                                      set to factory default values after which
                                      the chip resumes normal operation
                               Bit[6:4]: Resolution selection
                                   000: UXGA (full size) mode
                                   001: CIF mode
                                   100: SVGA mode
                               Bit[3]: Reserved
                               Bit[2]: Zoom mode
                               Bit[1]: Color bar test pattern
                                   0: OFF
                                   1: ON
                               Bit[O]: Reserved


                           TODO: Check how ArduCAM has
                                                                              */
    {0x5a, 0x57},       /* Reserved                                           */
    {0x4f, 0xbb},       /* BD50  RW             default = 0xCA
                           50Hz Banding AEC 8 LSBs
                                                                              */
    {0x50, 0x9c},       /* BD60  RW              default = 0xA8
                           60Hz Banding AEC 8 LSBs
                                                                              */
    {0xff, 0x00},       /* Switch to device control register list Table 12    */
    {0xe5, 0x7f},       /* Reserved                                           */
    {0xf9, 0b11000000}, /* MC_BIST  RW
                               Bit[7]: Microcontroller Reset
                               Bit[6]: Boot ROM select
                               Bit[5]: R/W 1 error for 12K-byte memory
                               Bit[4]: R/W 0 error for 12K-byte memory
                               Bit[3]: R/W 1 error for 512-byte memory
                               Bit[2]: R/W 0 error for 512-byte memory
                               Bit[1]: BIST busy bit for read; One-shot reset of
                               microcontroller for write
                               Bit[O]: Launch BIST


                           TODO: Check how ArduCAM has
                                                                              */
    {0x41, 0x24},       /* Reserved                                           */
    {0xe0, 0b00000100}, /* RESET  RW        default = 0b00000100  was:0b00010100
                               Bit[7]: Reserved
                               Bit[6]: Microcontroller
                               Bit[5]: SCCB
                               Bit[4]: JPEG
                               Bit[3]: Reserved
                               Bit[2]: DVP
                               Bit[1]: IPU
                               Bit[O}: CIF
                                                                              */
    {0x76, 0xff},       /* Reserved                                           */
    {0x33, 0xa0},       /* Reserved                                           */
    {0x42, 0x20},       /* Reserved                                           */
    {0x43, 0x18},       /* Reserved                                           */
    {0x4c, 0x00},       /* Reserved                                           */
    {0x87, 0b11010000}, /* CTRL3  RW                        default = 0b01010000
                               Module Enable
                                   Bit[7]:   BPC
                                   Bit[6]:   WPC
                                   Bit[5:0]: Reserved
                                                                              */
    {0x88, 0x3f},       /* Reserved                                           */
    {0xd7, 0x03},       /* Reserved                                           */
    {0xd9, 0x10},       /* Reserved                                           */
    {0xd3, 0b10000010}, /* R_DVP_SP  RW                     default = 0b10000010
                               Bit[7]:   Auto mode
                               Bit[6:0]: DVP output speed control
                                         DVP PCLK = sysclk (48)/[6:0] (YUVO)
                                                  = sysclk (48)/(2*[6:0]) (RAV)

                           TODO: It needs to be calculated carefully. Check how
                           ArduCAM has
                                                                              */
    {0xc8, 0x08},       /* Reserved                                           */
    {0xc9, 0x80},       /* Reserved                                           */

    // Below it seems like single registers are written more times, but it's
    // SDE. One command gives address and second data. Why? I do not know.

    {0x7c, 0x00}, /* BPADDR[3:0]  RW                              default = 0x00
                   SDE Indirect Register Access: Address
                                                                              */
    {0x7d, 0x00}, /* BPDATA[7:0]  RW                              default = 0x00
                   SDE Indirect Register Access: Data
                                                                              */
    {0x7c, 0x03}, /* BPADDR[3:0]  RW                              default = 0x00
                   SDE Indirect Register Access: Address
                                                                              */
    {0x7d, 0x48}, /* BPDATA[7:0]  RW                              default = 0x00
                   SDE Indirect Register Access: Data
                                                                              */
    {0x7d, 0x48},
    {0x7c, 0x08}, /* BPADDR[3:0]  RW                              default = 0x00
                   SDE Indirect Register Access: Address
                                                                              */
    {0x7d, 0x20}, /* BPDATA[7:0]  RW                              default = 0x00
                   SDE Indirect Register Access: Data
                                                                              */
    {0x7d, 0x10},
    {0x7d, 0x0e},
    {0x90, 0x00},       /* Reserved                                           */
    {0x91, 0x0e},       /* Reserved                                           */
    {0x91, 0x1a},       /* Reserved                                           */
    {0x91, 0x31},       /* Reserved                                           */
    {0x91, 0x5a},       /* Reserved                                           */
    {0x91, 0x69},       /* Reserved                                           */
    {0x91, 0x75},       /* Reserved                                           */
    {0x91, 0x7e},       /* Reserved                                           */
    {0x91, 0x88},       /* Reserved                                           */
    {0x91, 0x8f},       /* Reserved                                           */
    {0x91, 0x96},       /* Reserved                                           */
    {0x91, 0xa3},       /* Reserved                                           */
    {0x91, 0xaf},       /* Reserved                                           */
    {0x91, 0xc4},       /* Reserved                                           */
    {0x91, 0xd7},       /* Reserved                                           */
    {0x91, 0xe8},       /* Reserved                                           */
    {0x91, 0x20},       /* Reserved                                           */
    {0x92, 0x00},       /* Reserved                                           */
    {0x93, 0x06},       /* Reserved                                           */
    {0x93, 0xe3},       /* Reserved                                           */
    {0x93, 0x05},       /* Reserved                                           */
    {0x93, 0x05},       /* Reserved                                           */
    {0x93, 0x00},       /* Reserved                                           */
    {0x93, 0x04},       /* Reserved                                           */
    {0x93, 0x00},       /* Reserved                                           */
    {0x93, 0x00},       /* Reserved                                           */
    {0x93, 0x00},       /* Reserved                                           */
    {0x93, 0x00},       /* Reserved                                           */
    {0x93, 0x00},       /* Reserved                                           */
    {0x93, 0x00},       /* Reserved                                           */
    {0x93, 0x00},       /* Reserved                                           */
    {0x96, 0x00},       /* Reserved                                           */
    {0x97, 0x08},       /* Reserved                                           */
    {0x97, 0x19},       /* Reserved                                           */
    {0x97, 0x02},       /* Reserved                                           */
    {0x97, 0x0c},       /* Reserved                                           */
    {0x97, 0x24},       /* Reserved                                           */
    {0x97, 0x30},       /* Reserved                                           */
    {0x97, 0x28},       /* Reserved                                           */
    {0x97, 0x26},       /* Reserved                                           */
    {0x97, 0x02},       /* Reserved                                           */
    {0x97, 0x98},       /* Reserved                                           */
    {0x97, 0x80},       /* Reserved                                           */
    {0x97, 0x00},       /* Reserved                                           */
    {0x97, 0x00},       /* Reserved                                           */
    {0xc3, 0b11101101}, /* CTRL1  RW            default = 0xFF ArduCAM: 11101111
                               Module Enable
                                   Bit[7]: CIP
                                   Bit[6]: DMY
                                   Bit[5]: RAW_GMA
                                   Bit[4]: DG
                                   Bit[3]: AWB - Automatic White Balance
                                   Bit[2]: AWB_GAIN
                                   Bit[1]: LENC - LENC color shading correction
                                   Bit[O]: PRE
                                                                              */
    {0xa4, 0x00},       /* Reserved                                           */
    {0xa8, 0x00},       /* Reserved                                           */
    {0xc5, 0x11},       /* Reserved                                           */
    {0xc6, 0x51},       /* Reserved                                           */
    {0xbf, 0x80},       /* Reserved                                           */
    {0xc7, 0x10},       /* Reserved                                           */
    {0xb6, 0x66},       /* Reserved                                           */
    {0xb8, 0xA5},       /* Reserved                                           */
    {0xb7, 0x64},       /* Reserved                                           */
    {0xb9, 0x7C},       /* Reserved                                           */
    {0xb3, 0xaf},       /* Reserved                                           */
    {0xb4, 0x97},       /* Reserved                                           */
    {0xb5, 0xFF},       /* Reserved                                           */
    {0xb0, 0xC5},       /* Reserved                                           */
    {0xb1, 0x94},       /* Reserved                                           */
    {0xb2, 0x0f},       /* Reserved                                           */
    {0xc4, 0x5c},       /* Reserved                                           */

    // Size of an image is given to camera on couple of registers below.
    {0xc0, IMAGE_RESOLUTION_WIDTH >> 3},  /* HSIZE8[7:0]  RW
                                             Image Horizontal Size HSIZE[10:3]
                                             default = 0x80 = 128
                                             ArduCAM = 0xc8 = 200

                           TODO: It needs to be calculated carefully
                                                                              */
    {0xc1, IMAGE_RESOLUTION_HEIGHT >> 3}, /* VSIZE8[7:0]  RW
                                             Image Horizontal Size VSIZE[10:3]
                                             default = 0x60 = 96
                                             ArduCAM = 0x96 = 150

                           TODO: It needs to be calculated carefully
                                                                              */
    {0x8c, (((IMAGE_RESOLUTION_WIDTH >> 11) & 0b1) << 6) |
               ((IMAGE_RESOLUTION_WIDTH & 0b111) << 3) |
               (IMAGE_RESOLUTION_HEIGHT &
                0b111)}, /*                  SIZEL[5:0]  (not [6:0]?)  RW
                                             {HSIZE[11], HSIZE[2:0], VSIZE[2:0]}
                                             default = 0x00 ArduCAM: 0x00

                           TODO: It needs to be calculated carefully
                                                                              */
    {0x86, 0b00111101}, /* CTRL2  RW      default = 0b00001101 ArduCAM: 00111101
                               Module Enable
                                   Bit[7:6]: Reserved
                                   Bit[5]:   DCW
                                   Bit[4]:   SDE
                                   Bit[3]:   UV_ADJ
                                   Bit[2]:   UV_AVG
                                   Bit[1]:   Reserved
                                   Bit[O]:   CMX
                                                                              */
    {0x50, 0b00000000}, /* CTRLI[7:0]  RW           default = 0x00 ArduCAM: 0x00
                               Bit[7]: LP_DP
                               Bit[6]: Round
                               Bit[5:3]: V_DIVIDER
                               Bit[2:0]: H_DIVIDER
                           TODO: I have no idea why we need to divide V and H
                           and if it is a size value or what.
                                                                              */
    {0x51, 0xC8},       /* HSIZE[7:0]  RW
                           default = 64 ArduCAM = 200 skovholm = 144
                               H_SIZE[7:0] (real/4)

                           TODO: It needs to be calculated carefully

                           TODO: no idea where this value come from as I cannot
                                 calculate it from ArduCAM's previous height and
                                 width values
                                                                              */
    {0x52, 0x2c},       /* VSIZE[7:0]  RW
                           default = 240 ArduCAM = 150 skovholm = 44
                               V_SIZE[7:0] (real/4)
                               TODO: It needs to be calculated carefully
                                                                              */
    {0x53, 0x00},       /* XOFFL[7:0]  RW
                           default = 0 ArduCAM = 0 skovholm = 0
                               OFFSET_X[7:0]
                                                                              */
    {0x54, 0x00},       /* YOFFL[7:0]  RW
                           default = 0 ArduCAM = 0 skovholm = 0
                               OFFSET_Y[7:0]
                                                                              */
    {0x55, 0b10001000}, /* VHYX[7:0]  RW
                           default = 0b1000 ArduCAM = 0 skovholm = 0b10001000
                               Bit[7]: V_SIZE[8]
                               Bit[6:4]: OFFSET_Y[10:8]
                               Bit[3]: H_SIZE[8]
                               Bit[2:0]: OFFSET_X[10:8]
                               TODO: It needs to be calculated carefully
                                                                              */
    {0x57, 0x00},       /* TEST[3:0]  RW    default = 0 ArduCAM = 0 skovholm = 0
                               Bit[7]:   H_SIZE[9]
                               Bit[6:0]: Reserved
                               TODO: It needs to be calculated carefully
                                                                              */
    {0x5a, 80},         /* ZMOW[7:0]  RW
                           default = 88 ArduCAM = 200 skovholm = 80
                               OUTW[7:0] (real/4)
                               TODO: It needs to be calculated carefully
                                                                              */
    {0x5b, 60},         /* ZMOH[7:0]  RW
                           default = 72 ArduCAM = 150 skovholm = 60
                               OUTH[7:0] (real/4)
                               TODO: It needs to be calculated carefully
                                                                              */
    {0x5c, 0},          /* ZMHH[1:0]  RW    default = 0 ArduCAM = 0 skovholm = 0
                               Bit[7:4]: ZMSPD (zoom speed)
                               Bit[2]:   OUTH[8]
                               Bit[1:0]: OUTW[9:8]
                                                                              */
    {0xd3, 8},          /* R_DVP_SP  RW
                              default = 0b10000010  ArduCAM = 4 || 2  skovholm = 8
                                  Bit[7]: Auto mode
                                  Bit[6:0]: DVP output speed control
                                            DVP PCLK = sysclk (48)/[6:0] (YUVO);
                                                     = sysclk (48)/(2*[6:0]) (RAW)
                                  TODO: Problematic parameter probably
                                                                                 */
    {0xc3, 0b11101101}, /* CTRL1  RW
                         default = 0xFF ArduCAM: 0b11101101 skovholm: 0b11101111
                               Module Enable Bit[7]: CIP Bit[6]: DMY
                                   Bit[5]: RAW_GMA
                                   Bit[4]: DG
                                   Bit[3]: AWB - Automatic White Balance
                                   Bit[2]: AWB_GAIN
                                   Bit[1]: LENC - LENC color shading correction
                                   Bit[O]: PRE
                               TODO: Check LENC enabled
                                                                              */
    {0x7f, 0x00},       /* Reserved                                           */
    {0xda, 0b1001},     /* IMAGE_MODE  ?RW?
                           default = 0 ArduCAM = 0b00010000 skovholm = 0b1001
                           Image Output Format Select
                               Bit[7]: Reserved
                               Bit[6]: Y8 enable for DVP
                               Bit[5]: Reserved
                               Bit[4]: JPEG output enable
                                   0: Non-compressed
                                   1: JPEG output
                               Bit[3:2]: DVP output format
                                   00: YUV422
                                   01: RAW10 (DVP)
                                   10: RGB565
                                   11: Reserved
                              Bit[1]: HREF timing select in DVP JPEG output mode
                                   0: HREF is same as sensor
                                   1: HREF = VSYNC
                               Bit[O]: Byte swap enable for DVP
                                   0: High byte first YUYV (C2[4] = 0)
                                                      YVYU (C2[4] = 1)
                                   1: Low byte first UYVY (C2[4] = 0)
                                                     VYUY (C2[4] = 1)
                                                                              */
    {0xe5, 0x1f},       /* Reserved                                           */
    {0xe1, 0x67},       /* Reserved                                           */
    {0xe0, 0},          /* RESET  RW   default = 0b0100 ArduCAM = 0 skovholm = 0
                               Bit[7]: Reserved
                               Bit[6]: Microcontroller
                               Bit[5]: SCCB
                               Bit[4]: JPEG
                               Bit[3]: Reserved
                               Bit[2]: DVP
                               Bit[1]: IPU
                               Bit[O}: CIF
                                                                              */
    {0xdd, 0x7f},       /* Reserved                                           */
    {0x05, 0},          /* R_BYPASS  RW  default = 1 ArduCAM = 0 skovholm = 0
                               Bypass DSP
                                   Bit[7:1]: Reserved
                                   Bit[O]: Bypass DSP select
                                       0: DSP
                                       1: Bypass DSP, sensor out directly
                                                                              */
    {0x12, 0x40},       /* Reserved                                           */

    // Below are some duplicated register values that I decided to not write
    // again. Left in case that it will broke something
    /*{0xd3, 0x04},        AGAIN R_DVP_SP  RW
                           default = 0b10000010  ArduCAM = 4 || 2  skovholm = 8
                                                                              */
    /*{0xc0, 0xc8},        AGAIN HSIZE8[7:0]  RW
                       default = 0x80 ArduCAM = 0x16 < different skovholm = 0xc8
                                                                              */
    /*{0xc1, 0x96},        AGAIN VSIZE8[7:0]  RW
                       default = 0x60 ArduCAM = 0x12 < different skovholm = 0x96
                                                                              */
    /*{0x8c, 0x00},        AGAIN SIZEL[5:0]  (not [6:0]?)  RW
                           default = 0x00 ArduCAM = 0x00 skovholm = 0x00
                                                                              */
    /*{0x86, 0x3d},        AGAIN CTRL2  RW
                           default = 0xOD ArduCAM = 0x3d skovholm = 0x35
                                                                              */
    /*{0x50, 0x80},        AGAIN CTRLI[7:0]  RW
                           default = 0x00 ArduCAM = 0x00 skovholm = 0x80
                                                                              */
    /*{0x51, 0x90},        AGAIN HSIZE[7:0]  RW
                           default = 64 ArduCAM = 44 < different skovholm = 144
                                                                              */
    /*{0x52, 0x2c},        AGAIN VSIZE[7:0]  RW
                           default = 240 ArduCAM = 36 < different skovholm = 44
                                                                              */
    /*{0x53, 0x00},        AGAIN XOFFL[7:0]  RW
                           default = 0 ArduCAM = 0 skovholm = 0
                                                                              */
    /*{0x54, 0x00},        AGAIN YOFFL[7:0]  RW
                           default = 0 ArduCAM = 0 skovholm = 0
                                                                              */
    /*{0x55, 0b10001000},  AGAIN VHYX[7:0]  RW
                           default = 0b1000 ArduCAM = 0 skovholm = 0b10001000
                                                                              */
    /*{0x5a, 0x5A},        AGAIN ZMOW[7:0]  RW
                           default = 88 ArduCAM = 44 < different
                           skovholm = 90 < different
                                                                              */
    /*{0x5b, 0x5A},        AGAIN ZMOH[7:0]  RW
                           default = 72 ArduCAM = 36 < different
                           skovholm = 90 < different
                                                                              */
    /*{0x5c, 0x00},        AGAIN ZMHH[1:0]  RW
                           default = 0 ArduCAM = 0 skovholm = 0
                                                                              */

    // Below some additional registers described in ArduCAM code
    {0xc7, 0x40}, /* AGAIN Reserved
                     ArduCAM = 0x10 skovholm = 0x40
                         0x00  AWB ON
                         0x40  AWB OFF
                                                                              */
    {0xcc, 0x42}, /* AGAIN Reserved
                     ArduCAM Home = 0x42 skovholm = 0x5e
                         0x5e  sunny
                                                                              */
    {0xcd, 0x3f}, /* AGAIN Reserved
                     ArduCAM Home = 0x3f skovholm = 0x41
                                                                              */
    {0xce, 0x71}, /* AGAIN Reserved
                     ArduCAM Home = 0x71 skovholm = 0x54
                                                                              */
    {0xff, 0xff}  /* Switch to device control register list Table 13
                     Something like ending transmission. I do not know if
                     that means anything for the camera/
                                                                              */
};

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

    for (index = 0; index < (sizeof(InitializationSequence) / 2); index++)
    {
        cameraWrite(InitializationSequence[index][0],
                    InitializationSequence[index][1]);
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
