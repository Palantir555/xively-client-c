/* Copyright (c) 2003-2016, LogMeIn, Inc. All rights reserved.
 *
 * This is part of the Xively C Client library,
 * it is licensed under the BSD 3-Clause license.
 */

#include <xi_bsp_time.h>
#include <time.h>

void xi_bsp_time_init()
{
}

xi_time_t xi_bsp_time_getcurrenttime_milliseconds()
{
    return ( xi_time_t )TickConvertToMilliseconds( TickGet() );
}
