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
#ifndef MBED_SW_SERIAL_H
#define MBED_SW_SERIAL_H

//#include "platform/platform.h"

#if defined (DEVICE_SERIAL) || defined(DOXYGEN_ONLY)

#include "hal/serial_api.h"
#include "platform/FileHandle.h"
#include "drivers/DigitalOut.h"
#include "drivers/Timer.h"
#include "drivers/InterruptIn.h"

namespace mbed {
/** \addtogroup drivers */

/** A serial port (UART) for communication with other serial devices
 * This is a variation of the Serial class that doesn't use streams,
 * thus making it safe to use in interrupt handlers with the RTOS.
 *
 * Can be used for Full Duplex communication, or Simplex by specifying
 * one pin as NC (Not Connected)
 *
 * @note Synchronization level: Not protected
 *
 * Example:
 * @code
 * // Send a char to the PC
 *
 * #include "mbed.h"
 *
 * SWSerial pc(USBTX, USBRX);
 *
 * int main() {
 *     pc.putc('A');
 * }
 * @endcode
 * @ingroup drivers
 */
class SWSerial: public FileHandle {

public:
    /** Create a SWSerial port, connected to the specified transmit and receive pins, with the specified baud.
     *
     *  @param tx Transmit pin
     *  @param rx Receive pin
     *  @param baud The baud rate of the serial port (optional, defaults to MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE)
     *
     *  @note
     *    Either tx or rx may be specified as NC if unused
     */
    SWSerial(PinName tx, PinName rx, PinName triggerPin, int baud=9600);

    ssize_t read(void *buffer, size_t size);
    ssize_t write(const void *buffer, size_t size);
    off_t seek(off_t offset, int whence);
    int close();
private:
    Timer _t;
    DigitalOut _tx;    
    InterruptIn _trigger;
    int _baud;
    int _bit_rate_us;
};
} // namespace mbed

#endif

#endif
