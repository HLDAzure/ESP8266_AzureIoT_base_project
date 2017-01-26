

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <SPI.h>

#include <ESP8266WiFi.h>

#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 
#include <ArduinoOTA.h>

#include <AzureIoTHub.h>
#include <AzureIoTHubClient.h>
#include <AzureIoTUtility.h>
#include <AzureIoTProtocol_MQTT.h>
#include <AzureIoTProtocol_HTTP.h>


static WiFiClientSecure sslClient; // for ESP8266
static AzureIoTHubClient iotHubClient;
IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

/*String containing Hostname, Device Id & Device Key in the format:             */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"    */
static const char* connectionString = "HostName=MQTTIoT.azure-devices.net;DeviceId=WeMos;SharedAccessKey=ypn7H369RPpYEuhmwAnOLs1FGxMoNh+uaMyJr+KgnHs=";

void setup() {
    initSerial();
    initWifi();
    initTime();

    initAzureIoT(HTTP_Protocol);
}

void loop() {
  IoTHubClient_LL_DoWork(iotHubClientHandle);
  ThreadAPI_Sleep(100);
}

void initSerial() {
    // Start serial and initialize stdout
    Serial.begin(115200);
    Serial.setDebugOutput(true);
}

void initWifi() {

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();

        //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("AutoConnectAP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    Serial.println("Connected to wifi");
}

void initTime() {
    time_t epochTime;
    int retries = 0;
    
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    while (retries < 30) {
        epochTime = time(NULL);

        if (epochTime == 0) {
            Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
            delay(2000);
            retries++;
        } else {
            Serial.print("Fetched NTP epoch time is: ");
            Serial.println(epochTime);
            break;
        }
    }
}

void initAzureIoT(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol) {
    iotHubClient.begin(sslClient);
    serializer_init(NULL);
    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, protocol);
    if (iotHubClientHandle == NULL) {
      Serial.println("couldn't connect to Azure IoT Hub");
    } else {
      const char* message = "device connected";
      IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveAzureIoTMessage, NULL);
      //IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveAzureIoTMessage, myWeather);
      sendAzureIoTMessage(iotHubClientHandle, (const unsigned char*)message, sizeof(message));
    }  
}

static void sendAzureIoTMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size) {
    static unsigned int messageTrackingId;
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
    if (messageHandle == NULL) {
        printf("unable to create a new IoTHubMessage\r\n");
    } else {
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendAzureIoTCallback, (void*)(uintptr_t)messageTrackingId) != IOTHUB_CLIENT_OK) {
            printf("failed to hand over the message to IoTHubClient");
        } else {
            printf("IoTHubClient accepted the message for delivery\r\n");
        }
        IoTHubMessage_Destroy(messageHandle);
    }
    free((void*)buffer);
    messageTrackingId++;
}

static void sendAzureIoTCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback) {
    unsigned int messageTrackingId = (unsigned int)(uintptr_t)userContextCallback;
    (void)printf("Message Id: %u Received.\r\n", messageTrackingId);
    (void)printf("Result Call Back Called! Result is: %s \r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}

/*this function "links" IoTHub to the serialization library*/
static IOTHUBMESSAGE_DISPOSITION_RESULT receiveAzureIoTMessage(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    IOTHUBMESSAGE_DISPOSITION_RESULT result;
    const unsigned char* buffer;
    size_t size;
    if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        printf("unable to IoTHubMessage_GetByteArray\r\n");
        result = IOTHUBMESSAGE_ABANDONED;
    }
    else
    {
        /*buffer is not zero terminated*/
        char* temp = (char*) malloc(size + 1);
        if (temp == NULL)
        {
            printf("failed to malloc\r\n");
            result = IOTHUBMESSAGE_ABANDONED;
        }
        else
        {
            memcpy(temp, buffer, size);
            temp[size] = '\0';
            printf(temp);
            return IOTHUBMESSAGE_ACCEPTED;
            EXECUTE_COMMAND_RESULT executeCommandResult = EXECUTE_COMMAND(userContextCallback, temp);
            result =
                (executeCommandResult == EXECUTE_COMMAND_ERROR) ? IOTHUBMESSAGE_ABANDONED :
                (executeCommandResult == EXECUTE_COMMAND_SUCCESS) ? IOTHUBMESSAGE_ACCEPTED :
                IOTHUBMESSAGE_REJECTED;
            free(temp);
        }
    }
    return result;
}