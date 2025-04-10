# LED-Goals: Goal-Setting Corkboard

LED-Goals is a personal project designed to help you track and visualize your long-term goals. It uses an LED strip to track the progress you make toward your goals over the course of a month.

I designed a custom PCB to connect the button, LED strip, and Arduino Nano together.

## Hardware
- **PCB**: Custom-designed PCB to hold the necessary components.
- **LED Strip**: NeoPixel LED strip to visually represent your progress.
- **Button**: A button to increment progress and reset the LED strip when the goal is achieved.
- **Arduino Nano**: An Arduino Nano to control the LEDs and button logic.
- **9V Battery**: The system is powered by a 9V D-cell battery, making it portable.

## Software
The Arduino code controls the NeoPixel strip and button logic. It gradually fills the LED strip based on button presses or time, and resets when the goal is complete.

### Key Functions:
- **Incrementing LEDs**: Each press of the button adds one more LED to the strip.
- **Resetting LEDs**: When the strip is filled, pressing the button resets the strip back to 0 LEDs.

## Installation
1. **Hardware Setup**: Solder the components to the custom PCB, ensuring the button and LED strip are connected to the appropriate pins on the Arduino Nano.
2. **Software**: Download and install the Arduino IDE. Open the provided Arduino sketch, upload it to your Arduino Nano, and you're ready to start tracking your goals!

## Usage
Once powered on, the LEDs will gradually light up over the course of a month. Pressing the push-button will manually light up an addtional LED. When all LEDs are lit, pressing the button again will reset the strip.
