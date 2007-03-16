/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 * Tuner "middleware" for Philips TEA5767 chip
 *
 * Copyright (C) 2004 J�rg Hohensohn
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include "config.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "kernel.h"
#include "tuner.h" /* tuner abstraction interface */
#include "fmradio_i2c.h" /* physical interface driver */

#define I2C_ADR 0xC0
static unsigned char write_bytes[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };

/* tuner abstraction layer: set something to the tuner */
int philips_set(int setting, int value)
{
    switch(setting)
    {
        case RADIO_SLEEP:
            /* init values */
            write_bytes[0] |= (1<<7); /* mute */
#if CONFIG_TUNER_XTAL == 32768
            /* 32.768kHz, soft mute, stereo noise cancelling */
            write_bytes[3] |= (1<<4) | (1<<3) | (1<<1);
#else
            /* soft mute, stereo noise cancelling */
            write_bytes[3] |= (1<<3) | (1<<1); 
#endif
            /* sleep / standby mode */
            write_bytes[3] &= ~(1<<6) | (value ? (1<<6) : 0);
            break;

        case RADIO_FREQUENCY:
            {
                int n;
#if CONFIG_TUNER_XTAL == 32768
                n = (4 * (value - 225000) + 16384) / 32768;
#else
                n = (4 * (value - 225000)) / 50000;
#endif
                write_bytes[0] = (write_bytes[0] & 0xC0) | (n >> 8);
                write_bytes[1] = n & 0xFF;
            }
            break;

        case RADIO_SCAN_FREQUENCY:
            philips_set(RADIO_FREQUENCY, value);
            sleep(HZ/30);
            return philips_get(RADIO_TUNED);

        case RADIO_MUTE:
            write_bytes[0] = (write_bytes[0] & 0x7F) | (value ? 0x80 : 0);
            break;

        case RADIO_FORCE_MONO:
            write_bytes[2] = (write_bytes[2] & 0xF7) | (value ? 0x08 : 0);
            break;

        case RADIO_SET_DEEMPHASIS:
            write_bytes[4] = (write_bytes[4] & ~(1<<6)) | (value ? (1<<6) : 0);
            break;

        case RADIO_SET_BAND:
            write_bytes[3] = (write_bytes[3] & ~(1<<5)) | (value ? (1<<5) : 0);
        default:
            return -1;
    }
    fmradio_i2c_write(I2C_ADR, write_bytes, sizeof(write_bytes));
    return 1;
}

/* tuner abstraction layer: read something from the tuner */
int philips_get(int setting)
{
    unsigned char read_bytes[5];
    int val = -1; /* default for unsupported query */

    fmradio_i2c_read(I2C_ADR, read_bytes, sizeof(read_bytes));

    switch(setting)
    {
        case RADIO_PRESENT:
            val = 1; /* true */
            break;

        case RADIO_TUNED:
            val = 0;
            if (read_bytes[0] & 0x80) /* ready */
            {
                val = read_bytes[2] & 0x7F; /* IF counter */
                val = (abs(val - 0x36) < 2); /* close match */
            }
            break;

        case RADIO_STEREO:
            val = read_bytes[2] >> 7;
            break;
    }
    return val;
}

void philips_dbg_info(struct philips_dbg_info *info)
{
    fmradio_i2c_read(I2C_ADR, info->read_regs, 5);
    memcpy(info->write_regs, write_bytes, 5);
}
