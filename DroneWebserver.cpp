#include "DroneWebserver.h"
#include "Config.h"
#include <SPI.h>

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setupDroneWebserver()
{
    //Initialize serial and wait for port to open:
    Serial.begin(9600);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    // check for the presence of the shield:
    if (WiFi.status() == WL_NO_SHIELD)
    {
        Serial.println("[ERROR] WiFi shield not present");
        // don't continue:
        while (true)
            ;
    }

    String fv = WiFi.firmwareVersion();

    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);

        // wait 10 seconds for connection:
        delay(10000);
    }
    server.begin();
    // you're connected now, so print out the status:
    printWifiStatus();
}

void handleWebClients()
{
    // listen for incoming clients
    WiFiClient client = server.available();
    if (client)
    {
        String request = getRequestHeaderAndSendSomeHtml(client);
        int paramSepIndex = request.indexOf("GET /?");

        if (paramSepIndex != -1)
        {
            MotorSpeedUpdates motorSpeedUpdates = parseMotorUpdates(request);
            executeMotorUpdate(&motorSpeedUpdates);
        }
    }
}

MotorSpeedUpdates parseMotorUpdates(String request)
{
    int paramSepIndex = request.indexOf("GET /?");

    int firstParamSep = request.indexOf("&");
    int secondParamSep = request.indexOf("&", firstParamSep + 1);
    int thirdParamSep = request.indexOf("&", secondParamSep + 1);
    int fourthParamSep = request.indexOf("&", thirdParamSep + 1);

    int pathLineBreakIndex = request.indexOf('\n', firstParamSep);

    String firstParam = request.substring(paramSepIndex + 1, firstParamSep);
    String secondParam = request.substring(firstParamSep + 1, secondParamSep);
    String thirdParam = request.substring(secondParamSep + 1, thirdParamSep);
    String fourthParam = request.substring(thirdParamSep + 1, pathLineBreakIndex);

    int firstParamSplitEqualsSign = firstParam.indexOf("=");
    int secondParamSplitEqualsSign = secondParam.indexOf("=");
    int thirdParamSplitEqualsSign = thirdParam.indexOf("=");
    int fourthParamSplitEqualsSign = fourthParam.indexOf("=");

    String firstParamName = firstParam.substring(0, firstParamSplitEqualsSign);
    String firstParamValue = removeLineBreaks(firstParam.substring(firstParamSplitEqualsSign + 1));
    String secondParamName = secondParam.substring(0, secondParamSplitEqualsSign);
    String secondParamValue = removeLineBreaks(secondParam.substring(secondParamSplitEqualsSign + 1));
    String thirdParamName = thirdParam.substring(0, thirdParamSplitEqualsSign);
    String thirdParamValue = removeLineBreaks(thirdParam.substring(thirdParamSplitEqualsSign + 1));
    String fourthParamName = fourthParam.substring(0, fourthParamSplitEqualsSign);
    String fourthParamValue = removeLineBreaks(fourthParam.substring(fourthParamSplitEqualsSign + 1));

    MotorSpeedUpdates motorSpeedUpdates;
    motorSpeedUpdates.motor0 = -1;
    motorSpeedUpdates.motor1 = -1;
    motorSpeedUpdates.motor2 = -1;
    motorSpeedUpdates.motor3 = -1;

    parseMotorUpdate(firstParamName, firstParamValue, &motorSpeedUpdates);
    parseMotorUpdate(secondParamName, secondParamValue, &motorSpeedUpdates);
    parseMotorUpdate(thirdParamName, thirdParamValue, &motorSpeedUpdates);
    parseMotorUpdate(fourthParamName, fourthParamValue, &motorSpeedUpdates);
    
    return motorSpeedUpdates;
}

String getRequestHeaderAndSendSomeHtml(WiFiClient client)
{
    String request = "";

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
        if (client.available())
        {
            char c = client.read();

            // if you've gotten to the end of the line (received a newline
            // character) and the line is blank, the http request has ended,
            // so you can send a reply
            if (c == '\n' && currentLineIsBlank)
            {
                // send a standard http response header
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html");
                client.println("Connection: close"); // the connection will be closed after completion of the response
                client.println("Refresh: 5");        // refresh the page automatically every 5 sec
                client.println();
                client.println("<!DOCTYPE HTML>");
                client.println("<html>");
                // output the value of each analog input pin
                for (int analogChannel = 0; analogChannel < 6; analogChannel++)
                {
                    int sensorReading = analogRead(analogChannel);
                    client.print("analog input ");
                    client.print(analogChannel);
                    client.print(" is ");
                    client.print(sensorReading);
                    client.println("<br />");
                }
                client.println("</html>");
                break;
            }
            if (c == '\n')
            {
                // you're starting a new line
                currentLineIsBlank = true;
            }
            else if (c != '\r')
            {
                // you've gotten a character on the current line
                currentLineIsBlank = false;
            }

            request += c;
        }
    }
    // give the web browser time to receive the data
    // delay(1);

    // close the connection:
    client.stop();

    return request;
}

String removeLineBreaks(String value)
{
    int breakLineIndex = value.indexOf('\n');
    value = value.substring(0, breakLineIndex);
    return value;
}

void executeMotorUpdate(struct MotorSpeedUpdates *motorSpeedUpdates)
{
    if (motorSpeedUpdates->motor0 != -1)
    {
        Serial.println("Update speed of motor 0: " + String(motorSpeedUpdates->motor0));
    }
    if (motorSpeedUpdates->motor1 != -1)
    {
        Serial.println("Update speed of motor 1: " + String(motorSpeedUpdates->motor1));
    }
    if (motorSpeedUpdates->motor2 != -1)
    {
        Serial.println("Update speed of motor 2: " + String(motorSpeedUpdates->motor2));
    }
    if (motorSpeedUpdates->motor3 != -1)
    {
        Serial.println("Update speed of motor 3: " + String(motorSpeedUpdates->motor3));
    }
}

void parseMotorUpdate(String paramName, String paramValue, struct MotorSpeedUpdates *motorSpeedUpdates)
{

    Serial.println("paramName: " + paramName);
    Serial.println("paramValue: " + paramValue);

    int motorIndex = paramName.substring(paramName.length() - 1).toInt();
    int motorSpeed = paramValue.toInt();

    if (motorIndex == 0)
    {
        motorSpeedUpdates->motor0 = motorSpeed;
    }
    else if (motorIndex == 1)
    {
        motorSpeedUpdates->motor1 = motorSpeed;
    }
    else if (motorIndex == 2)
    {
        motorSpeedUpdates->motor2 = motorSpeed;
    }
    else if (motorIndex == 3)
    {
        motorSpeedUpdates->motor3 = motorSpeed;
    }
    else
    {
        Serial.println("[ERROR] Invalid motor index: " + String(motorIndex));
    }
}

void printWifiStatus()
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}