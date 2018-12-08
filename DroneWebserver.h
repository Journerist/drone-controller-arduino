#pragma once
#include <SPI.h>
#include <string.h>
#include <WiFiNINA.h>

struct MotorSpeedUpdates
{
    int motor0;
    int motor1;
    int motor2;
    int motor3;
};

// public
void handleWebClients();
void setupDroneWebserver();

// private 
String removeLineBreaks(String value);
void executeMotorUpdate(struct MotorSpeedUpdates *motorSpeedUpdates);
void parseMotorUpdate(String paramName, String paramValue, struct MotorSpeedUpdates *motorSpeedUpdates);
void printWifiStatus();
String getRequestHeaderAndSendSomeHtml(WiFiClient client);
MotorSpeedUpdates parseMotorUpdates(String request);
