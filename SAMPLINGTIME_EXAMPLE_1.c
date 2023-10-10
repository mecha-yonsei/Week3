/* SAMPLINGTIME EXAMPLE 1*/

#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>


int main(void)
{
    wiringPiSetupGpio();
    unsigned int startTime = millis();

    unsigned int checkTimeBefore = millis();
    unsigned int checkTime;

    int cnt = 0;

    while (cnt < 100)
    {
        checkTime = millis();
        cnt++;
        printf("loop time: %d msec, After init: %d msec\n",
            checkTime - checkTimeBefore,
            checkTime - startTime);
        checkTimeBefore = checkTime;
        delay(5);
    }
    return 0;
}
