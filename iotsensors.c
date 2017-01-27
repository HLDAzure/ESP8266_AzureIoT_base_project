// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iotsensors.h"
#include <AzureIotHub.h>

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
    extern IoTSensor* AzureIotDevice;
    AzureIotDevice = CREATE_MODEL_INSTANCE(IoTDevice, IoTSensor);
    AzureIotDevice->DeviceId = "WeMos";
}

void updateSensors() {
  extern IoTSensor* AzureIotDevice;
}
