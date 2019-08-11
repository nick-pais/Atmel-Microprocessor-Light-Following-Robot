# Atmel-Light-Following-Robot-Project
Using an 8-bit Atmel Microprocessor I created a robot that measures light and will drive toward it. The system is controlled by 3 CDS sensors that measure light and use the values to triangulate the direction of travel. The robot also comes equipped with snap-action switches to act as bumpers for collision detection.

## Required Microprocessor Peripherials
  1. ADC System
     * Used for taking in voltage from the CDS cells, through a wheatstone bridge configuration.
  2. I/O Ports
     * Used to take in data from snap-action switches.
  3. PWM System
     * Used to control motor speed. 
     
 ## Required Hardware
  1. 2 Servo Motors
     * Used for driving the robot.
  2. TB66FNG Motor Controller
     * Used for controlling the motors speed and direction from the PWM signal.
  3. CDS Cells
     * Cadmium-Sulfide Cells that change there resistance based on light intensity.
  4. Snap-Action Switches
     * Detects bumps and sends signal to microcontroller.
