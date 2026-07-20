/*==================================================================================================
 * Project : RTD AUTOSAR 4.7
 * Platform : CORTEXM
 * Peripheral : S32K3XX
 * Dependencies : none
 *
 * Autosar Version : 4.9.0
 * Autosar Revision : ASR_REL_4_9_REV_0000
 * Autosar Conf.Variant :
 * SW Version : 7.0.0
 * Build Version : S32K3_RTD_7_0_0_QLP03_D2512_ASR_REL_4_9_REV_0000_20251210
 *
 * Copyright 2020 - 2026 NXP
 *
 *   NXP Proprietary. This software is owned or controlled by NXP and may only be
 *   used strictly in accordance with the applicable license terms. By expressly
 *   accepting such terms or by downloading, installing, activating and/or otherwise
 *   using the software, you are agreeing that you have read, and that you agree to
 *   comply with and are bound by, such license terms. If you do not agree to be
 *   bound by the applicable license terms, then you may not retain, install,
 *   activate or otherwise use the software.
 ==================================================================================================*/

/**
 *   @file main.c
 *
 *   @addtogroup main_module main module documentation
 *   @{
 */

/* User includes */
#include "Mcu.h"            /*!< MCU        Driver */
#include "OsIf.h"           /*!< OSIF       Driver */
#include "Port.h"           /*!< PORT       Driver */
#include "Dio.h"            /*!< DIO        Driver */
#include "Platform.h"       /*!< PLATFORM   Driver */
#include "Icu.h"            /*!< ICU        Driver */
#include "Gpt.h"            /*!< GPT        Driver */
#include "Lpuart_Uart_Ip.h" /*!< LPUART     Driver */
#include <string.h>
#include <stdio.h>

/* LPUART instance used for terminal output (matches MEX configuration) */
#define LPUART_INSTANCE             6U

/* RTC clock is SIRC 32K; API Compare Value = Xms * 32 */
#define RTC_API_TIME        (160000U) /* API Compare Value = 5000ms * 32 = 160000 */

/* Delay before entering standby (in microseconds) */
#define AWAKE_TIME_US       (2000000U)   /* Stay awake for 2 seconds before entering standby */

/* Counts how many times the MCU has woken up from standby.
 * Placed in .standby_data which is NOT zeroed by the C startup on
 * any reset (including the functional reset triggered by standby wakeup),
 * so the value survives across standby/wakeup cycles.
 * The counter is explicitly reset to 0 on cold/non-standby boot.
 * volatile prevents the compiler from keeping the value in a register
 * instead of writing it back to SRAM. */
__attribute__ ((section (".standby_data"))) volatile uint32 wakeup_count;

/* Scratch buffer used for formatted UART messages */
static char FormattedMessage[128];

/*==================================================================================================
 *                                        LOCAL FUNCTIONS
 ==================================================================================================*/

/*!
 * @brief Transmit a null-terminated string over LPUART using synchronous (blocking) send.
 *
 * Each byte is sent individually. The function waits for the driver to become
 * ready before sending and waits for the transmission to complete afterwards.
 *
 * @param[in] Message  Pointer to the null-terminated string to transmit.
 */
static void SendMessage(const char *Message) {
	uint32_t msgLen = (uint32_t) strlen(Message);
	uint32_t bytesRemaining = 0U;
	uint8 txByte;

	for (uint32_t idx = 0U; idx < msgLen; idx++) {
		txByte = (uint8) Message[idx];

		/* Wait for the driver to be ready before sending the next byte */
		while (Lpuart_Uart_Ip_GetTransmitStatus(LPUART_INSTANCE,
				&bytesRemaining) == LPUART_UART_IP_STATUS_BUSY) {
			/* busy-wait */
		}

		/* Transmit one byte; 100 ms timeout */
		Lpuart_Uart_Ip_SyncSend(LPUART_INSTANCE, &txByte, 1U, 100000U);

		/* Wait for the current byte transmission to complete */
		while (Lpuart_Uart_Ip_GetTransmitStatus(LPUART_INSTANCE,
				&bytesRemaining) == LPUART_UART_IP_STATUS_BUSY) {
			/* busy-wait */
		}
	}
}

/*!
 \brief The main function for the project.
 \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
 */

int main(void) {
	/* OsIf_Init to initial a timer which will be used in another module to execute the timeout monitoring.
	 So this API must be called first, Otherwise, the timeout monitoring will be inaccurate. */
	OsIf_Init(NULL_PTR);

	/* To make the configuration setting for power down, clock and RAM sections visible within the MCU Driver. */
	Mcu_Init(&Mcu_Config);

	/* This function initializes the PLL and MCU specific clock options.
	 The clock configuration following Reference Manual:
	 Chapter 24.7.2.5 Option A - High Performance mode (CORE_CLK @ 160 MHz) */
	Mcu_InitClock(McuClockSettingConfig_Run);
	/* If the PLL is used as a clock source, call Mcu_GetPllStatus until it returns MCU_PLL_LOCKED and call
	 Mcu_DistributePllClock. */
	while (MCU_PLL_LOCKED != Mcu_GetPllStatus()) {
		/* Busy wait until the System PLL is locked */
	}
	/* To activates the PLL clock to the MCU clock distribution. */
	/* Note: The clock switching to PLL is completed by Mcu_DistributePllClock but not Mcu_InitClock. */
	Mcu_DistributePllClock();
	/* This function activates MCU power mode from configure structure selected by McuMode parameter.
	 If the driver state is invalid or McuMode is not in range the function will skip changing the MCU mode */
	Mcu_SetMode(McuModeSettingConf_Run);

	/* Read the reset reason to detect a standby wakeup.
	 * This call also clears RDSS to prevent automatic re-entry into standby. */
	Mcu_ResetType resetReason = Mcu_GetResetReason();

	/* Initialize Platform (interrupts enable and ISR installation) driver */
	Platform_Init(NULL_PTR);

	/* Initialize all pins using the Port driver */
	Port_Init(NULL_PTR);

	/* Initialize LPUART for terminal output */
	Lpuart_Uart_Ip_Init(LPUART_INSTANCE, &Lpuart_Uart_Ip_xHwConfigPB_0);

	/*  Turn-on Green LED */
	Dio_WriteChannel(DioConf_DioChannel_DioChannel_LED_GRN, STD_LOW);

	/* On S32K312, wakeup from standby causes a functional reset so execution
	 * always restarts from the top of main(). Detect this here via the reset
	 * reason and print the appropriate message. */

	if (MCU_WAKEUP_REASON == resetReason) {
		/* This boot is a wakeup from standby - increment and report it */
		wakeup_count++;

		if (wakeup_count > 100) {
			wakeup_count = 0;
		}

		/* Clean the D-cache line holding wakeup_count so the updated value
		 * is written back to physical SRAM before the next standby entry.
		 * The CM7 D-cache is write-back; a bare dsb does not flush dirty
		 * cache lines. SCB_DCCMVAC (0xE000EF64) triggers a clean (write-back)
		 * of the cache line at the given address. dsb sy + isb ensure the
		 * clean is complete before any subsequent access. */
#define SCB_DCCMVAC_REG  (*((volatile uint32_t *)0xE000EF64U))
		SCB_DCCMVAC_REG = (uint32_t) &wakeup_count;
		__asm volatile ("dsb sy" : : : "memory");
		__asm volatile ("isb" : : : "memory");

		snprintf(FormattedMessage, sizeof(FormattedMessage),
				"[WKPU] Hello! I woke up the %lu time. Going back to sleep now!\r\n",
				(unsigned long) wakeup_count);
		SendMessage(FormattedMessage);
	} else {
		/* Cold start or non-standby reset - reset the persistent counter and print startup banner */
		wakeup_count = 0U;

		/* Force write to memory */
		__asm volatile("dsb" ::: "memory");
		__asm volatile("isb" ::: "memory");

		SendMessage("[WKPU] RTC Periodic Wakeup demo started.\r\n");
		SendMessage(
				"[WKPU] Entering standby mode. MCU will wake up after ~5 s.\r\n");
	}

	/* Ensure all UART data is transmitted before proceeding */
	volatile uint32 uart_delay;
	for (uart_delay = 0; uart_delay < 1000000U; uart_delay++) {
		__asm volatile("nop");
	}

	/* Turn-off Green LED */
	Dio_WriteChannel(DioConf_DioChannel_DioChannel_LED_GRN, STD_HIGH);

	/* Ensure LED write completes */
	__asm volatile("dsb" ::: "memory");
	__asm volatile("isb" ::: "memory");

	/* Switch system clock to FIRC before entering standby.
	 * The clock configuration following Reference Manual:
	 * 24.7.2.8 Option C - Boot Standby mode (CORE_CLK @ 24 MHz) */
	Mcu_InitClock(McuClockSettingConfig_Standby);

	/* Initialize the Icu driver for wake up source configuration */
	Icu_Init(NULL_PTR);

	/* Enable edge detection on the RTC API wake up channel */
	Icu_EnableEdgeDetection(IcuChannel_RTC_API);

	/* Initialize GPT and start the RTC API timer.
	 * Stop the channel first (as per reference project) to ensure
	 * the timer is in a known state before starting it. */
	Gpt_Init(NULL_PTR);
	Gpt_StopTimer(GptConf_GptChannelConfiguration_GptChannelConfiguration_0);
	Gpt_StartTimer(GptConf_GptChannelConfiguration_GptChannelConfiguration_0,
	RTC_API_TIME);

	/* Clean the D-cache line holding wakeup_count immediately before standby.
	 * A speculative reload between the earlier clean and this point could have
	 * left the line dirty again. This guarantees SRAM holds the correct value
	 * when the cache is lost on standby entry. */
	SCB_DCCMVAC_REG = (uint32_t) &wakeup_count;
	__asm volatile ("dsb sy" : : : "memory");
	__asm volatile ("isb" : : : "memory");

	/* Enter Standby Mode.
	 * NOTE: On S32K312 wakeup from standby causes a functional reset.
	 * Execution does NOT resume after this call - the MCU reboots and
	 * restarts from the beginning of main() where MCU_WAKEUP_REASON
	 * is detected and the wakeup message is printed. */
	Mcu_SetMode(McuModeSettingConf_Standby);

	/* Should never be reached on S32K312 - standby exit is via reset */
	while (1) {
		/* Infinite loop - should never execute */
	}

	return (0U);
}
