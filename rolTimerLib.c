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
#include "rolTimerLib.h"

gsl_histogram *rolTimerHist[MAX_ROL_TIMER];
gsl_histogram *rolTimerTotalHist;
struct timespec tsStart, tsPrevious;
uint32_t currentTimer = 0;
uint32_t maxTimerUsed = 0;
uint32_t rolTimerNHist = 0;

#define rtERROR(format, ...) {fprintf(stderr,"%s: ERROR: ",__FUNCTION__); fprintf(stdout,format, ## __VA_ARGS__);}

int32_t
rolTimerInit(uint32_t ntimers, uint32_t min_time, uint32_t max_time, uint32_t bin_size)
{
  if(ntimers > MAX_ROL_TIMER)
    {
      rtERROR("Invalid ntimers (%d).  Max = %d\n", ntimers, MAX_ROL_TIMER);
      return -1;
    }

  /* histogram stuff */
  uint32_t ihist, nbins;

  nbins = (max_time - min_time) / bin_size;

  for(ihist = 0; ihist < ntimers; ihist++)
    {
      rolTimerHist[ihist] = gsl_histogram_alloc (nbins);
      gsl_histogram_set_ranges_uniform (rolTimerHist[ihist], min_time, max_time);
      gsl_histogram_reset(rolTimerHist[ihist]);
      rolTimerNHist++;
    }


  rolTimerTotalHist = gsl_histogram_alloc (nbins);
  gsl_histogram_set_ranges_uniform (rolTimerTotalHist, min_time, max_time);
  gsl_histogram_reset(rolTimerTotalHist);


  return 0;
}

int32_t
rolTimerReset()
{
  uint32_t ihist;

  for(ihist = 0; ihist < rolTimerNHist; ihist++)
    {
      gsl_histogram_reset(rolTimerHist[ihist]);
    }
  gsl_histogram_reset(rolTimerTotalHist);

  currentTimer = 0;

  return 0;
}

int32_t
rolTimerFree()
{
  uint32_t ihist;

  for(ihist = 0; ihist < rolTimerNHist; ihist++)
    {
      gsl_histogram_free(rolTimerHist[ihist]);
    }
  gsl_histogram_free(rolTimerTotalHist);

  return 0;
}

int32_t
rolTimerStartTime()
{
  int32_t rval = 0;
  rval = clock_gettime(CLOCK_MONOTONIC, &tsStart);

  tsPrevious = tsStart;
  currentTimer = 0;

  return rval;
}

int32_t
rolTimerRecordTime()
{
  int32_t rval = 0;
  int diff_total, diff_previous;

  struct timespec tsCurrent;
  rval = clock_gettime(CLOCK_MONOTONIC, &tsCurrent);

  diff_total = tsStart.tv_nsec - tsCurrent.tv_nsec +  1000000000 * (tsStart.tv_sec - tsCurrent.tv_sec);
  diff_previous = tsPrevious.tv_nsec - tsCurrent.tv_nsec +  1000000000 * (tsPrevious.tv_sec - tsCurrent.tv_sec);

  rval = gsl_histogram_increment (rolTimerTotalHist, diff_total);

  /* only fill the timer's we've allocated */
  if(currentTimer < rolTimerNHist)
    {
      rval = gsl_histogram_increment (rolTimerHist[currentTimer], diff_previous);
      maxTimerUsed = currentTimer;
      currentTimer++;
    }

  tsPrevious = tsCurrent;

  return rval;
}

int32_t
rolTimerPrintStats()
{
  uint32_t ihist;

  printf(" Allocated Histograms = %d   Used = %d\n",
	 rolTimerNHist, maxTimerUsed);

  for(ihist = 0; ihist < maxTimerUsed; ihist++)
    {
      printf("%2d: n = %4u   mean = %4.1f  sigma = %4.1f  \n",
	     ihist,  (uint32_t)rolTimerHist[ihist]->n,
	     gsl_histogram_mean(rolTimerHist[ihist]),
	     gsl_histogram_sigma(rolTimerHist[ihist]));
    }

  printf("TOTAL:\n      n = %4u   mean = %4.1f  sigma = %4.1f  \n",
	 (uint32_t)rolTimerHist[ihist]->n,
	 gsl_histogram_mean(rolTimerHist[ihist]),
	 gsl_histogram_sigma(rolTimerHist[ihist]));

  return 0;
}
