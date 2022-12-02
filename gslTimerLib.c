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
 * @file      gslTimerLib.c
 * @brief     Library for measuring processing times, using gsl
 *
 */
#include <stdio.h>
#include "gslTimerLib.h"

#define gtERROR(format, ...) {fprintf(stderr,"%s: ERROR: ",__func__); fprintf(stderr,format, ## __VA_ARGS__);}
#define gtDEBUG(format, ...) {if(gtHandle->Debug==1) {fprintf(stdout,"%s: DEBUG:",__func__); fprintf(stdout,format, ## __VA_ARGS__);}}

/**
 * @brief Initialize gslTimer_t handle
 * @details Initialize gslTimer_t handle created by user with timer parameters
 * @param[in] ntimers Maximum timers to allocate
 * @param[in] min_time Minimum value of time
 * @param[in] max_time Maximum value of time
 * @param[in] bin_size Size of time bins
 * @param[out] gtHandle gslTimer_t handle to be initialized
 * @return 0 if successful, otherwise -1
 */
int32_t
gslTimerInit(uint32_t ntimers, uint32_t min_time, uint32_t max_time, uint32_t bin_size, gslTimer_t *gtHandle)
{
  if(ntimers > MAX_GSL_TIMER)
    {
      gtERROR("Invalid ntimers (%d).  Max = %d\n", ntimers, MAX_GSL_TIMER);
      return -1;
    }

  /* Initialize gtHandle */
  gtHandle->currentTimer = 0;
  gtHandle->maxTimerUsed = 0;
  gtHandle->NHist = 0;
  gtHandle->Debug = 0;

  /* histogram stuff */
  uint32_t ihist, nbins;

  nbins = (max_time - min_time) / bin_size;

  for(ihist = 0; ihist < ntimers; ihist++)
    {
      gtHandle->Hist[ihist] = gsl_histogram_alloc (nbins);
      gsl_histogram_set_ranges_uniform (gtHandle->Hist[ihist], min_time, max_time);
      gsl_histogram_reset(gtHandle->Hist[ihist]);
      gtHandle->NHist++;
    }


  gtHandle->TotalHist = gsl_histogram_alloc (nbins);
  gsl_histogram_set_ranges_uniform (gtHandle->TotalHist, min_time, max_time);
  gsl_histogram_reset(gtHandle->TotalHist);

  return 0;
}

/**
 * @brief Reset the timer histograms
 * @details Reset the all timer histograms and set current timer to 0
 * @param[inout] gtHandle gslTimer_t handle
 * @return 0 if successful, otherwise -1
 */
int32_t
gslTimerReset(gslTimer_t *gtHandle)
{
  uint32_t ihist;

  for(ihist = 0; ihist < gtHandle->NHist; ihist++)
    {
      gsl_histogram_reset(gtHandle->Hist[ihist]);
    }
  gsl_histogram_reset(gtHandle->TotalHist);

  gtHandle->currentTimer = 0;

  return 0;
}

/**
 * @brief Free all timer histograms
 * @details Free all timer histograms
 * @param[inout] gtHandle gslTimer_t handle
 * @return 0 if successful, otherwise -1
 */
int32_t
gslTimerFree(gslTimer_t *gtHandle)
{
  uint32_t ihist;

  for(ihist = 0; ihist < gtHandle->NHist; ihist++)
    {
      gsl_histogram_free(gtHandle->Hist[ihist]);
    }
  gsl_histogram_free(gtHandle->TotalHist);

  return 0;
}

/**
 * @brief Mark the start time of the instance
 * @details Save the current time as the starting time for the instance
 * @param[inout] gtHandle gslTimer_t handle
 * @return 0 if successful, otherwise -1
 */
int32_t
gslTimerStartTime(gslTimer_t *gtHandle)
{
  int32_t rval = 0;

  /* Check if there's a previous measurement */
  if(gtHandle->currentTimer > 0)
    {
      int32_t diff_total = gtHandle->tsPrevious.tv_nsec - gtHandle->tsStart.tv_nsec +
	1000000000 * (gtHandle->tsPrevious.tv_sec - gtHandle->tsStart.tv_sec);

      rval = gsl_histogram_increment (gtHandle->TotalHist, diff_total);

      gtDEBUG(" tot: rval = %d, diff_total = %d\n",
	      rval, diff_total);

    }

  rval = clock_gettime(CLOCK_MONOTONIC, &gtHandle->tsStart);

  gtHandle->tsPrevious.tv_sec = gtHandle->tsStart.tv_sec;
  gtHandle->tsPrevious.tv_nsec = gtHandle->tsStart.tv_nsec;
  gtHandle->currentTimer = 0;

  return rval;
}

/**
 * @brief Mark the end time of the instance
 * @details Save the current time as the end time for the instance
 * @param[inout] gtHandle gslTimer_t handle
 * @return 0 if successful, otherwise -1
 */
int32_t
gslTimerEndTime(gslTimer_t *gtHandle)
{
  int32_t rval = 0;

  gslTimerRecordTime(gtHandle);
  /* Check if there's a previous measurement */
  if(gtHandle->currentTimer > 0)
    {
      int32_t diff_total = gtHandle->tsPrevious.tv_nsec - gtHandle->tsStart.tv_nsec +
	1000000000 * (gtHandle->tsPrevious.tv_sec - gtHandle->tsStart.tv_sec);

      rval = gsl_histogram_increment (gtHandle->TotalHist, diff_total);

      gtDEBUG(" tot: rval = %d, diff_total = %d\n",
	      rval, diff_total);

      gtHandle->currentTimer = 0;
    }

  return 0;
}

/**
 * @brief Mark the current time of the instance
 * @details Use the current time to save the time difference from the start time for this instance.
 * @param[inout] gtHandle gslTimer_t handle
 * @return 0 if successful, otherwise -1
 */
int32_t
gslTimerRecordTime(gslTimer_t *gtHandle)
{
  int32_t rval = 0;
  int32_t diff_previous = 0;

  struct timespec tsCurrent;
  rval = clock_gettime(CLOCK_MONOTONIC, &tsCurrent);

  /* only fill the timer's we've allocated */
  if(gtHandle->currentTimer < gtHandle->NHist)
    {
      diff_previous = tsCurrent.tv_nsec - gtHandle->tsPrevious.tv_nsec +
	1000000000 * (tsCurrent.tv_sec - gtHandle->tsPrevious.tv_sec);

      rval = gsl_histogram_increment (gtHandle->Hist[gtHandle->currentTimer], diff_previous);
      gtDEBUG("%3d: rval = %d, diff_previous = %d\n",
	      gtHandle->currentTimer, rval, diff_previous);
      gtHandle->maxTimerUsed = gtHandle->currentTimer;
      gtHandle->currentTimer++;
    }

  gtHandle->tsPrevious = tsCurrent;

  return rval;
}

/**
 * @brief Print Summary of timers
 * @details Print to standard out, the statistcs of the histograms for the provided gslTimer_t handle.
 * @param[inout] gtHandle gslTimer_t handle
 * @return 0 if successful, otherwise -1
 */
int32_t
gslTimerPrintStats(gslTimer_t *gtHandle)
{
  uint32_t ihist;

  printf(" Allocated Histograms = %d   Used = %d\n",
	 gtHandle->NHist, gtHandle->maxTimerUsed + 1);

  for(ihist = 0; ihist <= gtHandle->maxTimerUsed; ihist++)
    {
      printf("%2d: n = %.1f   mean = %4.1f  sigma = %4.1f  \n",
	     ihist,  gsl_histogram_sum(gtHandle->Hist[ihist]),
	     gsl_histogram_mean(gtHandle->Hist[ihist]),
	     gsl_histogram_sigma(gtHandle->Hist[ihist]));
    }

  printf("TOTAL:\n    n = %.1f   mean = %4.1f  sigma = %4.1f  \n",
	 gsl_histogram_sum(gtHandle->TotalHist),
	 gsl_histogram_mean(gtHandle->TotalHist),
	 gsl_histogram_sigma(gtHandle->TotalHist));

  return 0;
}
