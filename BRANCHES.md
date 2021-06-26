
# BRANCHES

Documentation of the branches of CANCMDDC2.

This is branch develop

This originates from CANCMDDC_V2.9 of 22/01/2021

CANCMDDC_V2a Beta 9
Ideas for using IO Abstraction library for task scheduling.

CANCMDDC Version 4a Beta 1

I have added the code to work as a CBUS module without buttons.

This is a version for my old hardware. This code needs more work to be consistent.

I am now using Io-abstraction for task management.

https://www.thecoderscorner.com/products/arduino-libraries/io-abstraction/

The only other example working is CANTEXT2
I will also use Martin Da Costa's modifications e.g. no defs.h file.
First version put together for testing.
At the moment I have no way to turn off the buzzer if a power alarm happens.

CANCMDDC Version 4a Beta 2

I have made more changes to use the setupCBUS routine as in CANmINnOUT. This needs to be tested.

This code works with H bridges.

I have tested this code on the old hardware and some of it works. I cannot get it to register with FCU at the moment. Some things are incomplete.

The version running on the hardware before was 2.5.

