/*
 * File:
 *    testTimers.c
 *
 * Description:
 *    Test Timers in the gslTimer library
 *
 *
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "gslTimerLib.h"

int
main(int argc, char *argv[])
{
  int i, n=10000;

  gslTimer_t gt;

  gslTimerInit(10, 0, 500000, 100, &gt);

  struct timespec tsWait = {0,150E3};

  for(i = 0; i < n; i++)
    {
      gslTimerStartTime(&gt);
      gslTimerRecordTime(&gt);//0
      gslTimerRecordTime(&gt);//1
      nanosleep(&tsWait, NULL);
      gslTimerRecordTime(&gt);//2
      gslTimerRecordTime(&gt);//3
      gslTimerEndTime(&gt);//4
    }

  gslTimerPrintStats(&gt);
  gslTimerFree(&gt);

  exit(0);
}

/*
  Local Variables:
  compile-command: "make -k testTimers "
  End:
*/
