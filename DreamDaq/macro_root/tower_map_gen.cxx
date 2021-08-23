#!/usr/bin/root -b -q -l

{


  const int side=6;
  const int nrings=4;

  int tower2ring[side*side+1];
  int tower2module[side*side+1];
  int tower2mtower[side*side+1];
  int mtower2tower[10][5];
  int tower2ring[0]=-1;
  int ring2tower[nrings][side*side+1];


  cout << "tower map: \n";
  for(int row = 0; row<side; row++){
    for(int col = 0; col<side; col++){
      int tow = 1 + col + row*side;

      if(col==2 & row==2)tower2ring[tow]=0;
      else if(col>=1 & col<=3 & row>=1 & row<=3) tower2ring[tow]=1;
      else if(col>=0 & col<=4 & row>=0 & row<=4) tower2ring[tow]=2;
      else tower2ring[tow]=3;
      //cout << row << ", " << col << " -> " << tow << ", " <<  tower2ring[tow] << endl;
      cout << tower2ring[tow] << ", ";
    }
    cout << endl;
  }
  cout << endl;

  cout << "m_tower map: \n";
  for(int row = 0; row<side; row++){
    for(int col = 0; col<side; col++){
      int tow = 1 + col + row*side;

      int mrow=row/2;
      int mcol=col/2;

      int trow=row%2;
      int tcol=col%2;

      int mtower_tmp = 1+tcol + trow * 2;
      int mtower;
      switch(mtower_tmp){
      case 1:mtower=4;break;
      case 2:mtower=3;break;
      case 3:mtower=1;break;
      case 4:mtower=2;break;
      }

      int mod = 1+mcol + mrow * side/2;
      tower2mtower[tow] = mtower;
      tower2module[tow] = mod;
      mtower2tower[mod][mtower] = tow ;
      cout << mod << "_"<< mtower << ", ";
    }
    cout << endl;
  }
  cout << endl;

  for(int ring = 0; ring<nrings; ring++){
    cout << "m_towers in ring " << ring << ": ";
    for(int tow = 0; tow<side*side+1; tow++){
      ring2tower[ring][tow]=-1;
      if(tower2ring[tow]==ring){
	cout << tower2module[tow] << "_"<< tower2mtower[tow] << ", ";
	//cout << tow<< ", ";
	ring2tower[ring][tow]=tow;
      }
    }
    cout << endl;
  }
  cout << endl;

  cout << "tower_to_ring[] = {";
    for(int tow = 0; tow<side*side+1; tow++){
      if(tow==0) cout << tower2ring[tow];
      else cout << ", " << tower2ring[tow];
  }
  cout << "}; \n";



  cout << "twiki formatted output" << endl;
  //%EDITTABLE{ format="| label | text, 10 |" changerows="on" columnwidths="20%, 10%"}%
// | *ADC0* | 0x0400 ||
// | *Readout Channel* | *Tower signal* | *Cable Label* |
// | ADC0-00 | T01-S | M1-S4 |

  int adc=0;
  int ch=0;
  cout << "\n";
  cout << "| *ADC"<<adc<<"* | 0x0"<< adc+4<<"00 |  |"<<endl
       << "| *Readout Channel* | *Tower signal* | *Cable Label* |"<<endl;
    for(int mod = 1; mod<=9; mod++){
      for(int mtower = 1; mtower<=4; mtower++){
	for(int fiber = 0; fiber<2; fiber++){
	  char c_fiber = (fiber==1) ? 'C' : 'S';
	  int tower = mtower2tower[mod][mtower];
	  cout << "| ADC"<< adc <<"-"<<Form("%02d",ch)
	       <<" | T"<< Form("%02d",tower)<<"-"<<c_fiber
	       <<" | M"<<mod<<"-"<<c_fiber<<mtower<<" |"<<endl;
	  ch++;
	  if(ch>31){
	    ch=0;
	    adc++;
	    cout << "\n";
	    cout << "| *ADC"<<adc<<"* | 0x0"<< adc+4<<"00 |  |"<<endl
		 << "| *Readout Channel* | *Tower signal* | *Cable Label* |"<<endl;
	  }  
	}
      }
    }
  
  cout << "\n";

  cout << "code for monitoring" << endl;
  int adc=0;
  int ch=0;
  cout << endl << "if(i_adc == "<< adc <<"){" << endl;
  cout << " switch(ch){" << endl;

  for(int mod = 1; mod<=9; mod++){
    for(int mtower = 1; mtower<=4; mtower++){
      for(int fiber = 0; fiber<2; fiber++){
	char c_fiber = (fiber==1) ? 'C' : 'S';
	int tower = mtower2tower[mod][mtower];
	cout << " case " << ch << ": tower = "<< tower <<"; fiber = "<<fiber <<"; amp = "<<0<<"; break;" 
	     << endl; 
	ch++;
	if(ch>31){
	  ch=0;
	  adc++;
	  cout << " }" << endl;
	  cout << "}" << endl;
	  cout << endl << "if(i_adc == "<< adc <<"){" << endl;
	  cout << " switch(ch){" << endl;
	}  
      }
    }
  }
  
  cout << " }" << endl;
  cout << "}" << endl;
  cout << "\n";

}
