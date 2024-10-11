/*
 * Copyright 2024 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/devicetree.h>
#include <zephyr/platform/hooks.h>
#include <soc.h>

#if CONFIG_BOARD_EARLY_INIT_HOOK
void board_early_init_hook(void)
{
#if defined(CONFIG_ETH_NXP_IMX_NETC) && (DT_CHILD_NUM_STATUS_OKAY(DT_NODELABEL(netc)) != 0)
	/* RMII mode */
	BLK_CTRL_WAKEUPMIX->NETC_LINK_CFG[4] = BLK_CTRL_WAKEUPMIX_NETC_LINK_CFG_MII_PROT(1);

	/* Output reference clock for RMII */
	BLK_CTRL_WAKEUPMIX->NETC_PORT_MISC_CFG |=
		BLK_CTRL_WAKEUPMIX_NETC_PORT_MISC_CFG_PORT4_RMII_REF_CLK_DIR_MASK;

	/* Unlock the IERB. It will warm reset whole NETC. */
	NETC_PRIV->NETCRR &= ~NETC_PRIV_NETCRR_LOCK_MASK;

	while ((NETC_PRIV->NETCRR & NETC_PRIV_NETCRR_LOCK_MASK) != 0U) {
	}

	/* Set the access attribute, otherwise MSIX access will be blocked. */
	NETC_IERB->ARRAY_NUM_RC[0].RCMSIAMQR &= ~(7U << 27);
	NETC_IERB->ARRAY_NUM_RC[0].RCMSIAMQR |= (1U << 27);

	/* Lock the IERB. */
	NETC_PRIV->NETCRR |= NETC_PRIV_NETCRR_LOCK_MASK;
	while ((NETC_PRIV->NETCSR & NETC_PRIV_NETCSR_STATE_MASK) != 0U) {
	}
#endif
}
#endif