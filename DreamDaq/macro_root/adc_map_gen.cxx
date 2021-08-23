#!/usr/bin/root -b -q -l

{

  for(int adc = 0; adc<3; adc++){
    cout << endl << "if(i_adc == "<< adc <<"){" << endl;
    cout << " switch(ch){" << endl;
    for(int ch=0; ch<32; ch++){
      if(adc == 2 & ch > 7 ) break;

      int tower = ch/2+1+adc*16;
      int fiber = ch%2;
      char c_fiber = (fiber==0)? 'S' : 'C';

      cout << " case " << ch << ": tower = "<< tower <<"; fiber = "<<fiber <<"; amp = "<<0<<"; break;" 
	   << " // " << Form("ADC%02d-%02d -> T%02d%c",adc,ch,tower,c_fiber)
	   << endl; 
    }
    cout << " }" << endl;
    cout << "}" << endl;
  }

}
