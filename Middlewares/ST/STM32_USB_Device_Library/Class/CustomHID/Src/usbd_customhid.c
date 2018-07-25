/**
  ******************************************************************************
  * @file    usbd_customhid.c
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   This file provides the CUSTOM_HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                CUSTOM_HID Class  Description
  *          =================================================================== 
  *           This module manages the CUSTOM_HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (CUSTOM_HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - Usage Page : Generic Desktop
  *             - Usage : Vendor
  *             - Collection : Application 
  *      
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_customhid.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"

	#include "usbd_custom_hid_if.h"
	#include "main.h"
/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_CUSTOM_HID 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_CUSTOM_HID_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_CUSTOM_HID_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 
/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_CUSTOM_HID_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx);

static uint8_t  USBD_CUSTOM_HID_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx);

static uint8_t  USBD_CUSTOM_HID_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_CUSTOM_HID_GetCfgDesc (uint16_t *length);

static uint8_t  *USBD_CUSTOM_HID_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_CUSTOM_HID_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_CUSTOM_HID_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_CUSTOM_HID_EP0_RxReady (USBD_HandleTypeDef  *pdev);


/**
  * @}
  */ 

/** @defgroup USBD_CUSTOM_HID_Private_Variables
  * @{
  */ 

USBD_ClassTypeDef  USBD_CUSTOM_HID = 
{
  USBD_CUSTOM_HID_Init,
  USBD_CUSTOM_HID_DeInit,
  USBD_CUSTOM_HID_Setup,
  NULL, /*EP0_TxSent*/  
  USBD_CUSTOM_HID_EP0_RxReady, /*EP0_RxReady*/ /* STATUS STAGE IN */
  USBD_CUSTOM_HID_DataIn, /*DataIn*/
  USBD_CUSTOM_HID_DataOut,
  NULL, /*SOF */
  NULL,
  NULL,      
  USBD_CUSTOM_HID_GetCfgDesc,
  USBD_CUSTOM_HID_GetCfgDesc, 
  USBD_CUSTOM_HID_GetCfgDesc,
  USBD_CUSTOM_HID_GetDeviceQualifierDesc,
};

/* USB CUSTOM_HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_HID_CfgDesc[USB_CUSTOM_HID_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_CUSTOM_HID_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xC0,         /*bmAttributes: bus powered */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
  
  /************** Descriptor of CUSTOM HID interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: CUSTOM_HID*/
  0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of CUSTOM_HID *************************/
  /* 18 */
  0x09,         /*bLength: CUSTOM_HID Descriptor size*/
  CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
  0x11,         /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
		LOBYTE(USBD_CUSTOM_HID_REPORT_DESC_SIZE),/*wItemLength: Total length of Report descriptor*/
		HIBYTE(USBD_CUSTOM_HID_REPORT_DESC_SIZE),/*wItemLength: Total length of Report descriptor*/
  /******************** Descriptor of Custom HID endpoints ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
  
  CUSTOM_HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  CUSTOM_HID_EPIN_SIZE, /*wMaxPacketSize: 2 Byte max */
  0x00,
  0x20,          /*bInterval: Polling Interval (20 ms)*/
  /* 34 */
  
  0x07,	         /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,	/* bDescriptorType: */
  CUSTOM_HID_EPOUT_ADDR,  /*bEndpointAddress: Endpoint Address (OUT)*/
  0x03,	/* bmAttributes: Interrupt endpoint */
  CUSTOM_HID_EPOUT_SIZE,	/* wMaxPacketSize: 2 Bytes max  */
  0x00,
  0x20,	/* bInterval: Polling Interval (20 ms) */
  /* 41 */
} ;

/* USB CUSTOM_HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_HID_Desc[USB_CUSTOM_HID_DESC_SIZ] __ALIGN_END =
{
  /* 18 */
  0x09,         /*bLength: CUSTOM_HID Descriptor size*/
  CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
  0x11,         /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
		LOBYTE(USBD_CUSTOM_HID_REPORT_DESC_SIZE),/*wItemLength: Total length of Report descriptor*/
		HIBYTE(USBD_CUSTOM_HID_REPORT_DESC_SIZE),/*wItemLength: Total length of Report descriptor*/
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */ 

/** @defgroup USBD_CUSTOM_HID_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         Initialize the CUSTOM_HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_HID_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx)
{
  uint8_t ret = 0;
  USBD_CUSTOM_HID_HandleTypeDef     *hhid;
  /* Open EP IN */
  USBD_LL_OpenEP(pdev,
                 CUSTOM_HID_EPIN_ADDR,
                 USBD_EP_TYPE_INTR,
                 CUSTOM_HID_EPIN_SIZE);  
  
  /* Open EP OUT */
  USBD_LL_OpenEP(pdev,
                 CUSTOM_HID_EPOUT_ADDR,
                 USBD_EP_TYPE_INTR,
                 CUSTOM_HID_EPOUT_SIZE);
  
  pdev->pClassData = USBD_malloc(sizeof (USBD_CUSTOM_HID_HandleTypeDef));
  
  if(pdev->pClassData == NULL)
  {
    ret = 1; 
  }
  else
  {
    hhid = (USBD_CUSTOM_HID_HandleTypeDef*) pdev->pClassData;
      
    hhid->state = CUSTOM_HID_IDLE;
    ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->Init();
          /* Prepare Out endpoint to receive 1st packet */ 
    USBD_LL_PrepareReceive(pdev, CUSTOM_HID_EPOUT_ADDR, hhid->Report_buf, 
                           USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
  }
    
  return ret;
}

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         DeInitialize the CUSTOM_HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_HID_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx)
{
  /* Close CUSTOM_HID EP IN */
  USBD_LL_CloseEP(pdev,
                  CUSTOM_HID_EPIN_ADDR);
  
  /* Close CUSTOM_HID EP OUT */
  USBD_LL_CloseEP(pdev,
                  CUSTOM_HID_EPOUT_ADDR);
  
  /* FRee allocated memory */
  if(pdev->pClassData != NULL)
  {
    ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->DeInit();
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }
  return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_Setup
  *         Handle the CUSTOM_HID setup packet

  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status

http://www.jungo.com/st/support/documentation/windriver/811/wdusb_man_mhtml/node55.html :

Byte	|	Field						|	Description
0			|	bmRequest Type	|	Bit 7: Request direction (0=Host to device - Out, 1=Device to host - In). 
			|									|	Bits 5-6: Request type (0=standard, 1=class, 2=vendor, 3=reserved). 
			|									|	Bits 0-4: Recipient (0=device, 1=interface, 2=endpoint,3=other).
1			|	bRequest				|	The actual request (see the Standard Device Request Codes table [9.1.5]).
2			|	wValueL					|	A word-size value that varies according to the request. For example, in the CLEAR_FEATURE request the value is used to select the feature,
			|									|		in the GET_DESCRIPTOR request the value indicates the descriptor type and in the SET_ADDRESS request the value contains the device address.
3			|	wValueH					|	The upper byte of the Value word.
4			|	wIndexL					|	A word-size value that varies according to the request. The index is generally used to specify an endpoint or an interface.
5			|	wIndexH					|	The upper byte of the Index word.
6			|	wLengthL				|	A word-size value that indicates the number of bytes to be transferred if there is a data stage.
7			|	wLengthH				|	The upper byte of the Length word.


bRequest								Value
GET_STATUS							0
CLEAR_FEATURE						1
Reserved for future use	2
SET_FEATURE							3
Reserved for future use	4
SET_ADDRESS							5
GET_DESCRIPTOR					6
SET_DESCRIPTOR					7
GET_CONFIGURATION				8
SET_CONFIGURATION				9
GET_INTERFACE						10
SET_INTERFACE						11
SYNCH_FRAME							12

  */
static uint8_t  USBD_CUSTOM_HID_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req)
{
  uint16_t len = 0;
  uint8_t  *pbuf = NULL;
  USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)pdev->pClassData;
	unsigned int lastSize0 = 0;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
    switch (req->bRequest)
    {

    case CUSTOM_HID_REQ_SET_PROTOCOL:
			printf("CUSTOM_HID_REQ_SET_PROTOCOL\r\n");
      hhid->Protocol = (uint8_t)(req->wValue);
      break;
      
    case CUSTOM_HID_REQ_GET_PROTOCOL:
			printf("CUSTOM_HID_REQ_GET_PROTOCOL\r\n");
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&hhid->Protocol,
                        1);    
      break;
      
    case CUSTOM_HID_REQ_SET_IDLE:
			printf("CUSTOM_HID_REQ_SET_IDLE\r\n");
      hhid->IdleState = (uint8_t)(req->wValue >> 8);
      break;
      
    case CUSTOM_HID_REQ_GET_IDLE:
			printf("CUSTOM_HID_REQ_GET_IDLE\r\n");
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&hhid->IdleState,
                        1);        
      break;      
    
    case CUSTOM_HID_REQ_SET_REPORT:
			printf("CUSTOM_HID_REQ_SET_REPORT\r\n");			
      hhid->IsReportAvailable = 1;
		// on prepare une place dans le buffer report_buf pour le prochain report. taille a reserver: req->wLength
      USBD_CtlPrepareRx (pdev, hhid->Report_buf, (uint8_t)(req->wLength));
		
		// for debug
//			printf("bmRequest: ");
//			printBits(sizeof(req->bmRequest), &req->bmRequest);
//			printf("\r");
//			printf("bRequest:  0x%02X\r\n", req->bRequest);
//			printf("wValue:    0x%02X\r\n", (req->wValue)&0xff);
//			printf("wIndex:    0x%02X\r\n", req->wIndex);		
//			printf("wLength:   0x%02X\r\n", req->wLength);		
//			printf("data: "); 
//			for (int i = 0; i < req->wLength; i ++) {
//				printf(" %02x", hhid->Report_buf[i]);
//			}
//			printf("\r\n");
			switch((req->wValue)>>8 )//pInformation->USBwValue1 )          // report type
        {
          case HID_FEATURE:
									printf("SET_REPORT_feature\r\n");			
            //CopyRoutine = CustomHID_SetReport_Feature;
						// SET, l'host m'envoie une info qu'il faut que je traite
						// exemple: hhid->Protocol = (uint8_t)(req->wValue);
						lastSize0 = USBD_LL_GetRxDataSize(pdev, 0);
					  switch ((req->wValue)&0xff )               // report ID
							{
								case 1:				// 1 = create new effect report
															// host demande allocation new effect
								if (req->wLength == 0)// was: if (Length == 0)
									{
																					// 4 bytes, report ID and report body
										req->wLength = 4;//        pInformation->Ctrl_Info.Usb_wLength = 4;
										//return NULL;
									} else {
										// 1 = create new effect report
										// 2 = effect type
										// 3|4 = byte count si custom effect, 00 00 pour les autres
										//return HID_SetReport_Value;
										//void * memcpy ( void * destination, const void * source, size_t num );
										memcpy (HID_SetReport_Value,hhid->Report_buf,req->wLength);
										//*HID_SetReport_Value = *hhid->Report_buf;
//										printf("ReqLen: %i\r\n", req->wLength);
//										printf("ReqwVal: 0x%02X\r\n", (req->wValue)&0xff);
										USBD_LL_FlushEP(pdev, 0);	
										hhid->IsReportAvailable = 0;
										hhid->state = CUSTOM_HID_IDLE;
										printf("lastSiz: %i\r\n", lastSize0);

									}			
//								default:
//									return NULL;
							}
            Request = CUSTOM_HID_REQ_SET_REPORT;
            break;
          case HID_OUTPUT:
            break;
        }     
      break;
		
    case CUSTOM_HID_REQ_GET_REPORT:
			printf("CUSTOM_HID_REQ_GET_REPORT\r\n");			
      //USBD_CtlSendData (pdev, hhid->Report_buf, (uint8_t)(req->wLength));
					//USBD_CtlSendData (USBD_HandleTypeDef *pdev, uint8_t *pbuf, uint16_t len)     
        switch( (req->wValue)>>8 )          // report type
        {
          case HID_FEATURE:
						printf("case HID_FEATURE\r\n");			
//					USBD_CtlSendData (pdev, (uint8_t *)&hhid->Protocol, 1);    
//            CopyRoutine = CustomHID_GetReport_Feature;
						switch ( (req->wValue)&0xff )               // report ID
						{
							case 1: // host demand allocation d'un bloc d'effet
											// rien a retourner
							break;
							case 2:		// Block load statuts//a-t'on reussi a creer l'effet
												// et quelle place reste t'il
								if (req->wLength == 0) // was lenght == 0 ??
								{
																				// 5 bytes, report ID and report body
									req->wLength = 5;
									//return NULL;
								} else {
									// here, put value to HID_GetReport_Value depending on report ID
									hhid->Report_buf[0] = 2;//(req->wValue)&0xff;// pInformation->USBwValue0;  // report ID
									hhid->Report_buf[1] = 1;//*GetNextFreeBlock();    //EffectBlockIndex                // body
									hhid->Report_buf[2] = 1;//*(GetNextFreeBlock()+1);    //block load succes|full|error (1,2 ou 3)                // body
									hhid->Report_buf[3] = FREE_MEM&0xFF;    //RAM Pool AvailableLSB                // body
									hhid->Report_buf[4] = FREE_MEM>>8;      //RAM Pool AvailableMSB                // body
									//return HID_GetReport_Value;
									//USBD_CtlSendData (pdev, hhid->Report_buf, (uint8_t)(req->wLength));
								}
								break;
							case 3:		//PID Pool Report //comment est gere le pool d'effects, sa taille....
								if (req->wLength == 0) // was(Length == 0)
								{
																				// 5 bytes, report ID and report body
									req->wLength = 5;//        pInformation->Ctrl_Info.Usb_wLength = 5;
									return NULL;
								} else {
									// here, put value to HID_GetReport_Value depending on report ID
									hhid->Report_buf[0] = 3;//(req->wValue)&0xff;// pInformation->USBwValue0;  // report ID
									hhid->Report_buf[1] = ALLOCATED_BLOCK_MEM&0xFF; 	// ram pool size LSB    
									hhid->Report_buf[2] = ALLOCATED_BLOCK_MEM>>8; 		// ram pool size MSB
									hhid->Report_buf[3] = MaxContiguousEffects; 			// max simultaneous effects
									hhid->Report_buf[4] = 0x01; // 2 bits devicemanagedpool 1,shared marameters 0
									//return HID_GetReport_Value;
									//USBD_CtlSendData (pdev, hhid->Report_buf, (uint8_t)(req->wLength));									
								}
								break;
//							default:
//								return NULL;
						}
						printf("Report_buf: ");
						for (int i = 0; i < 5; i ++) {
							printf(" %02x", hhid->Report_buf[i]);
						}
						printf("\r\n");
						USBD_CtlSendData (pdev, hhid->Report_buf, (uint8_t)(req->wLength));
						//return HID_GetReport_Value;
	
            break;
          case HID_INPUT:
						printf("case HID_INPUT\r\n");							
            break;
        }
      break;
		
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL; 
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
		printf("case USB_REQ_TYPE_STANDARD\r\n");		
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR: 
		printf("case USB_REQ_GET_DESCRIPTOR\r\n");					
      if( req->wValue >> 8 == CUSTOM_HID_REPORT_DESC)
      {
        len = MIN(USBD_CUSTOM_HID_REPORT_DESC_SIZE , req->wLength);
        pbuf =  ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->pReport;
      }
      else if( req->wValue >> 8 == CUSTOM_HID_DESCRIPTOR_TYPE)
      {
        pbuf = USBD_CUSTOM_HID_Desc;   
        len = MIN(USB_CUSTOM_HID_DESC_SIZ , req->wLength);
      }
      
      USBD_CtlSendData (pdev, 
                        pbuf,
                        len);
      
      break;
      
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        (uint8_t *)&hhid->AltSetting,
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      hhid->AltSetting = (uint8_t)(req->wValue);
      break;
    }
  }
  return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_SendReport 
  *         Send CUSTOM_HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_CUSTOM_HID_SendReport     (USBD_HandleTypeDef  *pdev, 
                                 uint8_t *report,
                                 uint16_t len)
{
  USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)pdev->pClassData;
  
  if (pdev->dev_state == USBD_STATE_CONFIGURED )
  {
    if(hhid->state == CUSTOM_HID_IDLE)
    {
      hhid->state = CUSTOM_HID_BUSY;
      USBD_LL_Transmit (pdev, 
                        CUSTOM_HID_EPIN_ADDR,                                      
                        report,
                        len);
    }
  }
  return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_GetCfgDesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_CUSTOM_HID_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_CUSTOM_HID_CfgDesc);
  return USBD_CUSTOM_HID_CfgDesc;
}

/**
  * @brief  USBD_CUSTOM_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_HID_DataIn (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{
  
  /* Ensure that the FIFO is empty before a new transfer, this condition could 
  be caused by  a new transfer before the end of the previous transfer */
  ((USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData)->state = CUSTOM_HID_IDLE;

  return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_HID_DataOut (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{
  
  USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)pdev->pClassData;  
  
  ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->OutEvent(hhid->Report_buf[0], 
                                                            hhid->Report_buf[1]);
    
  USBD_LL_PrepareReceive(pdev, CUSTOM_HID_EPOUT_ADDR , hhid->Report_buf, 
                         USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);

  return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_EP0_RxReady
  *         Handles control request data.
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CUSTOM_HID_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)pdev->pClassData;  

  if (hhid->IsReportAvailable == 1)
  {
    ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->OutEvent(hhid->Report_buf[0], 
                                                              hhid->Report_buf[1]);
    hhid->IsReportAvailable = 0;      
  }

  return USBD_OK;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t  *USBD_CUSTOM_HID_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_CUSTOM_HID_DeviceQualifierDesc);
  return USBD_CUSTOM_HID_DeviceQualifierDesc;
}

/**
* @brief  USBD_CUSTOM_HID_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CUSTOMHID Interface callback
  * @retval status
  */
uint8_t  USBD_CUSTOM_HID_RegisterInterface  (USBD_HandleTypeDef   *pdev, 
                                             USBD_CUSTOM_HID_ItfTypeDef *fops)
{
  uint8_t  ret = USBD_FAIL;
  
  if(fops != NULL)
  {
    pdev->pUserData= fops;
    ret = USBD_OK;    
  }
  
  return ret;
}
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
