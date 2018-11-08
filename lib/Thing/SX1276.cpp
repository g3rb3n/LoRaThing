
//#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "LowPower.h"

#include "SX1276_registers.h"
#include "SX1276.h"

using namespace gve;

uint8_t SX1276::readReg (uint8_t addr)
{
    hal_pin_nss(0);
    hal_spi(addr & 0x7F);
    u1_t val = hal_spi(0x00);
    hal_pin_nss(1);
    return val;
}

SX1276::SX1276()
{
}

SX1276::~SX1276()
{
}

float SX1276::fStep() const
{
    return static_cast<float>(32000000 >> 19);
//    return 61.0;
}

uint32_t SX1276::rfFrequency() const
{
    uint32_t msb = readReg(RegFrfMsb);
    uint32_t mid = readReg(RegFrfMid);
    uint32_t lsb = readReg(RegFrfLsb);
    uint64_t ret = 0;
    ret += msb << 16;
    ret += mid << 8;
    ret += lsb;
    ret *= 32000000;
    ret >>= 19;
    return ret;// * this->fStep();
}

float SX1276::rfFrequencyMHz() const
{
    return this->rfFrequency() / 1000000;
}

String SX1276::modeString() const
{
    Mode mode = this->mode();
    switch (mode){
        case SleepMode: return String("Sleep");
        case StanbyMode: return String("Standby");
        case FSModeTX: return String("FSTx");
        case TransmitterMode: return String("Transmitter");
        case FSModeRX: return String("FSRx");
        case ReceiverMode: return String("Receiver");
        default: return String("Unspecified");
    }
}

Mode SX1276::mode() const
{
    uint8_t mode = readReg(RegOpMode) & 0x03;
    return static_cast<Mode>(mode);
}
