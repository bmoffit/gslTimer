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

  gslTimerInit(10, 0, 500000, 100);

  struct timespec tsWait = {0,150E3};

  for(i = 0; i < n; i++)
    {
      gslTimerStartTime();
      gslTimerRecordTime();//0
      gslTimerRecordTime();//1
      nanosleep(&tsWait, NULL);
      gslTimerRecordTime();//2
      gslTimerRecordTime();//3
      gslTimerEndTime();//4
    }

  gslTimerPrintStats();
  gslTimerFree();

  exit(0);
}

/*
  Local Variables:
  compile-command: "make -k testTimers "
  End:
*/
