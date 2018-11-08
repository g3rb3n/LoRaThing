#ifndef _Thing_H
#define _Thing_H

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <LowPower.h>
#include <lmic.h>
#include <SPI.h>

#include "LoRaWan.h"

namespace gve
{
    class Thing;
    static Thing* instance;

    struct Payload
    {
        uint8_t* buffer;
        uint8_t length;
    };

    enum State
    {
        UNDEFINED,
        JOINING,
        JOINING_DONE,
        READY_TO_SEND,
        SENDING,
        SENDING_DONE,
        READY_TO_RECEIVE,
        RECEIVING,
        RECEIVING_DONE,
        SLEEPING,
        SLEEPING_DONE
    };

    class Thing
    {
    public:

    private:
        const u1_t* devEUI;
        const u1_t* appEUI;
        const u1_t* appKey;

        bool deepSleepEnabled = false;
        bool receiveAfterTransmit = true;
        int sleepCycles = 1;  // every sleepcycle will last 8 secs, total sleeptime will be sleepcycles * 8 sec
        unsigned long receiveTimeout;
        unsigned int nextPrint = 0;
        State state = JOINING;
        State lastState = UNDEFINED;
        uint8_t sleepCycleCount = 0;
        ev_t lastEvent;

        //void callback(char* callbackTopic, uint8_t* buffer, unsigned int length);

        void (*stateChangeCallback)(const String&);
        void stateChange(const String& msg);

        Payload (*sendCallback)();

        void dataReceived();
        void (*onDataReceivedCallback)(const Payload&);

        void printState(State state);
        void sleep();
        void printData();



    public:
        Thing(const u1_t* devEUI, const u1_t* appEUI, const u1_t* appKey);
        ~Thing();
        void setup();
        void handle();
        void disableChannelsExcept(Channel channel);

        void onStateChange(void (*callback)(const String&));
        void onReadyToSend(Payload (*callback)());
        void onDataReceived(void (*callback)(const Payload&));

        Payload readyToSend();

        void printSettings() const;

        void onEvent (ev_t ev);
        void os_getArtEui (u1_t* buf);
        void os_getDevEui (u1_t* buf);
        void os_getDevKey (u1_t* buf);
    };

}
#endif
