#include "readerfast.h"

uint32_t getFileSize(std::string &fname) {
  std::ifstream infile(fname, std::ios::binary | std::ios::ate);

  if (!infile) {
    std::cerr << "Error! Cannot open file " << fname << std::endl;
    throw std::runtime_error("");
  }

  uint32_t fileSize = (uint32_t)infile.tellg();

  infile.close();
  if (infile.is_open()) {
    std::cerr << "Cannot close file!" << std::endl;
    throw std::runtime_error("");
  }
  return fileSize;
}

std::vector<char> readRawData(std::string &fname, uint32_t size) {
  std::vector<char> data(size);
  std::ifstream infile(fname, std::ios::binary | std::ios::in);
  if (!infile) {
    throw std::runtime_error("Error! Cannot open file " + fname);
  }
  verbose("Start reading file " + fname);
  verbose("File size: " + std::to_string(size / (1024 * 1024)) + " Mb");

  infile.read(data.data(), size);
  infile.close();
  if (infile.is_open()) {
    throw std::runtime_error("Error! Cannot close file " + fname);
  }
  return data;
}

FileHeader getFileHeader(const std::vector<char> &rawData) {
  FileHeader header;
  uint8_t dfv1, dfv2, swv1, swv2, swv3;

  std::memcpy(&dfv1, &rawData[0], sizeof(uint8_t));
  std::memcpy(&dfv2, &rawData[1], sizeof(uint8_t));
  std::memcpy(&swv1, &rawData[2], sizeof(uint8_t));
  std::memcpy(&swv2, &rawData[3], sizeof(uint8_t));
  std::memcpy(&swv3, &rawData[4], sizeof(uint8_t));
  std::memcpy(&header.acqMode, &rawData[5], sizeof(uint8_t));
  std::memcpy(&header.acqStart, &rawData[6], sizeof(uint64_t));
  header.dataFormatVersion = std::to_string(dfv1) + "." + std::to_string(dfv2);
  header.softwareVersion = std::to_string(swv1) + "." + std::to_string(swv2) + "." + std::to_string(swv3);
  return header;
}

FileInfo getFileInfo(const std::vector<char> &rawData, const FileHeader &header) {
  FileInfo fileInfo;
  const uint64_t fileSize = rawData.size();
  uint32_t bytePosition = FILE_HEADER_SIZE; // Skip bytes (header)
  uint32_t errors = 0;                      // Errors in file

  fileInfo.eventStartByte.reserve(1000000); // Reasonable number

  fileInfo.acquisitionMode = header.acqMode;
  fileInfo.startAcqMs = header.acqStart;

  while (bytePosition < fileSize) {
    uint16_t eventSize;
    uint8_t boardId;
    uint64_t channelMask;

    std::memcpy(&eventSize, &rawData[bytePosition], sizeof(uint16_t));
    std::memcpy(&boardId, &rawData[bytePosition + 2], sizeof(uint8_t));
    std::memcpy(&channelMask, &rawData[bytePosition + 19], sizeof(uint64_t));

    const uint8_t nChannlesActive = popcount(channelMask);
    const uint32_t expectedEventSize =
        EVENT_HEADER_SIZE[fileInfo.acquisitionMode - 1] + nChannlesActive * EVENTS_SIZE[fileInfo.acquisitionMode - 1];

    if (eventSize != expectedEventSize | boardId > 15) {
      if (eventSize == 0) {
        std::cerr << "===============================================\n";
        std::cerr << "= Something went very wrong! Event size is 0! =\n";
        std::cerr << "===============================================\n";
        exit(EXIT_FAILURE);
      } else if (boardId > 15) {
        std::cerr << "=============================================\n";
        std::cerr << "= Something went very wrong! Board id > 15! =\n";
        std::cerr << "=============================================\n";
        exit(EXIT_FAILURE);
      }
      errors++;
    } else {
      fileInfo.nEventsPerBoard[boardId] += 1;
      fileInfo.eventStartByte.emplace_back(bytePosition);
      if (fileInfo.nBoards < boardId) {
        fileInfo.nBoards = boardId;
      }
      fileInfo.nEvents++;
    }
    bytePosition += eventSize;
  }
  // Number of boards is max id + 1
  fileInfo.nBoards += 1;

  if (errors) {
    verbose("Found " + std::to_string(errors) + " errors in file");
  }
  verbose("Number of boards detected: " + std::to_string(fileInfo.nBoards));
  verbose("Total number of events: " + std::to_string(fileInfo.nEvents));
  for (int i = 0; i < fileInfo.nBoards; ++i) {
    verbose("Number of events board " + std::to_string(i) + ": " + std::to_string(fileInfo.nEventsPerBoard[i]));
  }

  return fileInfo;
}

std::vector<Event> parseSpectroscopyData(const std::vector<char> &rawData, const FileInfo &fileInfo) {
  std::vector<Event> output(fileInfo.nEvents);

  for (uint32_t i = 0; i < fileInfo.nEvents; ++i) {
    Event event;
    const uint32_t startByte = fileInfo.eventStartByte[i];

    uint8_t boardId;
    double triggerTime;
    uint64_t triggerId;

    std::memcpy(&boardId, &rawData[startByte + 2], sizeof(uint8_t));
    std::memcpy(&triggerTime, &rawData[startByte + 3], sizeof(double));
    std::memcpy(&triggerId, &rawData[startByte + 11], sizeof(uint64_t));

    event.boardId = boardId;
    event.triggerTimeStamp = triggerTime;
    event.triggerId = triggerId;

    // 27 is event header size - 411 is event size
    const uint32_t eventDataStartByte = startByte + 27;
    for (int j = 0; j < NCHANNELS; ++j) {
      uint8_t channelId;
      uint16_t lgPha, hgPha;
      std::memcpy(&channelId, &rawData[eventDataStartByte + 6 * j], sizeof(uint8_t));
      std::memcpy(&lgPha, &rawData[eventDataStartByte + 6 * j + 2], sizeof(uint16_t));
      std::memcpy(&hgPha, &rawData[eventDataStartByte + 6 * j + 4], sizeof(uint16_t));
      channelId = MAPPING_LUT[channelId]; // Map channel to position in calo
      // TODO: Understand why there are values > 4096
      if (lgPha > 4096) {
        lgPha = 4096;
      }
      if (hgPha > 4096) {
        hgPha = 4096;
      }
      event.lgPha[channelId] = lgPha;
      event.hgPha[channelId] = hgPha;
    }
    output[i] = event;
  }

  return output;
}

std::vector<Event> parseSpectroscopyTimingData(const std::vector<char> &rawData, const FileInfo &fileInfo) {
  std::vector<Event> output(fileInfo.nEvents);

  for (uint32_t i = 0; i < fileInfo.nEvents; ++i) {
    Event event;
    const uint32_t startByte = fileInfo.eventStartByte[i];

    uint8_t boardId;
    double triggerTime;
    uint64_t triggerId;

    std::memcpy(&boardId, &rawData[startByte + 2], sizeof(uint8_t));
    std::memcpy(&triggerTime, &rawData[startByte + 3], sizeof(double));
    std::memcpy(&triggerId, &rawData[startByte + 11], sizeof(uint64_t));

    event.boardId = boardId;
    event.triggerTimeStamp = triggerTime;
    event.triggerId = triggerId;

    // 27 is event header size - 795 is event size
    const uint32_t eventDataStartByte = startByte + 27;
    for (int j = 0; j < NCHANNELS; ++j) {
      uint8_t channelId;
      uint16_t lgPha, hgPha, tot;
      uint32_t toa;
      std::memcpy(&channelId, &rawData[eventDataStartByte + 12 * j], sizeof(uint8_t));
      std::memcpy(&lgPha, &rawData[eventDataStartByte + 12 * j + 2], sizeof(uint16_t));
      std::memcpy(&hgPha, &rawData[eventDataStartByte + 12 * j + 4], sizeof(uint16_t));
      std::memcpy(&toa, &rawData[eventDataStartByte + 12 * j + 6], sizeof(uint32_t));
      std::memcpy(&tot, &rawData[eventDataStartByte + 12 * j + 10], sizeof(uint16_t));
      channelId = MAPPING_LUT[channelId]; // Map channel to position in calo
      // TODO: Understand why there are values > 4096
      if (lgPha > 4096) {
        lgPha = 4096;
      }
      if (hgPha > 4096) {
        hgPha = 4096;
      }
      event.lgPha[channelId] = lgPha;
      event.hgPha[channelId] = hgPha;
      event.toa[channelId] = toa;
      event.tot[channelId] = tot;
    }
    output[i] = event;
  }
  std::sort(output.begin(), output.end());
  return output;
}

void writeSpectroscopyToRoot(const std::vector<Event> &events, const FileInfo &info, const std::string &fname) {
  const std::string rootfname = fname.substr(0, fname.find_last_of(".")) + ".root";

  TFile rootFile(rootfname.c_str(), "RECREATE");

  TTree rootTreeInfo("EventInfo", "Informations about event");

  RootFileInfo rootfileinfo;
  rootTreeInfo.Branch("FileInfo", &rootfileinfo,
                      "AcquisitionStartMs/l:NumberOfEvents:NumberOfBoards/b:AcquisitionMode");
  rootfileinfo.acquisitionStartTimeMs = info.startAcqMs;
  rootfileinfo.nEvents = info.nEvents;
  rootfileinfo.nBoards = info.nBoards;
  rootfileinfo.acquisitionMode = info.acquisitionMode;
  rootTreeInfo.Fill();
  rootTreeInfo.Write();

  TTree rootTreeEvent("SiPMData", "Data from SiPM");
  uint16_t HighGainADC[NCHANNELS];
  uint16_t LowGainADC[NCHANNELS];
  uint64_t triggerId;
  double triggerTime;
  uint8_t boardId;
  rootTreeEvent.Branch("HighGainADC", HighGainADC, "HighGainADC[64]/s", 128000);
  rootTreeEvent.Branch("LowGainADC", LowGainADC, "LowGainADC[64]/s", 128000);
  rootTreeEvent.Branch("TriggerId", &triggerId, "Triggerid/l", 128000);
  rootTreeEvent.Branch("TriggerTimeStampUs", &triggerTime, "TriggerTimeStampUs/D", 128000);
  rootTreeEvent.Branch("BoardId", &boardId, "BoardId/b", 128000);

  for (int i = 0; i < info.nEvents; ++i) {
    boardId = events[i].boardId;
    triggerId = events[i].triggerId;
    triggerTime = events[i].triggerTimeStamp;
    for (int j = 0; j < NCHANNELS; ++j) {
      HighGainADC[j] = events[i].hgPha[j];
      LowGainADC[j] = events[i].lgPha[j];
    }
    rootTreeEvent.Fill();
  }
  rootTreeEvent.AutoSave();

  rootFile.mkdir("Histograms");
  rootFile.cd("Histograms");

  std::vector<std::vector<TH1F *>> histoslg(info.nBoards);
  std::vector<std::vector<TH1F *>> histoshg(info.nBoards);

  for (int i = 0; i < info.nBoards; ++i) {
    histoshg.resize(NCHANNELS);
    histoslg.resize(NCHANNELS);
  }

  for (int i = 0; i < info.nBoards; ++i) {
    for (int j = 0; j < NCHANNELS; ++j) {
      std::string titleHG = "HighGainADC Board " + std::to_string(i) + "Channel " + std::to_string(j);
      std::string titleLG = "LowGainADC Board " + std::to_string(i) + "Channel " + std::to_string(j);
      histoslg[i].push_back(new TH1F(titleLG.c_str(), "LowGainADC;ADC", 4096, 0, 4096));
      histoshg[i].push_back(new TH1F(titleHG.c_str(), "HighGainADC;ADC", 4096, 0, 4096));
    }
  }

  for (int i = 0; i < info.nEvents; ++i) {
    uint8_t bid = events[i].boardId;
    for (int j = 0; j < NCHANNELS; ++j) {
      histoslg[bid][j]->Fill(events[i].lgPha[j]);
      histoshg[bid][j]->Fill(events[i].hgPha[j]);
    }
  }

  for (int i = 0; i < info.nBoards; ++i) {
    for (int j = 0; j < NCHANNELS; ++j) {
      histoslg[i][j]->Write();
      histoshg[i][j]->Write();
    }
  }

  rootFile.mkdir("Calibrations");
  rootFile.cd("Calibrations");

  TTree rootTreeCalibration("SiPMCalibrations", "Calibrations and settings of SiPM");

  rootTreeCalibration.Write();

  rootFile.Close();
}

void writeSpectroscopyTimingToRoot(const std::vector<Event> &events, const FileInfo &info, const std::string &fname) {
  const std::string rootfname = fname.substr(0, fname.find_last_of(".")) + ".root";

  TFile rootFile(rootfname.c_str(), "RECREATE");

  TTree rootTreeInfo("EventInfo", "Informations about event");

  RootFileInfo rootfileinfo;
  rootTreeInfo.Branch("FileInfo", &rootfileinfo,
                      "AcquisitionStartMs/l:NumberOfEvents:NumberOfBoards/b:AcquisitionMode");
  rootfileinfo.acquisitionStartTimeMs = info.startAcqMs;
  rootfileinfo.nEvents = info.nEvents;
  rootfileinfo.nBoards = info.nBoards;
  rootfileinfo.acquisitionMode = info.acquisitionMode;
  rootTreeInfo.Fill();
  rootTreeInfo.Write();

  TTree rootTreeEvent("SiPMData", "Data from SiPM");
  uint16_t HighGainADC[NCHANNELS];
  uint16_t LowGainADC[NCHANNELS];
  uint32_t Toa[NCHANNELS];
  uint16_t Tot[NCHANNELS];
  uint64_t triggerId;
  double triggerTime;
  uint8_t boardId;
  rootTreeEvent.Branch("HighGainADC", HighGainADC, "HighGainADC[64]/s", 128000);
  rootTreeEvent.Branch("LowGainADC", LowGainADC, "LowGainADC[64]/s", 128000);
  rootTreeEvent.Branch("TimeOfArrival", LowGainADC, "Toa[64]/i", 128000);
  rootTreeEvent.Branch("TimeOverThreshold", LowGainADC, "Tot[64]/s", 128000);
  rootTreeEvent.Branch("TriggerId", &triggerId, "Triggerid/l", 128000);
  rootTreeEvent.Branch("TriggerTimeStampUs", &triggerTime, "TriggerTimeStampUs/D", 128000);
  rootTreeEvent.Branch("BoardId", &boardId, "BoardId/b", 128000);

  for (int i = 0; i < info.nEvents; ++i) {
    boardId = events[i].boardId;
    triggerId = events[i].triggerId;
    triggerTime = events[i].triggerTimeStamp;
    for (int j = 0; j < NCHANNELS; ++j) {
      HighGainADC[j] = events[i].hgPha[j];
      LowGainADC[j] = events[i].lgPha[j];
      Toa[j] = events[i].toa[j];
      Tot[j] = events[i].tot[j];
    }
    rootTreeEvent.Fill();
  }
  rootTreeEvent.AutoSave();

  rootFile.mkdir("Histograms");
  rootFile.cd("Histograms");

  std::vector<std::vector<TH1F *>> histoslg(info.nBoards);
  std::vector<std::vector<TH1F *>> histoshg(info.nBoards);

  for (int i = 0; i < info.nBoards; ++i) {
    histoshg.resize(NCHANNELS);
    histoslg.resize(NCHANNELS);
  }

  for (int i = 0; i < info.nBoards; ++i) {
    for (int j = 0; j < NCHANNELS; ++j) {
      std::string titleHG = "HighGainADC Board " + std::to_string(i) + "Channel " + std::to_string(j);
      std::string titleLG = "LowGainADC Board " + std::to_string(i) + "Channel " + std::to_string(j);
      histoslg[i].push_back(new TH1F(titleLG.c_str(), "LowGainADC;ADC", 4096, 0, 4096));
      histoshg[i].push_back(new TH1F(titleHG.c_str(), "HighGainADC;ADC", 4096, 0, 4096));
    }
  }

  for (int i = 0; i < info.nEvents; ++i) {
    uint8_t bid = events[i].boardId;
    for (int j = 0; j < NCHANNELS; ++j) {
      histoslg[bid][j]->Fill(events[i].lgPha[j]);
      histoshg[bid][j]->Fill(events[i].hgPha[j]);
    }
  }

  for (int i = 0; i < info.nBoards; ++i) {
    for (int j = 0; j < NCHANNELS; ++j) {
      histoslg[i][j]->Write();
      histoshg[i][j]->Write();
    }
  }

  rootFile.mkdir("Calibrations");
  rootFile.cd("Calibrations");

  TTree rootTreeCalibration("SiPMCalibrations", "Calibrations and settings of SiPM");

  rootTreeCalibration.Write();

  rootFile.Close();
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    std::cerr << "Error: Missing filename!\n";
    return 1;
  }
  std::string fname = argv[1];

  uint32_t fileSizeBytes = getFileSize(fname);

  // Read all file
  std::vector<char> rawData = readRawData(fname, fileSizeBytes);

  // Get file header
  FileHeader header = getFileHeader(rawData);

  // Get file info (nBoards, nEvents, acqMode, ...)
  FileInfo fileInfo = getFileInfo(rawData, header);

  switch (fileInfo.acquisitionMode) {
  case 1: { // Spectroscopy
    std::vector<Event> parsedData = parseSpectroscopyData(rawData, fileInfo);
    writeSpectroscopyToRoot(parsedData, fileInfo, fname);
    break;
  }
  case 2: { // Timing
    break;
  }
  case 3: { // Spectroscopy + Timing
    std::vector<Event> parsedData = parseSpectroscopyTimingData(rawData, fileInfo);
    writeSpectroscopyTimingToRoot(parsedData, fileInfo, fname);
    break;
  }
  case 4: { // Counting
    break;
  }
  }

  return 0;
}
