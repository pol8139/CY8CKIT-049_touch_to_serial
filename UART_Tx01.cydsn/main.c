/*******************************************************************************
* File Name: main.c
*
* Version: 2.1
*
* Description:
*  This project demonstrates the UART transmission mechanism. 
*  Data is sent through serial port that is visible in the Hyperterminal and on 
*  the LCD. ASCII characters from 0x20 to 0x7F(Space to ~ )  that can be 
*  displayed through Hyperterminal/LCD at the same time Interrupt is triggered
*  and incremented while sending.
*
********************************************************************************
* Copyright 2012, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <project.h>

#define LCD_NUM_COLUMNS     (16u)
#define START_CHAR_VALUE    (0x20u)
#define END_CHAR_VALUE      (0x7Eu)

uint8 trigger = 0u;


int main()
{    
    uint8 ch;           /* Data sent on the serial port */
    uint8 count = 0u;    /* Initializing the count value */
    uint8 pos = 0u;

    CyGlobalIntEnable; 

    isr_1_Start();      /* Initializing the ISR */
    UART_1_Start();     /* Enabling the UART */
    LCD_Char_1_Start(); /* Enabling the LCD */

        
    for(ch = START_CHAR_VALUE; ch <= END_CHAR_VALUE; ch++)
    {
        UART_1_WriteTxData(ch); /* Sending the data */

        count++;        
        if(count % LCD_NUM_COLUMNS == 0u) /* If the count value reaches the count 16 start from first location */
        {
            pos = 0u;  /* resets the count value */
            LCD_Char_1_WriteControl(LCD_Char_1_CLEAR_DISPLAY); /* Display will be cleared when reached count value 16 */
        }    

        LCD_Char_1_Position(0u, pos++);   /* LCD position to the count++ */
        LCD_Char_1_PutChar(ch);         /* print the value in the LCD */
                
        LCD_Char_1_Position(1u, 0u);
        LCD_Char_1_PrintInt8(count);    /* prints the count in the LCD */
        CyDelay(200u);
    }

    for(;;) {}
}


/* [] END OF FILE */

