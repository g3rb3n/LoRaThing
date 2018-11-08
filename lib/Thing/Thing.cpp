/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example will send Temperature and Humidity
 * using frequency and encryption settings matching those of
 * the The Things Network. Application will 'sleep' 7x8 seconds (56 seconds)
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in config.h.
 *
 *******************************************************************************/

#ifdef BOARD_LORA32U4_II_1_1
    #define DIO0 7
    #define DIO1 6
    #define DIO2 LMIC_UNUSED_PIN
    #define NSS_PIN 8
    #define RESET_PIN  1
#endif
#ifdef BOARD_LORA32U4_II_1_2
    #define DIO0 7
    #define DIO1 1
    #define DIO2 LMIC_UNUSED_PIN
    #define NSS_PIN 8
    #define RESET_PIN 4
#endif
#ifndef DIO1
    #error "Please specify which board version you have by adding it to platform.ini build_flags = -D"
#endif

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "LowPower.h"

#include "util.h"
#include "SX1276.h"
#include "Thing.h"

#define CPU_FREQ  8000000
#define NOP_COUNT 8000000 * 60

const lmic_pinmap lmic_pins =
{
    .nss = NSS_PIN,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = RESET_PIN, // Needed on RFM92/RFM95? (probably not) D0/GPIO16 on v1.1
    .dio = {DIO0, DIO1, DIO2}, // Specify pin numbers for DIO0, DIO1, DIO2
};

using namespace gve;

#define LedPin 22 //13 //22    // pin 13 LED is not used, because it is connected to the SPI port

static void initfunc (osjob_t*);

static osjob_t sendjob;
static osjob_t initjob;


/**********
 *
 * LMIC handler routines
 *
 **********/

void os_getArtEui(u1_t* buf){
    instance->os_getArtEui(buf);
}

void os_getDevEui(u1_t* buf){
    instance->os_getDevEui(buf);
}

void os_getDevKey(u1_t* buf){
    instance->os_getDevKey(buf);
}

void onEvent(ev_t ev){
    instance->onEvent(ev);
}

void do_send(osjob_t* j) {
    Payload payload = instance->readyToSend();
    uint8_t* buffer = payload.buffer;
    uint8_t len = payload.length;
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, (uint8_t*) buffer, len , 0);
        Serial.println(F("Sending: "));
    }
}

// initial job
static void initfunc(osjob_t* j)
{
    // reset MAC state
    LMIC_reset();
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
    // start joining
    LMIC_startJoining();
    // init done - onEvent() callback will be invoked...
}



/**********
 *
 * Thing
 *
 **********/
Thing::Thing(const u1_t* devEUI, const u1_t* appEUI, const u1_t* appKey)
:
    devEUI(devEUI),
    appEUI(appEUI),
    appKey(appKey)
{
    reverse(this->appEUI, 8);
    reverse(this->devEUI, 8);
    instance = this;
    Serial.println("Thing()");
}

Thing::~Thing()
{
}

void Thing::printSettings() const
{
    printArray(this->devEUI, 8);
    printArray(this->appEUI, 8);
    printArray(this->appKey, 16);
}

void Thing::onStateChange(void (*callback)(const String&))
{
    stateChangeCallback = callback;
}

void Thing::onReadyToSend(Payload (*callback)())
{
    sendCallback = callback;
}

void Thing::stateChange(const String& msg)
{
    if (stateChangeCallback) stateChangeCallback(msg);
}

Payload Thing::readyToSend()
{
    if (!sendCallback) return;
    Payload payload = sendCallback();
    return payload;
}

void Thing::os_getArtEui (u1_t* buf) {
    memcpy(buf, this->appEUI, 8);
}

void Thing::os_getDevEui (u1_t* buf) {
    memcpy(buf, this->devEUI, 8);
}

void Thing::os_getDevKey (u1_t* buf) {
    memcpy(buf, this->appKey, 16);
}

void setLed(ev_t event)
{
    unsigned int dev;
    switch (event)
    {
        case EV_SCAN_TIMEOUT: dev = 1000; break;
        case EV_BEACON_FOUND: dev = 1000; break;
        case EV_BEACON_MISSED: dev = 1000; break;
        case EV_BEACON_TRACKED: dev = 1000; break;
        case EV_JOINING: dev = 200; break;
        case EV_JOINED: dev = 1000; break;
        case EV_RFU1: dev = 1000; break;
        case EV_JOIN_FAILED: dev = 1000; break;
        case EV_REJOIN_FAILED: dev = 1000; break;
        case EV_TXCOMPLETE: dev = 1000; break;
        case EV_LOST_TSYNC: dev = 1000; break;
        case EV_RESET: dev = 1000; break;
        case EV_RXCOMPLETE: dev = 1000; break;
        case EV_LINK_DEAD: dev = 1000; break;
        case EV_LINK_ALIVE: dev = 1000; break;
        default: dev = 100; break;
    }
    digitalWrite(LedPin,((millis()/dev) % 2));
}
/*
void setLed(State state){
    unsigned int dev;
    switch (state)
    {
        case JOINING: dev = 1000; break;
        case READY_TO_SEND: dev = 400; break;
        case READY_TO_RECEIVE: dev = 200; break;
        case DATA_RECEIVED: dev = 100; break;
        case SLEEPING: dev = 5000; break;
        default: dev = 50; break;
    }
    digitalWrite(LedPin,((millis()/dev) % 2));
}
*/

char* stateToCString(State state)
{
    switch (state)
    {
        case JOINING: return "joining";
        case READY_TO_SEND: return "sending";
        case READY_TO_RECEIVE: return "receiving";
        case DATA_RECEIVED: return "printing";
        case SLEEPING: return "sleeping";
        default: return "undefined";
    }
}

const char* eventToCString(ev_t ev)
{
    switch (ev)
    {
        case EV_SCAN_TIMEOUT: return "EV_SCAN_TIMEOUT";
        case EV_BEACON_FOUND: return "EV_BEACON_FOUND";
        case EV_BEACON_MISSED: return "EV_BEACON_MISSED";
        case EV_BEACON_TRACKED: return "EV_BEACON_TRACKED";
        case EV_JOINING: return "EV_JOINING";
        case EV_JOINED: return "EV_JOINED";
        case EV_RFU1: return "EV_RFU1";
        case EV_JOIN_FAILED: return "EV_JOIN_FAILED";
        case EV_REJOIN_FAILED: return "EV_REJOIN_FAILED";
        case EV_TXCOMPLETE: return "EV_TXCOMPLETE";
        case EV_LOST_TSYNC: return "EV_LOST_TSYNC";
        case EV_RESET: return "EV_RESET";
        case EV_RXCOMPLETE: return "EV_RXCOMPLETE";
        case EV_LINK_DEAD: return "EV_LINK_DEAD";
        case EV_LINK_ALIVE: return "EV_LINK_ALIVE";
        default: return "Unknown event";
    }
}

void Thing::onEvent (ev_t ev)
{
    Serial.println(eventToCString(ev));
    switch (ev) {
        case EV_SCAN_TIMEOUT: break;
        case EV_BEACON_FOUND: break;
        case EV_BEACON_MISSED:break;
        case EV_BEACON_TRACKED: break;
        case EV_JOINING: break;
        case EV_RFU1: break;
        case EV_JOIN_FAILED: break;
        case EV_LOST_TSYNC: break;
        case EV_RESET: break;
        case EV_RXCOMPLETE: break;
        case EV_LINK_DEAD: break;
        case EV_LINK_ALIVE: break;
        case EV_JOINED:
            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            state = READY_TO_SEND;
            break;
        case EV_REJOIN_FAILED:
            // Re-init
            os_setCallback(&initjob, initfunc);
            break;
        case EV_TXCOMPLETE:
            state = DATA_RECEIVED;
            //if (LMIC.dataLen)
            //    state = DATA_RECEIVED;
            break;
        default: break;
    }
}

void printOpModeMode(uint8_t opmode)
{
    switch (opmode){
        case 0b000: Serial.println("Sleep mode"); break;
        case 0b001: Serial.println("Standby mode"); break;
        case 0b010: Serial.println("FS mode TX"); break;
        case 0b011: Serial.println("Transmitter mode"); break;
        case 0b100: Serial.println("FS mode RX"); break;
        case 0b101: Serial.println("Receiver mode"); break;
        default: Serial.println("Unspecified mode"); break;
    }
}

void printOpMode(uint8_t opmode)
{
    Serial.print("LongRangeMode      ");
    Serial.println(opmode >> 7 & 0x1, HEX);
    Serial.print("ModulationType     ");
    Serial.println(opmode >> 5 & 0x2, HEX);
    Serial.print("LowFrequencyModeOn ");
    Serial.println(opmode >> 3 & 0x1, HEX);
    Serial.print("Mode               ");
    printOpModeMode(opmode >> 0 & 0x3);
    //Serial.println(opmode >> 0 & 0x3, HEX);
}

#define CHANNEL 0
void Thing::setup()
{
    Serial.begin(9600);
    Serial.println(F("Starting"));
    delay(2000);
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    os_setCallback(&initjob, initfunc);
    LMIC_reset();

}

void Thing::disableChannelsExcept(Channel channel)
{
    for (uint8_t i = 0; i < 9; i++)
        LMIC_disableChannel(i);
#if defined(CFG_us915)
    LMIC_enableChannel(channel);
#endif
}

void Thing::printData()
{
    int i = LMIC.frame[LMIC.dataBeg];
//    data = LMIC.frame[LMIC.dataBeg];
    // data received in rx slot after tx
    // if any data received, a LED will blink
    // this number of times, with a maximum of 10
    Serial.println(LMIC.frame[LMIC.dataBeg],HEX);
    // i (0..255) can be used as data for any other application
    // like controlling a relay, showing a display message etc.
    if (i > 10) i = 10;     // maximum number of BLINKs
    for(int j = 0 ; j < i ; ++j)
    {
        digitalWrite(LedPin,HIGH);
        delay(200);
        digitalWrite(LedPin,LOW);
        delay(400);
    }
}

void Thing::printState(State state)
{
    unsigned int now = millis();
    if (now > this->nextPrint)
    {
        SX1276 sx;
        Serial.print("Freq:");
        Serial.print(sx.rfFrequency());
        Serial.print(" Mode:");
        Serial.print(sx.modeString());
        Serial.print(" State:");
        Serial.print(stateToCString(state));
        Serial.println();
        //u1_t v = readReg(RegOpMode);
        //printOpMode(v);
        //Serial.print("Expected 0x12 from SX1276, read=0x");
        //Serial.println(v, HEX);
        nextPrint = now + 1000;
    }
}

void Thing::sleep()
{
    if (this->deepSleepEnabled)
        for (int i = 0 ; i < this->sleepcycles ; i++)
            LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);    //sleep 8 seconds
    else
        for (long i = 0 ; i < 60 ; i++)
            delay(1000);
}

void Thing::handle()
{
    switch(state){
        case JOINING: os_runloop_once(); break;
        case READY_TO_SEND: do_send(&sendjob); state = READY_TO_RECEIVE; break;
        case READY_TO_RECEIVE: os_runloop_once(); break;
        case DATA_RECEIVED: this->printData(); state = SLEEPING; break;
        case SLEEPING: this->sleep(); state = READY_TO_SEND; break;
        default: os_runloop_once(); Serial.println("Undefined state");
    }
    this->printState(state);
    //setLed(state);
}
