
#include "Thing.h"
#include "config/app_midwolderbos_test.h"
#include "config/dev_lora32u4ii_1.h"

using namespace gve;

Thing thing(DEVEUI, APPEUI, APPKEY);

void onStateChange(const String& msg)
{
    Serial.println(msg);
}

void onDataReceived(const Payload& payload)
{
    for (uint8_t i = 0 ; i < payload.length ; ++i)
        Serial.print(static_cast<char>(payload.buffer[i]));
    Serial.println();
}

Payload onReadyToSend()
{
    String msg = "Hello world!";
    Payload payload;
    payload.length = msg.length();
    payload.buffer = new uint8_t[payload.length];
    for (uint8_t i = 0 ; i < payload.length ; ++i)
        payload.buffer[i] = msg[i];
    return payload;
}

void setup()
{
    Serial.begin(230400);
    delay(2000);
    Serial.println();

    thing.onStateChange(onStateChange);
    thing.onReadyToSend(onReadyToSend);
    thing.onDataReceived(onDataReceived);
    thing.setup();
    Serial.println("Setup done");
}

void loop()
{
    thing.handle();
}
