# Problems

## AddSessionConsist

I have this problem at about line 2300 in AddSessionConsist.

  // add the consist address to the loco
  // remove for now -
  //invalid narrowing conversion from "int" to "unsigned char"
  //controllers[index].consist = { (consist & 0x7f), 0, ((consist & 0x80) == 0x80) };
  
  // I have now worked out what this is doing and this version works.

  controllers[index].consist.address = (consist & 0x7f);
  controllers[index].consist.session = 0;
  controllers[index].consist.reverse = ((consist & 0x80) == 0x80);

# Include files and libraries

I have had quite a lot of problems with the system reporting definitions it cannot find even when it has found the include file. I do not have an explanation at the moment. I just adjust the c_cpp_properties.json file until the problem goes away.

# encoder.h and encoder3.h

It turns out that there are two versions of this file. At some point a version was created which works with encoders using three pins. That is the one needed for this code so I have added it with the name encoder3.h.

