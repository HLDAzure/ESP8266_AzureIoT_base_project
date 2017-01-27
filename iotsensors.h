// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <AzureIotHub.h>

#ifndef IOTSENSORS_H
#define IOTSENSORS_H

// Define the Model
BEGIN_NAMESPACE(IoTDevice);

DECLARE_MODEL(IoTSensor,

#ifdef DUMMYSENSOR
WITH_DATA(int, Reading),
#endif

#ifdef DHT11SENSOR
WITH_DATA(int, Temperature),
WITH_DATA(int, Humidity),
#endif

#ifdef LIGHTSENSOR
WITH_DATA(int, Light),
#endif


WITH_DATA(ascii_char_ptr, DeviceId),
WITH_ACTION(checkForFirmwareUpdate),
WITH_ACTION(SetSomeValue, int, someValue)
);
END_NAMESPACE(IoTDevice);

#ifdef DUMMYSENSOR
#include "dummysensor.h"
#endif
#ifdef DHT11SENSOR
#include "dht11sensor.h"
#endif
#ifdef LIGHTSENSOR
#include "lightsensor.h"
#endif
#ifdef PIRSENSOR
#include "pirsensor.h"
#endif
#ifdef SWITCHSENSOR
#include "switchsensor.h"
#endif

IoTSensor* AzureIotDevice;
void initIotSensors(void);
void sendAzureIoTMessage(IOTHUB_CLIENT_LL_HANDLE, const unsigned char*, size_t);
void sendAzureIoTCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT, void*);
IOTHUBMESSAGE_DISPOSITION_RESULT receiveAzureIoTMessage(IOTHUB_MESSAGE_HANDLE, void*);

EXECUTE_COMMAND_RESULT checkForFirmwareUpdate(IoTSensor* device)
{
    (void)device;
    (void)printf("kicking off ArduinoOTA check.\n");
    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT SetSomeValue(IoTSensor* device, int someValue)
{
    (void)device;
    (void)printf("Setting some setting to %d.\n", someValue);
    return EXECUTE_COMMAND_SUCCESS;
}

void initIotSensors() {
    //extern IoTSensor* AzureIotDevice;
    AzureIotDevice = CREATE_MODEL_INSTANCE(IoTDevice, IoTSensor);
    AzureIotDevice->DeviceId = "WeMos";
}

void updateSensors() {
    //extern IoTSensor* AzureIotDevice;
    extern IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
    IoTHubClient_LL_DoWork(iotHubClientHandle);
    ThreadAPI_Sleep(100);
}

void sendAzureIoTMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size) {
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

void sendAzureIoTCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback) {
    unsigned int messageTrackingId = (unsigned int)(uintptr_t)userContextCallback;
    (void)printf("Message Id: %u Received.\r\n", messageTrackingId);
    (void)printf("Result Call Back Called! Result is: %s \r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}

/*this function "links" IoTHub to the serialization library*/
IOTHUBMESSAGE_DISPOSITION_RESULT receiveAzureIoTMessage(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
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


#endif /* IOTSENSORS_H */
