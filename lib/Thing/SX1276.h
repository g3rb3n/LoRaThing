#ifndef _SX1276_H
#define _SX1276_H

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "LowPower.h"


namespace gve
{
    enum Mode {
        SleepMode,
        StanbyMode,
        FSModeTX,
        TransmitterMode,
        FSModeRX,
        ReceiverMode
    };

    class SX1276
    {
    public:

    private:



    public:
        SX1276();
        ~SX1276();

        uint32_t rfFrequency() const;
        float rfFrequencyMHz() const;

        Mode mode() const;
        String modeString() const;

        float fStep() const;

        uint8_t readReg (uint8_t addr);

    };

}
#endif
