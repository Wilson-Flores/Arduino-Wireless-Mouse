# Arduino-Wireless-Mouse
Using the NRF24 transceiver, the following code will transmit data from a Teensy 3.5 to a Arduino Pro-Micro. 
The Teensy 3.5 gets input data from the optical sensor, a P2S joystick and 3 buttons, 
which controls the left and right mouse clicks as well as having a button for switching between modes. 
The modes are between using the optical sensor and the joystick. 

Inside the OpticalDriver folder contains 3 seperate arduino codes, 
one is for a modified version of mrjohnk's pmw3366 code following the necessary ROM file.

This Transmitter code is modified to a Teensy 3.5, which i have changed mrjohnk's code so the sensor would be in a seperate SPI port.
The NRF24 is used to transmit the data wirelessly to a Arduino Pro Micro which will use the RecieverCode file.

This was for my Senior Design Project. We attempted to create a wireless wearable mouse that allowed you to switch between using the sensor
with your hand flat on the table or using the joystick so you could operate the keyboard with 2 hands without having the buttons and sensor
from interfering. 

