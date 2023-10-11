/* DATA_LOGGING EXAMPLE*/

#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

#define LOOPTIME 5		
#define ENCODERA 17		
#define ENCODERB 27		
#define ENC2REDGEAR 216

#define MOTOR1 19
#define MOTOR2 26
#define PGAIN 10

#define NUM_ROWS 3000
#define NUM_COLUMNS 2
#define DAQ_TIME 10000  // 10s

int encA;
int encB;
int encoderPosition = 0;
float redGearPosition = 0;
float referencePosition = 10;
float errorPosition = 0;

unsigned int checkTime;
unsigned int checkTimeBefore;
unsigned int startTime;

int dataIndex = 0;
float dataArray[NUM_ROWS][NUM_COLUMNS];

void updateDataArray()
{
    dataArray[dataIndex][0] = (float)(checkTime - startTime) / 1000.0;
    dataArray[dataIndex][1] = redGearPosition;
    dataIndex++;
}

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
}

int main(void)
{
    char filename[100];
    FILE* file;

    printf("Enter the file name: ");
    scanf("%s", filename);
    
    file = fopen(strcat(filename,".csv"), "w+");
    
    wiringPiSetupGpio();
    pinMode(ENCODERA, INPUT);
    pinMode(ENCODERB, INPUT);
    softPwmCreate(MOTOR1, 0, 100);
    softPwmCreate(MOTOR2, 0, 100);
    wiringPiISR(ENCODERA, INT_EDGE_BOTH, funcEncoderA);
    wiringPiISR(ENCODERB, INT_EDGE_BOTH, funcEncoderB);

    errorPosition = referencePosition - redGearPosition;
    startTime = millis();
    checkTimeBefore = millis();
    while (millis() - startTime < DAQ_TIME)
    {
        checkTime = millis();
        if (checkTime - checkTimeBefore >= LOOPTIME)
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
            updateDataArray();
            checkTimeBefore = checkTime;
        }
    }

    for (int i = 0; i < dataIndex; i++)
    {
        fprintf(file, "%.3f,%.3f\n", dataArray[i][0], dataArray[i][1]);
    }
    fclose(file);
    return 0;
}

