#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

#define LOOPTIME 5		// Sampling Time
#define ENCODERA 17		// Hall Sensor A
#define ENCODERB 27		// Hall Sensor B
#define ENC2REDGEAR 216

#define MOTOR1 19
#define MOTOR2 26
#define PGAIN 10

int encA;
int encB;
int encoderPosition = 0;
float redGearPosition = 0;

float referencePosition = 10;
float errorPosition = 0;

unsigned int checkTime;
unsigned int checkTimeBefore;

void funcEncoderA()
{
    encA = digitalRead(ENCODERA);
    encB = digitalRead(ENCODERB);
    if (encA == HIGH)
    {
        if (encB == LOW) encoderPosition++;
        else encoderPosition--;
    }
    else
    {
        if (encB == LOW) encoderPosition--;
        else encoderPosition++;
    }
    redGearPosition = (float)encoderPosition / ENC2REDGEAR;
    errorPosition = referencePosition - redGearPosition;
    printf("refPos: %f gearPos: %f  err: %f\n",
        referencePosition, redGearPosition, errorPosition);
}

void funcEncoderB()
{
    encA = digitalRead(ENCODERA);
    encB = digitalRead(ENCODERB);
    if (encB == HIGH)
    {
        if (encA == LOW) encoderPosition--;
        else encoderPosition++;
    }
    else
    {
        if (encA == LOW) encoderPosition++;
        else encoderPosition--;
    }
    redGearPosition = (float)encoderPosition / ENC2REDGEAR;
    errorPosition = referencePosition - redGearPosition;
    printf("refPos: %f gearPos: %f  err: %f\n",
        referencePosition, redGearPosition, errorPosition);
}

int main(void)
{
    wiringPiSetupGpio();
    pinMode(ENCODERA, INPUT);		// Set ENCODERA as input
    pinMode(ENCODERB, INPUT);		// Set ENCODERB as input

    softPwmCreate(MOTOR1, 0, 100);		// Create soft Pwm
    softPwmCreate(MOTOR2, 0, 100); 	// Create soft Pwm

    wiringPiISR(ENCODERA, INT_EDGE_BOTH, funcEncoderA);
    wiringPiISR(ENCODERB, INT_EDGE_BOTH, funcEncoderB);

    errorPosition = referencePosition - redGearPosition;
    checkTimeBefore = millis();
    while (1)
    {
        checkTime = millis();
        if (checkTime - checkTimeBefore > LOOPTIME)
        {
            if (errorPosition > 0)
            {
                softPwmWrite(MOTOR1, errorPosition * PGAIN);
                softPwmWrite(MOTOR2, 0);
            }
            else
            {
                softPwmWrite(MOTOR2, -errorPosition * PGAIN);
                softPwmWrite(MOTOR1, 0);
            }
            checkTimeBefore = checkTime;
        }
    }
    return 0;
}
