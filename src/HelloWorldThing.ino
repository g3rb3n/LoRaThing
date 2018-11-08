
#include "Thing.h"
#include "config/app_midwolderbos_test.h"
#include "config/dev_lora32u4ii_1.h"

using namespace gve;

Thing thing(DEVEUI, APPEUI, APPKEY);

int count = 0;

void onStateChange(const String& msg){
    Serial.println(msg);
}

Payload getSendPayload(){
    Payload payload;
    payload.buffer = new uint8_t[12];
    payload.buffer[0] = 'H';
    payload.buffer[1] = 'e';
    payload.buffer[2] = 'l';
    payload.buffer[3] = 'l';
    payload.buffer[4] = 'o';
    payload.buffer[5] = ' ';
    payload.buffer[6] = 'w';
    payload.buffer[7] = 'o';
    payload.buffer[8] = 'r';
    payload.buffer[9] = 'l';
    payload.buffer[10] = 'd';
    payload.buffer[11] = '!';
    payload.length = 12;
    return payload;
}

void setup()
{
    Serial.begin(230400);
    delay(2000);
    Serial.println();
    thing.printSettings();
    Serial.println("Set callback");
    thing.onStateChange(onStateChange);
    thing.onReadyToSend(getSendPayload);
    Serial.println("Setup");
    thing.setup();
    Serial.println("Setup done");
}

void loop()
{
    thing.handle();
}
