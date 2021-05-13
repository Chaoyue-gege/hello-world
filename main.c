/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0  + STM32 ¹Ì¼þ¿âÀý³Ì
  *********************************************************************
  * @attention
  *
  * ÊµÑéÆ½Ì¨:Ò°»ð STM32 È«ÏµÁÐ¿ª·¢°å 
  * ÂÛÌ³    :http://www.firebbs.cn
  * ÌÔ±¦    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */

/*
*************************************************************************
*                             °üº¬µÄÍ·ÎÄ¼þ
*************************************************************************
*/
/* FreeRTOSÍ·ÎÄ¼þ */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* ¿ª·¢°åÓ²¼þbspÍ·ÎÄ¼þ */
#include "./bsp_all.h"
/* STemWINÍ·ÎÄ¼þ */
#include "GUI.h"
#include "MainTask.h"

/* FATFS */
#include "ff.h"
#include "diskio.h"
#include "integer.h"

/**************************** ÈÎÎñ¾ä±ú ********************************/
/* 
 * ÈÎÎñ¾ä±úÊÇÒ»¸öÖ¸Õë£¬ÓÃÓÚÖ¸ÏòÒ»¸öÈÎÎñ£¬µ±ÈÎÎñ´´½¨ºÃÖ®ºó£¬Ëü¾Í¾ßÓÐÁËÒ»¸öÈÎÎñ¾ä±ú
 * ÒÔºóÎÒÃÇÒªÏë²Ù×÷Õâ¸öÈÎÎñ¶¼ÐèÒªÍ¨¹ýÕâ¸öÈÎÎñ¾ä±ú£¬Èç¹ûÊÇ×ÔÉíµÄÈÎÎñ²Ù×÷×Ô¼º£¬ÄÇÃ´
 * Õâ¸ö¾ä±ú¿ÉÒÔÎªNULL¡£
 */
///* ´´½¨ÈÎÎñ¾ä±ú */
static TaskHandle_t AppTaskCreate_Handle = NULL;
///* LEDÈÎÎñ¾ä±ú */
//static TaskHandle_t KeyScan_Task_Handle = NULL;
///* TouchÈÎÎñ¾ä±ú */
static TaskHandle_t Touch_Task_Handle = NULL;
///* GUIÈÎÎñ¾ä±ú */
static TaskHandle_t GUI_Task_Handle = NULL;
//static TaskHandle_t CPU_Task_Handle = NULL;

/********************************** ÄÚºË¶ÔÏó¾ä±ú *********************************/
/*
 * ÐÅºÅÁ¿£¬ÏûÏ¢¶ÓÁÐ£¬ÊÂ¼þ±êÖ¾×é£¬Èí¼þ¶¨Ê±Æ÷ÕâÐ©¶¼ÊôÓÚÄÚºËµÄ¶ÔÏó£¬ÒªÏëÊ¹ÓÃÕâÐ©ÄÚºË
 * ¶ÔÏó£¬±ØÐëÏÈ´´½¨£¬´´½¨³É¹¦Ö®ºó»á·µ»ØÒ»¸öÏàÓ¦µÄ¾ä±ú¡£Êµ¼ÊÉÏ¾ÍÊÇÒ»¸öÖ¸Õë£¬ºóÐøÎÒ
 * ÃÇ¾Í¿ÉÒÔÍ¨¹ýÕâ¸ö¾ä±ú²Ù×÷ÕâÐ©ÄÚºË¶ÔÏó¡£
 *
 * ÄÚºË¶ÔÏóËµ°×ÁË¾ÍÊÇÒ»ÖÖÈ«¾ÖµÄÊý¾Ý½á¹¹£¬Í¨¹ýÕâÐ©Êý¾Ý½á¹¹ÎÒÃÇ¿ÉÒÔÊµÏÖÈÎÎñ¼äµÄÍ¨ÐÅ£¬
 * ÈÎÎñ¼äµÄÊÂ¼þÍ¬²½µÈ¸÷ÖÖ¹¦ÄÜ¡£ÖÁÓÚÕâÐ©¹¦ÄÜµÄÊµÏÖÎÒÃÇÊÇÍ¨¹ýµ÷ÓÃÕâÐ©ÄÚºË¶ÔÏóµÄº¯Êý
 * À´Íê³ÉµÄ
 * 
 */

/******************************* È«¾Ö±äÁ¿ÉùÃ÷ ************************************/
/*
 * µ±ÎÒÃÇÔÚÐ´Ó¦ÓÃ³ÌÐòµÄÊ±ºò£¬¿ÉÄÜÐèÒªÓÃµ½Ò»Ð©È«¾Ö±äÁ¿¡£
 */
FATFS fs; /* FatFsÎÄ¼þÏµÍ³¶ÔÏó */
FIL file; /* file objects */
UINT bw;  /* File R/W count */
FRESULT result;
FILINFO fno;
DIR dir;

int8_t SRS_Select = 5;
int8_t Frq_Select = 4;

/*
*************************************************************************
*                             º¯ÊýÉùÃ÷
*************************************************************************
//*/
static void AppTaskCreate(void); /* ÓÃÓÚ´´½¨ÈÎÎñ */

//static void KeyScan_Task(void *parameter); /* DataProcess_Task */
static void GUI_Task(void *parameter);         /* GUI_TaskÈÎÎñÊµÏÖ */
static void Touch_Task(void *parameter);
//static void CPU_Task(void *pvParameters); /* CPU_TaskÈÎÎñÊµÏÖ */

static void BSP_Init(void); /* ÓÃÓÚ³õÊ¼»¯°åÔØÏà¹Ø×ÊÔ´ */

/*****************************************************************
  * @brief  Ö÷º¯Êý
  * @param  ÎÞ
  * @retval ÎÞ
  * @note   µÚÒ»²½£º¿ª·¢°åÓ²¼þ³õÊ¼»¯ 
            µÚ¶þ²½£º´´½¨APPÓ¦ÓÃÈÎÎñ
            µÚÈý²½£ºÆô¶¯FreeRTOS£¬¿ªÊ¼¶àÈÎÎñµ÷¶È
  ****************************************************************/
int main(void)
{
    BaseType_t xReturn = pdPASS; /* ¶¨ÒåÒ»¸ö´´½¨ÐÅÏ¢·µ»ØÖµ£¬Ä¬ÈÏÎªpdPASS */

    /* ¿ª·¢°åÓ²¼þ³õÊ¼»¯ */
    BSP_Init();

    printf("\r\n ********** emwin DEMO *********** \r\n");

    /* ´´½¨AppTaskCreateÈÎÎñ */
    xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,          /* ÈÎÎñÈë¿Úº¯Êý */
                          (const char *)"AppTaskCreate",          /* ÈÎÎñÃû×Ö */
                          (uint16_t)256,                          /* ÈÎÎñÕ»´óÐ¡ */
                          (void *)NULL,                           /* ÈÎÎñÈë¿Úº¯Êý²ÎÊý */
                          (UBaseType_t)1,                         /* ÈÎÎñµÄÓÅÏÈ¼¶ */
                          (TaskHandle_t *)&AppTaskCreate_Handle); /* ÈÎÎñ¿ØÖÆ¿éÖ¸Õë */
    /* Æô¶¯ÈÎÎñµ÷¶È */
    if (pdPASS == xReturn)
        vTaskStartScheduler(); /* Æô¶¯ÈÎÎñ£¬¿ªÆôµ÷¶È */
    else
        return -1;

    while (1)
        ; /* Õý³£²»»áÖ´ÐÐµ½ÕâÀï */
}

/***********************************************************************
  * @ º¯ÊýÃû  £º AppTaskCreate
  * @ ¹¦ÄÜËµÃ÷£º ÎªÁË·½±ã¹ÜÀí£¬ËùÓÐµÄÈÎÎñ´´½¨º¯Êý¶¼·ÅÔÚÕâ¸öº¯ÊýÀïÃæ
  * @ ²ÎÊý    £º ÎÞ  
  * @ ·µ»ØÖµ  £º ÎÞ
  **********************************************************************/
static void AppTaskCreate(void)
{
    BaseType_t xReturn = pdPASS; /* ¶¨ÒåÒ»¸ö´´½¨ÐÅÏ¢·µ»ØÖµ£¬Ä¬ÈÏÎªpdPASS */

    taskENTER_CRITICAL(); //½øÈëÁÙ½çÇø

    xReturn = xTaskCreate((TaskFunction_t)KeyScan_Task,        /* ÈÎÎñÈë¿Úº¯Êý */
                          (const char *)"KeyScan_Task",        /* ÈÎÎñÃû³Æ */
                          (uint16_t)128,                           /* ÈÎÎñÕ»´óÐ¡ */
                          (void *)NULL,                            /* ÈÎÎñÈë¿Úº¯Êý²ÎÊý */
                          (UBaseType_t)10,                         /* ÈÎÎñµÄÓÅÏÈ¼¶ */
                          (TaskHandle_t)&KeyScan_Task_Handle); /* ÈÎÎñ¿ØÖÆ¿éÖ¸Õë */
    if (pdPASS == xReturn)
        printf("´´½¨KeyScan_TaskÈÎÎñ³É¹¦£¡\r\n");

    xReturn = xTaskCreate((TaskFunction_t)Touch_Task,        /* ÈÎÎñÈë¿Úº¯Êý */
                          (const char *)"Touch_Task",        /* ÈÎÎñÃû³Æ */
                          (uint16_t)128,                     /* ÈÎÎñÕ»´óÐ¡ */
                          (void *)NULL,                      /* ÈÎÎñÈë¿Úº¯Êý²ÎÊý */
                          (UBaseType_t)4,                    /* ÈÎÎñµÄÓÅÏÈ¼¶ */
                          (TaskHandle_t)&Touch_Task_Handle); /* ÈÎÎñ¿ØÖÆ¿éÖ¸Õë */
    if (pdPASS == xReturn)
        printf("´´½¨Touch_TaskÈÎÎñ³É¹¦£¡\r\n");

    xReturn = xTaskCreate((TaskFunction_t)GUI_Task,        /* ÈÎÎñÈë¿Úº¯Êý */
                          (const char *)"GUI_Task",        /* ÈÎÎñÃû³Æ */
                          (uint16_t)512,                  /* ÈÎÎñÕ»´óÐ¡ */
                          (void *)NULL,                    /* ÈÎÎñÈë¿Úº¯Êý²ÎÊý */
                          (UBaseType_t)3,                  /* ÈÎÎñµÄÓÅÏÈ¼¶ */
                          (TaskHandle_t)&GUI_Task_Handle); /* ÈÎÎñ¿ØÖÆ¿éÖ¸Õë */
    if (pdPASS == xReturn)
        printf("´´½¨GUI_TaskÈÎÎñ³É¹¦£¡\r\n");

    xReturn = xTaskCreate((TaskFunction_t)CPU_Task,          /* ÈÎÎñÈë¿Úº¯Êý */
                          (const char *)"CPU_Task",          /* ÈÎÎñÃû×Ö */
                          (uint16_t)256,                     /* ÈÎÎñÕ»´óÐ¡ */
                          (void *)NULL,                      /* ÈÎÎñÈë¿Úº¯Êý²ÎÊý */
                          (UBaseType_t)5,                    /* ÈÎÎñµÄÓÅÏÈ¼¶ */
                          (TaskHandle_t *)&CPU_Task_Handle); /* ÈÎÎñ¿ØÖÆ¿éÖ¸Õë */
    if (pdPASS == xReturn)
        printf("´´½¨CPU_TaskÈÎÎñ³É¹¦!\r\n");

    vTaskDelete(AppTaskCreate_Handle); //É¾³ýAppTaskCreateÈÎÎñ

    taskEXIT_CRITICAL(); //ÍË³öÁÙ½çÇø
}

///**
//  * @brief LEDÈÎÎñÖ÷Ìå
//  * @note ÎÞ
//  * @param ÎÞ
//  * @retval ÎÞ
//  */


static void KeyScan_Task(void *parameter)
{
    static int8_t Index, LastIndex;

    while (1)
    {
        switch (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN))
        {
        case KEY_ON:;
            _cbKey(Index, 1);
            break;
        
        case KEY_HOLD:
            _cbKey(Index, 1);
            break;

        case KEY_2ClICK:
            LastIndex = Index;
            if(Index++ == waveform)
                Index = channel;
            PickActiveWin(Index, LastIndex);

            break;
        case KEY_3ClICK:
            _cbKey(dac, 1);
            break;

        case KEY_ERROR:
            printf("error\n");
            break;
        default:

            break;
        }

        switch (Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN))
        {
        case KEY_ON:
            _cbKey(Index, 0);
            break;
        
        case KEY_HOLD:
            _cbKey(Index, 0);
            break;

        case KEY_2ClICK:
            LastIndex = Index;
            if(Index-- == channel)
                Index = waveform;
            PickActiveWin(Index, LastIndex);
            break;

         case KEY_3ClICK:
            _cbKey(dac, 0);

            break;

        case KEY_ERROR:
            printf("error\n");
            break;
        default:
          
            break;
        }
        
        
        
        vTaskDelay(10);
    }
}

///**
//  * @brief ´¥Ãþ¼ì²âÈÎÎñÖ÷Ìå
//  * @note ÎÞ
//  * @param ÎÞ
//  * @retval ÎÞ
//  */
static void Touch_Task(void *parameter)
{
    while (1)
    {
        GUI_TOUCH_Exec(); //´¥ÃþÆÁ¶¨Ê±É¨Ãè
        vTaskDelay(10);
    }
}

///**
//  * @brief GUIÈÎÎñÖ÷Ìå
//  * @note ÎÞ
//  * @param ÎÞ
//  * @retval ÎÞ
//  */
static void GUI_Task(void *parameter)
{
    /* ³õÊ¼»¯STemWin */

    while (1)
    {
        MainTask();
    }
}

static void CPU_Task(void *parameter)
{
    uint8_t CPU_RunInfo[400]; //±£´æÈÎÎñÔËÐÐÊ±¼äÐÅÏ¢

    while (1)
    {
        memset(CPU_RunInfo, 0, 400); //ÐÅÏ¢»º³åÇøÇåÁã

        vTaskList((char *)&CPU_RunInfo); //»ñÈ¡ÈÎÎñÔËÐÐÊ±¼äÐÅÏ¢

        printf("---------------------------------------------\r\n");
        printf("ÈÎÎñÃû      ÈÎÎñ×´Ì¬ ÓÅÏÈ¼¶   Ê£ÓàÕ» ÈÎÎñÐòºÅ\r\n");
        printf("%s", CPU_RunInfo);
        printf("---------------------------------------------\r\n");

        memset(CPU_RunInfo, 0, 400); //ÐÅÏ¢»º³åÇøÇåÁã

        vTaskGetRunTimeStats((char *)&CPU_RunInfo);

        printf("ÈÎÎñÃû       ÔËÐÐ¼ÆÊý         ÀûÓÃÂÊ\r\n");
        printf("%s", CPU_RunInfo);
        printf("---------------------------------------------\r\n\n");
        vTaskDelay(20000); /* ÑÓÊ±500¸ötick */
    }
}

/**
  * @brief °å¼¶ÍâÉè³õÊ¼»¯
  * @note ËùÓÐ°å×ÓÉÏµÄ³õÊ¼»¯¾ù¿É·ÅÔÚÕâ¸öº¯ÊýÀïÃæ
  * @param ÎÞ
  * @retval ÎÞ
  */
static void BSP_Init(void)
{
    /* CRCºÍemWinÃ»ÓÐ¹ØÏµ£¬Ö»ÊÇËûÃÇÎªÁË¿âµÄ±£»¤¶ø×öµÄ
   * ÕâÑùSTemWinµÄ¿âÖ»ÄÜÓÃÔÚSTµÄÐ¾Æ¬ÉÏÃæ£¬±ðµÄÐ¾Æ¬ÊÇÎÞ·¨Ê¹ÓÃµÄ¡£
   */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

    /*
	 * STM32ÖÐ¶ÏÓÅÏÈ¼¶·Ö×éÎª4£¬¼´4bit¶¼ÓÃÀ´±íÊ¾ÇÀÕ¼ÓÅÏÈ¼¶£¬·¶Î§Îª£º0~15
	 * ÓÅÏÈ¼¶·Ö×éÖ»ÐèÒª·Ö×éÒ»´Î¼´¿É£¬ÒÔºóÈç¹ûÓÐÆäËûµÄÈÎÎñÐèÒªÓÃµ½ÖÐ¶Ï£¬
	 * ¶¼Í³Ò»ÓÃÕâ¸öÓÅÏÈ¼¶·Ö×é£¬Ç§Íò²»ÒªÔÙ·Ö×é£¬ÇÐ¼É¡£
	 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    DAC_Mode_Init();

    ADCx_Init();
    /* LED ³õÊ¼»¯ */
    LED_GPIO_Config();

    /* ·äÃùÆ÷³õÊ¼»¯ */
    Beep_Init();

    /* ´®¿Ú³õÊ¼»¯	*/
    USART_Config();

    /* °´¼ü³õÊ¼»¯	*/
    Key_GPIO_Config();

    /* ´¥ÃþÆÁ³õÊ¼»¯ */
    XPT2046_Init();

    BASIC_TIM_Init();
}

/********************************END OF FILE****************************/
