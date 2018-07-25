
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "stm32f1xx_hal.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
	struct WheelHID_t strucWheelHID;	
	
	uint8_t HID_GetReport_Value[5]; //valeur a priori du plus long feature report
	uint8_t Request = 0;                                // completed request at status stage
  //pour 16 bits data send LSB first
	//le premier byte est le ReportID
 	uint8_t In_Report_1_Buffer[15] = {0x01 ,0x00, 0x25, 0x34, 0x46, 0x52, 0x06, 0x46, 0x52, 0x06, 0x01 ,0x00, 0x25, 0x12, 0x25};
	// HID status report
	uint8_t In_Report_2_Buffer[3] = {0x02 ,0x12, 0x00};
	uint8_t sendrep2 = 0; // doit'on envoyer le status report ou le standard
	uint8_t HID_SetReport_Value[5];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
strucWheelHID.steering = 0;
strucWheelHID.accelerator = 10;
strucWheelHID.brake = 20;
strucWheelHID.clutch = 30;
strucWheelHID.handbrake = 40;
strucWheelHID.boutons = 127;
strucWheelHID.dummy = 0xFF;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USB_DEVICE_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
printf("SerialWorking\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	strucWheelHID.steering += 10;
		
	In_Report_1_Buffer[0] = 1;													//report nb 1		
	In_Report_1_Buffer[1] = strucWheelHID.steering>>8;
	In_Report_1_Buffer[2] = strucWheelHID.steering&0xFF;
	In_Report_1_Buffer[3] = strucWheelHID.accelerator>>8;
	In_Report_1_Buffer[4] = strucWheelHID.accelerator&0xFF;
	In_Report_1_Buffer[5] = strucWheelHID.brake>>8;
	In_Report_1_Buffer[6] = strucWheelHID.brake&0xFF;
	In_Report_1_Buffer[7] = strucWheelHID.clutch>>8;
	In_Report_1_Buffer[8] = strucWheelHID.clutch&0xFF;
	In_Report_1_Buffer[9] = strucWheelHID.handbrake>>8;
	In_Report_1_Buffer[10] = strucWheelHID.handbrake&0xFF;
	In_Report_1_Buffer[11] = strucWheelHID.boutons;
	In_Report_1_Buffer[12] = strucWheelHID.dummy;
		
	if (sendrep2 == 1){
			USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,In_Report_2_Buffer,3);// sizeof(In_Report_2_Buffer));
			sendrep2 = 0;
		} else
		{
			USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,In_Report_1_Buffer, 13); //14?? = 13+1
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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/*******************************************************************************
* Function Name  : Joystick_GetReport_Feature.
* Description    : Gets the HID report feature.
* Input          : Length
* Output         : HID_GetReport_Value.
* Return         : l'etat du bloc d'effets.
*******************************************************************************/
/*
bmRequestType	bRequest								wValue													wIndex									wLength								Data
1000 0000b	GET_STATUS 				(0x00)	Zero														Zero										Two										Device Status
0000 0000b	CLEAR_FEATURE 		(0x01)	Feature Selector								Zero										Zero									None
0000 0000b	SET_FEATURE 			(0x03)	Feature Selector								Zero										Zero									None
0000 0000b	SET_ADDRESS 			(0x05)	Device Address									Zero										Zero									None
1000 0000b	GET_DESCRIPTOR 		(0x06)	Descriptor Type & Index					Zero or Language ID			Descriptor 	Length		Descriptor
0000 0000b	SET_DESCRIPTOR 		(0x07)	Descriptor Type & Index					Zero or Language ID			Descriptor 	Length		Descriptor
1000 0000b	GET_CONFIGURATION (0x08)	Zero														Zero										1											Configuration Value
0000 0000b	SET_CONFIGURATION (0x09)	Configuration Value							Zero										Zero									None
*/
uint8_t *Joystick_GetReport_Feature(uint16_t Length)//,USBD_SetupReqTypedef *req)
{		// GET: le host demande quelque chose
USBD_SetupReqTypedef *req;
  switch ( (req->wValue)&0xff )               // report ID
  {
    case 1: // host demand allocation d'un bloc d'effet
						// rien a retourner
		break;
    case 2:		// Block load statuts//a-t'on reussi a creer l'effet
							// et quelle place reste t'il
      if (Length == 0)
      {
                              // 5 bytes, report ID and report body
        req->wLength = 5;
        return NULL;
      } else {
        // here, put value to HID_GetReport_Value depending on report ID
        HID_GetReport_Value[0] = (req->wValue)&0xff;// pInformation->USBwValue0;  // report ID
        HID_GetReport_Value[1] = 1;//*GetNextFreeBlock();    //EffectBlockIndex                // body
        HID_GetReport_Value[2] = 1;//*(GetNextFreeBlock()+1);    //block load succes|full|error (1,2 ou 3)                // body
        HID_GetReport_Value[3] = FREE_MEM&0xFF;    //RAM Pool AvailableLSB                // body
        HID_GetReport_Value[4] = FREE_MEM>>8;      //RAM Pool AvailableMSB                // body
        return HID_GetReport_Value;
      }
//			break;
    case 3:		//PID Pool Report //comment est gere le pool d'effects, sa taille....
      if (Length == 0)
      {
                              // 5 bytes, report ID and report body
        req->wLength = 5;//        pInformation->Ctrl_Info.Usb_wLength = 5;
        return NULL;
      } else {
        // here, put value to HID_GetReport_Value depending on report ID
        HID_GetReport_Value[0] = (req->wValue)&0xff;// pInformation->USBwValue0;  // report ID
        HID_GetReport_Value[1] = ALLOCATED_BLOCK_MEM&0xFF; 	// ram pool size LSB    
        HID_GetReport_Value[2] = ALLOCATED_BLOCK_MEM>>8; 		// ram pool size MSB
        HID_GetReport_Value[3] = MaxContiguousEffects; 			// max simultaneous effects
        HID_GetReport_Value[4] = 0x01; // 2 bits devicemanagedpool 1,shared marameters 0
        return HID_GetReport_Value;
      }
//			break;
    case 4:
    case 5:
    case 6:
    case 7:
    default:
      return NULL;
  }

	return HID_GetReport_Value;

}





uint8_t *Joystick_SetReport_Feature(uint16_t Length)
{		//SET c'est a moi de recuperer des data utiles
	//interface report:
//	USBD_CustomHID_fops_FS.DeInit
//	USBD_CustomHID_fops_FS.Init
//	USBD_CustomHID_fops_FS.OutEvent
//	USBD_CustomHID_fops_FS.pReport
	
// USB Device handle	
//	hUsbDeviceFS.dev_address
//	hUsbDeviceFS.dev_config
//	hUsbDeviceFS.dev_config_status
//	hUsbDeviceFS.dev_connection_status
//	hUsbDeviceFS.dev_default_config
//	hUsbDeviceFS.dev_old_state
//	hUsbDeviceFS.dev_remote_wakeup
//	hUsbDeviceFS.dev_speed
//	hUsbDeviceFS.dev_state
//	hUsbDeviceFS.dev_test_mode
//	hUsbDeviceFS.ep0_data_len
//	hUsbDeviceFS.ep0_state
//	hUsbDeviceFS.ep_in
//	hUsbDeviceFS.ep_out
//	hUsbDeviceFS.id
//	hUsbDeviceFS.pClass
//	hUsbDeviceFS.pClassData
//	hUsbDeviceFS.pData
//	hUsbDeviceFS.pDesc
//	hUsbDeviceFS.pUserData
//	hUsbDeviceFS.request
	
USBD_SetupReqTypedef *req;	
	
  switch ((req->wValue)&0xff )               // report ID
  {
    case 1:				// 1 = create new effect report
									// host demande allocation new effect
      if (Length == 0)
      {
                              // 4 bytes, report ID and report body
        req->wLength = 4;//        pInformation->Ctrl_Info.Usb_wLength = 4;
        return NULL;
      } else {
				// 1 = create new effect report
				// 2 = effect type
				// 3|4 = byte count si custom effect, 00 00 pour les autres
        return HID_SetReport_Value;
      }			
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    default:
      return NULL;
  }
	
}

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
	//int8_t errcod;
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
	//HAL_UART_Transmit_DMA(&huart2, (uint8_t *)&ch, sizeof(ch));//, 0xFFFF);
	//HAL_USART_Transmit_IT(&husart2, (uint8_t *)&ch, 1);
  
  return ch;
}
//assumes little endian
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
				printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
  /* User can add his own implementation to report the file name and line number,     tex:  */
printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
