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
* (c) 2025, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
* 
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
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
