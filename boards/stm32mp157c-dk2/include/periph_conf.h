/*
 * Copyright (C) 2020 Savoir-faire Linux
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32mp157c-dk2
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32MP157C-DK2 board
 *
 * @author      Gilles DOFFE <gilles.doffe@savoirfairelinux.com>
 *
 * @}
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_timer_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock settings
 *
 * @note    This is auto-generated from
 *          `cpu/stm32_common/dist/clk_conf/clk_conf.c`
 * @{
 */

/* If not in Engineering mode (Cortex-M4 only), following parameters must be
 * equal to the Cortex-A7 clocks configuration from Linux/U-boot device tree.
 * */

/* give the target core clock (HCLK) frequency [in Hz],
 * maximum: 209MHz (rounded) */
#define CLOCK_CORECLOCK     MHZ(208)
/* 0: no external high speed crystal available
 * else: actual crystal frequency [in Hz] */
#define CLOCK_HSE           MHZ(24)
/* 0: no external low speed crystal available,
 * 1: external crystal available (always 32.768kHz) */
#define CLOCK_LSE           (1U)
/* 0: no internal oscillator
 * else: actual internal oscillator frequency [in Hz] */
#define CLOCK_HSI           MHZ(64)

/* peripheral clock setup */
#define CLOCK_MCU           (CLOCK_CORECLOCK / 1)      /* max 209MHz */
#define CLOCK_APB1          (CLOCK_CORECLOCK / 2)      /* max 104MHz */
#define CLOCK_APB2          (CLOCK_CORECLOCK / 2)      /* max 104MHz */
#define CLOCK_APB3          (CLOCK_CORECLOCK / 2)      /* max 104MHz */

/* The following registers should not be set if not in engineering mode
 * (Cortex-M4 only), because it could override Cortex-A7 clock
 * settings.
 */
#if IS_USED(MODULE_STM32MP1_ENG_MODE)

#define CLOCK_MCU_DIV       RCC_MCUDIVR_MCUDIV_0       /* max 209MHz */
#define CLOCK_APB1_DIV      RCC_APB1DIVR_APB1DIV_1     /* max 104MHz */
#define CLOCK_APB2_DIV      RCC_APB2DIVR_APB2DIV_1     /* max 104MHz */
#define CLOCK_APB3_DIV      RCC_APB3DIVR_APB3DIV_1     /* max 104MHz */

/* Main PLL factors */
#define CLOCK_PLL_M          (2)
#define CLOCK_PLL_N          (52)
#define CLOCK_PLL_P          (3)
#define CLOCK_PLL_Q          (13)

#endif

/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART3,
        .rcc_mask   = RCC_MC_APB1ENSETR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_B, 12),
        .tx_pin     = GPIO_PIN(PORT_B, 10),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
#if IS_USED(MODULE_STM32MP1_ENG_MODE)
        .clk_src    = RCC_UART35CKSELR_UART35SRC_4, /* HSE clock source */
#else
        .clk_src    = RCC_UART35CKSELR_UART35SRC_2, /* HSI clock source */
#endif
        .irqn       = USART3_IRQn,
#ifdef UART_USE_DMA
        .dma_stream = 4,
        .dma_chan   = 4
#endif
    }
};

#define UART_0_ISR          (isr_usart3)
#define UART_0_DMA_ISR      (isr_dma1_stream4)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
