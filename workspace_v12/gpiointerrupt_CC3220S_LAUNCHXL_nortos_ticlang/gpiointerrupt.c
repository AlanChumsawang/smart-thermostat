/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== gpiointerrupt.c ========
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* TI Driver Header Files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

/* Definitions */
#define DISPLAY(x) UART_write(uartHandle, &output, x)
#define EVENT_NUM 3
#define TIMER_PERIOD 100
#define BUTTON_PERIOD 200
#define TEMP_PERIOD 500
#define UPDATE_PERIOD 1000


enum BUTTON_STATES {INCREASE_TEMP, DECREASE_TEMP, BUTTON_INIT} BUTTON_STATE;
enum TEMPERATURE_SENSOR_STATES {READ_TEMPERATURE, TEMPERATURE_SENSOR_INIT};                
enum HEATING_STATES {HEAT_OFF, HEAT_ON, HEAT_INIT};  


I2C_Handle i2cHandle;         
Timer_Handle timer;    
UART_Handle uartHandle;

char output[64];
int bytesToSend;


static const struct
{
    uint8_t address;
    uint8_t resultReg;
    char *id;
}
sensors[3] = {
    { 0x48, 0x0000, "11X" },
    { 0x49, 0x0000, "116" },
    { 0x41, 0x0001, "006" }
};
uint8_t txBuffer[1];
uint8_t rxBuffer[2];
I2C_Transaction i2cTransaction;

volatile unsigned char TimerFlag = 0;
int16_t currentTemp = 0;
int16_t setPoint = 20;
int seconds = 0;


typedef struct task {
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*tickFunction)(int);
} task;

//Callback

void timerCallback(Timer_Handle myHandle, int_fast16_t status){TimerFlag = 1;}
void gpioIncreaseTemperatureCallback(uint_least8_t index){ BUTTON_STATE = INCREASE_TEMP; }
void gpioDecreaseTemperatureCallback(uint_least8_t index){BUTTON_STATE = DECREASE_TEMP;}



void initUART(void)
{
    UART_Params uartParams;

    // Init the driver
    UART_init();

    // Configure the driver
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.baudRate = 115200;

    // Open the driver
    uartHandle = UART_open(CONFIG_UART_0, &uartParams);
    if (uartHandle == NULL)
    {
        /* UART_open() failed */
        while (1);
    }
}

void initI2C(void)
{
    int8_t i, found;
    I2C_Params i2cParams;

    DISPLAY(snprintf(output, 64, "Initializing I2C Driver - "));

    // Init the driver
    I2C_init();

    // Configure the driver
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    // Open the driver
    i2cHandle = I2C_open(CONFIG_I2C_0, &i2cParams);
    if (i2cHandle == NULL)
    {
        DISPLAY(snprintf(output, 64, "Failed\n\r"));
        while (1);
    }

    DISPLAY(snprintf(output, 32, "Passed\n\r"));

    // Boards were shipped with different sensors.
    // Welcome to the world of embedded systems.
    // Try to determine which sensor we have.
    // Scan through the possible sensor addresses.

    /* Common I2C transaction setup */
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 0;

    found = false;
    for (i=0; i<3; ++i)
    {
         i2cTransaction.slaveAddress = sensors[i].address;
         txBuffer[0] = sensors[i].resultReg;

         DISPLAY(snprintf(output, 64, "Is this %s? ", sensors[i].id));
         if (I2C_transfer(i2cHandle, &i2cTransaction))
         {
             DISPLAY(snprintf(output, 64, "Found\n\r"));
             found = true;
             break;
         }
         DISPLAY(snprintf(output, 64, "No\n\r"));
    }

    if(found)
    {
        DISPLAY(snprintf(output, 64, "Detected TMP%s I2C address: %x\n\r", sensors[i].id, i2cTransaction.slaveAddress));
    }
    else
    {
        DISPLAY(snprintf(output, 64, "Temperature sensor not found, contact professor\n\r"));
    }
}

void initGPIO(void) {
    GPIO_init();

    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);

    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioIncreaseTemperatureCallback);
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);

    #if defined(CONFIG_GPIO_BUTTON_1)
        GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
        GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioDecreaseTemperatureCallback);
        GPIO_enableInt(CONFIG_GPIO_BUTTON_1);
    #endif

    BUTTON_STATE = BUTTON_INIT;

    }


void initTimer(void)
{
    Timer_Params params;

    // Init the driver
    Timer_init();

    // Configure the driver
    Timer_Params_init(&params);
    params.period = 100000;                         // Set period to 1/10th of 1 second.
    params.periodUnits = Timer_PERIOD_US;           // Period specified in micro seconds
    params.timerMode = Timer_CONTINUOUS_CALLBACK;   // Timer runs continuously.
    params.timerCallback = timerCallback;           // Calls timerCallback method for timer callback.

    // Open the driver
    timer = Timer_open(CONFIG_TIMER_0, &params);
    if (timer == NULL)
    {
        /* Failed to initialized timer */
        while (1) {}
    }
    if (Timer_start(timer) == Timer_STATUS_ERROR)
    {
        /* Failed to start timer */
        while (1) {}
    }
}


int changeTemp(int state)
{
    switch (state)
    {
        case INCREASE_TEMP:
            if (setPoint < 99)
            {
                setPoint++;
            }
            BUTTON_STATE = BUTTON_INIT;
            break;
        case DECREASE_TEMP:
            if (setPoint > 0)
            {
                setPoint--;
            }
            BUTTON_STATE = BUTTON_INIT;
            break;
    }
    state = BUTTON_STATE;

    return state;
}


int16_t readTemp(void)
{
    int16_t temperature = 0;
    i2cTransaction.readCount = 2;
    if (I2C_transfer(i2cHandle, &i2cTransaction))
    {
        /*
        * Extract degrees C from the received data;
        * see TMP sensor datasheet
        */
        temperature = (rxBuffer[0] << 8) | (rxBuffer[1]); temperature *= 0.0078125;
        /*
        * If the MSB is set '1', then we have a 2's complement * negative value which needs to be sign extended
        */
        if (rxBuffer[0] & 0x80)
        {
            temperature |= 0xF000;
        }
    }
    else
    {
        DISPLAY(snprintf(output, 64, "Error reading temperature sensor (%d)\n\r",i2cTransaction.status));
        DISPLAY(snprintf(output, 64, "Please power cycle your board by unplugging USB and plugging back in.\n\r"));
    }
    return temperature;
}


int getTemp(int state)
{
    switch (state)
    {
        case TEMPERATURE_SENSOR_INIT:
            state = READ_TEMPERATURE;
            break;
        case READ_TEMPERATURE:
            currentTemp = readTemp();
            break;
    }

    return state;
}


int heatOnOff(int state)
{
    if (seconds != 0)
    {

        if (currentTemp < setPoint)
        {
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
            state = HEAT_ON;
        }
        else
        {
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
            state = HEAT_OFF;
        }

        DISPLAY(snprintf(output,
                             64,
                             "<%02d,%02d,%d,%04d>\n\r",
                             currentTemp,
                             setPoint,
                             state,
                             seconds));
    }

    seconds++;

    return state;
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    task tasks[EVENT_NUM] = {
        {BUTTON_INIT, BUTTON_PERIOD, BUTTON_PERIOD, &changeTemp},   //changeTemp
        {TEMPERATURE_SENSOR_INIT, TEMP_PERIOD, TEMP_PERIOD, &getTemp},  //readTemp
        {HEAT_INIT, UPDATE_PERIOD, UPDATE_PERIOD, &heatOnOff}   //heatOnOff
    };

    // Initialize drivers
    initUART(); initI2C(); initGPIO(); initTimer();

    // Main loop
    while (1)
    {
        for (unsigned int i = 0; i < EVENT_NUM; ++i)
            if ((tasks[i].elapsedTime += TIMER_PERIOD) >= tasks[i].period) {
                tasks[i].state = tasks[i].tickFunction(tasks[i].state);
                tasks[i].elapsedTime = 0;
            }

        while (!TimerFlag);
        TimerFlag = 0;
    }
    return NULL;

}
