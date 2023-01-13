/*
 * xen/arch/arm/platforms/s32-gen1.c
 *
 * NXP S32-Gen1 Platform-specific settings
 *
 * Andrei Cherechesu <andrei.cherechesu@nxp.com>
 * Copyright 2021-2023 NXP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <xen/mm.h>
#include <xen/vmap.h>
#include <asm/platform.h>
#include <asm/io.h>
#include <asm/regs.h>
#include <asm/smccc.h>

#define S32GEN1_MC_ME_CTL_KEY               0x0
#define S32GEN1_MC_ME_CTL_KEY_KEY           0x00005AF0
#define S32GEN1_MC_ME_CTL_KEY_INVERTEDKEY   0x0000A50F

#define S32GEN1_MC_ME_MODE_CONF             0x4
#define S32GEN1_MC_ME_MODE_CONF_DRST        BIT(0, UL)

#define S32GEN1_MC_ME_MODE_UPD              0x8
#define S32GEN1_MC_ME_MODE_UPD_UPD          BIT(0, UL)

#define S32GEN1_MC_RGM_DRET                 0x1C

#define S32GEN1_SMC_SCMI_FN                 0xFE
#define S32GEN1_SMCCC_FID(fn) ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, \
                                                 ARM_SMCCC_CONV_64,   \
                                                 ARM_SMCCC_OWNER_SIP, \
                                                 fn)

static void __iomem *s32gen1_map_device(const char *compatible)
{
    void __iomem *dev_addr;
    struct dt_device_node *node;
    paddr_t dev_start, dev_len;
    int ret;

    node = dt_find_compatible_node(NULL, NULL, compatible);
    if ( !node )
    {
        dprintk(XENLOG_ERR, "S32-Gen1: Cannot find matching %s node in DT\n",
            compatible);
        return NULL;
    }

    ret = dt_device_get_address(node, 0, &dev_start, &dev_len);
    if ( ret )
    {
        dprintk(XENLOG_ERR, "S32-Gen1: Cannot read device registers address\n");
        return NULL;
    }

    dev_addr = ioremap_nocache(dev_start, PAGE_SIZE);
    if ( !dev_addr )
    {
        dprintk(XENLOG_ERR, "S32-Gen1: Unable to map device registers\n");
        return NULL;
    }

    return dev_addr;
}

static void s32gen1_reset(void)
{
    void __iomem *mc_me, *mc_rgm;

    /* Map MC_ME MMIO region */
    mc_me = s32gen1_map_device("fsl,s32gen1-mc_me");
    if ( !mc_me )
        return;

    /* Map MC_RGM MMIO region */
    mc_rgm = s32gen1_map_device("fsl,s32gen1-rgm");
    if ( !mc_rgm )
        return;

    /* Prevent reset escalation */
    writel(0, mc_rgm + S32GEN1_MC_RGM_DRET);

    /* Destructive reset request */
    writel(S32GEN1_MC_ME_MODE_CONF_DRST, mc_me + S32GEN1_MC_ME_MODE_CONF);
    writel(S32GEN1_MC_ME_MODE_UPD_UPD, mc_me + S32GEN1_MC_ME_MODE_UPD);

    /* Write valid key sequence to trigger the reset */
    writel(S32GEN1_MC_ME_CTL_KEY_KEY, mc_me + S32GEN1_MC_ME_CTL_KEY);
    writel(S32GEN1_MC_ME_CTL_KEY_INVERTEDKEY, mc_me + S32GEN1_MC_ME_CTL_KEY);

    iounmap(mc_me);
    iounmap(mc_rgm);

    panic("Chip Destructive Reset\n");
}

static bool s32gen1_smc(struct cpu_user_regs *regs)
{
    struct arm_smccc_res res;
    uint32_t fid = get_user_reg(regs, 0);

    /* Check for SMCCC 1.1 availability */
    if ( !cpus_have_const_cap(ARM_SMCCC_1_1) )
    {
        printk_once(XENLOG_WARNING
                    "S32-Gen1: No SMCCC 1.1 support, disabling fw calls.");
        return false;
    }

    switch (fid)
    {
    /* SCMI */
    case S32GEN1_SMCCC_FID(S32GEN1_SMC_SCMI_FN):
        goto forward_to_fw;

    default:
        gprintk(XENLOG_WARNING, "S32-Gen1: Unhandled SMC call: %u\n", fid);
        return false;
    }

forward_to_fw:
    arm_smccc_1_1_smc(get_user_reg(regs, 0),
                      get_user_reg(regs, 1),
                      get_user_reg(regs, 2),
                      get_user_reg(regs, 3),
                      get_user_reg(regs, 4),
                      get_user_reg(regs, 5),
                      get_user_reg(regs, 6),
                      get_user_reg(regs, 7),
                      &res);

    set_user_reg(regs, 0, res.a0);
    set_user_reg(regs, 1, res.a1);
    set_user_reg(regs, 2, res.a2);
    set_user_reg(regs, 3, res.a3);
    return true;
}

static const char * const s32gen1_dt_compat[] __initconst =
{
    "nxp,s32g2",
    "nxp,s32g3",
    "nxp,s32r45",
    NULL
};

PLATFORM_START(s32gen1, "NXP S32-Gen1")
    .compatible = s32gen1_dt_compat,
    .smc = s32gen1_smc,
    .reset = s32gen1_reset,
PLATFORM_END

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
