/* SAMPLINGTIME EXAMPLE 2*/

#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

int main(void)
{
    wiringPiSetupGpio();
    unsigned int startTime = millis();

    unsigned int checkTimeBefore = millis();
    unsigned int checkTime;

    while (1)
    {
        checkTime = millis();
        if (checkTime - checkTimeBefore >= 5)	// 5 msec
        {
            printf("loop time: %d msec, After init: %d msec\n",
                checkTime - checkTimeBefore,
                checkTime - startTime);
            checkTimeBefore = checkTime;
        }
    }
    return 0;
}
