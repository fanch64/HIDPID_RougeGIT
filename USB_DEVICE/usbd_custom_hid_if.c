/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v2.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */
#include "main.h"
#include "usart.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define MAX_BLOCK_INDEX 16
uint8_t dataToReceive [ 20 ] ; 

extern __IO uint8_t PrevXferComplete;
extern uint8_t *pUSB_POOL;
uint32_t nReceivedBytes = 0;
uint8_t Receive_Buffer[20];
uint8_t bindex = 0;
uint8_t Global_Gain = 0xFF;
uint8_t id_create=0;
//int16_t Magnitude = 0;
uint16_t USED_BLOCKS = 0;		//bit adressable 1=used 0=free
uint16_t FREE_MEM = ALLOCATED_BLOCK_MEM - (SET_EFFECT_REPORT_SIZE * MAX_BLOCK_INDEX);

uint8_t EffectsPlaying[MaxContiguousEffects];					// effects en cours, limitées a 8

// callback function prototypes
//uint8_t *Joystick_GetReport_Feature(uint16_t Length);
//uint8_t *Joystick_SetReport_Feature(uint16_t Length);
//uint8_t HID_GetReport_Value[5]; //valeur a priori du plus long feature report
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */

//working volant descriptor size: 1294

	0x05,0x01,  //    Usage Page Generic Desktop
		0x09,0x04,  //    Usage Joystick
			0xA1,0x01,  //    Collection Application
				0x85,0x01,        //    Report ID 1
		0x05,0x02,	//usage page simcontrol
					0x09,0xC8,	//usage steering
						0x16, 0x01, 0x80,              //   LOGICAL_MINIMUM (-32768)
						0x26, 0xff, 0x7f,              //   LOGICAL_MAXIMUM (32767)
						0x36, 0x3e, 0xfe,              //   PHYSICAL_MINIMUM (-450°)
						0x46, 0xc2, 0x01,              //   PHYSICAL_MAXIMUM (450°)
						0x65, 0x12,                    //   UNIT (SI Rot:Angular Pos)		
						0x75, 0x10,        						//    Report Size Ah (16d)
						0x95, 0x01,     							   //    Report Count 1
						0x81, 0x02,        //    Input (Variable)
					0x09, 0xC4,	//usage accelerator
					0x09, 0xC5,	//usage brake
						0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
						0x26, 0xFF, 0x0F,              //     LOGICAL_MAXIMUM (4095)
						0x35, 0x00,            			   //   PHYSICAL_MINIMUM (0)
						0x46, 0xFF, 0x0F,        			 //   PHYSICAL_MAXIMUM (4095)
						0x75, 0x10,        //    Report Size Ah (16d)
						0x95, 0x02,        //    Report Count 2
						0x81, 0x02,        //    Input (Variable)
				0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
					0x09, 0x01,                    //   USAGE (Pointer)
						0xa1, 0x00,                    //   COLLECTION (Physical)
							0x09, 0x33,                    //     USAGE (Rx)
							0x09, 0x34,                    //     USAGE (Ry)
								0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
								0x26, 0xFF, 0x0F,              //     LOGICAL_MAXIMUM (4095)
								0x46, 0xFF, 0x0F,             			 //   PHYSICAL_MAXIMUM (4095)
								0x75, 0x10,                    //     REPORT_SIZE (16)
								0x95, 0x02,                    //     REPORT_COUNT (2)
								0x81, 0x02,                    //     INPUT (Data,Var,Abs)
						0xC0    ,         //    End Collection (Physical)
				0x05,0x09,        //    Usage Page Button
					0x19,0x00,        //    Usage Minimum no button pressed
					0x29,0x10,        //    Usage Maximum Button 08
					0x15,0x00,        //    Logical Minimum 0
					0x25,0x01,        //    Logical Maximum 1
					0x35,0x00,        //    Physical Minimum 0
					0x45,0x01,        //    Physical Maximum 1
					0x75,0x01,        //    Report Size 1
					0x95,0x10,        //    Report Count 8
					0x81,0x02,        //    Input (Variable)

//-------------------------------------------		
    0x05,0x0F,        //    Usage Page Physical Interface size=69
    0x09,0x92,        //    PID State Report
    0xA1,0x02,        //    Collection Logical
       0x85,0x02,    //    Report ID 2
       0x09,0x9F,    //    Usage DS Device pause
       0x09,0xA0,    //    Usage Actuator enable
       0x09,0xA4,    //    safety switch
       0x09,0xA5,    //    actuator override switch
       0x09,0xA6,    //    actuator power
       0x15,0x00,    //    Logical Minimum 0
       0x25,0x01,    //    Logical Maximum 1
       0x35,0x00,    //    Physical Minimum 0
       0x45,0x01,    //    Physical Maximum 1
       0x75,0x01,    //    Report Size 1
       0x95,0x05,    //    Report Count 5
       0x81,0x02,    //    Input (Variable)
       0x95,0x03,    //    Report Count 3
       0x81,0x03,    //    Input (Constant, Variable)
       0x09,0x94,    //    Effect playing
       0x15,0x00,    //    Logical Minimum 0
       0x25,0x01,    //    Logical Maximum 1
       0x35,0x00,    //    Physical Minimum 0
       0x45,0x01,    //    Physical Maximum 1
       0x75,0x01,    //    Report Size 1
       0x95,0x01,    //    Report Count 1
       0x81,0x02,    //    Input (Variable)
       0x09,0x22,    //    Usage Effect Block Index	//doit servir a retourner l'effet entrain de jouer
       0x15,0x01,    //    Logical Minimum 1
       0x25,MAX_BLOCK_INDEX,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,MAX_BLOCK_INDEX,    //    Physical Maximum 28h (40d)
       0x75,0x07,    //    Report Size 7
       0x95,0x01,    //    Report Count 1
       0x81,0x02,    //    Input (Variable)
    0xC0    ,    // End Collection

//---------------------------------------outputs -----------------------------		
    0x09,0x21,    //    Usage Set Effect Report
    0xA1,0x02,    //    Collection Logical
       0x85,0x01,    //    Report ID 1
       0x09,0x22,    //    Usage Effect Block Index
       0x15,0x01,    //    Logical Minimum 1
       0x25,MAX_BLOCK_INDEX,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,MAX_BLOCK_INDEX,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x09,0x25,    //    Usage Effect Type
       0xA1,0x02,    //    Collection Logical
          0x09,0x26,    //01    Usage ET Constant Force
          0x09,0x27,    //02    Usage ET Ramp
          0x09,0x30,    //03    Usage ET Square
          0x09,0x31,    //04    Usage ET Sine
          0x09,0x32,    //05    Usage ET Triangle
          0x09,0x33,    //06    Usage ET Sawtooth Up
          0x09,0x34,    //07    Usage ET Sawtooth Down
          0x09,0x40,    //08    Usage ET Spring
          0x09,0x41,    //09    Usage ET Damper
          0x09,0x42,    //0a    Usage ET Inertia
          0x09,0x43,    //0b    Usage ET Friction
          0x09,0x28,    //0c    Usage ET Custom Force Data
          0x25,0x0C,    //    Logical Maximum Ch (12d)
          0x15,0x01,    //    Logical Minimum 1
          0x35,0x01,    //    Physical Minimum 1
          0x45,0x0C,    //    Physical Maximum Ch (12d)
          0x75,0x08,    //    Report Size 8
          0x95,0x01,    //    Report Count 1
          0x91,0x00,    //    Output
       0xC0    ,          //    End Collection
       0x09,0x50,         //    Usage Duration
       0x09,0x54,         //    Usage Trigger Repeat Interval
       0x09,0x51,         //    Usage Sample Period
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x7F,    //    Logical Maximum 7FFFh (32767d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x7F,    //    Physical Maximum 7FFFh (32767d)
       0x66,0x03,0x10,    //    Unit 1003h (4099d)
       0x55,0xFD,         //    Unit Exponent FDh (-3?253d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x03,         //    Report Count 3
       0x91,0x02,         //    Output (Variable)
       0x55,0x00,         //    Unit Exponent 0
       0x66,0x00,0x00,    //    Unit 0
       0x09,0x52,         //    Usage Gain
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x53,         //    Usage Trigger Button
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x08,         //    Logical Maximum 8
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x08,         //    Physical Maximum 8
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
			 //------------
			 //US – The Axes Enable collection contains joint collections. Each
			//joint collection contains axes or vectors from the Generic Desktop
			//page. This usage changes the type of these Generic Desktop
			//usages to Dynamic Flags (DF) where each usage identifies
			//whether the respective axis or vector is enabled for this effect.
       0x09,0x55,         //    Usage Axes Enable
       0xA1,0x02,         //    Collection Logical
          0x05,0x01,    //    Usage Page Generic Desktop
          0x09,0x30,    //    Usage X
					//0x09,0x00,
          0x09,0x31,    //    Usage Y
          0x15,0x00,    //    Logical Minimum 0
          0x25,0x01,    //    Logical Maximum 1
          0x75,0x01,    //    Report Size 1
          0x95,0x02,//0x02    //    Report Count 2
          0x91,0x02,    //    Output (Variable)
       0xC0     ,    // End Collection
			 //------------
			 //DF – If the Direction Enable flag is set then the Axes Enable
			//flags are ignored, only one Condition Parameter Block is defined
			//and the Direction is applied to the Condition Parameter Block as a
			//polar direction.
       0x05,0x0F,    //    Usage Page Physical Interface
       0x09,0x56,    //    Usage Direction Enable
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x95,0x05,    //    Report Count 5
       0x91,0x03,    // filler    Output (Constant, Variable)
			 //----------------
			 //CL – The Direction collection contains joint collections. Each
			//joint collection contains axes or vectors from the Generic Desktop
			//page. Each axis usage is treated as a Dynamic Variable (DV).
			//If the values are in Cartesian coordinates then axes usages X, Y or
			//Z or vector usages Vx, Vy or Vz will be declared in this collection
			//as normalized values.
			//If the values are in polar coordinates then axes usages Rx, Ry orDevice Class Definition for Physical Interface Devices (PID) Version 1.0.10
			//Rz or vector usages Vbrx, Vbry or Vbrz will be declared in this
			//collection as normalized values.
			// comme direction enable est SET, val1 = angle, val2 = magnitude
       0x09,0x57,    //    Usage Direction
       0xA1,0x02,    //    Collection Logical
          0x0B,0x01,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 1
          0x0B,0x02,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 2
          0x66,0x14,0x00,              //    Unit 14h (20d)
          0x55,0xFE,                   //    Unit Exponent FEh (254d)
          0x15,0x00,                   //    Logical Minimum 0
          0x26,0xFF,0x00,              //    Logical Maximum FFh (255d)
          0x35,0x00,                   //    Physical Minimum 0
          0x47,0xA0,0x8C,0x00,0x00,    //00?    Physical Maximum 8CA0h (36000d)
          0x66,0x00,0x00,              //    Unit 0
          0x75,0x08,                   //    Report Size 8
          0x95,0x02,                   //    Report Count 2
          0x91,0x02,                   //    Output (Variable)
          0x55,0x00,                   //    Unit Exponent 0
          0x66,0x00,0x00,              //    Unit 0
       0xC0     ,         //    End Collection
       0x05,0x0F,         //    Usage Page Physical Interface
       0x09,0xA7,         //    Start Delay
       0x66,0x03,0x10,    //    Unit 1003h (4099d)
       0x55,0xFD,         //    Unit Exponent FDh (-3?253d)
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x7F,    //    Logical Maximum 7FFFh (32767d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x7F,    //    Physical Maximum 7FFFh (32767d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x66,0x00,0x00,    //    Unit 0
       0x55,0x00,         //    Unit Exponent 0
    0xC0     ,    //    End Collection
//-----------------------------------------------		
    0x05,0x0F,    //    Usage Page Physical Interface
    0x09,0x5A,    //    Usage Set Envelope Report
    0xA1,0x02,    //    Collection Logical
       0x85,0x02,         //    Report ID 2
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,MAX_BLOCK_INDEX,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,MAX_BLOCK_INDEX,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x5B,         //    Usage Attack Level
       0x09,0x5D,         //    Usage Fade Level
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
       0x09,0x5C,         //    Usage Attack Time
       0x09,0x5E,         //    Usage Fade Time
       0x66,0x03,0x10,    //    Unit 1003h (4099d)
       0x55,0xFD,         //    Unit Exponent FDh (-3?253d)
       0x26,0xFF,0x7F,    //    Logical Maximum 7FFFh (32767d)
       0x46,0xFF,0x7F,    //    Physical Maximum 7FFFh (32767d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x91,0x02,         //    Output (Variable)
       0x45,0x00,         //    Physical Maximum 0
       0x66,0x00,0x00,    //    Unit 0
       0x55,0x00,         //    Unit Exponent 0
    0xC0     ,            //    End Collection
//--------------------------------------------------------		
    0x09,0x5F,    //    Usage Set Condition Report
    0xA1,0x02,    //    Collection logical
       0x85,0x03,    //    Report ID 3
       0x09,0x22,    //    Usage Effect Block Index
       0x15,0x01,    //    Logical Minimum 1
       0x25,MAX_BLOCK_INDEX,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,MAX_BLOCK_INDEX,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x09,0x23,    //    Usage Parameter Block Offset
       0x15,0x00,    //    Logical Minimum 0
       0x25,0x01,    //    Logical Maximum 1
       0x35,0x00,    //    Physical Minimum 0
       0x45,0x01,    //    Physical Maximum 1
       0x75,0x04,    //    Report Size 4
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x09,0x58,    //    Usage Type Specific Block Off...
       0xA1,0x02,    //    Collection Logical
          0x0B,0x01,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 1
          0x0B,0x02,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 2
          0x75,0x02,                   //    Report Size 2
          0x95,0x02,                   //    Report Count 2
          0x91,0x02,                   //    Output (Variable)
       0xC0     ,         //    End Collection
       0x15,0x81,         //    Logical Minimum 81h (-128d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x09,0x60,         //    Usage CP Offset
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x09,0x61,         //    Usage Positive Coefficient
       0x09,0x62,         //    Usage Negative Coefficient
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x09,0x63,         //    Usage Positive Saturation
       0x09,0x64,         //    Usage Negative Saturation
       0x75,0x08,         //    Report Size 8
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
       0x09,0x65,         //    Usage Dead Band
       0x35,0x00,         //    Physical Minimum 0
			 0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
    0xC0     ,    //    End Collection
//------------------------------------------------------------------------------------------------		
    0x09,0x6E,    //    Usage Set Periodic Report
    0xA1,0x02,    //    Collection Logical
       0x85,0x04,                   //    Report ID 4
       0x09,0x22,                   //    Usage Effect Block Index
       0x15,0x01,                   //    Logical Minimum 1
       0x25,MAX_BLOCK_INDEX,                   //    Logical Maximum 28h (40d)
       0x35,0x01,                   //    Physical Minimum 1
       0x45,MAX_BLOCK_INDEX,                   //    Physical Maximum 28h (40d)
       0x75,0x08,                   //    Report Size 8
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x09,0x70,                   //    Usage Magnitude
       0x15,0x00,                   //    Logical Minimum 0
       0x26,0xFF,0x00,              //    Logical Maximum FFh (255d)
       0x35,0x00,                   //    Physical Minimum 0
       0x46,0x10,0x27,              //    Physical Maximum 2710h (10000d)
       0x75,0x08,                   //    Report Size 8
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x09,0x6F,                   //    Usage Offset
       0x15,0x81,                   //    Logical Minimum 81h (-127d)
       0x25,0x7F,                   //    Logical Maximum 7Fh (127d)
       0x36,0xF0,0xD8,              //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,              //    Physical Maximum 2710h (10000d)
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x09,0x71,                   //    Usage Phase
       0x66,0x14,0x00,              //    Unit 14h (20d)
       0x55,0xFE,                   //    Unit Exponent FEh (254d)
       0x15,0x00,                   //    Logical Minimum 0
       0x26,0xFF,0x00,              //    Logical Maximum FFh (255d)
       0x35,0x00,                   //    Physical Minimum 0
       0x47,0xA0,0x8C,0x00,0x00,    //    Physical Maximum 8CA0h (36000d)
       0x91,0x02,                   //    Output (Variable)
       0x09,0x72,                   //    Usage Period
       0x26,0xFF,0x7F,              //    Logical Maximum 7FFFh (32767d)
       0x46,0xFF,0x7F,              //    Physical Maximum 7FFFh (32767d)
       0x66,0x03,0x10,              //    Unit 1003h (4099d)
       0x55,0xFD,                   //    Unit Exponent FDh (253d)
       0x75,0x10,                   //    Report Size 10h (16d)
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x66,0x00,0x00,              //    Unit 0
       0x55,0x00,                   //    Unit Exponent 0
    0xC0     ,    // End Collection
    0x09,0x73,    //    Usage Set Constant Force Rep...
    0xA1,0x02,    //    Collection Logical
// ------------------------------------------------		
       0x85,0x05,         //    Report ID 5
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,MAX_BLOCK_INDEX,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,MAX_BLOCK_INDEX,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x70,         //    Usage Magnitude
       0x16,0x00,0x80, //-32768   //    Logical Minimum FF01h (-32768d)
       0x26,0xFF,0x7f, //32767   //    Logical Maximum FFh (32767d)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
    0xC0     ,    //    End Collection
// ----------------------------------------------------------		
    0x09,0x74,    //    Usage Set Ramp Force Report
    0xA1,0x02,    //    Collection Logical
       0x85,0x06,         //    Report ID 6
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,MAX_BLOCK_INDEX,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,MAX_BLOCK_INDEX,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x75,         //    Usage Ramp Start
       0x09,0x76,         //    Usage Ramp End
       0x15,0x80,         //    Logical Minimum 80h (-128d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x08,         //    Report Size 8
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
    0xC0     ,    //    End Collection
// --------------------------------------------------------------------
    0x09,0x68,    //    Usage Custom Force Data Rep...
    0xA1,0x02,    //    Collection Logical
       0x85,0x07,         //    Report ID 7
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,MAX_BLOCK_INDEX,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,MAX_BLOCK_INDEX,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x6C,         //    Usage Custom Force Data Offset
       0x15,0x00,         //    Logical Minimum 0
       0x26,0x10,0x27,    //    Logical Maximum 2710h (10000d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x69,         //    Usage Custom Force Data
       0x15,0x81,         //    Logical Minimum 81h (-127d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x75,0x08,         //    Report Size 8
       0x95,0x0C,         //    Report Count Ch (12d)
       0x92,0x02,0x01,    //       Output (Variable, Buffered)
    0xC0     ,    //    End Collection
// ----------------------------------------------------------------------		
    0x09,0x66,    //    Usage Download Force Sample
    0xA1,0x02,    //    Collection Logical
       0x85,0x08,         //    Report ID 8
       0x05,0x01,         //    Usage Page Generic Desktop
       0x09,0x30,         //    Usage X
       0x09,0x31,         //    Usage Y
       0x15,0x81,         //    Logical Minimum 81h (-127d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x75,0x08,         //    Report Size 8
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
    0xC0     ,   //    End Collection
//-----------------------------------------------------------------		
    0x05,0x0F,   //    Usage Page Physical Interface
    0x09,0x77,   //    Usage Effect Operation Report
    0xA1,0x02,   //    Collection Logical
       0x85,0x0A,    //    Report ID Ah (10d)
       0x09,0x22,    //    Usage Effect Block Index
       0x15,0x01,    //    Logical Minimum 1
       0x25,MAX_BLOCK_INDEX,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,MAX_BLOCK_INDEX,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)									8
       0x09,0x78,    //    Usage Effect Operation
       0xA1,0x02,    //    Collection Logical
          0x09,0x79,    //    Usage Op Effect Start
          0x09,0x7A,    //    Usage Op Effect Start Solo
          0x09,0x7B,    //    Usage Op Effect Stop
          0x15,0x01,    //    Logical Minimum 1
          0x25,0x03,    //    Logical Maximum 3
          0x75,0x08,    //    Report Size 8
          0x95,0x01,    //    Report Count 1
          0x91,0x00,    //    Output													8
       0xC0     ,         //    End Collection
       0x09,0x7C,         //    Usage Loop Count
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x91,0x02,         //    Output (Variable)							8
    0xC0     ,    //    End Collection
//--------------------------------------------------------------------		
    0x09,0x90,    //    Usage PID Block Free Report
    0xA1,0x02,    //    Collection logical
       0x85,0x0B,    //    Report ID Bh (11d)
       0x09,0x22,    //    Usage Effect Block Index
       0x25,MAX_BLOCK_INDEX,    //    Logical Maximum 28h (40d)
       0x15,0x01,    //    Logical Minimum 1
       0x35,0x01,    //    Physical Minimum 1
       0x45,MAX_BLOCK_INDEX,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
    0xC0     ,    //    End Collection
//-----------------------------------------------------------------		
    0x09,0x96,    //    Usage PID Device Control
    0xA1,0x02,    //    Collection Logical
       0x85,0x0C,    //    Report ID Ch (12d)
       0x09,0x97,    //1    Usage Enable Actuators
       0x09,0x98,    //2    Usage Disable Actuators
       0x09,0x99,    //3    Usage Stop All Effects
       0x09,0x9A,    //4    Usage Device Reset
       0x09,0x9B,    //5    Usage Device Pause
       0x09,0x9C,    //6    Usage Device continue
       0x15,0x01,    //    Logical Minimum 1
       0x25,0x06,    //    Logical Maximum 6
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x00,    //    Output
    0xC0     ,    //    End Collection
//		-------------------------------
    0x09,0x7D,    //    Usage Device Gain Report
    0xA1,0x02,    //    Collection Logical
       0x85,0x0D,         //    Report ID Dh (13d)
       0x09,0x7E,         //    Usage Device Gain
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
    0xC0     ,            //    End Collection
// -----------------------------------------------		
    0x09,0x6B,    //    Usage Set Custom Force Report
    0xA1,0x02,    //    Collection Logical
       0x85,0x0E,         //    Report ID Eh (14d)
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,MAX_BLOCK_INDEX,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,MAX_BLOCK_INDEX,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x6D,         //    Usage Sample Count
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x51,         //    Usage Sample Period
       0x66,0x03,0x10,    //    Unit 1003h (4099d)
       0x55,0xFD,         //    Unit Exponent FDh (253d)
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x7F,    //    Logical Maximum 7FFFh (32767d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x7F,    //    Physical Maximum 7FFFh (32767d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x55,0x00,         //    Unit Exponent 0
       0x66,0x00,0x00,    //    Unit 0
    0xC0     ,    //    End Collection
//--------------------------------------------------------------
    0x09,0xAB,    //    Create New Effect Report (0xab)
    0xA1,0x02,    //    Collection Logical
       0x85,0x01,    //    Report ID 1
       0x09,0x25,    //    Usage Effect Type
       0xA1,0x02,    //    Collection Logical
       0x09,0x26,    //    Usage ET Constant Force
       0x09,0x27,    //    Usage ET Ramp
       0x09,0x30,    //    Usage ET Square
       0x09,0x31,    //    Usage ET Sine
       0x09,0x32,    //    Usage ET Triangle
       0x09,0x33,    //    Usage ET Sawtooth Up
       0x09,0x34,    //    Usage ET Sawtooth Down
       0x09,0x40,    //    Usage ET Spring
       0x09,0x41,    //    Usage ET Damper
       0x09,0x42,    //    Usage ET Inertia
       0x09,0x43,    //    Usage ET Friction
       0x09,0x28,    //    Usage ET Custom Force Data
       0x25,0x0C,    //    Logical Maximum Ch (12d)
       0x15,0x01,    //    Logical Minimum 1
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x0C,    //    Physical Maximum Ch (12d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0xB1,0x00,    //    Feature
    0xC0     ,    // End Collection
    0x05,0x01,         //    Usage Page Generic Desktop
    0x09,0x3B,         //    Usage Byte Count
    0x15,0x00,         //    Logical Minimum 0
    0x26,0xFF,0x01,    //    Logical Maximum 1FFh (511d)
    0x35,0x00,         //    Physical Minimum 0
    0x46,0xFF,0x01,    //    Physical Maximum 1FFh (511d)
    0x75,0x0A,         //    Report Size Ah (10d)
    0x95,0x01,         //    Report Count 1
    0xB1,0x02,         //    Feature (Variable)
    0x75,0x06,         //    Report Size 6
    0xB1,0x01,         //    Feature (Constant)
 0xC0     ,    //    End Collection
// //74
//------------------------------------------------------------------
 0x05,0x0F,    //    Usage Page Physical Interface
 0x09,0x89,    //    Usage PID Block Load Report
 0xA1,0x02,    //    Collection Logical
    0x85,0x02,    //    Report ID 2
    0x09,0x22,    //    Usage Effect Block Index
    0x25,MAX_BLOCK_INDEX,    //    Logical Maximum 28h (40d)
    0x15,0x01,    //    Logical Minimum 1
    0x35,0x01,    //    Physical Minimum 1
    0x45,MAX_BLOCK_INDEX,    //    Physical Maximum 28h (40d)
    0x75,0x08,    //    Report Size 8
    0x95,0x01,    //    Report Count 1
    0xB1,0x02,    //    Feature (Variable)
    0x09,0x8B,    //    Usage Block Load Status
    0xA1,0x02,    //    Collection Logical
       0x09,0x8C,    //    Usage Block Load succes
       0x09,0x8D,    //    Usage Block load full
       0x09,0x8E,    //    Usage Block load error
       0x25,0x03,    //    Logical Maximum 3
       0x15,0x01,    //    Logical Minimum 1
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x03,    //    Physical Maximum 3
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0xB1,0x00,    //    Feature
    0xC0     ,                   // End Collection
    0x09,0xAC,                   //    Usage RAM Pool Available (0xac)
    0x15,0x00,                   //    Logical Minimum 0
    0x27,0xFF,0xFF,0x00,0x00,    //    Logical Maximum FFFFh (65535d)
    0x35,0x00,                   //    Physical Minimum 0
    0x47,0xFF,0xFF,0x00,0x00,    //    Physical Maximum FFFFh (65535d)
    0x75,0x10,                   //    Report Size 10h (16d)
    0x95,0x01,                   //    Report Count 1
    0xB1,0x00,                   //    Feature
 0xC0     ,    //    End Collection
// //72
//---------------------------------------------------------------------
 0x09,0x7E, //7F   //    Usage PID Pool Report (0x7f)	
 0xA1,0x02,    //    Collection Logical
    0x85,0x03,                   //    Report ID 3
    0x09,0x80,                   //    Usage Ram Pool Size
    0x75,0x10,                   //    Report Size 10h (16d)
    0x95,0x01,                   //    Report Count 1
    0x15,0x00,                   //    Logical Minimum 0
    0x35,0x00,                   //    Physical Minimum 0
    0x27,0xFF,0xFF,0x00,0x00,    //    Logical Maximum FFFFh (65535d)
    0x47,0xFF,0xFF,0x00,0x00,    //    Physical Maximum FFFFh (65535d)
    0xB1,0x02,                   //    Feature (Variable)
    0x09,0x83,                   //    Usage Simultaneous Effects Max (0x83)
    0x26,0xFF,0x00,              //    Logical Maximum FFh (255d)
    0x46,0xFF,0x00,              //    Physical Maximum FFh (255d)
    0x75,0x08,                   //    Report Size 8
    0x95,0x01,                   //    Report Count 1
    0xB1,0x02,                   //    Feature (Variable)
    0x09,0xA9,                   //    Usage Device Managed Pool (0xa9)
    0x09,0xAA,                   //    Usage Shared Parameter Blocks (0xaa)
    0x75,0x01,                   //    Report Size 1
    0x95,0x02,                   //    Report Count 2
    0x15,0x00,                   //    Logical Minimum 0
    0x25,0x01,                   //    Logical Maximum 1
    0x35,0x00,                   //    Physical Minimum 0
    0x45,0x01,                   //    Physical Maximum 1
    0xB1,0x02,                   //    Feature (Variable)
    0x75,0x06,                   //    Report Size 6
    0x95,0x01,                   //    Report Count 1
    0xB1,0x03,                   //    Feature (Constant, Variable)
    0xC0,   										 //    End Collection
//67
			
  /* USER CODE END 0 */
  0xC0    /*     END_COLLECTION	             */
};

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  /* USER CODE BEGIN 6 */

	USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData; 

if (	hhid->IsReportAvailable == 1){
printf("CUSTOM_HID_OutEvent_FS\r\n");
	uint32_t lastSize0 = USBD_LL_GetRxDataSize(&hUsbDeviceFS, 0);
	uint32_t lastSize1 = USBD_LL_GetRxDataSize(&hUsbDeviceFS, 1);
	if (lastSize0 != 0)
	{
		memcpy(Receive_Buffer,hhid->Report_buf,lastSize0);
		USBD_LL_FlushEP(&hUsbDeviceFS, 0);	
		hhid->IsReportAvailable = 0;
		hhid->state = CUSTOM_HID_IDLE;

		printf("RecBuff_0: ");
	  for (int i = 0; i < lastSize0; i ++) {
			printf(" %02x", Receive_Buffer[i]);
    }
    printf("\r\n");
		}
	if (lastSize1 != 0)
	{
		memcpy(Receive_Buffer,hhid->Report_buf,lastSize1);
		USBD_LL_FlushEP(&hUsbDeviceFS, 1);	
		hhid->IsReportAvailable = 0;
		hhid->state = CUSTOM_HID_IDLE;
		
		printf("RecBuff_1: ");
	  for (int i = 0; i < lastSize1; i ++) {
			printf(" %02x", Receive_Buffer[i]);
    }
    printf("\r\n");
	
		EP1_OUT_Callback(hhid->Report_buf[0]);
	}	

}
  return (USBD_OK);
  /* USER CODE END 6 */
}
/* USER CODE BEGIN 7 */
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_OUT_Callback(uint8_t rep_idx)
{
  /* Read received data (2 bytes) */  
	uint8_t	EffectBlockIndex;
//	uint8_t i=0;
//	uint8_t *Report_Adress = NULL;
	uint8_t PIDDeviceControl = 0;
//  nReceivedBytes = USB_SIL_Read(EP1_OUT, Receive_Buffer);
	
	printf("EP1_OUT, rep_idx: %i", rep_idx);
	switch (rep_idx)
{
	case 1: //Set Effect Report //SET_EFFECT_REPORT_SIZE:14bytes
					//+ID +index
/*			EffectBlockIndex = Receive_Buffer[1]-1; //position dans la table, de 0 a max_bloc_index
			id_create=EffectBlockIndex+1;// for debug purpose
			Report_Adress = pUSB_POOL + (EffectBlockIndex*SET_EFFECT_REPORT_SIZE);
			for (i=2;i<SET_EFFECT_REPORT_SIZE+2;i++) { //on ignore les 2 premiers
				*(Report_Adress+i-2) = Receive_Buffer[i];
			}
			*(Report_Adress+14) = 2*EffectBlockIndex*PARAMETER_BLOC_SIZE;
			*(Report_Adress+15) = ((2*EffectBlockIndex)+1)*PARAMETER_BLOC_SIZE;
			 //on met a 1 le bit du block utilisé
			USED_BLOCKS = USED_BLOCKS|0x01<<(Receive_Buffer[1]-1);
*/		break;
	case 2: //Set Enveloppe Report //SET_ENVELOPPE_REPORT_SIZE:6bytes + adress
/*				Report_Adress = PARAMETERS_BASE_ADRESS + (2*(Receive_Buffer[1]-1)*PARAMETER_BLOC_SIZE)+PARAMETER_BLOC_SIZE;
				for (i=0;i<SET_ENVELOPPE_REPORT_SIZE;i++) {
					*(Report_Adress+i) = Receive_Buffer[i+2];
				}
				FREE_MEM -= PARAMETER_BLOC_SIZE;
*/		break;
	case 3: //Set Condition Report //SET_CONDITION_REPORT_SIZE:6bytes + adress
/*				if (Receive_Buffer[2] == 0){		// bloc de l'axe X
					Report_Adress = PARAMETERS_BASE_ADRESS + (2*(Receive_Buffer[1]-1)*PARAMETER_BLOC_SIZE);
				} else {												// bloc de l'axe Y
					Report_Adress = PARAMETERS_BASE_ADRESS + (2*(Receive_Buffer[1]-1)*PARAMETER_BLOC_SIZE)+PARAMETER_BLOC_SIZE;
				}
				for (i=0;i<SET_CONDITION_REPORT_SIZE;i++) {
					*(Report_Adress+i) = Receive_Buffer[i+2];
				}
				FREE_MEM -= PARAMETER_BLOC_SIZE;
*/			//EffectBlockIndex = Receive_Buffer[1];
// 			ParameterBlockOffset = Receive_Buffer[2]&0x0F;
// 			TypeSpecificBlockOffset = (Receive_Buffer[2]>>4)&0x0F; //a recouper en 2x2bits
// 			CPOffset = Receive_Buffer[3];
// 			PositiveCoefficient = Receive_Buffer[3];
// 			NegativeCoefficient = Receive_Buffer[4];
// 			PositiveSaturation = Receive_Buffer[5];
// 			NegativeSaturation = Receive_Buffer[6];
// 			DeadBand = Receive_Buffer[7];
		break;
	case 4: //Set Periodic Report //SET_PERIODIC_REPORT_SIZE:5bytes + adress
/*				Report_Adress = PARAMETERS_BASE_ADRESS + (2*(Receive_Buffer[1]-1)*PARAMETER_BLOC_SIZE);
				for (i=0;i<SET_PERIODIC_REPORT_SIZE;i++) {
					*(Report_Adress+i) = Receive_Buffer[i+2];
				}
				FREE_MEM -= PARAMETER_BLOC_SIZE;
*/			//EffectBlockIndex = Receive_Buffer[1];
// 			Magnitude = Receive_Buffer[2];
// 			Offset = Receive_Buffer[3];
// 			Phase = Receive_Buffer[4];
// 			Period = Receive_Buffer[5]|Receive_Buffer[6]<<8;
		break;
	case 5: //Set Constant force report //2Bytes
/*				Report_Adress = PARAMETERS_BASE_ADRESS + (2*(Receive_Buffer[1]-1)*PARAMETER_BLOC_SIZE);
				for (i=0;i<SET_CONSTANT_FORCE_REPORT_SIZE;i++) {
					*(Report_Adress+i) = Receive_Buffer[i+2];
				}
				FREE_MEM -= PARAMETER_BLOC_SIZE;
*/		//	EffectBlockIndex = Receive_Buffer[1];
		//	Magnitude = *Report_Adress|*(Report_Adress+1)<<8;
	break;
		case 6: //Set RampForce Report //2byte
/*				Report_Adress = PARAMETERS_BASE_ADRESS + (2*(Receive_Buffer[1]-1)*PARAMETER_BLOC_SIZE);
				for (i=0;i<SET_RAMP_FORCE_REPORT_SIZE;i++) {
					*(Report_Adress+i) = Receive_Buffer[i+2];
				}
				FREE_MEM -= PARAMETER_BLOC_SIZE;			
*/			//EffectBlockIndex = Receive_Buffer[1];
// 			RampStart = Receive_Buffer[2];
// 			RampEnd = Receive_Buffer[3];
		break;
		case 7: //Set CustomForce Data Report //16Bytes
//			EffectBlockIndex = Receive_Buffer[1];
// 			CustomForceDataOffset = Receive_Buffer[2]|Receive_Buffer[3]<<8;
// 			CustomForceData1 = Receive_Buffer[4];
//								...
// 			CustomForceData12 = Receive_Buffer[17];
		break;		
		case 8: //Download Force Sample
//			EffectX = Receive_Buffer[1];
// 			EffectY = Receive_Buffer[2];
		break;		
		case 9: //rien
		break;		
		case 0x0A: //Effect Operation Report
			EffectBlockIndex = Receive_Buffer[1];
// 			LoopCount = Receive_Buffer[3];
//			Operation = Receive_Buffer[2]; //1,2ou3: Effect (all?) Start,Effect Start solo,Effect Stop
			switch (Receive_Buffer[2]) {
				case 1:
//					start_effect(EffectBlockIndex,Receive_Buffer[3]); //a creer pour gerer le running_effect, le timer...
					In_Report_2_Buffer[2] = EffectBlockIndex<<1 | 0x01;					
					sendrep2 = 1;
				break;
				case 2:
//					start_effect(EffectBlockIndex,Receive_Buffer[3]); //a creer pour gerer le running_effect, le timer...
					In_Report_2_Buffer[2] = EffectBlockIndex<<1 | 0x01;					
					sendrep2 = 1;
				break;
				case 3:
//					stop_effect(EffectBlockIndex);	//idem
					In_Report_2_Buffer[2] = EffectBlockIndex<<1 & ~0x01;					
					sendrep2 = 1;
				break;
			}

		break;				
		case 0x0B: //PID Block Free Report on efface le bloc de l'index
					USED_BLOCKS = USED_BLOCKS&~(0x01<<(Receive_Buffer[1]-1)); //on met a 1 le bit du block utilisé
			//EffectBlockIndex = Receive_Buffer[1];
		break;
		case 0x0C: //PID Device Control //direct action
			PIDDeviceControl = Receive_Buffer[1]; //1à6:
// 			In_Report_2_Buffer[1] = 0;//{0x02 ,0x00, 0x00};
// 			In_Report_2_Buffer[2] = 0;//{0x02 ,0x00, 0x00};
			switch (PIDDeviceControl) {
				case 01:	//1=DS Enable Actuators:Sel – Enable all device actuators
						//timer_moteur.enable;
						In_Report_2_Buffer[1] |= 0x01<<1;
				break;
				case 02:	//2=DS Disable Actuators:Sel – Disable all device actuators
						//timer_moteur.Disable;
						In_Report_2_Buffer[1] &= ~(0x01<<1);
				break;
				case 03:	//3=Stop All Effects:Sel - Issues a stop on every running effect
//					for (i=0;i<MaxContiguousEffects;i++){
//						Running_effect_timing[i].EffectBlockIndex = 0;
//					}					
					In_Report_2_Buffer[1] =0; //&= ~(0x01<<1);//effect playing = 0
					In_Report_2_Buffer[2] =0; //&= ~(0x01<<1);//effect playing = 0
				break;
				case 04:	//4=Device Reset:Sel – Clears any device paused condition, 
									//enables all actuators and clears all effects from memory
//					for (i=0;i<MaxContiguousEffects;i++){
//						Running_effect_timing[i].EffectBlockIndex = 0;
//					}					//timer_moteur.enable;
//					TIM_Cmd(TIM7, ENABLE);	//timer_effect.clock = Enable;
					In_Report_2_Buffer[1] = 0x12; // 10010
// 					In_Report_2_Buffer[1] |= 0x01<<1;//actuators en
// 					In_Report_2_Buffer[1] &= ~(0x01); //pause =0
					In_Report_2_Buffer[2] =0; //&= ~(0x01<<1);//effects plying=0
				break;
				case 05:	//5=Device Pause:Sel – The all effects on the device are paused at the current time step.
//						TIM_Cmd(TIM7, DISABLE);	//timer_effect.clock = disable;
						In_Report_2_Buffer[1] |= 0x01; //pause =1
				break;
				case 06:	//6=DC Device Continue:Sel – The all effects that running when 
									//the device was paused are restarted from their last time step.
//						TIM_Cmd(TIM7, ENABLE);	//timer_effect.clock = Enable;
						In_Report_2_Buffer[1] &= ~(0x01); //pause =0
				break;
// 				default:
// 					In_Report_2_Buffer[1] = 0x00;//, 0x12;
// 					In_Report_2_Buffer[2] = 0x12;//, 0x12;
// 				break;
			}
			sendrep2 = 1;
			//Send_PID_State_Report();			
		break;
		case 0x0D: //Device Gain
//			Global_Gain = Receive_Buffer[1]; //1à6:
		break;
		case 0x0E: //Set Custom Force Report //3bytes
//			EffectBlockIndex = Receive_Buffer[1]; //1à6:
//			SampleCount = Receive_Buffer[2];
//			SamplePeriod = Receive_Buffer[3]|Receive_Buffer[4]>>8;		
		break;
		default:
		break;						
} // end of switch
//USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,In_Report_2_Buffer, sizeof(In_Report_2_Buffer));
//USBD_CtlSendData (&hUsbDeviceFS, In_Report_2_Buffer, 3);
}

uint8_t reportBuffer[4] = {0};
uint8_t asyncReport = 0;
 
#define  STATUS_REPORT_ID 3
void MainLoop_Tick(){
        if(asyncReport != 0){
                if(asyncReport == STATUS_REPORT_ID){
                        //DO SENSOR STUFF
                        reportBuffer[0] = STATUS_REPORT_ID;
                        USBD_CtlSendData (&hUsbDeviceFS, reportBuffer, 4);
                }
        }
}
 
void USBD_HID_ProcessGetFeatureEvent(uint8_t report_id){
        switch(report_id){
                case STATUS_REPORT_ID:
                        asyncReport = STATUS_REPORT_ID;
                        break;
                default:
                        reportBuffer[0] = report_id;
                        USBD_CtlSendData (&hUsbDeviceFS, reportBuffer, 4);
                        break;
        }
}

///*******************************************************************************
//* Function Name  : Joystick_GetReport_Feature.
//* Description    : Gets the HID report feature.
//* Input          : Length
//* Output         : HID_GetReport_Value.
//* Return         : l'etat du bloc d'effets.
//*******************************************************************************/
///*
//bmRequestType	bRequest								wValue													wIndex									wLength								Data
//1000 0000b	GET_STATUS 				(0x00)	Zero														Zero										Two										Device Status
//0000 0000b	CLEAR_FEATURE 		(0x01)	Feature Selector								Zero										Zero									None
//0000 0000b	SET_FEATURE 			(0x03)	Feature Selector								Zero										Zero									None
//0000 0000b	SET_ADDRESS 			(0x05)	Device Address									Zero										Zero									None
//1000 0000b	GET_DESCRIPTOR 		(0x06)	Descriptor Type & Index					Zero or Language ID			Descriptor 	Length		Descriptor
//0000 0000b	SET_DESCRIPTOR 		(0x07)	Descriptor Type & Index					Zero or Language ID			Descriptor 	Length		Descriptor
//1000 0000b	GET_CONFIGURATION (0x08)	Zero														Zero										1											Configuration Value
//0000 0000b	SET_CONFIGURATION (0x09)	Configuration Value							Zero										Zero									None
//*/
//uint8_t *Joystick_GetReport_Feature(uint16_t Length)
//{		// GET: le host demande quelque chose
///*
//  switch ( (req->wValue)&0xff )               // report ID
//  {
//    case 1: // host demand allocation d'un bloc d'effet
//						// rien a retourner
//		break;
//    case 2:		// Block load statuts//a-t'on reussi a creer l'effet
//							// et quelle place reste t'il
//      if (Length == 0)
//      {
//                              // 5 bytes, report ID and report body
//        pInformation->Ctrl_Info.Usb_wLength = 5;
//        return NULL;
//      } else {
//        // here, put value to HID_GetReport_Value depending on report ID
//        HID_GetReport_Value[0] = pInformation->USBwValue0;  // report ID
//        HID_GetReport_Value[1] = *GetNextFreeBlock();    //EffectBlockIndex                // body
//        HID_GetReport_Value[2] = *(GetNextFreeBlock()+1);    //block load succes|full|error (1,2 ou 3)                // body
//        HID_GetReport_Value[3] = FREE_MEM&0xFF;    //RAM Pool AvailableLSB                // body
//        HID_GetReport_Value[4] = FREE_MEM>>8;      //RAM Pool AvailableMSB                // body
//        return HID_GetReport_Value;
//      }
////			break;
//    case 3:		//PID Pool Report //comment est gere le pool d'effects, sa taille....
//      if (Length == 0)
//      {
//                              // 5 bytes, report ID and report body
//        pInformation->Ctrl_Info.Usb_wLength = 5;
//        return NULL;
//      } else {
//        // here, put value to HID_GetReport_Value depending on report ID
//        HID_GetReport_Value[0] = pInformation->USBwValue0;  // report ID
//        HID_GetReport_Value[1] = ALLOCATED_BLOCK_MEM&0xFF; 	// ram pool size LSB    
//        HID_GetReport_Value[2] = ALLOCATED_BLOCK_MEM>>8; 		// ram pool size MSB
//        HID_GetReport_Value[3] = MaxContiguousEffects; 			// max simultaneous effects
//        HID_GetReport_Value[4] = 0x01; // 2 bits devicemanagedpool 1,shared marameters 0
//        return HID_GetReport_Value;
//      }
////			break;
//    case 4:
//    case 5:
//    case 6:
//    case 7:
//    default:
//      return NULL;
//  }
//	*/
//	return HID_GetReport_Value;

//}


//uint8_t HID_SetReport_Value[5];


//uint8_t *Joystick_SetReport_Feature(uint16_t Length)
//{		//SET c'est a moi de recuperer des data utiles
//	//interface report:
////	USBD_CustomHID_fops_FS.DeInit
////	USBD_CustomHID_fops_FS.Init
////	USBD_CustomHID_fops_FS.OutEvent
////	USBD_CustomHID_fops_FS.pReport
//	
//// USB Device handle	
////	hUsbDeviceFS.dev_address
////	hUsbDeviceFS.dev_config
////	hUsbDeviceFS.dev_config_status
////	hUsbDeviceFS.dev_connection_status
////	hUsbDeviceFS.dev_default_config
////	hUsbDeviceFS.dev_old_state
////	hUsbDeviceFS.dev_remote_wakeup
////	hUsbDeviceFS.dev_speed
////	hUsbDeviceFS.dev_state
////	hUsbDeviceFS.dev_test_mode
////	hUsbDeviceFS.ep0_data_len
////	hUsbDeviceFS.ep0_state
////	hUsbDeviceFS.ep_in
////	hUsbDeviceFS.ep_out
////	hUsbDeviceFS.id
////	hUsbDeviceFS.pClass
////	hUsbDeviceFS.pClassData
////	hUsbDeviceFS.pData
////	hUsbDeviceFS.pDesc
////	hUsbDeviceFS.pUserData
////	hUsbDeviceFS.request
//	
//	
//	/*
//  switch ((req->wValue)&0xff )               // report ID
//  {
//    case 1:				// 1 = create new effect report
//									// host demande allocation new effect
//      if (Length == 0)
//      {
//                              // 4 bytes, report ID and report body
//        pInformation->Ctrl_Info.Usb_wLength = 4;
//        return NULL;
//      } else {
//				// 1 = create new effect report
//				// 2 = effect type
//				// 3|4 = byte count si custom effect, 00 00 pour les autres
//        return HID_SetReport_Value;
//      }			
//    case 2:
//    case 3:
//    case 4:
//    case 5:
//    case 6:
//    case 7:
//    default:
//      return NULL;
//  }
//	*/
//}


//void Joystick_Status_In(void)
//{
//  if (Request == SET_REPORT)                        // SET_REPORT completion
//  {
//    Request = 0;
////		LCD_ShowNum (10,190,HID_SetReport_Value[1],4,16);
//		STM_EVAL_LEDToggle(LED1);
//    //
//    // now, stack has filled HID_SetReport_Value
//    // HID_SetReport_Value[0] holds report ID
//    // HID_SetReport_Value[1], report body
//    // process it
//    //
//  }
//}

/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

