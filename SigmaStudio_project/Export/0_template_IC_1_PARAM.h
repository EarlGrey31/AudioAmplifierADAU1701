/*
 * File:           C:\Users\PaulH\Documents\rp2040\SigmaDSP\examples\10_Amp_Test\SigmaStudio_project\Export\0_template_IC_1_PARAM.h
 *
 * Created:        Wednesday, December 18, 2024 3:09:18 PM
 * Description:    0_template:IC 1 parameter RAM definitions.
 *
 * This software is distributed in the hope that it will be useful,
 * but is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * This software may only be used to program products purchased from
 * Analog Devices for incorporation by you into audio products that
 * are intended for resale to audio product end users. This software
 * may not be distributed whole or in any part to third parties.
 *
 * Copyright ©2024 Analog Devices, Inc. All rights reserved.
 */
#ifndef __0_TEMPLATE_IC_1_PARAM_H__
#define __0_TEMPLATE_IC_1_PARAM_H__


/* Module DC1 - DC Input Entry*/
#define MOD_DC1_COUNT                                  1
#define MOD_DC1_DEVICE                                 "IC1"
#define MOD_DC1_DCINPALG1_ADDR                         0
#define MOD_DC1_DCINPALG1_FIXPT                        0xFDEADC0D
#define MOD_DC1_DCINPALG1_VALUE                        SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-4.1651595831)
#define MOD_DC1_DCINPALG1_TYPE                         SIGMASTUDIOTYPE_FIXPOINT

/* Module ReadBack1 - DSP Readback*/
#define MOD_READBACK1_COUNT                            2
#define MOD_READBACK1_DEVICE                           "IC1"
#define MOD_READBACK1_ALG0_VAL0_ADDR                   2074
#define MOD_READBACK1_ALG0_VAL0_VALUES                 SIGMASTUDIOTYPE_SPECIAL(0x0042)
#define MOD_READBACK1_ALG0_VAL0_TYPE                   SIGMASTUDIOTYPE_SPECIAL
#define MOD_READBACK1_ALG0_VAL0_READBACK_ADDR          0
#define MOD_READBACK1_ALG0_VAL1_ADDR                   2074
#define MOD_READBACK1_ALG0_VAL1_VALUE                  SIGMASTUDIOTYPE_5_19_CONVERT(0)
#define MOD_READBACK1_ALG0_VAL1_TYPE                   SIGMASTUDIOTYPE_5_19

#endif
