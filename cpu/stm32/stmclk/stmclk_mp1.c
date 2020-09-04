/*
 * Copyright (C) 2020 Savoir-faire Linux
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration for STM32MP1
 *
 * @author      Gilles DOFFE <gilles.doffe@savoirfairelinux.com>
 *
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

#ifdef STM32MP1_ENGINEERING_MODE

/* make sure we have all needed information about the clock configuration */
#ifndef CLOCK_HSE
#error "Please provide CLOCK_HSE in your board's perhip_conf.h"
#endif
#ifndef CLOCK_LSE
#error "Please provide CLOCK_LSE in your board's periph_conf.h"
#endif
#ifndef CLOCK_CORECLOCK
#error "Please provide CLOCK_CORECLOCK in your board's periph_conf.h"
#endif

/**
 * @name    PLL configuration
 * @{
 */

/* figure out which input to use */
#if (CLOCK_HSE)
#define PLL_SRC                  RCC_RCK3SELR_PLL3SRC_1 /* HSE */
#else
#define PLL_SRC                  RCC_RCK3SELR_PLL3SRC_0 /* HSI */
#endif

/* now we get the actual bitfields */
#define PLL_P                   (((CLOCK_PLL_P / 2)) << RCC_PLL3CFGR2_DIVP_Pos)
#define PLL_M                   ((CLOCK_PLL_M) << RCC_PLL3CFGR1_DIVM3_Pos)
#define PLL_N                   ((CLOCK_PLL_N) << RCC_PLL3CFGR1_DIVN_Pos)
#define PLL_Q                   ((CLOCK_PLL_Q) << RCC_PLL3CFGR2_DIVQ_Pos)
#if defined(RCC_PLL3CFGR2_DIVR) && defined(CLOCK_PLL_R)
#define PLL_R                   ((CLOCK_PLL_R) << RCC_PLL3CFGR2_DIVR_Pos)
#else
#define PLL_R                   (0)
#endif

/** @} */

void stmclk_enable_hsi(void)
{
    RCC->OCENSETR |= RCC_OCENSETR_HSION;
    while (!(RCC->OCRDYR & RCC_OCRDYR_HSIRDY)) {}
}

void stmclk_disable_hsi(void)
{
    /* we only disable the HSI clock if not used as input for the PLL and if
     * not used directly as system clock */
    if (CLOCK_HSE) {
        RCC->OCENCLRR |= RCC_OCENCLRR_HSION;
        while (!(RCC->OCRDYR & RCC_OCRDYR_HSIRDY)) {}
    }
}

static void stmclk_enable_hse(void)
{
    RCC->OCENSETR |= RCC_OCENSETR_HSEON;
    while (!(RCC->OCRDYR & RCC_OCRDYR_HSERDY)) {}
}

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    RCC->MC_CIFR = 0;

    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configure by the board */
    /* MCUDIV */
    RCC->MCUDIVR =  CLOCK_MCU_DIV;
    while ((RCC->MCUDIVR & RCC_MCUDIVR_MCUDIVRDY)
            != RCC_MCUDIVR_MCUDIVRDY) {}
    /* APB1DIV */
    RCC->APB1DIVR =  CLOCK_APB1_DIV;
    while ((RCC->APB1DIVR & RCC_APB1DIVR_APB1DIVRDY)
            != RCC_APB1DIVR_APB1DIVRDY) {}
    /* APB2DIV */
    RCC->APB2DIVR =  CLOCK_APB2_DIV;
    while ((RCC->APB2DIVR & RCC_APB2DIVR_APB2DIVRDY)
            != RCC_APB2DIVR_APB2DIVRDY) {}
    /* APB3DIV */
    RCC->APB3DIVR =  CLOCK_APB3_DIV;
    while ((RCC->APB3DIVR & RCC_APB3DIVR_APB3DIVRDY)
            != RCC_APB3DIVR_APB3DIVRDY) {}

    /* MCU clock source */
    RCC->MSSCKSELR = RCC_MSSCKSELR_MCUSSRC_0; /* HSI */
    while ((RCC->MSSCKSELR & RCC_MSSCKSELR_MCUSSRCRDY)
            != RCC_MSSCKSELR_MCUSSRCRDY) {}

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->OCENCLRR = ~(RCC_OCENSETR_HSION);

#if (CLOCK_MCO1_SRC)
#ifndef RCC_CFGR_MCO1
#error "stmclk: no MCO1 on this device"
#endif
    RCC->CFGR |= CLOCK_MCO1_SRC | CLOCK_MCO1_PRE;
#endif
#if (CLOCK_MCO2_SRC)
#ifndef RCC_CFGR_MCO2
#error "stmclk: no MCO2 on this device"
#endif
    RCC->CFGR |= CLOCK_MCO2_SRC | CLOCK_MCO2_PRE;
#endif

    /* if configured, we need to enable the HSE clock now */
#if (CLOCK_HSE)
    stmclk_enable_hse();
#endif

    /* now we can safely configure the PLL */
    RCC->PLL3CFGR1 = (PLL_M | PLL_N);
    RCC->PLL3CFGR2 = (PLL_P | PLL_Q | PLL_R);

    RCC->RCK3SELR |= PLL_SRC;
    while (!(RCC->RCK3SELR & RCC_RCK3SELR_PLL3SRCRDY)) {}

    /* and start the PLL */
    RCC->PLL3CR |= (RCC_PLL3CR_DIVPEN | RCC_PLL3CR_DIVQEN
            | RCC_PLL3CR_DIVREN | RCC_PLL3CR_PLLON);
    while (!(RCC->PLL3CR & RCC_PLL3CR_PLL3RDY)) {}

    /* now that the PLL is running, we use it as system clock */
    RCC->MSSCKSELR = RCC_MSSCKSELR_MCUSSRC_3; /* PLL3 */
    while (!(RCC->MSSCKSELR & RCC_MSSCKSELR_MCUSSRCRDY)) {}

    irq_restore(is);
}

#endif /* STM32MP1_ENGINEERING_MODE */
