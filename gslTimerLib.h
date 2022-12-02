#pragma once
/*
 * Copyright 2022, Jefferson Science Associates, LLC.
 * Subject to the terms in the LICENSE file found in the top-level directory.
 *
 *     Authors: Bryan Moffit
 *              moffit@jlab.org                   Jefferson Lab, MS-12B3
 *              Phone: (757) 269-5660             12000 Jefferson Ave.
 *              Fax:   (757) 269-5800             Newport News, VA 23606
 *
 * Description: Header for Driver Template
 *
 */
#include <stdint.h>
#define MAX_GSL_TIMER 10

int32_t gslTimerInit(uint32_t ntimers, uint32_t min_time, uint32_t max_time, uint32_t bin_size);
int32_t gslTimerReset();
int32_t gslTimerFree();
int32_t gslTimerStartTime();
int32_t gslTimerRecordTime();
int32_t gslTimerPrintStats();
