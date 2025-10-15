/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the PSoC 4 - TCPWM timer
 *             for ModusToolbox.
*
* Related Document: See README.md 
*
*
*******************************************************************************
* Copyright 2025, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include "cy_pdl.h"
#include "cybsp.h"
/******************************************************************************/
/*  Macros                                                                    */
/******************************************************************************/
#define MY_TCPWM_CNT_MULT_MASK  (uint32_t)(ASTIMER_MASK)  /* |\ */
                                              /* (1UL << MY_TCPWM_CNT_5) |\ */
                                              /* (1UL << MY_TCPWM_CNT_6))   */

/******************************************************************************/
/*  Global variables                                                          */
/******************************************************************************/
cy_stc_sysint_t timer_intr_cfg =
{
    .intrSrc = ASTIMER_IRQ,  /* Source of interrupt signal*/
    .intrPriority = 3UL    /* Interrupt priority */
};

/******************************************************************************
 * Function Name: timer_isr_handler
 * Summary:
 *   TIMER interrupt service routine.
 *   Although interrupt is called by CC and TC, CC will be skiped according to intStatus..
 * Parameters:
 *  none
 * Return:
 *  int
 ******************************************************************************
 */
void Timer_Handler(void)
{
    uint32_t intStatus = Cy_TCPWM_GetInterruptStatusMasked(ASTIMER_HW, ASTIMER_NUM);
    Cy_TCPWM_ClearInterrupt(ASTIMER_HW, ASTIMER_NUM, intStatus);

    if ((intStatus & CY_TCPWM_INT_ON_TC ) != 0UL)
    {
        Cy_GPIO_Inv(CYBSP_USER_LED6_PORT, CYBSP_USER_LED6_PIN);
    }
}

/******************************************************************************
 * Function Name: main
 * Summary:
 *  This is the main function.
 *   1. Initializes the BSP.
 *   2. Configures the interrupt.
 *   3. Configures the timer and counter peripherals.
 *   4. Trigger the Timer block.
 * Parameters:
 *  none
 * Return:
 *  int
 ******************************************************************************
 */
int main(void)
{
    cy_rslt_t result;
    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize TIMER interrupt */
    result = Cy_SysInt_Init(&timer_intr_cfg, Timer_Handler);
    if(result != CY_SYSINT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    /* Clearing and enabling the Timer interrupt in NVIC */
    NVIC_ClearPendingIRQ(timer_intr_cfg.intrSrc);
    NVIC_EnableIRQ(timer_intr_cfg.intrSrc);

    /* Initialize the TIMER */
    if (CY_TCPWM_SUCCESS != Cy_TCPWM_Counter_Init(ASTIMER_HW, ASTIMER_NUM, &ASTIMER_config))
    {
        /* Handle possible errors */
        CY_ASSERT(0);
    }
    /* Enables the counter and TriggerStart*/
    Cy_TCPWM_Counter_Enable(ASTIMER_HW, ASTIMER_NUM);
    Cy_TCPWM_TriggerStart(ASTIMER_HW, MY_TCPWM_CNT_MULT_MASK);

    for (;;)
    {
    }
}

/* [] END OF FILE */
