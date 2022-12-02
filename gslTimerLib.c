/**
 * @copyright Copyright 2022, Jefferson Science Associates, LLC.
 *            Subject to the terms in the LICENSE file found in the
 *            top-level directory.
 *
 * @author    Bryan Moffit
 *            moffit@jlab.org                   Jefferson Lab, MS-12B3
 *            Phone: (757) 269-5660             12000 Jefferson Ave.
 *            Fax:   (757) 269-5800             Newport News, VA 23606
 *
 * @file      templateLib.c
 * @brief     Library Template for generic use
 *
 */
#include <time.h>
#include <stdio.h>
#include <gsl/gsl_histogram.h>
#include "gslTimerLib.h"

gsl_histogram *gslTimerHist[MAX_GSL_TIMER];
gsl_histogram *gslTimerTotalHist;
struct timespec tsStart, tsPrevious;
uint32_t currentTimer = 0;
uint32_t maxTimerUsed = 0;
uint32_t gslTimerNHist = 0;

#define rtERROR(format, ...) {fprintf(stderr,"%s: ERROR: ",__FUNCTION__); fprintf(stdout,format, ## __VA_ARGS__);}

int32_t
gslTimerInit(uint32_t ntimers, uint32_t min_time, uint32_t max_time, uint32_t bin_size)
{
  if(ntimers > MAX_GSL_TIMER)
    {
      rtERROR("Invalid ntimers (%d).  Max = %d\n", ntimers, MAX_GSL_TIMER);
      return -1;
    }

  /* histogram stuff */
  uint32_t ihist, nbins;

  nbins = (max_time - min_time) / bin_size;

  for(ihist = 0; ihist < ntimers; ihist++)
    {
      gslTimerHist[ihist] = gsl_histogram_alloc (nbins);
      gsl_histogram_set_ranges_uniform (gslTimerHist[ihist], min_time, max_time);
      gsl_histogram_reset(gslTimerHist[ihist]);
      gslTimerNHist++;
    }


  gslTimerTotalHist = gsl_histogram_alloc (nbins);
  gsl_histogram_set_ranges_uniform (gslTimerTotalHist, min_time, max_time);
  gsl_histogram_reset(gslTimerTotalHist);


  return 0;
}

int32_t
gslTimerReset()
{
  uint32_t ihist;

  for(ihist = 0; ihist < gslTimerNHist; ihist++)
    {
      gsl_histogram_reset(gslTimerHist[ihist]);
    }
  gsl_histogram_reset(gslTimerTotalHist);

  currentTimer = 0;

  return 0;
}

int32_t
gslTimerFree()
{
  uint32_t ihist;

  for(ihist = 0; ihist < gslTimerNHist; ihist++)
    {
      gsl_histogram_free(gslTimerHist[ihist]);
    }
  gsl_histogram_free(gslTimerTotalHist);

  return 0;
}

int32_t
gslTimerStartTime()
{
  int32_t rval = 0;
  rval = clock_gettime(CLOCK_MONOTONIC, &tsStart);

  tsPrevious = tsStart;
  currentTimer = 0;

  return rval;
}

int32_t
gslTimerRecordTime()
{
  int32_t rval = 0;
  int diff_total, diff_previous;

  struct timespec tsCurrent;
  rval = clock_gettime(CLOCK_MONOTONIC, &tsCurrent);

  diff_total = tsStart.tv_nsec - tsCurrent.tv_nsec +  1000000000 * (tsStart.tv_sec - tsCurrent.tv_sec);
  diff_previous = tsPrevious.tv_nsec - tsCurrent.tv_nsec +  1000000000 * (tsPrevious.tv_sec - tsCurrent.tv_sec);

  rval = gsl_histogram_increment (gslTimerTotalHist, diff_total);

  /* only fill the timer's we've allocated */
  if(currentTimer < gslTimerNHist)
    {
      rval = gsl_histogram_increment (gslTimerHist[currentTimer], diff_previous);
      maxTimerUsed = currentTimer;
      currentTimer++;
    }

  tsPrevious = tsCurrent;

  return rval;
}

int32_t
gslTimerPrintStats()
{
  uint32_t ihist;

  printf(" Allocated Histograms = %d   Used = %d\n",
	 gslTimerNHist, maxTimerUsed);

  for(ihist = 0; ihist < maxTimerUsed; ihist++)
    {
      printf("%2d: n = %4u   mean = %4.1f  sigma = %4.1f  \n",
	     ihist,  (uint32_t)gslTimerHist[ihist]->n,
	     gsl_histogram_mean(gslTimerHist[ihist]),
	     gsl_histogram_sigma(gslTimerHist[ihist]));
    }

  printf("TOTAL:\n      n = %4u   mean = %4.1f  sigma = %4.1f  \n",
	 (uint32_t)gslTimerHist[ihist]->n,
	 gsl_histogram_mean(gslTimerHist[ihist]),
	 gsl_histogram_sigma(gslTimerHist[ihist]));

  return 0;
}
