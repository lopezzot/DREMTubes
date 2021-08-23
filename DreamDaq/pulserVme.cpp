
#include <sys/time.h>
#include <sys/resource.h>
#include <termios.h>

#include <csignal>
#include <cstring>

#include <iostream>
#include <sstream>

#include "myV2718.h"

v2718 v2718m(0,"/V2718/cvA24_U_DATA/0");

using namespace std;

int32_t getkey() {
    int32_t character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
}

int32_t main( int32_t argc, char** argv )
 {
  uint16_t chan;
  uint64_t period, width;
  uint8_t unit;
  if ((argc != 5) && (argc != 6))
   {
    cout << "Usage " << argv[0] << " <channel> <period> <duration> <units['s/m/u/n']> [<\"continue\">]" << endl;
    exit(1);
   }
  stringstream ss (stringstream::in | stringstream::out);
  ss << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4];
  ss >> chan >> period >> width >> unit;
  bool nreset = (argc == 6) && (strcasecmp(argv[5],"continue")==0);
  cout << (nreset ? "[Reset: False]" : "[Reset: True]") << endl;
  if (chan > 1) exit(2);
  if ((unit != 's') && (unit != 'm') && (unit != 'u') && (unit != 'n')) exit(3);
  if (unit == 's')
   {
    period *= 1000000000;
    width *= 1000000000;
   }
  else if (unit == 'm')
   {
    period *= 1000000;
    width *= 1000000;
   }
  else if (unit == 'u')
   {
    period *= 1000;
    width *= 1000;
   }
  if (chan == 0) v2718m.setPulserA(period, width);
  else v2718m.setPulserB(period, width);
  int32_t key;
  while(1)
   {
    key = getkey();
    // terminate loop on ESC (0x1B) or Ctrl-D (0x04) on STDIN
    if (key == 0x1B || key == 0x04) break;
   }
  cout << ((key == 0x1B) ? "[Got ESC]" : (key == 0x04) ? "[Got Ctrl-D]" : "[Got ???]")
       << endl;
  if (!nreset) (chan == 0) ? v2718m.resetPulserA() : v2718m.resetPulserB();
  return 0;
 }
