#include <Arduino.h>
#include <lmic.h>  // For u1_t

void reverse( void *start, int size )
{
    u1_t *lo = start;
    u1_t *hi = start + size - 1;
    u1_t swap;
    while (lo < hi) {
        swap = *lo;
        *lo++ = *hi;
        *hi-- = swap;
    }
}

void printArray(u1_t* arr, unsigned int len)
{
    for (unsigned int i = 0 ; i < len ; ++i)
    {
        if (arr[i] < 16) Serial.print("0");
        Serial.print(arr[i], HEX);
    }
    Serial.println();
}
