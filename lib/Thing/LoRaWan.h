#ifndef LoRaWan_h
#define LoRaWan_h

enum SpreadingFactor
{
    SpreadingFactor6 = 6,
    SpreadingFactor7,
    SpreadingFactor8,
    SpreadingFactor9,
    SpreadingFactor10,
    SpreadingFactor11,
    SpreadingFactor12
};

enum Frequency {
    Frequency0 = 868100000,    // Frequency 0, 868.1 MHz primary
    Frequency1 = 868300000,    // Frequency 1, 868.3 MHz mandatory
    Frequency2 = 868500000,    // Frequency 2, 868.5 MHz mandatory
    Frequency3 = 867100000,    // Frequency 3, 867.1 MHz optional
    Frequency4 = 867300000,
    Frequency5 = 867500000,
    Frequency6 = 867700000,
    Frequency7 = 867900000,
    Frequency8 = 868800000,
    Frequency9 = 869525000,    // Frequency, for responses gateway (10%)
    // TTN defines an additional channel at 869.525Mhz using SF9 for class B. Not used
};

enum Channel {
    Channel0,
    Channel1,
    Channel2,
    Channel3,
    Channel4,
    Channel5,
    Channel6,
    Channel7,
    Channel8,
    Channel9,
};

#endif
