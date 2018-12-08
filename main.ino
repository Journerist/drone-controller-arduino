#include <SPI.h>
#include <string.h>
#include "DroneWebserver.h"
//DroneWebserver.ino does get included automatically by arduino IDE

void setup()
{
    setupDroneWebserver();
}

int i = 0;

void loop()
{
    // handle http requests with a low priority
    if (i % 1000000 == 0)
    {
        handleWebClients();
    }

    i++;
}
