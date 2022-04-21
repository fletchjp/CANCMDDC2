# CANCMDDC 

@mainpage

This is the code for the version of CANCMDDC which can handle CBUS events.

This is a rewrite of the original CANCMDDC code using the Arduino CBUS Library.

This is under development with optional additions of a number of different hardware options.


## TOWNSEND version

Paul Townsend wants a version with two encoders and a keypad  (4 by 3). I need to check with him the keypad size.

I am going to change the encoder to use Martin Da Costa's library. This will involve a change in some pin usage.

For two encoders this should not be a problem.

I now have two encoders working using EncoderEvent for the TOWNSEND option.
