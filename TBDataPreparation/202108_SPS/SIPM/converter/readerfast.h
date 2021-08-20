
#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TTree.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>

#include "hardcodedfast.h"

struct FileHeader {
  uint64_t acqStart;
  uint8_t vSoftware, acqMode;
  std::string dataFormatVersion;
  std::string softwareVersion;
};

struct FileInfo {
  std::vector<uint32_t> eventStartByte;
  uint64_t nEventsPerBoard[MAX_BOARDS] = {0};
  uint64_t nEvents = 0;
  uint64_t startAcqMs;
  uint8_t nBoards = 0;
  uint8_t acquisitionMode;
};

struct Event {
  std::array<uint16_t, NCHANNELS> lgPha;
  std::array<uint16_t, NCHANNELS> hgPha;
  std::array<uint32_t, NCHANNELS> toa;
  std::array<uint16_t, NCHANNELS> tot;
  uint64_t triggerId;
  double triggerTimeStamp;
  uint8_t boardId;
  bool operator<(const Event &lhs) { return this->triggerId < lhs.triggerId; }
};

typedef struct {
  uint64_t acquisitionStartTimeMs, nEvents;
  uint8_t nBoards, acquisitionMode;
} RootFileInfo;

std::vector<Event> parseSpectroscopyData(const std::vector<char> &, const FileInfo &);
std::vector<Event> parseSpectroscopyTimingData(const std::vector<char> &, const FileInfo &);
void writeSpectroscopyToRoot(const std::vector<Event> &, const FileInfo &, const std::string &);
void writeSpectroscopyTimingToRoot(const std::vector<Event> &, const FileInfo &, const std::string &);

void verbose(const std::string &message) {
  if (VERBOSE) {
    std::cout << message << std::endl;
  }
}

inline uint8_t popcount(uint64_t x) {
  u_int8_t v = 0;
  while (x != 0) {
    x &= x - 1;
    v++;
  }
  return v;
}
