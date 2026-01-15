/*
 * xen/arch/arm/platforms/s32.c
 *
 * NXP S32 Platform-specific settings
 *
 * Andrei Cherechesu <andrei.cherechesu@nxp.com>
 * Copyright 2021-2023, 2026 NXP
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

#include <asm/platform.h>
#include <asm/regs.h>
#include <asm/smccc.h>

#define S32_SMC_SCMI_FN                 0xFE
#define S32_SMCCC_FID(fn) ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, \
                                               ARM_SMCCC_CONV_64,   \
                                               ARM_SMCCC_OWNER_SIP, \
                                               fn)

static bool s32_smc(struct cpu_user_regs *regs)
{
    struct arm_smccc_res res;
    uint32_t fid = get_user_reg(regs, 0);

    /* Check for SMCCC 1.1 availability */
    if ( !cpus_have_const_cap(ARM_SMCCC_1_1) )
    {
        printk_once(XENLOG_WARNING
                    "S32: No SMCCC 1.1 support, disabling fw calls.");
        return false;
    }

    switch (fid)
    {
    /* SCMI */
    case S32_SMCCC_FID(S32_SMC_SCMI_FN):
        goto forward_to_fw;

    default:
        gprintk(XENLOG_WARNING, "S32: Unhandled SMC call: %u\n", fid);
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

static const char * const s32_dt_compat[] __initconst =
{
    "nxp,s32g2",
    "nxp,s32g3",
    "nxp,s32r45",
    "nxp,s32n79",
    NULL
};

PLATFORM_START(s32, "NXP S32")
    .compatible = s32_dt_compat,
    .smc = s32_smc,
PLATFORM_END

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
