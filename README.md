# Simon Says Game

A Simon Says game developed as part of a university project using a custom-built QUTy board featuring an AVR microcontroller.

## Overview

SimonSaysQUTy is a microcontroller-based memory game created for a university assessment. The game runs on the QUTy development board, which includes:

- USB-C for power
- UART communication interface
- UPDI programming interface
- 4 pushbuttons
- Piezo buzzer
- Potentiometer
- 2-digit 7-segment LED display

The project is written in C and demonstrates a range of embedded systems principles including timer control, interrupts, and serial communication.

## Features

- Adjustable difficulty using the on-board potentiometer to control playback speed
- Game sequence output through both the 7-segment LED display and buzzer
- Supports user input via pushbuttons or UART commands
- Real-time score display on the LED interface
- Input debouncing to improve button reliability
- Managed through a finite state machine for structured game logic

## Technologies Used

- Timers and delay functions
- Interrupt vector handling
- UART input/output
- Software debouncing
- State machine architecture

## License

This project was developed as part of a university coursework submission for educational purposes.
