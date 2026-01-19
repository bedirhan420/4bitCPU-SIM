#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <cstdint>

class GPIO_Unit {
private:
    uint8_t switch_register; 
    uint8_t led_register;    

public:
    GPIO_Unit() : switch_register(0), led_register(0) {}

    void ToggleSwitch(int bitIndex) {
        if (bitIndex >= 0 && bitIndex < 4) {
            switch_register ^= (1 << bitIndex); 
        }
    }

    uint8_t ReadInputPort() {
        return switch_register;
    }

    void WriteOutputPort(uint8_t value) {
        led_register = value & 0x0F;
    }
    
    void Reset() {
        switch_register = 0;
        led_register = 0;
    }

    uint8_t getLEDs() const { return led_register; }
    
    uint8_t getSwitches() const { return switch_register; }
};

#endif