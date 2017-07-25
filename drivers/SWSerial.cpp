/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "drivers/SWSerial.h"
#include "platform/mbed_wait_api.h"
#include "platform/mbed_critical.h"
#include "platform/FileHandle.h"
#include <cstdarg>
#include <errno.h>

#if DEVICE_SERIAL

#define STRING_STACK_LIMIT    120
#define START                   0
#define STOP                    1

namespace mbed {

SWSerial::SWSerial(PinName tx, PinName rx, PinName triggerPin, int baud) : _tx(tx, 1), _baud(baud), _trigger(triggerPin) {
    _tx = 1;
    _bit_rate_us = (int)((((float)1)/_baud)*1e6);
}

ssize_t SWSerial::read(void *buffer, size_t size) {
    return -EINVAL; // Not supported yet
}

ssize_t SWSerial::write(const void *buffer, size_t size) {
    core_util_critical_section_enter();

    uint8_t index = 0;
    uint8_t bit_index = 0;
    uint8_t mask = 0x01; 
    uint8_t state;
    const char *c_buff = static_cast<const char *>(buffer);

    for(size_t i=0; i < size*10; i++) {
        while(!_trigger.read()) {}
        //_t.start();
        // UART packet state machine
        switch(bit_index) {
            case 0:    // START
                state = START;
                bit_index++;
                break;
            case 9:    // STOP
                state = STOP;
                bit_index = 0;
                break;
            default:   // construct data segment of UART packet
                state = ((c_buff[index] & mask) > 0) ? 1 : 0;
                mask <<= 1;
                bit_index++;
                if(mask == 0) {
                    mask = 0x01;
                    index++;
                }
                break;
        }     
        _tx = state;
        
        while(_trigger.read()) {}
        //while(_t.read_us() < _bit_rate_us) {} 
        //_t.reset();
    }
    
    core_util_critical_section_exit();
    return size;
}

off_t SWSerial::seek(off_t offset, int whence) {
    return -EINVAL; // Not supported yet
}

int SWSerial::close() {
    return -EINVAL; // Not supported yet
}

} // namespace mbed

#endif
