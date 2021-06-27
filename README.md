<img align="right" src="arduino_cbus_logo.png"  width="150" height="75">

# CANCMDDC2

This repository is to hold the version of CANCMDDC which uses the Arduino CBUS library of Duncan Greenwood.

I started work on this in 2019 using as a basis the CANCMDDC code which is in the CANCMDDC-Arduino repository based on MERG-DEV. I have publicised this in the Journal and described it as CANCMDDC2.

I am going to use the name CANCMDDC for the actual code here as that is what it is going to be called in FCU as the original CANCMDDC does not work with CBUS events.

The problem which I have starting this work now is that I already have a lot of versions of the code which I have worked with and shared.

There have been several stages where problems have been located and there are also options which I still want to add. My current aim is to have a main branch which is designed to be usable for others, with other branches which incorporate new options.

One thing to bear in mind is that at the beginning of 2020 I was attempting to make the code as flexible as possible to allow for different hardware options. I was unable to test anything other than my own hardware before the lockdown in March 2020 made meetings impossible.

I think that now I will attempt to cover options in a different way with code branches in GitHub.

John Fletcher (M6777)

## Main Branch

This is the place to look for a working version of CANCMDDC.

## Other Branches

The old codes are separate branches.

I will document these in [BRANCHES](BRANCHES.md).

I have made version-2.9 into old-main as it is the most uptodate of the previous branches. It contains work on task management missing from older code. This will be the version used with H bridge drivers.

version-3.0 is older. It does have some preliminary work on serial connection using RS485.

I am going to make a new branch for the work with the DC controller. It is not there at present. The previous develop branch has been merged into main.

## Problems

I am going to log code problems in [PROBLEMS](PROBLEMS.md).

## Installation and use

I need to put together some instructions for use of this code.

It is designed to be used on a range of different hardware and I will need to explain that.

It requires the installation of a number of Arduino libraries and I need to explain that.

## Design

CANCMDDC follows the design of the earlier versions built by Dave Radcliffe and others. The purpose is to drive a number of DC powered track sections using CANCAB or JMRI cabs in the same way as can be done for DCC. The sections are numbered in a series and controlled in the same way that a loco would be in DCC. These numbers are allocated in a series e.g 1001, 1002 which can be chosen as part of the configuration.

The output for each section is a pair of wires to power the setion. This could be subject to switching using relays external to the CANCMDDC which has no provision for switching.

The difference between this CANCMDDC and earlier versions is that it can also send and receive CBUS events, separately from the control messages from the CABS. These can be configured as needed by the user. Some examples are provided.

The program runs on an Arduino MEGA.

