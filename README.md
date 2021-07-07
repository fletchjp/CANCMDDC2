<img align="right" src="arduino_cbus_logo.png"  width="150" height="75">

# CANCMDDC2

This repository holds a version of CANCMDDC which uses the Arduino CBUS library of Duncan Greenwood. This means that it can exchange CBUS events with other modules. These events need to be coded by the user.

I started work on this code in 2019 using as a basis the CANCMDDC code which is in the CANCMDDC-Arduino repository based on MERG-DEV. I wrote an article in the MERG Journal and described it as CANCMDDC2. I did not at that stage release any code.

I am going to use the name CANCMDDC for the code here as that is what it is going to be called in FCU as the original CANCMDDC does not work with CBUS events.

The code now contains a number of significant developments from the version I was using for tests in early 2020. The most important of these is that I have introduced task management to control the actions of different parts of the code.

John Fletcher (M6777)

## Previous work

The problem which I had in restarting this work is that I already had a lot of versions of the code which I have worked with and shared. At that time I was not using GitHub and so the various versions which were shared were not coordinated. Various problems arose and modified versions were also shared.

I have gathered here a number of the versions which already existed. There are also options which I still want to add. These are in various branches which may change from time to time

It is the main branch which is designed to be usable for others, with other branches which incorporate new options. The main branch code will include options for different hardware and I have started to develop documentation for it.

One thing to bear in mind is that at the beginning of 2020 I was attempting to make the code as flexible as possible to allow for different hardware options. I was unable to test anything other than my own hardware before the lockdown in March 2020 made meetings impossible.

I have now made GitHub branches which cover the old codes. New versions will also be in code branches in GitHub.

## Main Branch

This is the place to look for a working version of CANCMDDC.

I need to provide documentation for it.

## Other Branches

The old codes are separate branches.

I have documented these in [BRANCHES](BRANCHES.md).

I have made version-2.9 into old-main as it is the most uptodate of the previous branches. It contains work on task management missing from older code. This will be the version used with H bridge drivers.

version-3.0 is older. It does have some preliminary work on serial connection using RS485.

I am going to make a new branch for the work with the DC controller. It is not there at present. The previous develop branch has been merged into main.

## Problems

I am going to log code problems in [PROBLEMS](PROBLEMS.md).

## Installation and use

I need to put together some instructions for use of this code.

It is designed to be used on a range of different hardware and I will need to explain that.

It requires the installation of a number of Arduino libraries and I need to explain that.

Users will need to decide what events to build into the code. There is a need for a document to describe that.

## Design

CANCMDDC follows the design of the earlier versions built by Dave Radcliffe and others. The purpose is to drive a number of DC powered track sections using CANCAB or JMRI cabs in the same way as can be done for DCC. The sections are numbered in a series and controlled in the same way that a loco would be in DCC. These numbers are allocated in a series e.g 1001, 1002 which can be chosen as part of the configuration.

The output for each section is a pair of wires to power the setion. This could be subject to switching using relays external to the CANCMDDC which has no provision for switching.

The difference between this CANCMDDC and earlier versions is that it can also send and receive CBUS events, separately from the control messages from the CABS. These can be configured as needed by the user. Some examples are provided.

The program runs on an Arduino MEGA. A UNO does not have enough memory or IO pins.

## Acceleration

The existing code has a fixed factor for acceleration and deceleration. I am looking into adding code developed by Paul Miller which allows the CANCAB to change the acceleration setting.

In order to implement this I have to understand more about the codes sent by the CANCAB. This has lead me to the study of the MERG CBUS DCC system which I had not looked at before.

## Equipment

I have two versions of the equipment.

The first one I built in 2019/20 and have taken out to meetings to demonstrate it. It is built on an old model railway baseboard section.

I am building a second one using a Totem frame to hold the components and fit into a plastic box. I am currently sorting out the wiring.


