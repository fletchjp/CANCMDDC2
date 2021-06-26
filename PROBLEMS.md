# Problems

I have this problem at about line 2300 in AddSessionConsist.

  // add the consist address to the loco
  // remove for now -
  //invalid narrowing conversion from "int" to "unsigned char"
  //controllers[index].consist = { (consist & 0x7f), 0, ((consist & 0x80) == 0x80) };

  For the solution to this and some other problems see the main branch.
  
# Problem running this code

I am running the code on my old hardware.

The hardware will run the old CANCMDDC 2.5 code and I can link that to the CBUS network and see it working as a FLiM node.

When I recompile the new code it at first recognises the old configuration. It then reverts to SLiM on its own and floods the CBUS with messages as seen on CANACT. These show up as transmission errors.

I have checked versions of libraries and updated the Arduino version.
