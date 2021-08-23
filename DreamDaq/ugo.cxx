int initV513()
 {
  uint16_t w;
  xio.read16phys(0xFE, &w);
  cout << hex << " V513 FE " << w << endl;
  usleep(1);
  xio.read16phys(0xFC, &w);
  cout << hex << " V513 FC " << w << endl;
  usleep(1);
  xio.read16phys(0xFA, &w);
  cout << hex << " V513 FA " << w << endl;
  usleep(1);
  w=0;
  xio.write16phys(0x48, w);
  usleep(1);
  xio.write16phys(0x46, w);
  usleep(1);
  xio.write16phys(0x42, w);
  usleep(1);

  xio.read16phys(0x04, &w);
  cout << hex << " V513 0x4 " << w << endl;
  usleep(1);

  for (int i=0; i<8; i++)
   {
    int reg = 0x10+i*2;
    xio.setChannelInput(i);
    usleep(1);
    xio.read16phys(reg, &w);
    cout << hex << " V513 " << reg << " " << w << endl;
    usleep(1);
   }

  for (int i=8; i<16; i++)
   {
    int reg = 0x10+i*2;
    xio.setChannelOutput(i);
    usleep(1);
    xio.read16phys(reg, &w);
    cout << hex << " V513 " << reg << " " << w << endl;
    usleep(1);
   }
 }
