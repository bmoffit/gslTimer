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
 * @file      gslTimerLib.h
 * @brief     Header for Library for measuring processing times, using gsl
 *
 */
#include <stdint.h>
#include <time.h>
#include <gsl/gsl_histogram.h>
#define MAX_GSL_TIMER 10

typedef struct
{
  gsl_histogram *Hist[MAX_GSL_TIMER];
  gsl_histogram *TotalHist;
  uint32_t NHist;
  struct timespec tsStart, tsPrevious;
  uint32_t currentTimer;
  uint32_t maxTimerUsed;
  int32_t Debug;
} gslTimer_t;

int32_t gslTimerInit(uint32_t ntimers, uint32_t min_time, uint32_t max_time, uint32_t bin_size, gslTimer_t *gtHandle);
int32_t gslTimerReset(gslTimer_t *gtHandle);
int32_t gslTimerFree(gslTimer_t *gtHandle);
int32_t gslTimerStartTime(gslTimer_t *gtHandle);
int32_t gslTimerRecordTime(gslTimer_t *gtHandle);
int32_t gslTimerPrintStats(gslTimer_t *gtHandle);
