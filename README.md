# LoRaThing
A c++ wrapper around LMIC and LowPower to make LoRa implementations easy.

## Example
```
#include "Thing.h"

using namespace gve;

static const u1_t DEVEUI[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const u1_t APPEUI[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const u1_t APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

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
```

## ToDo
- Fix missing delete
- Use static buffer?
