#include "myV792AC.h" // CAEN ADC V792AC class derived from VME
#include "myV862AC.h"
#include "myV775.h"

v792ac adc0(0x4000000,"/dev/vmedrv32d32");
v792ac adc1(0x5000000,"/dev/vmedrv32d32");
v792ac adc2(0x6000000,"/dev/vmedrv32d32");
v862ac adc3(0x7000000,"/dev/vmedrv32d32");
v775 ntdc(0x8000000,"/dev/vmedrv32d16");

v792ac* QDCs[] = {&adc0,&adc1,&adc2,&adc3};
unsigned int n_qdcs = 4;

void initQDC(){

  for(size_t i=0; i < n_qdcs; ++i){
    cout << "Press return to continue...";
    cin.get();
    QDCs[i]->print();
    QDCs[i]->reset();
    QDCs[i]->disableSlide();
    QDCs[i]->disableOverflowSupp();
    QDCs[i]->disableZeroSupp();

    unsigned short ped;
    ped = QDCs[i]->getI1();
    cout << "adc" << i << " id " << hex << QDCs[i]->id() << dec << ": Default pedestal value is " << ped << endl;

    ped = 146;
    QDCs[i]->setI1(ped);  // Set I1 current to define pedestal position
    ped =QDCs[i]->getI1();// in the range [0x00-0xFF] (see manual par. 4.34.2)
    cout << "Now adc" << i << " ped is set to " << ped << endl;
  }
}

int main()
 {
  initQDC();
  return 0;
 }
