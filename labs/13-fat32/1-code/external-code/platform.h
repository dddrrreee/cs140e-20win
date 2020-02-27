/*
 * Copyright (c) 2014 Marco Maccaferri and Others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <unistd.h>
#include <stdint.h>

#define PERIPHERAL_BASE                 0x20000000 // Peripheral Base Address
#define BUS_ADDRESSES_l2CACHE_ENABLED   0x40000000 // Bus Addresses: disable_l2cache=0
#define BUS_ADDRESSES_l2CACHE_DISABLED  0xC0000000 // Bus Addresses: disable_l2cache=1

#define IO_TO_BUS(x)                    (((uint32_t)(x) & 0x00FFFFFF) | 0x7E000000)
#define BUS_ADDRES(x)                   (((uint32_t)(x) & 0x3FFFFFFF) | BUS_ADDRESSES_l2CACHE_ENABLED)

#define MAIL_BASE   (PERIPHERAL_BASE +   0xB880) // Mailbox Base Address
#define I2C0_BASE   (PERIPHERAL_BASE + 0x205000)
#define I2C1_BASE   (PERIPHERAL_BASE + 0x804000)
#define UART0_BASE  (PERIPHERAL_BASE + 0x201000)
#define UART1_BASE  (PERIPHERAL_BASE + 0x215000)
#define TIMER_BASE  (PERIPHERAL_BASE + 0x003000)

/* Mailbox */
#define MAIL_READ      0x00 // Mailbox Read Register
#define MAIL_CONFIG    0x1C // Mailbox Config Register
#define MAIL_STATUS    0x18 // Mailbox Status Register
#define MAIL_WRITE     0x20 // Mailbox Write Register

#define MAIL_EMPTY  0x40000000 // Mailbox Status Register: Mailbox Empty (There is nothing to read from the Mailbox)
#define MAIL_FULL   0x80000000 // Mailbox Status Register: Mailbox Full  (There is no space to write into the Mailbox)

#define MAIL_POWER    0x0 // Mailbox Channel 0: Power Management Interface
#define MAIL_FB       0x1 // Mailbox Channel 1: Frame Buffer
#define MAIL_VUART    0x2 // Mailbox Channel 2: Virtual UART
#define MAIL_VCHIQ    0x3 // Mailbox Channel 3: VCHIQ Interface
#define MAIL_LEDS     0x4 // Mailbox Channel 4: LEDs Interface
#define MAIL_BUTTONS  0x5 // Mailbox Channel 5: Buttons Interface
#define MAIL_TOUCH    0x6 // Mailbox Channel 6: Touchscreen Interface
#define MAIL_COUNT    0x7 // Mailbox Channel 7: Counter
#define MAIL_TAGS     0x8 // Mailbox Channel 8: Tags (ARM to VC)

/* Tags (ARM to VC) */
#define Get_Firmware_Revision  0x00000001 // VideoCore: Get Firmware Revision (Response: Firmware Revision)
#define Get_Board_Model        0x00010001 // Hardware: Get Board Model (Response: Board Model)
#define Get_Board_Revision     0x00010002 // Hardware: Get Board Revision (Response: Board Revision)
#define Get_Board_MAC_Address  0x00010003 // Hardware: Get Board MAC Address (Response: MAC Address In Network Byte Order)
#define Get_Board_Serial       0x00010004 // Hardware: Get Board Serial (Response: Board Serial)
#define Get_ARM_Memory         0x00010005 // Hardware: Get ARM Memory (Response: Base Address In Bytes, Size In Bytes)
#define Get_VC_Memory          0x00010006 // Hardware: Get VC Memory (Response: Base Address In Bytes, Size In Bytes)
#define Get_Clocks             0x00010007 // Hardware: Get Clocks (Response: Parent Clock ID (0 For A Root Clock), Clock ID)
#define Get_Power_State        0x00020001 // Power: Get Power State (Response: Device ID, State)
#define Get_Timing             0x00020002 // Power: Get Timing (Response: Device ID, Enable Wait Time In Microseconds)
#define Set_Power_State        0x00028001 // Power: Set Power State (Response: Device ID, State)
#define Get_Clock_State        0x00030001 // Clocks: Get Clock State (Response: Clock ID, State)
#define Get_Clock_Rate         0x00030002 // Clocks: Get Clock Rate (Response: Clock ID, Rate In Hz)
#define Get_Voltage            0x00030003 // Voltage: Get Voltage (Response: Voltage ID, Value Offset From 1.2V In Units Of 0.025V)
#define Get_Max_Clock_Rate     0x00030004 // Clocks: Get Max Clock Rate (Response: Clock ID, Rate In Hz)
#define Get_Max_Voltage        0x00030005 // Voltage: Get Max Voltage (Response: Voltage ID, Value Offset From 1.2V In Units Of 0.025V)
#define Get_Temperature        0x00030006 // Voltage: Get Temperature (Response: Temperature ID, Value In Degrees C)
#define Get_Min_Clock_Rate     0x00030007 // Clocks: Get Min Clock Rate (Response: Clock ID, Rate In Hz)
#define Get_Min_Voltage        0x00030008 // Voltage: Get Min Voltage (Response: Voltage ID, Value Offset From 1.2V In Units Of 0.025V)
#define Get_Turbo              0x00030009 // Clocks: Get Turbo (Response: ID, Level)
#define Get_Max_Temperature    0x0003000A // Voltage: Get Max Temperature (Response: Temperature ID, Value In Degrees C)
#define Allocate_Memory        0x0003000C // Memory: Allocates Contiguous Memory On The GPU (Response: Handle)
#define Lock_Memory            0x0003000D // Memory: Lock Buffer In Place, And Return A Bus Address (Response: Bus Address)
#define Unlock_Memory          0x0003000E // Memory: Unlock Buffer (Response: Status)
#define Release_Memory         0x0003000F // Memory: Free The Memory Buffer (Response: Status)
#define Execute_Code           0x00030010 // Memory: Calls The Function At Given (Bus) Address And With Arguments Given
#define Execute_QPU            0x00030011 // QPU: Calls The QPU Function At Given (Bus) Address And With Arguments Given (Response: Number Of QPUs, Control, No Flush, Timeout In ms)
#define Enable_QPU             0x00030012 // QPU: Enables The QPU (Response: Enable State)
#define Get_EDID_Block         0x00030020 // HDMI: Read Specificed EDID Block From Attached HDMI/DVI Device (Response: Block Number, Status, EDID Block (128 Bytes))
#define Set_Clock_State        0x00038001 // Clocks: Set Clock State (Response: Clock ID, State)
#define Set_Clock_Rate         0x00038002 // Clocks: Set Clock Rate (Response: Clock ID, Rate In Hz)
#define Set_Voltage            0x00038003 // Voltage: Set Voltage (Response: Voltage ID, Value Offset From 1.2V In Units Of 0.025V)
#define Set_Turbo              0x00038009 // Clocks: Set Turbo (Response: ID, Level)
#define Allocate_Buffer        0x00040001 // Frame Buffer: Allocate Buffer (Response: Frame Buffer Base Address In Bytes, Frame Buffer Size In Bytes)
#define Blank_Screen           0x00040002 // Frame Buffer: Blank Screen (Response: State)
#define Get_Physical_Display   0x00040003 // Frame Buffer: Get Physical (Display) Width/Height (Response: Width In Pixels, Height In Pixels)
#define Get_Virtual_Buffer     0x00040004 // Frame Buffer: Get Virtual (Buffer) Width/Height (Response: Width In Pixels, Height In Pixels)
#define Get_Depth              0x00040005 // Frame Buffer: Get Depth (Response: Bits Per Pixel)
#define Get_Pixel_Order        0x00040006 // Frame Buffer: Get Pixel Order (Response: State)
#define Get_Alpha_Mode         0x00040007 // Frame Buffer: Get Alpha Mode (Response: State)
#define Get_Pitch              0x00040008 // Frame Buffer: Get Pitch (Response: Bytes Per Line)
#define Get_Virtual_Offset     0x00040009 // Frame Buffer: Get Virtual Offset (Response: X In Pixels, Y In Pixels)
#define Get_Overscan           0x0004000A // Frame Buffer: Get Overscan (Response: Top In Pixels, Bottom In Pixels, Left In Pixels, Right In Pixels)
#define Get_Palette            0x0004000B // Frame Buffer: Get Palette (Response: RGBA Palette Values (Index 0 To 255))
#define Test_Physical_Display  0x00044003 // Frame Buffer: Test Physical (Display) Width/Height (Response: Width In Pixels, Height In Pixels)
#define Test_Virtual_Buffer    0x00044004 // Frame Buffer: Test Virtual (Buffer) Width/Height (Response: Width In Pixels, Height In Pixels)
#define Test_Depth             0x00044005 // Frame Buffer: Test Depth (Response: Bits Per Pixel)
#define Test_Pixel_Order       0x00044006 // Frame Buffer: Test Pixel Order (Response: State)
#define Test_Alpha_Mode        0x00044007 // Frame Buffer: Test Alpha Mode (Response: State)
#define Test_Virtual_Offset    0x00044009 // Frame Buffer: Test Virtual Offset (Response: X In Pixels, Y In Pixels)
#define Test_Overscan          0x0004400A // Frame Buffer: Test Overscan (Response: Top In Pixels, Bottom In Pixels, Left In Pixels, Right In Pixels)
#define Test_Palette           0x0004400B // Frame Buffer: Test Palette (Response: RGBA Palette Values (Index 0 To 255))
#define Release_Buffer         0x00048001 // Frame Buffer: Release Buffer (Response: Releases And Disables The Frame Buffer)
#define Set_Physical_Display   0x00048003 // Frame Buffer: Set Physical (Display) Width/Height (Response: Width In Pixels, Height In Pixels)
#define Set_Virtual_Buffer     0x00048004 // Frame Buffer: Set Virtual (Buffer) Width/Height (Response: Width In Pixels, Height In Pixels)
#define Set_Depth              0x00048005 // Frame Buffer: Set Depth (Response: Bits Per Pixel)
#define Set_Pixel_Order        0x00048006 // Frame Buffer: Set Pixel Order (Response: State)
#define Set_Alpha_Mode         0x00048007 // Frame Buffer: Set Alpha Mode (Response: State)
#define Set_Virtual_Offset     0x00048009 // Frame Buffer: Set Virtual Offset (Response: X In Pixels, Y In Pixels)
#define Set_Overscan           0x0004800A // Frame Buffer: Set Overscan (Response: Top In Pixels, Bottom In Pixels, Left In Pixels, Right In Pixels)
#define Set_Palette            0x0004800B // Frame Buffer: Set Palette (Response: RGBA Palette Values (Index 0 To 255))
#define Get_Command_Line       0x00050001 // Config: Get Command Line (Response: ASCII Command Line String)
#define Get_DMA_Channels       0x00060001 // Shared Resource Management: Get DMA Channels (Response: Bits 0-15: DMA channels 0-15)

/* Power: Unique Device ID's */
#define PWR_SD_Card_ID  0x0 // SD Card
#define PWR_UART0_ID    0x1 // UART0
#define PWR_UART1_ID    0x2 // UART1
#define PWR_USB_HCD_ID  0x3 // USB HCD
#define PWR_I2C0_ID     0x4 // I2C0
#define PWR_I2C1_ID     0x5 // I2C1
#define PWR_I2C2_ID     0x6 // I2C2
#define PWR_SPI_ID      0x7 // SPI
#define PWR_CCP2TX_ID   0x8 // CCP2TX

/* Clocks: Unique Clock ID's */
#define CLK_EMMC_ID     0x1 // EMMC
#define CLK_UART_ID     0x2 // UART
#define CLK_ARM_ID      0x3 // ARM
#define CLK_CORE_ID     0x4 // CORE
#define CLK_V3D_ID      0x5 // V3D
#define CLK_H264_ID     0x6 // H264
#define CLK_ISP_ID      0x7 // ISP
#define CLK_SDRAM_ID    0x8 // SDRAM
#define CLK_PIXEL_ID    0x9 // PIXEL
#define CLK_PWM_ID      0xA // PWM

/* Voltage: Unique Voltage ID's */
#define VLT_Core_ID     0x1 // Core
#define VLT_SDRAM_C_ID  0x2 // SDRAM_C
#define VLT_SDRAM_P_ID  0x3 // SDRAM_P
#define VLT_SDRAM_I_ID  0x4 // SDRAM_I

/* CM / Clock Manager */

struct _clock {
    uint32_t GNRICCTL;  // Clock Manager Generic Clock Control
    uint32_t GNRICDIV; // Clock Manager Generic Clock Divisor
    uint32_t VPUCTL;   // Clock Manager VPU Clock Control
    uint32_t VPUDIV;   // Clock Manager VPU Clock Divisor
    uint32_t SYSCTL;   // Clock Manager System Clock Control
    uint32_t SYSDIV;   // Clock Manager System Clock Divisor
    uint32_t PERIACTL; // Clock Manager PERIA Clock Control
    uint32_t PERIADIV; // Clock Manager PERIA Clock Divisor
    uint32_t PERIICTL; // Clock Manager PERII Clock Control
    uint32_t PERIIDIV; // Clock Manager PERII Clock Divisor
    uint32_t H264CTL;  // Clock Manager H264 Clock Control
    uint32_t H264DIV;  // Clock Manager H264 Clock Divisor
    uint32_t ISPCTL;   // Clock Manager ISP Clock Control
    uint32_t ISPDIV;   // Clock Manager ISP Clock Divisor
    uint32_t V3DCTL;   // Clock Manager V3D Clock Control
    uint32_t V3DDIV;   // Clock Manager V3D Clock Divisor
    uint32_t CAM0CTL;  // Clock Manager Camera 0 Clock Control
    uint32_t CAM0DIV;  // Clock Manager Camera 0 Clock Divisor
    uint32_t CAM1CTL;  // Clock Manager Camera 1 Clock Control
    uint32_t CAM1DIV;  // Clock Manager Camera 1 Clock Divisor
    uint32_t CCP2CTL;  // Clock Manager CCP2 Clock Control
    uint32_t CCP2DIV;  // Clock Manager CCP2 Clock Divisor
    uint32_t DSI0ECTL; // Clock Manager DSI0E Clock Control
    uint32_t DSI0EDIV; // Clock Manager DSI0E Clock Divisor
    uint32_t DSI0PCTL; // Clock Manager DSI0P Clock Control
    uint32_t DSI0PDIV; // Clock Manager DSI0P Clock Divisor
    uint32_t DPICTL;   // Clock Manager DPI Clock Control
    uint32_t DPIDIV;   // Clock Manager DPI Clock Divisor
    uint32_t GP0CTL;   // Clock Manager General Purpose 0 Clock Control
    uint32_t GP0DIV;   // Clock Manager General Purpose 0 Clock Divisor
    uint32_t GP1CTL;   // Clock Manager General Purpose 1 Clock Control
    uint32_t GP1DIV;   // Clock Manager General Purpose 1 Clock Divisor
    uint32_t GP2CTL;   // Clock Manager General Purpose 2 Clock Control
    uint32_t GP2DIV;   // Clock Manager General Purpose 2 Clock Divisor
    uint32_t HSMCTL;   // Clock Manager HSM Clock Control
    uint32_t HSMDIV;   // Clock Manager HSM Clock Divisor
    uint32_t OTPCTL;   // Clock Manager OTP Clock Control
    uint32_t OTPDIV;   // Clock Manager OTP Clock Divisor
    uint32_t PCMCTL;   // Clock Manager PCM / I2S Clock Control
    uint32_t PCMDIV;   // Clock Manager PCM / I2S Clock Divisor
    uint32_t PWMCTL;   // Clock Manager PWM Clock Control
    uint32_t PWMDIV;   // Clock Manager PWM Clock Divisor
    uint32_t SLIMCTL;  // Clock Manager SLIM Clock Control
    uint32_t SLIMDIV;  // Clock Manager SLIM Clock Divisor
    uint32_t SMICTL;   // Clock Manager SMI Clock Control
    uint32_t SMIDIV;   // Clock Manager SMI Clock Divisor
    uint32_t TCNTCTL;  // Clock Manager TCNT Clock Control
    uint32_t TCNTDIV;  // Clock Manager TCNT Clock Divisor
    uint32_t TECCTL;   // Clock Manager TEC Clock Control
    uint32_t TECDIV;   // Clock Manager TEC Clock Divisor
    uint32_t TD0CTL;   // Clock Manager TD0 Clock Control
    uint32_t TD0DIV;   // Clock Manager TD0 Clock Divisor
    uint32_t TD1CTL;   // Clock Manager TD1 Clock Control
    uint32_t TD1DIV;   // Clock Manager TD1 Clock Divisor
    uint32_t TSENSCTL; // Clock Manager TSENS Clock Control
    uint32_t TSENSDIV; // Clock Manager TSENS Clock Divisor
    uint32_t TIMERCTL; // Clock Manager Timer Clock Control
    uint32_t TIMERDIV; // Clock Manager Timer Clock Divisor
    uint32_t UARTCTL;  // Clock Manager UART Clock Control
    uint32_t UARTDIV;  // Clock Manager UART Clock Divisor
    uint32_t VECCTL;   // Clock Manager VEC Clock Control
    uint32_t VECDIV;   // Clock Manager VEC Clock Divisor
    uint32_t OSCCOUNT; // Clock Manager Oscillator Count
    uint32_t PLLA;     // Clock Manager PLLA
    uint32_t PLLC;     // Clock Manager PLLC
    uint32_t PLLD;     // Clock Manager PLLD
    uint32_t PLLH;     // Clock Manager PLLH
    uint32_t LOCK;     // Clock Manager Lock
    uint32_t EVENT;    // Clock Manager Event
    uint32_t INTEN;    // Clock Manager INTEN
    uint32_t DSI0HSCK; // Clock Manager DSI0HSCK
    uint32_t CKSM;     // Clock Manager CKSM
    uint32_t OSCFREQI; // Clock Manager Oscillator Frequency Integer
    uint32_t OSCFREQF; // Clock Manager Oscillator Frequency Fraction
    uint32_t PLLTCTL;  // Clock Manager PLLT Control
    uint32_t PLLTCNT0; // Clock Manager PLLT0
    uint32_t PLLTCNT1; // Clock Manager PLLT1
    uint32_t PLLTCNT2; // Clock Manager PLLT2
    uint32_t PLLTCNT3; // Clock Manager PLLT3
    uint32_t TDCLKEN;  // Clock Manager TD Clock Enable
    uint32_t BURSTCTL; // Clock Manager Burst Control
    uint32_t BURSTCNT; // Clock Manager Burst
    uint32_t fill1[2];
    uint32_t DSI1ECTL; // Clock Manager DSI1E Clock Control
    uint32_t DSI1EDIV; // Clock Manager DSI1E Clock Divisor
    uint32_t DSI1PCTL; // Clock Manager DSI1P Clock Control
    uint32_t DSI1PDIV; // Clock Manager DSI1P Clock Divisor
    uint32_t DFTCTL;   // Clock Manager DFT Clock Control
    uint32_t DFTDIV;   // Clock Manager DFT Clock Divisor
    uint32_t PLLB;     // Clock Manager PLLB
    uint32_t fill2[3];
    uint32_t PULSECTL; // Clock Manager Pulse Clock Control
    uint32_t PULSEDIV; // Clock Manager Pulse Clock Divisor
    uint32_t fill3[4];
    uint32_t SDCCTL;   // Clock Manager SDC Clock Control
    uint32_t SDCDIV;   // Clock Manager SDC Clock Divisor
    uint32_t ARMCTL;   // Clock Manager ARM Clock Control
    uint32_t ARMDIV;   // Clock Manager ARM Clock Divisor
    uint32_t AVEOCTL;  // Clock Manager AVEO Clock Control
    uint32_t AVEODIV;  // Clock Manager AVEO Clock Divisor
    uint32_t EMMCCTL;  // Clock Manager EMMC Clock Control
    uint32_t EMMCDIV;  // Clock Manager EMMC Clock Divisor
};

#define CM_BASE   0x101000 // Clock Manager Base Address
#define CM_GNRICCTL  0x000 // Clock Manager Generic Clock Control
#define CM_GNRICDIV  0x004 // Clock Manager Generic Clock Divisor
#define CM_VPUCTL    0x008 // Clock Manager VPU Clock Control
#define CM_VPUDIV    0x00C // Clock Manager VPU Clock Divisor
#define CM_SYSCTL    0x010 // Clock Manager System Clock Control
#define CM_SYSDIV    0x014 // Clock Manager System Clock Divisor
#define CM_PERIACTL  0x018 // Clock Manager PERIA Clock Control
#define CM_PERIADIV  0x01C // Clock Manager PERIA Clock Divisor
#define CM_PERIICTL  0x020 // Clock Manager PERII Clock Control
#define CM_PERIIDIV  0x024 // Clock Manager PERII Clock Divisor
#define CM_H264CTL   0x028 // Clock Manager H264 Clock Control
#define CM_H264DIV   0x02C // Clock Manager H264 Clock Divisor
#define CM_ISPCTL    0x030 // Clock Manager ISP Clock Control
#define CM_ISPDIV    0x034 // Clock Manager ISP Clock Divisor
#define CM_V3DCTL    0x038 // Clock Manager V3D Clock Control
#define CM_V3DDIV    0x03C // Clock Manager V3D Clock Divisor
#define CM_CAM0CTL   0x040 // Clock Manager Camera 0 Clock Control
#define CM_CAM0DIV   0x044 // Clock Manager Camera 0 Clock Divisor
#define CM_CAM1CTL   0x048 // Clock Manager Camera 1 Clock Control
#define CM_CAM1DIV   0x04C // Clock Manager Camera 1 Clock Divisor
#define CM_CCP2CTL   0x050 // Clock Manager CCP2 Clock Control
#define CM_CCP2DIV   0x054 // Clock Manager CCP2 Clock Divisor
#define CM_DSI0ECTL  0x058 // Clock Manager DSI0E Clock Control
#define CM_DSI0EDIV  0x05C // Clock Manager DSI0E Clock Divisor
#define CM_DSI0PCTL  0x060 // Clock Manager DSI0P Clock Control
#define CM_DSI0PDIV  0x064 // Clock Manager DSI0P Clock Divisor
#define CM_DPICTL    0x068 // Clock Manager DPI Clock Control
#define CM_DPIDIV    0x06C // Clock Manager DPI Clock Divisor
#define CM_GP0CTL    0x070 // Clock Manager General Purpose 0 Clock Control
#define CM_GP0DIV    0x074 // Clock Manager General Purpose 0 Clock Divisor
#define CM_GP1CTL    0x078 // Clock Manager General Purpose 1 Clock Control
#define CM_GP1DIV    0x07C // Clock Manager General Purpose 1 Clock Divisor
#define CM_GP2CTL    0x080 // Clock Manager General Purpose 2 Clock Control
#define CM_GP2DIV    0x084 // Clock Manager General Purpose 2 Clock Divisor
#define CM_HSMCTL    0x088 // Clock Manager HSM Clock Control
#define CM_HSMDIV    0x08C // Clock Manager HSM Clock Divisor
#define CM_OTPCTL    0x090 // Clock Manager OTP Clock Control
#define CM_OTPDIV    0x094 // Clock Manager OTP Clock Divisor
#define CM_PCMCTL    0x098 // Clock Manager PCM / I2S Clock Control
#define CM_PCMDIV    0x09C // Clock Manager PCM / I2S Clock Divisor
#define CM_PWMCTL    0x0A0 // Clock Manager PWM Clock Control
#define CM_PWMDIV    0x0A4 // Clock Manager PWM Clock Divisor
#define CM_SLIMCTL   0x0A8 // Clock Manager SLIM Clock Control
#define CM_SLIMDIV   0x0AC // Clock Manager SLIM Clock Divisor
#define CM_SMICTL    0x0B0 // Clock Manager SMI Clock Control
#define CM_SMIDIV    0x0B4 // Clock Manager SMI Clock Divisor
#define CM_TCNTCTL   0x0C0 // Clock Manager TCNT Clock Control
#define CM_TCNTDIV   0x0C4 // Clock Manager TCNT Clock Divisor
#define CM_TECCTL    0x0C8 // Clock Manager TEC Clock Control
#define CM_TECDIV    0x0CC // Clock Manager TEC Clock Divisor
#define CM_TD0CTL    0x0D0 // Clock Manager TD0 Clock Control
#define CM_TD0DIV    0x0D4 // Clock Manager TD0 Clock Divisor
#define CM_TD1CTL    0x0D8 // Clock Manager TD1 Clock Control
#define CM_TD1DIV    0x0DC // Clock Manager TD1 Clock Divisor
#define CM_TSENSCTL  0x0E0 // Clock Manager TSENS Clock Control
#define CM_TSENSDIV  0x0E4 // Clock Manager TSENS Clock Divisor
#define CM_TIMERCTL  0x0E8 // Clock Manager Timer Clock Control
#define CM_TIMERDIV  0x0EC // Clock Manager Timer Clock Divisor
#define CM_UARTCTL   0x0F0 // Clock Manager UART Clock Control
#define CM_UARTDIV   0x0F4 // Clock Manager UART Clock Divisor
#define CM_VECCTL    0x0F8 // Clock Manager VEC Clock Control
#define CM_VECDIV    0x0FC // Clock Manager VEC Clock Divisor
#define CM_OSCCOUNT  0x100 // Clock Manager Oscillator Count
#define CM_PLLA      0x104 // Clock Manager PLLA
#define CM_PLLC      0x108 // Clock Manager PLLC
#define CM_PLLD      0x10C // Clock Manager PLLD
#define CM_PLLH      0x110 // Clock Manager PLLH
#define CM_LOCK      0x114 // Clock Manager Lock
#define CM_EVENT     0x118 // Clock Manager Event
#define CM_INTEN     0x118 // Clock Manager INTEN
#define CM_DSI0HSCK  0x120 // Clock Manager DSI0HSCK
#define CM_CKSM      0x124 // Clock Manager CKSM
#define CM_OSCFREQI  0x128 // Clock Manager Oscillator Frequency Integer
#define CM_OSCFREQF  0x12C // Clock Manager Oscillator Frequency Fraction
#define CM_PLLTCTL   0x130 // Clock Manager PLLT Control
#define CM_PLLTCNT0  0x134 // Clock Manager PLLT0
#define CM_PLLTCNT1  0x138 // Clock Manager PLLT1
#define CM_PLLTCNT2  0x13C // Clock Manager PLLT2
#define CM_PLLTCNT3  0x140 // Clock Manager PLLT3
#define CM_TDCLKEN   0x144 // Clock Manager TD Clock Enable
#define CM_BURSTCTL  0x148 // Clock Manager Burst Control
#define CM_BURSTCNT  0x14C // Clock Manager Burst
#define CM_DSI1ECTL  0x158 // Clock Manager DSI1E Clock Control
#define CM_DSI1EDIV  0x15C // Clock Manager DSI1E Clock Divisor
#define CM_DSI1PCTL  0x160 // Clock Manager DSI1P Clock Control
#define CM_DSI1PDIV  0x164 // Clock Manager DSI1P Clock Divisor
#define CM_DFTCTL    0x168 // Clock Manager DFT Clock Control
#define CM_DFTDIV    0x16C // Clock Manager DFT Clock Divisor
#define CM_PLLB      0x170 // Clock Manager PLLB
#define CM_PULSECTL  0x190 // Clock Manager Pulse Clock Control
#define CM_PULSEDIV  0x194 // Clock Manager Pulse Clock Divisor
#define CM_SDCCTL    0x1A8 // Clock Manager SDC Clock Control
#define CM_SDCDIV    0x1AC // Clock Manager SDC Clock Divisor
#define CM_ARMCTL    0x1B0 // Clock Manager ARM Clock Control
#define CM_ARMDIV    0x1B4 // Clock Manager ARM Clock Divisor
#define CM_AVEOCTL   0x1B8 // Clock Manager AVEO Clock Control
#define CM_AVEODIV   0x1BC // Clock Manager AVEO Clock Divisor
#define CM_EMMCCTL   0x1C0 // Clock Manager EMMC Clock Control
#define CM_EMMCDIV   0x1C4 // Clock Manager EMMC Clock Divisor

#define CM_SRC_OSCILLATOR        0x01 // Clock Control: Clock Source   Oscillator
#define CM_SRC_TESTDEBUG0        0x02 // Clock Control: Clock Source   Test Debug 0
#define CM_SRC_TESTDEBUG1        0x03 // Clock Control: Clock Source   Test Debug 1
#define CM_SRC_PLLAPER           0x04 // Clock Control: Clock Source   PLLA Per
#define CM_SRC_PLLCPER           0x05 // Clock Control: Clock Source   PLLC Per
#define CM_SRC_PLLDPER           0x06 // Clock Control: Clock Source   PLLD Per
#define CM_SRC_HDMIAUX           0x07 // Clock Control: Clock Source   HDMI Auxiliary
#define CM_SRC_GND               0x08 // Clock Control: Clock Source   GND
#define CM_ENAB                  0x10 // Clock Control: Enable The Clock Generator
#define CM_KILL                  0x20 // Clock Control: Kill The Clock Generator
#define CM_BUSY                  0x80 // Clock Control: Clock Generator Is Running
#define CM_FLIP                 0x100 // Clock Control: Invert The Clock Generator Output
#define CM_MASH_1               0x200 // Clock Control: MASH Control   1-Stage MASH (Equivalent To Non-MASH Dividers)
#define CM_MASH_2               0x400 // Clock Control: MASH Control   2-Stage MASH
#define CM_MASH_3               0x600 // Clock Control: MASH Control   3-Stage MASH
#define CM_PASSWORD        0x5A000000 // Clock Control: Password "5A"

/* DMA Controller */

struct _dma {
    struct {
        uint32_t cs;         // DMA Channel Control and Status
        uint32_t conblk_ad;  // DMA Channel Control Block Address
        uint32_t ti;         // DMA Channel Transfer Information
        uint32_t source_ad;  // DMA Channel Source Address
        uint32_t dest_ad;    // DMA Channel Destination Address
        uint32_t txfr_len;   // DMA Channel Transfer Length
        uint32_t stride;     // DMA Channel 2D Mode Stride
        uint32_t nextconbk;  // DMA Channel Next Control Block Address
        uint32_t debug;      // DMA Channel Debug
        uint32_t fill[55];
    } ch[14];
    uint32_t fill2[56];
    uint32_t int_status;     // Interrupt status of each DMA channel
    uint32_t enable;         // Global enable bits for each DMA channel
};

typedef struct __attribute__((aligned (32))) {
    uint32_t ti;         // DMA Transfer Information
    uint32_t source_ad;  // DMA Source Address
    uint32_t dest_ad;    // DMA Destination Address
    uint32_t txfr_len;   // DMA Transfer Length
    uint32_t stride;     // DMA 2D Mode Stride
    uint32_t nextconbk;  // DMA Next Control Block Address
    uint32_t resv1;      // Reserved 1
    uint32_t resv2;      // Reserved 2
} DMA_CB;

#define DMA0_BASE   0x7000 // DMA Channel 0 Register Set
#define DMA1_BASE   0x7100 // DMA Channel 1 Register Set
#define DMA2_BASE   0x7200 // DMA Channel 2 Register Set
#define DMA3_BASE   0x7300 // DMA Channel 3 Register Set
#define DMA4_BASE   0x7400 // DMA Channel 4 Register Set
#define DMA5_BASE   0x7500 // DMA Channel 5 Register Set
#define DMA6_BASE   0x7600 // DMA Channel 6 Register Set
#define DMA7_BASE   0x7700 // DMA Channel 7 Register Set
#define DMA8_BASE   0x7800 // DMA Channel 8 Register Set
#define DMA9_BASE   0x7900 // DMA Channel 9 Register Set
#define DMA10_BASE  0x7A00 // DMA Channel 10 Register Set
#define DMA11_BASE  0x7B00 // DMA Channel 11 Register Set
#define DMA12_BASE  0x7C00 // DMA Channel 12 Register Set
#define DMA13_BASE  0x7D00 // DMA Channel 13 Register Set
#define DMA14_BASE  0x7E00 // DMA Channel 14 Register Set

#define DMA_INT_STATUS  0x7FE0 // Interrupt Status of each DMA Channel
#define DMA_ENABLE      0x7FF0 // Global Enable bits for each DMA Channel

#define DMA15_BASE  0xE05000 // DMA Channel 15 Register Set

#define DMA_CS          0x00 // DMA Channel 0..14 Control & Status
#define DMA_CONBLK_AD   0x04 // DMA Channel 0..14 Control Block Address
#define DMA_TI          0x08 // DMA Channel 0..14 CB Word 0 (Transfer Information)
#define DMA_SOURCE_AD   0x0C // DMA Channel 0..14 CB Word 1 (Source Address)
#define DMA_DEST_AD     0x10 // DMA Channel 0..14 CB Word 2 (Destination Address)
#define DMA_TXFR_LEN    0x14 // DMA Channel 0..14 CB Word 3 (Transfer Length)
#define DMA_STRIDE      0x18 // DMA Channel 0..14 CB Word 4 (2D Stride)
#define DMA_NEXTCONBK   0x1C // DMA Channel 0..14 CB Word 5 (Next CB Address)
#define DMA_DEBUG       0x20 // DMA Channel 0..14 Debug

#define DMA_ACTIVE                                 0x1 // DMA Control & Status: Activate the DMA
#define DMA_END                                    0x2 // DMA Control & Status: DMA End Flag
#define DMA_INT                                    0x4 // DMA Control & Status: Interrupt Status
#define DMA_DREQ                                   0x8 // DMA Control & Status: DREQ State
#define DMA_PAUSED                                0x10 // DMA Control & Status: DMA Paused State
#define DMA_DREQ_STOPS_DMA                        0x20 // DMA Control & Status: DMA Paused by DREQ State
#define DMA_WAITING_FOR_OUTSTANDING_WRITES        0x40 // DMA Control & Status: DMA is Waiting for the Last Write to be Received
#define DMA_ERROR                                0x100 // DMA Control & Status: DMA Error
#define DMA_PRIORITY_0                             0x0 // DMA Control & Status: AXI Priority Level 0
#define DMA_PRIORITY_1                         0x10000 // DMA Control & Status: AXI Priority Level 1
#define DMA_PRIORITY_2                         0x20000 // DMA Control & Status: AXI Priority Level 2
#define DMA_PRIORITY_3                         0x30000 // DMA Control & Status: AXI Priority Level 3
#define DMA_PRIORITY_4                         0x40000 // DMA Control & Status: AXI Priority Level 4
#define DMA_PRIORITY_5                         0x50000 // DMA Control & Status: AXI Priority Level 5
#define DMA_PRIORITY_6                         0x60000 // DMA Control & Status: AXI Priority Level 6
#define DMA_PRIORITY_7                         0x70000 // DMA Control & Status: AXI Priority Level 7
#define DMA_PRIORITY_8                         0x80000 // DMA Control & Status: AXI Priority Level 8
#define DMA_PRIORITY_9                         0x90000 // DMA Control & Status: AXI Priority Level 9
#define DMA_PRIORITY_10                        0xA0000 // DMA Control & Status: AXI Priority Level 10
#define DMA_PRIORITY_11                        0xB0000 // DMA Control & Status: AXI Priority Level 11
#define DMA_PRIORITY_12                        0xC0000 // DMA Control & Status: AXI Priority Level 12
#define DMA_PRIORITY_13                        0xD0000 // DMA Control & Status: AXI Priority Level 13
#define DMA_PRIORITY_14                        0xE0000 // DMA Control & Status: AXI Priority Level 14
#define DMA_PRIORITY_15                        0xF0000 // DMA Control & Status: AXI Priority Level 15
#define DMA_PRIORITY                           0xF0000 // DMA Control & Status: AXI Priority Level
#define DMA_PANIC_PRIORITY_0                       0x0 // DMA Control & Status: AXI Panic Priority Level 0
#define DMA_PANIC_PRIORITY_1                  0x100000 // DMA Control & Status: AXI Panic Priority Level 1
#define DMA_PANIC_PRIORITY_2                  0x200000 // DMA Control & Status: AXI Panic Priority Level 2
#define DMA_PANIC_PRIORITY_3                  0x300000 // DMA Control & Status: AXI Panic Priority Level 3
#define DMA_PANIC_PRIORITY_4                  0x400000 // DMA Control & Status: AXI Panic Priority Level 4
#define DMA_PANIC_PRIORITY_5                  0x500000 // DMA Control & Status: AXI Panic Priority Level 5
#define DMA_PANIC_PRIORITY_6                  0x600000 // DMA Control & Status: AXI Panic Priority Level 6
#define DMA_PANIC_PRIORITY_7                  0x700000 // DMA Control & Status: AXI Panic Priority Level 7
#define DMA_PANIC_PRIORITY_8                  0x800000 // DMA Control & Status: AXI Panic Priority Level 8
#define DMA_PANIC_PRIORITY_9                  0x900000 // DMA Control & Status: AXI Panic Priority Level 9
#define DMA_PANIC_PRIORITY_10                 0xA00000 // DMA Control & Status: AXI Panic Priority Level 10
#define DMA_PANIC_PRIORITY_11                 0xB00000 // DMA Control & Status: AXI Panic Priority Level 11
#define DMA_PANIC_PRIORITY_12                 0xC00000 // DMA Control & Status: AXI Panic Priority Level 12
#define DMA_PANIC_PRIORITY_13                 0xD00000 // DMA Control & Status: AXI Panic Priority Level 13
#define DMA_PANIC_PRIORITY_14                 0xE00000 // DMA Control & Status: AXI Panic Priority Level 14
#define DMA_PANIC_PRIORITY_15                 0xF00000 // DMA Control & Status: AXI Panic Priority Level 14
#define DMA_PANIC_PRIORITY                    0xF00000 // DMA Control & Status: AXI Panic Priority Level
#define DMA_WAIT_FOR_OUTSTANDING_WRITES     0x10000000 // DMA Control & Status: Wait for Outstanding Writes
#define DMA_DISDEBUG                        0x20000000 // DMA Control & Status: Disable Debug Pause Signal
#define DMA_ABORT                           0x40000000 // DMA Control & Status: Abort DMA
#define DMA_RESET                           0x80000000 // DMA Control & Status: DMA Channel Reset

#define DMA_INTEN                  0x1 // DMA Transfer Information: Interrupt Enable
#define DMA_TDMODE                 0x2 // DMA Transfer Information: 2D Mode
#define DMA_WAIT_RESP              0x8 // DMA Transfer Information: Wait for a Write Response
#define DMA_DEST_INC              0x10 // DMA Transfer Information: Destination Address Increment
#define DMA_DEST_WIDTH            0x20 // DMA Transfer Information: Destination Transfer Width
#define DMA_DEST_DREQ             0x40 // DMA Transfer Information: Control Destination Writes with DREQ
#define DMA_DEST_IGNORE           0x80 // DMA Transfer Information: Ignore Writes
#define DMA_SRC_INC              0x100 // DMA Transfer Information: Source Address Increment
#define DMA_SRC_WIDTH            0x200 // DMA Transfer Information: Source Transfer Width
#define DMA_SRC_DREQ             0x400 // DMA Transfer Information: Control Source Reads with DREQ
#define DMA_SRC_IGNORE           0x800 // DMA Transfer Information: Ignore Reads
#define DMA_BURST_LENGTH_1         0x0 // DMA Transfer Information: Burst Transfer Length 1 Word
#define DMA_BURST_LENGTH_2      0x1000 // DMA Transfer Information: Burst Transfer Length 2 Words
#define DMA_BURST_LENGTH_3      0x2000 // DMA Transfer Information: Burst Transfer Length 3 Words
#define DMA_BURST_LENGTH_4      0x3000 // DMA Transfer Information: Burst Transfer Length 4 Words
#define DMA_BURST_LENGTH_5      0x4000 // DMA Transfer Information: Burst Transfer Length 5 Words
#define DMA_BURST_LENGTH_6      0x5000 // DMA Transfer Information: Burst Transfer Length 6 Words
#define DMA_BURST_LENGTH_7      0x6000 // DMA Transfer Information: Burst Transfer Length 7 Words
#define DMA_BURST_LENGTH_8      0x7000 // DMA Transfer Information: Burst Transfer Length 8 Words
#define DMA_BURST_LENGTH_9      0x8000 // DMA Transfer Information: Burst Transfer Length 9 Words
#define DMA_BURST_LENGTH_10     0x9000 // DMA Transfer Information: Burst Transfer Length 10 Words
#define DMA_BURST_LENGTH_11     0xA000 // DMA Transfer Information: Burst Transfer Length 11 Words
#define DMA_BURST_LENGTH_12     0xB000 // DMA Transfer Information: Burst Transfer Length 12 Words
#define DMA_BURST_LENGTH_13     0xC000 // DMA Transfer Information: Burst Transfer Length 13 Words
#define DMA_BURST_LENGTH_14     0xD000 // DMA Transfer Information: Burst Transfer Length 14 Words
#define DMA_BURST_LENGTH_15     0xE000 // DMA Transfer Information: Burst Transfer Length 15 Words
#define DMA_BURST_LENGTH_16     0xF000 // DMA Transfer Information: Burst Transfer Length 16 Words
#define DMA_BURST_LENGTH        0xF000 // DMA Transfer Information: Burst Transfer Length
#define DMA_PERMAP_0               0x0 // DMA Transfer Information: Peripheral Mapping Continuous Un-paced Transfer
#define DMA_PERMAP_1           0x10000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 1
#define DMA_PERMAP_2           0x20000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 2
#define DMA_PERMAP_3           0x30000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 3
#define DMA_PERMAP_4           0x40000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 4
#define DMA_PERMAP_5           0x50000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 5
#define DMA_PERMAP_6           0x60000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 6
#define DMA_PERMAP_7           0x70000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 7
#define DMA_PERMAP_8           0x80000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 8
#define DMA_PERMAP_9           0x90000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 9
#define DMA_PERMAP_10          0xA0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 10
#define DMA_PERMAP_11          0xB0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 11
#define DMA_PERMAP_12          0xC0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 12
#define DMA_PERMAP_13          0xD0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 13
#define DMA_PERMAP_14          0xE0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 14
#define DMA_PERMAP_15          0xF0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 15
#define DMA_PERMAP_16         0x100000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 16
#define DMA_PERMAP_17         0x110000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 17
#define DMA_PERMAP_18         0x120000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 18
#define DMA_PERMAP_19         0x130000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 19
#define DMA_PERMAP_20         0x140000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 20
#define DMA_PERMAP_21         0x150000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 21
#define DMA_PERMAP_22         0x160000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 22
#define DMA_PERMAP_23         0x170000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 23
#define DMA_PERMAP_24         0x180000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 24
#define DMA_PERMAP_25         0x190000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 25
#define DMA_PERMAP_26         0x1A0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 26
#define DMA_PERMAP_27         0x1B0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 27
#define DMA_PERMAP_28         0x1C0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 28
#define DMA_PERMAP_29         0x1D0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 29
#define DMA_PERMAP_30         0x1E0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 30
#define DMA_PERMAP_31         0x1F0000 // DMA Transfer Information: Peripheral Mapping Peripheral Number 31
#define DMA_PERMAP            0x1F0000 // DMA Transfer Information: Peripheral Mapping
#define DMA_WAITS_0                0x0 // DMA Transfer Information: Add No Wait Cycles
#define DMA_WAITS_1           0x200000 // DMA Transfer Information: Add 1 Wait Cycle
#define DMA_WAITS_2           0x400000 // DMA Transfer Information: Add 2 Wait Cycles
#define DMA_WAITS_3           0x600000 // DMA Transfer Information: Add 3 Wait Cycles
#define DMA_WAITS_4           0x800000 // DMA Transfer Information: Add 4 Wait Cycles
#define DMA_WAITS_5           0xA00000 // DMA Transfer Information: Add 5 Wait Cycles
#define DMA_WAITS_6           0xC00000 // DMA Transfer Information: Add 6 Wait Cycles
#define DMA_WAITS_7           0xE00000 // DMA Transfer Information: Add 7 Wait Cycles
#define DMA_WAITS_8          0x1000000 // DMA Transfer Information: Add 8 Wait Cycles
#define DMA_WAITS_9          0x1200000 // DMA Transfer Information: Add 9 Wait Cycles
#define DMA_WAITS_10         0x1400000 // DMA Transfer Information: Add 10 Wait Cycles
#define DMA_WAITS_11         0x1600000 // DMA Transfer Information: Add 11 Wait Cycles
#define DMA_WAITS_12         0x1800000 // DMA Transfer Information: Add 12 Wait Cycles
#define DMA_WAITS_13         0x1A00000 // DMA Transfer Information: Add 13 Wait Cycles
#define DMA_WAITS_14         0x1C00000 // DMA Transfer Information: Add 14 Wait Cycles
#define DMA_WAITS_15         0x1E00000 // DMA Transfer Information: Add 15 Wait Cycles
#define DMA_WAITS_16         0x2000000 // DMA Transfer Information: Add 16 Wait Cycles
#define DMA_WAITS_17         0x2200000 // DMA Transfer Information: Add 17 Wait Cycles
#define DMA_WAITS_18         0x2400000 // DMA Transfer Information: Add 18 Wait Cycles
#define DMA_WAITS_19         0x2600000 // DMA Transfer Information: Add 19 Wait Cycles
#define DMA_WAITS_20         0x2800000 // DMA Transfer Information: Add 20 Wait Cycles
#define DMA_WAITS_21         0x2A00000 // DMA Transfer Information: Add 21 Wait Cycles
#define DMA_WAITS_22         0x2C00000 // DMA Transfer Information: Add 22 Wait Cycles
#define DMA_WAITS_23         0x2E00000 // DMA Transfer Information: Add 23 Wait Cycles
#define DMA_WAITS_24         0x3000000 // DMA Transfer Information: Add 24 Wait Cycles
#define DMA_WAITS_25         0x3200000 // DMA Transfer Information: Add 25 Wait Cycles
#define DMA_WAITS_26         0x3400000 // DMA Transfer Information: Add 26 Wait Cycles
#define DMA_WAITS_27         0x3600000 // DMA Transfer Information: Add 27 Wait Cycles
#define DMA_WAITS_28         0x3800000 // DMA Transfer Information: Add 28 Wait Cycles
#define DMA_WAITS_29         0x3A00000 // DMA Transfer Information: Add 29 Wait Cycles
#define DMA_WAITS_30         0x3C00000 // DMA Transfer Information: Add 30 Wait Cycles
#define DMA_WAITS_31         0x3E00000 // DMA Transfer Information: Add 31 Wait Cycles
#define DMA_WAITS            0x3E00000 // DMA Transfer Information: Add Wait Cycles
#define DMA_NO_WIDE_BURSTS   0x4000000 // DMA Transfer Information: Don't Do Wide Writes as a 2 Beat Burst

#define DMA_XLENGTH      0xFFFF // DMA Transfer Length: Transfer Length in Bytes
#define DMA_YLENGTH  0x3FFF0000 // DMA Transfer Length: When in 2D Mode, This is the Y Transfer Length

#define DMA_S_STRIDE      0xFFFF // DMA 2D Stride: Source Stride (2D Mode)
#define DMA_D_STRIDE  0xFFFF0000 // DMA 2D Stride: Destination Stride (2D Mode)

#define DMA_READ_LAST_NOT_SET_ERROR         0x1 // DMA Debug: Read Last Not Set Error
#define DMA_FIFO_ERROR                      0x2 // DMA Debug: Fifo Error
#define DMA_READ_ERROR                      0x4 // DMA Debug: Slave Read Response Error
#define DMA_OUTSTANDING_WRITES             0xF0 // DMA Debug: DMA Outstanding Writes Counter
#define DMA_ID                           0xFF00 // DMA Debug: DMA ID
#define DMA_STATE                     0x1FF0000 // DMA Debug: DMA State Machine State
#define DMA_VERSION                   0xE000000 // DMA Debug: DMA Version
#define DMA_LITE                     0x10000000 // DMA Debug: DMA Lite

#define DMA_INT0      0x1 // DMA Interrupt Status: Interrupt Status of DMA Engine 0
#define DMA_INT1      0x2 // DMA Interrupt Status: Interrupt Status of DMA Engine 1
#define DMA_INT2      0x4 // DMA Interrupt Status: Interrupt Status of DMA Engine 2
#define DMA_INT3      0x8 // DMA Interrupt Status: Interrupt Status of DMA Engine 3
#define DMA_INT4     0x10 // DMA Interrupt Status: Interrupt Status of DMA Engine 4
#define DMA_INT5     0x20 // DMA Interrupt Status: Interrupt Status of DMA Engine 5
#define DMA_INT6     0x40 // DMA Interrupt Status: Interrupt Status of DMA Engine 6
#define DMA_INT7     0x80 // DMA Interrupt Status: Interrupt Status of DMA Engine 7
#define DMA_INT8    0x100 // DMA Interrupt Status: Interrupt Status of DMA Engine 8
#define DMA_INT9    0x200 // DMA Interrupt Status: Interrupt Status of DMA Engine 9
#define DMA_INT10   0x400 // DMA Interrupt Status: Interrupt Status of DMA Engine 10
#define DMA_INT11   0x800 // DMA Interrupt Status: Interrupt Status of DMA Engine 11
#define DMA_INT12  0x1000 // DMA Interrupt Status: Interrupt Status of DMA Engine 12
#define DMA_INT13  0x2000 // DMA Interrupt Status: Interrupt Status of DMA Engine 13
#define DMA_INT14  0x4000 // DMA Interrupt Status: Interrupt Status of DMA Engine 14
#define DMA_INT15  0x8000 // DMA Interrupt Status: Interrupt Status of DMA Engine 15

#define DMA_EN0      0x1 // DMA Enable: Enable DMA Engine 0
#define DMA_EN1      0x2 // DMA Enable: Enable DMA Engine 1
#define DMA_EN2      0x4 // DMA Enable: Enable DMA Engine 2
#define DMA_EN3      0x8 // DMA Enable: Enable DMA Engine 3
#define DMA_EN4     0x10 // DMA Enable: Enable DMA Engine 4
#define DMA_EN5     0x20 // DMA Enable: Enable DMA Engine 5
#define DMA_EN6     0x40 // DMA Enable: Enable DMA Engine 6
#define DMA_EN7     0x80 // DMA Enable: Enable DMA Engine 7
#define DMA_EN8    0x100 // DMA Enable: Enable DMA Engine 8
#define DMA_EN9    0x200 // DMA Enable: Enable DMA Engine 9
#define DMA_EN10   0x400 // DMA Enable: Enable DMA Engine 10
#define DMA_EN11   0x800 // DMA Enable: Enable DMA Engine 11
#define DMA_EN12  0x1000 // DMA Enable: Enable DMA Engine 12
#define DMA_EN13  0x2000 // DMA Enable: Enable DMA Engine 13
#define DMA_EN14  0x4000 // DMA Enable: Enable DMA Engine 14

/* GPIO */

struct _gpio {
    uint32_t gpfsel[6];
    uint32_t fill1;
    uint32_t gpset[2];
    uint32_t fill2;
    uint32_t gpclr[2];
    uint32_t fill3;
    uint32_t gplev[2];
    uint32_t fill4;
    uint32_t gpeds[2];
    uint32_t fill5;
    uint32_t gpren[2];
    uint32_t fill6;
    uint32_t gpfen[2];
    uint32_t fill7;
    uint32_t gphen[2];
    uint32_t fill8;
    uint32_t gplen[2];
    uint32_t fill9;
    uint32_t gparen[2];
    uint32_t fill10;
    uint32_t gpafen[2];
    uint32_t fill11;
    uint32_t gppud;
    uint32_t gppudclk[2];
    uint32_t test;
};

#define GPIO_BASE       0x200000 // GPIO Base Address
#define GPIO_GPFSEL0         0x0 // GPIO Function Select 0
#define GPIO_GPFSEL1         0x4 // GPIO Function Select 1
#define GPIO_GPFSEL2         0x8 // GPIO Function Select 2
#define GPIO_GPFSEL3         0xC // GPIO Function Select 3
#define GPIO_GPFSEL4         0x10 // GPIO Function Select 4
#define GPIO_GPFSEL5         0x14 // GPIO Function Select 5
#define GPIO_GPSET0          0x1C // GPIO Pin Output Set 0
#define GPIO_GPSET1          0x20 // GPIO Pin Output Set 1
#define GPIO_GPCLR0          0x28 // GPIO Pin Output Clear 0
#define GPIO_GPCLR1          0x2C // GPIO Pin Output Clear 1
#define GPIO_GPLEV0          0x34 // GPIO Pin Level 0
#define GPIO_GPLEV1          0x38 // GPIO Pin Level 1
#define GPIO_GPEDS0          0x40 // GPIO Pin Event Detect Status 0
#define GPIO_GPEDS1          0x44 // GPIO Pin Event Detect Status 1
#define GPIO_GPREN0          0x4C // GPIO Pin Rising Edge Detect Enable 0
#define GPIO_GPREN1          0x50 // GPIO Pin Rising Edge Detect Enable 1
#define GPIO_GPFEN0          0x58 // GPIO Pin Falling Edge Detect Enable 0
#define GPIO_GPFEN1          0x5C // GPIO Pin Falling Edge Detect Enable 1
#define GPIO_GPHEN0          0x64 // GPIO Pin High Detect Enable 0
#define GPIO_GPHEN1          0x68 // GPIO Pin High Detect Enable 1
#define GPIO_GPLEN0          0x70 // GPIO Pin Low Detect Enable 0
#define GPIO_GPLEN1          0x74 // GPIO Pin Low Detect Enable 1
#define GPIO_GPAREN0         0x7C // GPIO Pin Async. Rising Edge Detect 0
#define GPIO_GPAREN1         0x80 // GPIO Pin Async. Rising Edge Detect 1
#define GPIO_GPAFEN0         0x88 // GPIO Pin Async. Falling Edge Detect 0
#define GPIO_GPAFEN1         0x8C // GPIO Pin Async. Falling Edge Detect 1
#define GPIO_GPPUD           0x94 // GPIO Pin Pull-up/down Enable
#define GPIO_GPPUDCLK0       0x98 // GPIO Pin Pull-up/down Enable Clock 0
#define GPIO_GPPUDCLK1       0x9C // GPIO Pin Pull-up/down Enable Clock 1
#define GPIO_TEST            0xB0 // GPIO Test

#define GPIO_FSEL0_IN    0x0 // GPIO Function Select: GPIO Pin X0 Is An Input
#define GPIO_FSEL0_OUT   0x1 // GPIO Function Select: GPIO Pin X0 Is An Output
#define GPIO_FSEL0_ALT0  0x4 // GPIO Function Select: GPIO Pin X0 Takes Alternate Function 0
#define GPIO_FSEL0_ALT1  0x5 // GPIO Function Select: GPIO Pin X0 Takes Alternate Function 1
#define GPIO_FSEL0_ALT2  0x6 // GPIO Function Select: GPIO Pin X0 Takes Alternate Function 2
#define GPIO_FSEL0_ALT3  0x7 // GPIO Function Select: GPIO Pin X0 Takes Alternate Function 3
#define GPIO_FSEL0_ALT4  0x3 // GPIO Function Select: GPIO Pin X0 Takes Alternate Function 4
#define GPIO_FSEL0_ALT5  0x2 // GPIO Function Select: GPIO Pin X0 Takes Alternate Function 5
#define GPIO_FSEL0_CLR   0x7 // GPIO Function Select: GPIO Pin X0 Clear Bits

#define GPIO_FSEL1_IN     0x0 // GPIO Function Select: GPIO Pin X1 Is An Input
#define GPIO_FSEL1_OUT    0x8 // GPIO Function Select: GPIO Pin X1 Is An Output
#define GPIO_FSEL1_ALT0  0x20 // GPIO Function Select: GPIO Pin X1 Takes Alternate Function 0
#define GPIO_FSEL1_ALT1  0x28 // GPIO Function Select: GPIO Pin X1 Takes Alternate Function 1
#define GPIO_FSEL1_ALT2  0x30 // GPIO Function Select: GPIO Pin X1 Takes Alternate Function 2
#define GPIO_FSEL1_ALT3  0x38 // GPIO Function Select: GPIO Pin X1 Takes Alternate Function 3
#define GPIO_FSEL1_ALT4  0x18 // GPIO Function Select: GPIO Pin X1 Takes Alternate Function 4
#define GPIO_FSEL1_ALT5  0x10 // GPIO Function Select: GPIO Pin X1 Takes Alternate Function 5
#define GPIO_FSEL1_CLR   0x38 // GPIO Function Select: GPIO Pin X1 Clear Bits

#define GPIO_FSEL2_IN      0x0 // GPIO Function Select: GPIO Pin X2 Is An Input
#define GPIO_FSEL2_OUT    0x40 // GPIO Function Select: GPIO Pin X2 Is An Output
#define GPIO_FSEL2_ALT0  0x100 // GPIO Function Select: GPIO Pin X2 Takes Alternate Function 0
#define GPIO_FSEL2_ALT1  0x140 // GPIO Function Select: GPIO Pin X2 Takes Alternate Function 1
#define GPIO_FSEL2_ALT2  0x180 // GPIO Function Select: GPIO Pin X2 Takes Alternate Function 2
#define GPIO_FSEL2_ALT3  0x1C0 // GPIO Function Select: GPIO Pin X2 Takes Alternate Function 3
#define GPIO_FSEL2_ALT4   0xC0 // GPIO Function Select: GPIO Pin X2 Takes Alternate Function 4
#define GPIO_FSEL2_ALT5   0x80 // GPIO Function Select: GPIO Pin X2 Takes Alternate Function 5
#define GPIO_FSEL2_CLR   0x1C0 // GPIO Function Select: GPIO Pin X2 Clear Bits

#define GPIO_FSEL3_IN      0x0 // GPIO Function Select: GPIO Pin X3 Is An Input
#define GPIO_FSEL3_OUT   0x200 // GPIO Function Select: GPIO Pin X3 Is An Output
#define GPIO_FSEL3_ALT0  0x800 // GPIO Function Select: GPIO Pin X3 Takes Alternate Function 0
#define GPIO_FSEL3_ALT1  0xA00 // GPIO Function Select: GPIO Pin X3 Takes Alternate Function 1
#define GPIO_FSEL3_ALT2  0xC00 // GPIO Function Select: GPIO Pin X3 Takes Alternate Function 2
#define GPIO_FSEL3_ALT3  0xE00 // GPIO Function Select: GPIO Pin X3 Takes Alternate Function 3
#define GPIO_FSEL3_ALT4  0x600 // GPIO Function Select: GPIO Pin X3 Takes Alternate Function 4
#define GPIO_FSEL3_ALT5  0x400 // GPIO Function Select: GPIO Pin X3 Takes Alternate Function 5
#define GPIO_FSEL3_CLR   0xE00 // GPIO Function Select: GPIO Pin X3 Clear Bits

#define GPIO_FSEL4_IN       0x0 // GPIO Function Select: GPIO Pin X4 Is An Input
#define GPIO_FSEL4_OUT   0x1000 // GPIO Function Select: GPIO Pin X4 Is An Output
#define GPIO_FSEL4_ALT0  0x4000 // GPIO Function Select: GPIO Pin X4 Takes Alternate Function 0
#define GPIO_FSEL4_ALT1  0x5000 // GPIO Function Select: GPIO Pin X4 Takes Alternate Function 1
#define GPIO_FSEL4_ALT2  0x6000 // GPIO Function Select: GPIO Pin X4 Takes Alternate Function 2
#define GPIO_FSEL4_ALT3  0x7000 // GPIO Function Select: GPIO Pin X4 Takes Alternate Function 3
#define GPIO_FSEL4_ALT4  0x3000 // GPIO Function Select: GPIO Pin X4 Takes Alternate Function 4
#define GPIO_FSEL4_ALT5  0x2000 // GPIO Function Select: GPIO Pin X4 Takes Alternate Function 5
#define GPIO_FSEL4_CLR   0x7000 // GPIO Function Select: GPIO Pin X4 Clear Bits

#define GPIO_FSEL5_IN        0x0 // GPIO Function Select: GPIO Pin X5 Is An Input
#define GPIO_FSEL5_OUT    0x8000 // GPIO Function Select: GPIO Pin X5 Is An Output
#define GPIO_FSEL5_ALT0  0x20000 // GPIO Function Select: GPIO Pin X5 Takes Alternate Function 0
#define GPIO_FSEL5_ALT1  0x28000 // GPIO Function Select: GPIO Pin X5 Takes Alternate Function 1
#define GPIO_FSEL5_ALT2  0x30000 // GPIO Function Select: GPIO Pin X5 Takes Alternate Function 2
#define GPIO_FSEL5_ALT3  0x38000 // GPIO Function Select: GPIO Pin X5 Takes Alternate Function 3
#define GPIO_FSEL5_ALT4  0x18000 // GPIO Function Select: GPIO Pin X5 Takes Alternate Function 4
#define GPIO_FSEL5_ALT5  0x10000 // GPIO Function Select: GPIO Pin X5 Takes Alternate Function 5
#define GPIO_FSEL5_CLR   0x38000 // GPIO Function Select: GPIO Pin X5 Clear Bits

#define GPIO_FSEL6_IN         0x0 // GPIO Function Select: GPIO Pin X6 Is An Input
#define GPIO_FSEL6_OUT    0x40000 // GPIO Function Select: GPIO Pin X6 Is An Output
#define GPIO_FSEL6_ALT0  0x100000 // GPIO Function Select: GPIO Pin X6 Takes Alternate Function 0
#define GPIO_FSEL6_ALT1  0x140000 // GPIO Function Select: GPIO Pin X6 Takes Alternate Function 1
#define GPIO_FSEL6_ALT2  0x180000 // GPIO Function Select: GPIO Pin X6 Takes Alternate Function 2
#define GPIO_FSEL6_ALT3  0x1C0000 // GPIO Function Select: GPIO Pin X6 Takes Alternate Function 3
#define GPIO_FSEL6_ALT4   0xC0000 // GPIO Function Select: GPIO Pin X6 Takes Alternate Function 4
#define GPIO_FSEL6_ALT5   0x80000 // GPIO Function Select: GPIO Pin X6 Takes Alternate Function 5
#define GPIO_FSEL6_CLR   0x1C0000 // GPIO Function Select: GPIO Pin X6 Clear Bits

#define GPIO_FSEL7_IN         0x0 // GPIO Function Select: GPIO Pin X7 Is An Input
#define GPIO_FSEL7_OUT   0x200000 // GPIO Function Select: GPIO Pin X7 Is An Output
#define GPIO_FSEL7_ALT0  0x800000 // GPIO Function Select: GPIO Pin X7 Takes Alternate Function 0
#define GPIO_FSEL7_ALT1  0xA00000 // GPIO Function Select: GPIO Pin X7 Takes Alternate Function 1
#define GPIO_FSEL7_ALT2  0xC00000 // GPIO Function Select: GPIO Pin X7 Takes Alternate Function 2
#define GPIO_FSEL7_ALT3  0xE00000 // GPIO Function Select: GPIO Pin X7 Takes Alternate Function 3
#define GPIO_FSEL7_ALT4  0x600000 // GPIO Function Select: GPIO Pin X7 Takes Alternate Function 4
#define GPIO_FSEL7_ALT5  0x400000 // GPIO Function Select: GPIO Pin X7 Takes Alternate Function 5
#define GPIO_FSEL7_CLR   0xE00000 // GPIO Function Select: GPIO Pin X7 Clear Bits

#define GPIO_FSEL8_IN          0x0 // GPIO Function Select: GPIO Pin X8 Is An Input
#define GPIO_FSEL8_OUT   0x1000000 // GPIO Function Select: GPIO Pin X8 Is An Output
#define GPIO_FSEL8_ALT0  0x4000000 // GPIO Function Select: GPIO Pin X8 Takes Alternate Function 0
#define GPIO_FSEL8_ALT1  0x5000000 // GPIO Function Select: GPIO Pin X8 Takes Alternate Function 1
#define GPIO_FSEL8_ALT2  0x6000000 // GPIO Function Select: GPIO Pin X8 Takes Alternate Function 2
#define GPIO_FSEL8_ALT3  0x7000000 // GPIO Function Select: GPIO Pin X8 Takes Alternate Function 3
#define GPIO_FSEL8_ALT4  0x3000000 // GPIO Function Select: GPIO Pin X8 Takes Alternate Function 4
#define GPIO_FSEL8_ALT5  0x2000000 // GPIO Function Select: GPIO Pin X8 Takes Alternate Function 5
#define GPIO_FSEL8_CLR   0x7000000 // GPIO Function Select: GPIO Pin X8 Clear Bits

#define GPIO_FSEL9_IN           0x0 // GPIO Function Select: GPIO Pin X9 Is An Input
#define GPIO_FSEL9_OUT    0x8000000 // GPIO Function Select: GPIO Pin X9 Is An Output
#define GPIO_FSEL9_ALT0  0x20000000 // GPIO Function Select: GPIO Pin X9 Takes Alternate Function 0
#define GPIO_FSEL9_ALT1  0x28000000 // GPIO Function Select: GPIO Pin X9 Takes Alternate Function 1
#define GPIO_FSEL9_ALT2  0x30000000 // GPIO Function Select: GPIO Pin X9 Takes Alternate Function 2
#define GPIO_FSEL9_ALT3  0x38000000 // GPIO Function Select: GPIO Pin X9 Takes Alternate Function 3
#define GPIO_FSEL9_ALT4  0x18000000 // GPIO Function Select: GPIO Pin X9 Takes Alternate Function 4
#define GPIO_FSEL9_ALT5  0x10000000 // GPIO Function Select: GPIO Pin X9 Takes Alternate Function 5
#define GPIO_FSEL9_CLR   0x38000000 // GPIO Function Select: GPIO Pin X9 Clear Bits

#define GPIO_0          0x1 // GPIO Pin 0: 0
#define GPIO_1          0x2 // GPIO Pin 0: 1
#define GPIO_2          0x4 // GPIO Pin 0: 2
#define GPIO_3          0x8 // GPIO Pin 0: 3
#define GPIO_4         0x10 // GPIO Pin 0: 4
#define GPIO_5         0x20 // GPIO Pin 0: 5
#define GPIO_6         0x40 // GPIO Pin 0: 6
#define GPIO_7         0x80 // GPIO Pin 0: 7
#define GPIO_8        0x100 // GPIO Pin 0: 8
#define GPIO_9        0x200 // GPIO Pin 0: 9
#define GPIO_10       0x400 // GPIO Pin 0: 10
#define GPIO_11       0x800 // GPIO Pin 0: 11
#define GPIO_12      0x1000 // GPIO Pin 0: 12
#define GPIO_13      0x2000 // GPIO Pin 0: 13
#define GPIO_14      0x4000 // GPIO Pin 0: 14
#define GPIO_15      0x8000 // GPIO Pin 0: 15
#define GPIO_16     0x10000 // GPIO Pin 0: 16
#define GPIO_17     0x20000 // GPIO Pin 0: 17
#define GPIO_18     0x40000 // GPIO Pin 0: 18
#define GPIO_19     0x80000 // GPIO Pin 0: 19
#define GPIO_20    0x100000 // GPIO Pin 0: 20
#define GPIO_21    0x200000 // GPIO Pin 0: 21
#define GPIO_22    0x400000 // GPIO Pin 0: 22
#define GPIO_23    0x800000 // GPIO Pin 0: 23
#define GPIO_24   0x1000000 // GPIO Pin 0: 24
#define GPIO_25   0x2000000 // GPIO Pin 0: 25
#define GPIO_26   0x4000000 // GPIO Pin 0: 26
#define GPIO_27   0x8000000 // GPIO Pin 0: 27
#define GPIO_28  0x10000000 // GPIO Pin 0: 28
#define GPIO_29  0x20000000 // GPIO Pin 0: 29
#define GPIO_30  0x40000000 // GPIO Pin 0: 30
#define GPIO_31  0x80000000 // GPIO Pin 0: 31

#define GPIO_32         0x1 // GPIO Pin 1: 32
#define GPIO_33         0x2 // GPIO Pin 1: 33
#define GPIO_34         0x4 // GPIO Pin 1: 34
#define GPIO_35         0x8 // GPIO Pin 1: 35
#define GPIO_36        0x10 // GPIO Pin 1: 36
#define GPIO_37        0x20 // GPIO Pin 1: 37
#define GPIO_38        0x40 // GPIO Pin 1: 38
#define GPIO_39        0x80 // GPIO Pin 1: 39
#define GPIO_40       0x100 // GPIO Pin 1: 40
#define GPIO_41       0x200 // GPIO Pin 1: 41
#define GPIO_42       0x400 // GPIO Pin 1: 42
#define GPIO_43       0x800 // GPIO Pin 1: 43
#define GPIO_44      0x1000 // GPIO Pin 1: 44
#define GPIO_45      0x2000 // GPIO Pin 1: 45
#define GPIO_46      0x4000 // GPIO Pin 1: 46
#define GPIO_47      0x8000 // GPIO Pin 1: 47
#define GPIO_48     0x10000 // GPIO Pin 1: 48
#define GPIO_49     0x20000 // GPIO Pin 1: 49
#define GPIO_50     0x40000 // GPIO Pin 1: 50
#define GPIO_51     0x80000 // GPIO Pin 1: 51
#define GPIO_52    0x100000 // GPIO Pin 1: 52
#define GPIO_53    0x200000 // GPIO Pin 1: 53

/* PCM / I2S Audio Interface */
#define PCM_BASE      0x203000 // PCM Base Address
#define PCM_CS_A           0x0 // PCM Control & Status
#define PCM_FIFO_A         0x4 // PCM FIFO Data
#define PCM_MODE_A         0x8 // PCM Mode
#define PCM_RXC_A          0xC // PCM Receive Configuration
#define PCM_TXC_A         0x10 // PCM Transmit Configuration
#define PCM_DREQ_A        0x14 // PCM DMA Request Level
#define PCM_INTEN_A       0x18 // PCM Interrupt Enables
#define PCM_INTSTC_A      0x1C // PCM Interrupt Status & Clear
#define PCM_GRAY          0x20 // PCM Gray Mode Control

#define PCM_EN             0x1 // PCM Control & Status: Enable the PCM Audio Interface
#define PCM_RXON           0x2 // PCM Control & Status: Enable Reception
#define PCM_TXON           0x4 // PCM Control & Status: Enable Transmission
#define PCM_TXCLR          0x8 // PCM Control & Status: Clear the TX FIFO
#define PCM_RXCLR         0x10 // PCM Control & Status: Clear the RX FIFO
#define PCM_TXTHR_0        0x0 // PCM Control & Status: Sets the TX FIFO Threshold at which point the TXW flag is Set when the TX FIFO is Empty
#define PCM_TXTHR_1       0x20 // PCM Control & Status: Sets the TX FIFO Threshold at which point the TXW flag is Set when the TX FIFO is less than Full
#define PCM_TXTHR_2       0x40 // PCM Control & Status: Sets the TX FIFO Threshold at which point the TXW flag is Set when the TX FIFO is less than Full
#define PCM_TXTHR_3       0x60 // PCM Control & Status: Sets the TX FIFO Threshold at which point the TXW flag is Set when the TX FIFO is Full but for one Sample
#define PCM_TXTHR         0x60 // PCM Control & Status: Sets the TX FIFO Threshold at which point the TXW flag is Set
#define PCM_RXTHR_0        0x0 // PCM Control & Status: Sets the RX FIFO Threshold at which point the RXR flag is Set when we have a single Sample in the RX FIFO
#define PCM_RXTHR_1       0x80 // PCM Control & Status: Sets the RX FIFO Threshold at which point the RXR flag is Set when the RX FIFO is at least Full
#define PCM_RXTHR_2      0x100 // PCM Control & Status: Sets the RX FIFO Threshold at which point the RXR flag is Set when the RX FIFO is at least Full
#define PCM_RXTHR_3      0x180 // PCM Control & Status: Sets the RX FIFO Threshold at which point the RXR flag is Set when the RX FIFO is Full
#define PCM_RXTHR        0x180 // PCM Control & Status: Sets the RX FIFO Threshold at which point the RXR flag is Set
#define PCM_DMAEN        0x200 // PCM Control & Status: DMA DREQ Enable
#define PCM_TXSYNC      0x2000 // PCM Control & Status: TX FIFO Sync
#define PCM_RXSYNC      0x4000 // PCM Control & Status: RX FIFO Sync
#define PCM_TXERR       0x8000 // PCM Control & Status: TX FIFO Error
#define PCM_RXERR      0x10000 // PCM Control & Status: RX FIFO Error
#define PCM_TXW        0x20000 // PCM Control & Status: Indicates that the TX FIFO needs Writing
#define PCM_RXR        0x40000 // PCM Control & Status: Indicates that the RX FIFO needs Reading
#define PCM_TXD        0x80000 // PCM Control & Status: Indicates that the TX FIFO can accept Data
#define PCM_RXD       0x100000 // PCM Control & Status: Indicates that the RX FIFO contains Data
#define PCM_TXE       0x200000 // PCM Control & Status: TX FIFO is Empty
#define PCM_RXF       0x400000 // PCM Control & Status: RX FIFO is Full
#define PCM_RXSEX     0x800000 // PCM Control & Status: RX Sign Extend
#define PCM_SYNC     0x1000000 // PCM Control & Status: PCM Clock Sync helper
#define PCM_STBY     0x2000000 // PCM Control & Status: RAM Standby

#define PCM_FSLEN         0x3FF // PCM Mode: Frame Sync Length
#define PCM_FLEN        0xFFC00 // PCM Mode: Frame Length
#define PCM_FSI        0x100000 // PCM Mode: Frame Sync Invert this logically inverts the Frame Sync Signal
#define PCM_FSM        0x200000 // PCM Mode: Frame Sync Mode
#define PCM_CLKI       0x400000 // PCM Mode: Clock Invert this logically inverts the #define PCM_CLK Signal
#define PCM_CLKM       0x800000 // PCM Mode: PCM Clock Mode
#define PCM_FTXP      0x1000000 // PCM Mode: Transmit Frame Packed Mode
#define PCM_FRXP      0x2000000 // PCM Mode: Receive Frame Packed Mode
#define PCM_PDME      0x4000000 // PCM Mode: PDM Input Mode Enable
#define PCM_PDMN      0x8000000 // PCM Mode: PDM Decimation Factor (N)
#define PCM_CLK_DIS  0x10000000 // PCM Mode: PCM Clock Disable

#define PCM_CH2WID_8          0x0 // PCM Receive & Transmit Configuration: Channel 2 Width 8 bits Wide
#define PCM_CH2WID_9          0x1 // PCM Receive & Transmit Configuration: Channel 2 Width 9 bits Wide
#define PCM_CH2WID_10         0x2 // PCM Receive & Transmit Configuration: Channel 2 Width 10 bits Wide
#define PCM_CH2WID_11         0x3 // PCM Receive & Transmit Configuration: Channel 2 Width 11 bits Wide
#define PCM_CH2WID_12         0x4 // PCM Receive & Transmit Configuration: Channel 2 Width 12 bits Wide
#define PCM_CH2WID_13         0x5 // PCM Receive & Transmit Configuration: Channel 2 Width 13 bits Wide
#define PCM_CH2WID_14         0x6 // PCM Receive & Transmit Configuration: Channel 2 Width 14 bits Wide
#define PCM_CH2WID_15         0x7 // PCM Receive & Transmit Configuration: Channel 2 Width 15 bits Wide
#define PCM_CH2WID_16         0x8 // PCM Receive & Transmit Configuration: Channel 2 Width 16 bits Wide
#define PCM_CH2WID_17         0x9 // PCM Receive & Transmit Configuration: Channel 2 Width 17 bits Wide
#define PCM_CH2WID_18         0xA // PCM Receive & Transmit Configuration: Channel 2 Width 18 bits Wide
#define PCM_CH2WID_19         0xB // PCM Receive & Transmit Configuration: Channel 2 Width 19 bits Wide
#define PCM_CH2WID_20         0xC // PCM Receive & Transmit Configuration: Channel 2 Width 20 bits Wide
#define PCM_CH2WID_21         0xD // PCM Receive & Transmit Configuration: Channel 2 Width 21 bits Wide
#define PCM_CH2WID_22         0xE // PCM Receive & Transmit Configuration: Channel 2 Width 22 bits Wide
#define PCM_CH2WID_23         0xF // PCM Receive & Transmit Configuration: Channel 2 Width 23 bits Wide
#define PCM_CH2WID            0xF // PCM Receive & Transmit Configuration: Channel 2 Width
#define PCM_CH2WID_24      0x8000 // PCM Receive & Transmit Configuration: Channel 2 Width 24 bits wide
#define PCM_CH2WID_25      0x8001 // PCM Receive & Transmit Configuration: Channel 2 Width 25 bits wide
#define PCM_CH2WID_26      0x8002 // PCM Receive & Transmit Configuration: Channel 2 Width 26 bits wide
#define PCM_CH2WID_27      0x8003 // PCM Receive & Transmit Configuration: Channel 2 Width 27 bits wide
#define PCM_CH2WID_28      0x8004 // PCM Receive & Transmit Configuration: Channel 2 Width 28 bits wide
#define PCM_CH2WID_29      0x8005 // PCM Receive & Transmit Configuration: Channel 2 Width 29 bits wide
#define PCM_CH2WID_30      0x8006 // PCM Receive & Transmit Configuration: Channel 2 Width 30 bits wide
#define PCM_CH2WID_31      0x8007 // PCM Receive & Transmit Configuration: Channel 2 Width 31 bits wide
#define PCM_CH2WID_32      0x8008 // PCM Receive & Transmit Configuration: Channel 2 Width 32 bits wide
#define PCM_CH2POS         0x3FF0 // PCM Receive & Transmit Configuration: Channel 2 Position
#define PCM_CH2EN          0x4000 // PCM Receive & Transmit Configuration: Channel 2 Enable
#define PCM_CH2WEX         0x8000 // PCM Receive & Transmit Configuration: Channel 2 Width Extension Bit
#define PCM_CH1WID_8          0x0 // PCM Receive & Transmit Configuration: Channel 1 Width 8 bits Wide
#define PCM_CH1WID_9      0x10000 // PCM Receive & Transmit Configuration: Channel 1 Width 9 bits Wide
#define PCM_CH1WID_10     0x20000 // PCM Receive & Transmit Configuration: Channel 1 Width 10 bits Wide
#define PCM_CH1WID_11     0x30000 // PCM Receive & Transmit Configuration: Channel 1 Width 11 bits Wide
#define PCM_CH1WID_12     0x40000 // PCM Receive & Transmit Configuration: Channel 1 Width 12 bits Wide
#define PCM_CH1WID_13     0x50000 // PCM Receive & Transmit Configuration: Channel 1 Width 13 bits Wide
#define PCM_CH1WID_14     0x60000 // PCM Receive & Transmit Configuration: Channel 1 Width 14 bits Wide
#define PCM_CH1WID_15     0x70000 // PCM Receive & Transmit Configuration: Channel 1 Width 15 bits Wide
#define PCM_CH1WID_16     0x80000 // PCM Receive & Transmit Configuration: Channel 1 Width 16 bits Wide
#define PCM_CH1WID_17     0x90000 // PCM Receive & Transmit Configuration: Channel 1 Width 17 bits Wide
#define PCM_CH1WID_18     0xA0000 // PCM Receive & Transmit Configuration: Channel 1 Width 18 bits Wide
#define PCM_CH1WID_19     0xB0000 // PCM Receive & Transmit Configuration: Channel 1 Width 19 bits Wide
#define PCM_CH1WID_20     0xC0000 // PCM Receive & Transmit Configuration: Channel 1 Width 20 bits Wide
#define PCM_CH1WID_21     0xD0000 // PCM Receive & Transmit Configuration: Channel 1 Width 21 bits Wide
#define PCM_CH1WID_22     0xE0000 // PCM Receive & Transmit Configuration: Channel 1 Width 22 bits Wide
#define PCM_CH1WID_23     0xF0000 // PCM Receive & Transmit Configuration: Channel 1 Width 23 bits Wide
#define PCM_CH1WID        0xF0000 // PCM Receive & Transmit Configuration: Channel 1 Width
#define PCM_CH1WID_24  0x80000000 // PCM Receive & Transmit Configuration: Channel 1 Width 24 bits wide
#define PCM_CH1WID_25  0x80010000 // PCM Receive & Transmit Configuration: Channel 1 Width 25 bits wide
#define PCM_CH1WID_26  0x80020000 // PCM Receive & Transmit Configuration: Channel 1 Width 26 bits wide
#define PCM_CH1WID_27  0x80030000 // PCM Receive & Transmit Configuration: Channel 1 Width 27 bits wide
#define PCM_CH1WID_28  0x80040000 // PCM Receive & Transmit Configuration: Channel 1 Width 28 bits wide
#define PCM_CH1WID_29  0x80050000 // PCM Receive & Transmit Configuration: Channel 1 Width 29 bits wide
#define PCM_CH1WID_30  0x80060000 // PCM Receive & Transmit Configuration: Channel 1 Width 30 bits wide
#define PCM_CH1WID_31  0x80070000 // PCM Receive & Transmit Configuration: Channel 1 Width 31 bits wide
#define PCM_CH1WID_32  0x80080000 // PCM Receive & Transmit Configuration: Channel 1 Width 32 bits wide
#define PCM_CH1POS     0x3FF00000 // PCM Receive & Transmit Configuration: Channel 1 Position
#define PCM_CH1EN      0x40000000 // PCM Receive & Transmit Configuration: Channel 1 Enable
#define PCM_CH1WEX     0x80000000 // PCM Receive & Transmit Configuration: Channel 1 Width Extension Bit

#define PCM_RX              0x7F // PCM DMA Request Level: RX Request Level
#define PCM_TX            0x7F00 // PCM DMA Request Level: TX Request Level
#define PCM_RX_PANIC    0x7F0000 // PCM DMA Request Level: RX Panic Level
#define PCM_TX_PANIC  0x7F000000 // PCM DMA Request Level: TX Panic Level

//#define PCM_TXW    0x1 // PCM Interrupt Enables & Interrupt Status & Clear: TX Write Interrupt Enable
//#define PCM_RXR    0x2 // PCM Interrupt Enables & Interrupt Status & Clear: RX Read Interrupt Enable
//#define PCM_TXERR  0x4 // PCM Interrupt Enables & Interrupt Status & Clear: TX Error Interrupt
//#define PCM_RXERR  0x8 // PCM Interrupt Enables & Interrupt Status & Clear: RX Error Interrupt

#define PCM_GRAY_EN           0x1 // PCM Gray Mode Control: Enable GRAY Mode
#define PCM_GRAY_CLR          0x2 // PCM Gray Mode Control: Clear the GRAY Mode Logic
#define PCM_GRAY_FLUSH        0x4 // PCM Gray Mode Control: Flush the RX Buffer into the RX FIFO
#define PCM_RXLEVEL         0x3F0 // PCM Gray Mode Control: The Current Fill Level of the RX Buffer
#define PCM_FLUSHED        0xFC00 // PCM Gray Mode Control: The Number of Bits that were Flushed into the RX FIFO
#define PCM_RXFIFOLEVEL  0x3F0000 // PCM Gray Mode Control: The Current Level of the RX FIFO

/* PWM / Pulse Width Modulator Interface */

struct _pwm {
    uint32_t ctl;   // PWM Control
    uint32_t sta;   // PWM Status
    uint32_t dmac;  // PWM DMA Configuration
    uint32_t res1;
    uint32_t rng1;  // PWM Channel 1 Range
    uint32_t dat1;  // PWM Channel 1 Data
    uint32_t fif1;  // PWM FIFO Input
    uint32_t res2;
    uint32_t rng2;  // PWM Channel 2 Range
    uint32_t dat2;  // PWM Channel 2 Data
};

#define PWM_BASE  0x20C000 // PWM Base Address
#define PWM_CTL        0x0 // PWM Control
#define PWM_STA        0x4 // PWM Status
#define PWM_DMAC       0x8 // PWM DMA Configuration
#define PWM_RNG1      0x10 // PWM Channel 1 Range
#define PWM_DAT1      0x14 // PWM Channel 1 Data
#define PWM_FIF1      0x18 // PWM FIFO Input
#define PWM_RNG2      0x20 // PWM Channel 2 Range
#define PWM_DAT2      0x24 // PWM Channel 2 Data

#define PWM_PWEN1     0x1 // PWM Control: Channel 1 Enable
#define PWM_MODE1     0x2 // PWM Control: Channel 1 Mode
#define PWM_RPTL1     0x4 // PWM Control: Channel 1 Repeat Last Data
#define PWM_SBIT1     0x8 // PWM Control: Channel 1 Silence Bit
#define PWM_POLA1    0x10 // PWM Control: Channel 1 Polarity
#define PWM_USEF1    0x20 // PWM Control: Channel 1 Use Fifo
#define PWM_CLRF1    0x40 // PWM Control: Clear Fifo
#define PWM_MSEN1    0x80 // PWM Control: Channel 1 M/S Enable
#define PWM_PWEN2   0x100 // PWM Control: Channel 2 Enable
#define PWM_MODE2   0x200 // PWM Control: Channel 2 Mode
#define PWM_RPTL2   0x400 // PWM Control: Channel 2 Repeat Last Data
#define PWM_SBIT2   0x800 // PWM Control: Channel 2 Silence Bit
#define PWM_POLA2  0x1000 // PWM Control: Channel 2 Polarity
#define PWM_USEF2  0x2000 // PWM Control: Channel 2 Use Fifo
#define PWM_MSEN2  0x8000 // PWM Control: Channel 2 M/S Enable

#define PWM_FULL1     0x1 // PWM Status: Fifo Full Flag
#define PWM_EMPT1     0x2 // PWM Status: Fifo Empty Flag
#define PWM_WERR1     0x4 // PWM Status: Fifo Write Error Flag
#define PWM_RERR1     0x8 // PWM Status: Fifo Read Error Flag
#define PWM_GAPO1    0x10 // PWM Status: Channel 1 Gap Occurred Flag
#define PWM_GAPO2    0x20 // PWM Status: Channel 2 Gap Occurred Flag
#define PWM_GAPO3    0x40 // PWM Status: Channel 3 Gap Occurred Flag
#define PWM_GAPO4    0x80 // PWM Status: Channel 4 Gap Occurred Flag
#define PWM_BERR    0x100 // PWM Status: Bus Error Flag
#define PWM_STA1    0x200 // PWM Status: Channel 1 State
#define PWM_STA2    0x400 // PWM Status: Channel 2 State
#define PWM_STA3    0x800 // PWM Status: Channel 3 State
#define PWM_STA4   0x1000 // PWM Status: Channel 4 State

#define PWM_ENAB  0x80000000 // PWM DMA Configuration: DMA Enable

/* Interrupt Registers */

struct _irq {
    uint32_t irqBasicPending;
    uint32_t irq1Pending;
    uint32_t irq2Pending;
    uint32_t fiqControl;
    uint32_t irq1Enable;
    uint32_t irq2Enable;
    uint32_t irqBasicEnable;
    uint32_t irq1Disable;
    uint32_t irq2Disable;
    uint32_t irqBasicDisable;
};

#define IRQ_BASE                      0xB200
#define IRQ0_PENDING                     0x0
#define IRQ1_PENDING                     0x4
#define IRQ2_PENDING                     0x8
#define FIQ_PENDING                      0xC
#define IRQ1_ENABLE                     0x10
#define IRQ2_ENABLE                     0x14
#define IRQ0_ENABLE                     0x18
#define IRQ1_DISABLE                    0x1C
#define IRQ2_DISABLE                    0x20
#define IRQ0_DISABLE                    0x24

/* IRQ1 */

#define INTERRUPT_TIMER0                 0x1
#define INTERRUPT_TIMER1                 0x2
#define INTERRUPT_TIMER2                 0x4
#define INTERRUPT_TIMER3                 0x8
#define INTERRUPT_CODEC0                0x10
#define INTERRUPT_CODEC1                0x20
#define INTERRUPT_CODEC2                0x40
#define INTERRUPT_VC_JPEG               0x80
#define INTERRUPT_ISP                  0x100
#define INTERRUPT_VC_USB               0x200
#define INTERRUPT_VC_3D                0x400
#define INTERRUPT_TRANSPOSER           0x800
#define INTERRUPT_MULTICORESYNC0      0x1000
#define INTERRUPT_MULTICORESYNC1      0x2000
#define INTERRUPT_MULTICORESYNC2      0x4000
#define INTERRUPT_MULTICORESYNC3      0x8000
#define INTERRUPT_DMA0               0x10000
#define INTERRUPT_DMA1               0x20000
#define INTERRUPT_VC_DMA2            0x40000
#define INTERRUPT_VC_DMA3            0x80000
#define INTERRUPT_DMA4              0x100000
#define INTERRUPT_DMA5              0x200000
#define INTERRUPT_DMA6              0x400000
#define INTERRUPT_DMA7              0x800000
#define INTERRUPT_DMA8             0x1000000
#define INTERRUPT_DMA9             0x2000000
#define INTERRUPT_DMA10            0x4000000
#define INTERRUPT_DMA11            0x8000000
#define INTERRUPT_DMA12           0x10000000
#define INTERRUPT_AUX             0x20000000
#define INTERRUPT_ARM             0x40000000
#define INTERRUPT_VPUDMA          0x80000000

/* IRQ2 */

#define INTERRUPT_HOSTPORT               0x1
#define INTERRUPT_VIDEOSCALER            0x2
#define INTERRUPT_CCP2TX                 0x4
#define INTERRUPT_SDC                    0x8
#define INTERRUPT_DSI0                  0x10
#define INTERRUPT_AVE                   0x20
#define INTERRUPT_CAM0                  0x40
#define INTERRUPT_CAM1                  0x80
#define INTERRUPT_HDMI0                0x100
#define INTERRUPT_HDMI1                0x200
#define INTERRUPT_PIXELVALVE1          0x400
#define INTERRUPT_I2CSPISLV            0x800
#define INTERRUPT_DSI1                0x1000
#define INTERRUPT_PWA0                0x2000
#define INTERRUPT_PWA1                0x4000
#define INTERRUPT_CPR                 0x8000
#define INTERRUPT_SMI                0x10000
#define INTERRUPT_GPIO0              0x20000
#define INTERRUPT_GPIO1              0x40000
#define INTERRUPT_GPIO2              0x80000
#define INTERRUPT_GPIO3             0x100000
#define INTERRUPT_VC_I2C            0x200000
#define INTERRUPT_VC_SPI            0x400000
#define INTERRUPT_VC_I2SPCM         0x800000
#define INTERRUPT_VC_SDIO          0x1000000
#define INTERRUPT_VC_UART          0x2000000
#define INTERRUPT_SLIMBUS          0x4000000
#define INTERRUPT_VEC              0x8000000
#define INTERRUPT_CPG             0x10000000
#define INTERRUPT_RNG             0x20000000
#define INTERRUPT_VC_ARASANSDIO   0x40000000
#define INTERRUPT_AVSPMON         0x80000000

/* IRQ0 / Basic */

#define INTERRUPT_ARM_TIMER              0x1
#define INTERRUPT_ARM_MAILBOX            0x2
#define INTERRUPT_ARM_DOORBELL_0         0x4
#define INTERRUPT_ARM_DOORBELL_1         0x8
#define INTERRUPT_VPU0_HALTED           0x10
#define INTERRUPT_VPU1_HALTED           0x20
#define INTERRUPT_ILLEGAL_TYPE0         0x40
#define INTERRUPT_ILLEGAL_TYPE1         0x80
#define INTERRUPT_PENDING1             0x100
#define INTERRUPT_PENDING2             0x200
#define INTERRUPT_JPEG                 0x400
#define INTERRUPT_USB                  0x800
#define INTERRUPT_3D                  0x1000
#define INTERRUPT_DMA2                0x2000
#define INTERRUPT_DMA3                0x4000
#define INTERRUPT_I2C                 0x8000
#define INTERRUPT_SPI                0x10000
#define INTERRUPT_I2SPCM             0x20000
#define INTERRUPT_SDIO               0x40000
#define INTERRUPT_UART               0x80000
#define INTERRUPT_ARASANSDIO        0x100000

struct timer_wait {
    unsigned int usec;
    unsigned int trigger_value;
    int          rollover;
};

#if defined(__cplusplus)
extern "C" {
#endif

__attribute__ ((naked)) void dmb();
__attribute__ ((naked)) void flush_cache();

void     mmio_write(uint32_t reg, uint32_t data);
uint32_t mmio_read(uint32_t reg);

void     mbox_write(uint8_t channel, uint32_t data);
uint32_t mbox_read(uint8_t channel);

int      usleep(useconds_t usec);

void register_timer(struct timer_wait * tw, unsigned int usec);
int compare_timer(struct timer_wait * tw);

#if defined(__cplusplus)
}
#endif

extern volatile struct   _dma * DMA;
extern volatile struct _clock * CLK;
extern volatile struct   _pwm * PWM;
extern volatile struct  _gpio * GPIO;
extern volatile struct   _irq * IRQ;

#endif /* PLATFORM_H_ */
