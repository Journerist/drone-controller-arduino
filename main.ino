#include <SPI.h>
#include <string.h>
#include "DroneWebserver.h"
#include "MotionService.h"
#include "DistanceService.h"
//DroneWebserver.ino does get included automatically by arduino IDE

void setup()
{
    Serial.begin(9600);
    delay(1000);
    setupMotionSensors();
    delay(1000);
    setupDroneWebserver();
    delay(1000);
    setupDistanceSensors();
    delay(1000);
}

int i = 0;

void loop()
{
    // handle http requests with a low priority
    if (i % 1000000 == 0)
    {
        handleWebClients();
        readMotionSensors();
        readDistanceSensors();
    }

    i++;
}
