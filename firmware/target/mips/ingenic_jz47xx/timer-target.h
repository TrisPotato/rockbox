/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2008 by Maurus Cuelenaere
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#ifndef __TIMER_H_
#define __TIMER_H_

#define TIMER_FREQ (27000000)

bool __timer_set(long cycles, bool set);
bool __timer_register(void);
void __timer_unregister(void);

#define __TIMER_SET(cycles, set) \
    __timer_set(cycles, set)

#define __TIMER_REGISTER(reg_prio, unregister_callback, cycles, \
                              int_prio, timer_callback) \
    __timer_register()

#define __TIMER_UNREGISTER(...) \
    __timer_unregister()

#endif /* __TIMER_H_ */
