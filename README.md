## Overview

This project implements a state machine to manage three primary tasks triggered by a timer and button interrupts. The system performs the following operations:
- Monitors button inputs to adjust the temperature set-point.
- Reads temperature data from a sensor.
- Controls an LED to simulate a heater.
- Reports the current system state through UART output.

## Features

- **Adjustable Temperature Set-Point**: Users can increase or decrease the temperature set-point using button inputs.
- **Temperature Monitoring**: The system reads the current temperature at specified intervals.
- **Heater Control**: An LED represents the heater, turning on or off based on the current state.
- **State Reporting**: Outputs the current state through UART for monitoring.

## State Descriptions

| State            | State Value           | Trigger             |
|-------------------|-----------------------|---------------------|
| **INCREASE_TEMP** | Increase set temp by 1 | Button input        |
| **DECREASE_TEMP** | Decrease set temp by 1 | Button input        |
| **READ_TEMPERATURE** | Reads current temp   | Timer interrupt     |
| **HEAT_ON**       | LED turns on           | GPIO signal (LED ON)|
| **HEAT_OFF**      | LED turns off          | GPIO signal (LED OFF)|


## Task Scheduler

The state machine operates using a task scheduler that coordinates the three tasks:
1. **Temperature Adjustment**: Triggered by button interrupts to adjust the set-point.
2. **Temperature Reading**: Periodically triggered by the timer interrupt.
3. **LED Control**: Responds to the temperature reading to determine whether the heater LED should be ON or OFF.

## System Requirements

- **Hardware**:
    - Buttons for temperature adjustment.
    - Temperature sensor for real-time readings.
    - LED to represent the heater.
    - UART for system state reporting.

- **Software**:
    - Embedded C/C++ code implementing the state machine logic.
    - Timer and GPIO interrupt configuration.

## How to Run

1. Connect the hardware components (buttons, temperature sensor, LED, UART).
2. Load the embedded firmware onto the microcontroller.
3. Monitor system state via UART to observe state transitions.
4. Use buttons to adjust the temperature set-point.

## Future Improvements

- Add support for additional sensor types.
- Implement a graphical user interface (GUI) for monitoring states.
- Add hysteresis to temperature control for smoother operation.

---
