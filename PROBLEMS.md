# Problems

I have this problem at about line 2300 in AddSessionConsist.

  // add the consist address to the loco
  // remove for now -
  //invalid narrowing conversion from "int" to "unsigned char"
  //controllers[index].consist = { (consist & 0x7f), 0, ((consist & 0x80) == 0x80) };
  
  // I have now worked out what this is doing and this version works.

  controllers[index].consist.address = (consist & 0x7f);
  controllers[index].consist.session = 0;
  controllers[index].consist.reverse = ((consist & 0x80) == 0x80);
