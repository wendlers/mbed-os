/* mbed Microcontroller Library
 * Copyright (c) 2017 ARM Limited
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
#include "mbed.h"
#include "greentea-client/test_env.h"
#include "rtos.h"

#if defined(MBED_RTOS_SINGLE_THREAD)
  #error [NOT_SUPPORTED] test not supported
#endif

typedef struct {
    float    voltage;   /* AD result of measured voltage */
    float    current;   /* AD result of measured current */
    uint32_t counter;   /* A counter value               */
} mail_t;

#define CREATE_VOLTAGE(COUNTER) (COUNTER * 0.1) * 33
#define CREATE_CURRENT(COUNTER) (COUNTER * 0.1) * 11
#define QUEUE_SIZE       16
#define QUEUE_PUT_DELAY  100

#define STACK_SIZE 1024

Mail<mail_t, QUEUE_SIZE> mail_box;

void send_thread () {
    static uint32_t i = 10;
    while (true) {
        i++; // fake data update
        mail_t *mail = mail_box.alloc();
        mail->voltage = CREATE_VOLTAGE(i);
        mail->current = CREATE_CURRENT(i);
        mail->counter = i;
        mail_box.put(mail);
        Thread::wait(QUEUE_PUT_DELAY);
    }
}

int main (void) {
    GREENTEA_SETUP(20, "default_auto");

    Thread thread(osPriorityNormal, STACK_SIZE);
    thread.start(send_thread);
    bool result = true;
    int result_counter = 0;

    while (true) {
        osEvent evt = mail_box.get();
        if (evt.status == osEventMail) {
            mail_t *mail = (mail_t*)evt.value.p;
            const float expected_voltage = CREATE_VOLTAGE(mail->counter);
            const float expected_current = CREATE_CURRENT(mail->counter);
            // Check using macros if received values correspond to values sent via queue
            bool expected_values = (expected_voltage == mail->voltage) &&
                                   (expected_current == mail->current);
            result = result && expected_values;
            const char *result_msg = expected_values ? "OK" : "FAIL";
            printf("%3d %.2fV %.2fA ... [%s]\r\n", mail->counter,
                                                   mail->voltage,
                                                   mail->current,
                                                   result_msg);
            mail_box.free(mail);
            if (result == false || ++result_counter == QUEUE_SIZE) {
                break;
            }
        }
    }
    GREENTEA_TESTSUITE_RESULT(result);
    return 0;
}
