#include <stdio.h>
#include <time.h>
#include <zlib.h>

#include "MagicRawData.h"

int RunFilename(run_header_str *RunHeader, char* filename, char* path) {
  time_t time_now_secs;
  time_t time_day_secs;
  struct tm *time_day;
  char   runtype;

  switch (RunHeader->RunType) {
      case RUN_TYPE_NORMAL:
	runtype = 'D';
	break;
      case RUN_TYPE_PEDESTAL:
	runtype = 'P';
	break;
      case RUN_TYPE_CALIBRATION:
	runtype = 'C';
	break;
      case RUN_TYPE_POINT:
	runtype = 'S';
	break;
      case RUN_TYPE_DOMINOCALIB:
	runtype = 'L';
	break;
      default:
	runtype = 'D';
	fprintf(stderr, "run_file_name: Wrong RunType: %d\n", RunHeader->RunType);
	break;
  }

  time(&time_now_secs);  

  // get time 12 hours after (date of day at the end of the shift) //
  time_day_secs = time_now_secs + 43200;  
  time_day = localtime(&time_day_secs);

#if (FORMAT_VERSION >= 10)
  sprintf(filename, "%04d%02d%02d_M2_%08d.%03d_%c_%.30s.raw", 
	  1900+time_day->tm_year, 1+time_day->tm_mon, time_day->tm_mday,
	  RunHeader->RunNumber,
	  RunHeader->FileNumber,
	  runtype,
	  RunHeader->ProjectName);
#else
  sprintf(filename, "%04d%02d%02d_%08d_%c_%.22s_E.raw", 
	  1900+time_day->tm_year, 1+time_day->tm_mon, time_day->tm_mday,
	  RunHeader->RunNumber,
	  runtype,
	  RunHeader->ProjectName);
#endif

  sprintf(path, "%04d_%02d_%02d",
	  1900+time_day->tm_year, 1+time_day->tm_mon, time_day->tm_mday);

  return 0;
}


int fWriteRunHeader(run_header_str *RunHeader, FILE *fptr) {
  int nPix = RunHeader->NumPixInChannel * RunHeader->NumChannels;
  
  fwrite(&RunHeader->MagicNumber,        sizeof RunHeader->MagicNumber,        1, fptr);
  fwrite(&RunHeader->FormatVersion,      sizeof RunHeader->FormatVersion,      1, fptr);
#if (FORMAT_VERSION >= 11)
  fwrite(&RunHeader->RunHeaderSize,      sizeof RunHeader->RunHeaderSize,      1, fptr);
  fwrite(&RunHeader->EventHeaderSize,    sizeof RunHeader->EventHeaderSize,    1, fptr);
  fwrite(&RunHeader->ChannelHeaderSize,  sizeof RunHeader->ChannelHeaderSize,  1, fptr);
#endif
  fwrite(&RunHeader->SoftVersion,        sizeof RunHeader->SoftVersion,        1, fptr);
  fwrite(&RunHeader->FadcType,           sizeof RunHeader->FadcType,           1, fptr);
  fwrite(&RunHeader->CameraVersion,      sizeof RunHeader->CameraVersion,      1, fptr);
  fwrite(&RunHeader->TelescopeNumber,    sizeof RunHeader->TelescopeNumber,    1, fptr);
  fwrite(&RunHeader->RunType,            sizeof RunHeader->RunType,            1, fptr);
  fwrite(&RunHeader->RunNumber,          sizeof RunHeader->RunNumber,          1, fptr);
#if (FORMAT_VERSION >= 10)
  fwrite(&RunHeader->FileNumber,         sizeof RunHeader->FileNumber,         1, fptr);
#endif
  fwrite( RunHeader->ProjectName,        SIZE_PROJECT_NAME,                    1, fptr);
  fwrite( RunHeader->SourceName,         SIZE_SOURCE_NAME,                     1, fptr);
  fwrite(&RunHeader->ObservationMode,    SIZE_OBSERVATION_MODE,                1, fptr);
  fwrite(&RunHeader->SourceRA,           sizeof RunHeader->SourceRA,           1, fptr);
  fwrite(&RunHeader->SourceDEC,          sizeof RunHeader->SourceDEC,          1, fptr);
  fwrite(&RunHeader->TelescopeRA,        sizeof RunHeader->TelescopeRA,        1, fptr);
  fwrite(&RunHeader->TelescopeDEC,       sizeof RunHeader->TelescopeDEC,       1, fptr);
  fwrite( RunHeader->SourceEpochChar,    SIZE_SOURCE_EPOCH,                    1, fptr);
  fwrite(&RunHeader->SourceEpochDate,    sizeof RunHeader->SourceEpochDate,    1, fptr);
  fwrite(&RunHeader->NumChannels,        sizeof RunHeader->NumChannels,        1, fptr);
  fwrite(&RunHeader->NumPixInChannel,    sizeof RunHeader->NumPixInChannel,    1, fptr);
#if (FORMAT_VERSION < 11)
  fwrite(&RunHeader->Dummy,              sizeof RunHeader->Dummy,              1, fptr);
#endif
  fwrite(&RunHeader->NumSamplesPerPixel, sizeof RunHeader->NumSamplesPerPixel, 1, fptr);
#if (FORMAT_VERSION >= 9)
  if (RunHeader->FormatVersion >= 9) {
    fwrite(&RunHeader->NumSamplesRemovedHead, sizeof RunHeader->NumSamplesRemovedHead, 1, fptr);
    fwrite(&RunHeader->NumSamplesRemovedTail, sizeof RunHeader->NumSamplesRemovedTail, 1, fptr);
  }
#endif
  fwrite(&RunHeader->NumEvents,          sizeof RunHeader->NumEvents,          1, fptr);
#if (FORMAT_VERSION >= 9)
  if (RunHeader->FormatVersion >= 9) {
    fwrite(&RunHeader->NumEventsRead,    sizeof RunHeader->NumEventsRead,      1, fptr);
  }
#endif
  fwrite(&RunHeader->NumBytesPerSample,  sizeof RunHeader->NumBytesPerSample,  1, fptr);
  fwrite(&RunHeader->SamplingFrequency,  sizeof RunHeader->SamplingFrequency,  1, fptr);
  fwrite(&RunHeader->FadcResolution,     sizeof RunHeader->FadcResolution,     1, fptr);
  fwrite(&RunHeader->StartYear,          sizeof RunHeader->StartYear,          1, fptr);
  fwrite(&RunHeader->StartMonth,         sizeof RunHeader->StartMonth,         1, fptr);
  fwrite(&RunHeader->StartDay,           sizeof RunHeader->StartDay,           1, fptr);
  fwrite(&RunHeader->StartHour,          sizeof RunHeader->StartHour,          1, fptr);
  fwrite(&RunHeader->StartMinute,        sizeof RunHeader->StartMinute,        1, fptr);
  fwrite(&RunHeader->StartSecond,        sizeof RunHeader->StartSecond,        1, fptr);
  fwrite(&RunHeader->EndYear,            sizeof RunHeader->EndYear,            1, fptr);
  fwrite(&RunHeader->EndMonth,           sizeof RunHeader->EndMonth,           1, fptr);
  fwrite(&RunHeader->EndDay,             sizeof RunHeader->EndDay,             1, fptr);
  fwrite(&RunHeader->EndHour,            sizeof RunHeader->EndHour,            1, fptr);
  fwrite(&RunHeader->EndMinute,          sizeof RunHeader->EndMinute,          1, fptr);
  fwrite(&RunHeader->EndSecond,          sizeof RunHeader->EndSecond,          1, fptr);
  fwrite( RunHeader->PixAssignment,      sizeof *RunHeader->PixAssignment,  nPix, fptr); 

  return 0;
}

int gzWriteRunHeader(run_header_str *RunHeader, FILE *fptr) {
  int nPix = RunHeader->NumPixInChannel * RunHeader->NumChannels;
  
  gzwrite(fptr, &RunHeader->MagicNumber,        sizeof RunHeader->MagicNumber          );
  gzwrite(fptr, &RunHeader->FormatVersion,      sizeof RunHeader->FormatVersion        );
#if (FORMAT_VERSION >= 11)
  gzwrite(fptr, &RunHeader->RunHeaderSize,      sizeof RunHeader->RunHeaderSize        );
  gzwrite(fptr, &RunHeader->EventHeaderSize,    sizeof RunHeader->EventHeaderSize      );
  gzwrite(fptr, &RunHeader->ChannelHeaderSize,  sizeof RunHeader->ChannelHeaderSize    );
#endif
  gzwrite(fptr, &RunHeader->SoftVersion,        sizeof RunHeader->SoftVersion          );
  gzwrite(fptr, &RunHeader->FadcType,           sizeof RunHeader->FadcType             );
  gzwrite(fptr, &RunHeader->CameraVersion,      sizeof RunHeader->CameraVersion        );
  gzwrite(fptr, &RunHeader->TelescopeNumber,    sizeof RunHeader->TelescopeNumber      );
  gzwrite(fptr, &RunHeader->RunType,            sizeof RunHeader->RunType              );
  gzwrite(fptr, &RunHeader->RunNumber,          sizeof RunHeader->RunNumber            );
#if (FORMAT_VERSION >= 10)
  gzwrite(fptr, &RunHeader->FileNumber,         sizeof RunHeader->FileNumber           );
#endif
  gzwrite(fptr,  RunHeader->ProjectName,        SIZE_PROJECT_NAME                      );
  gzwrite(fptr,  RunHeader->SourceName,         SIZE_SOURCE_NAME                       );
  gzwrite(fptr, &RunHeader->ObservationMode,    SIZE_OBSERVATION_MODE                  );
  gzwrite(fptr, &RunHeader->SourceRA,           sizeof RunHeader->SourceRA             );
  gzwrite(fptr, &RunHeader->SourceDEC,          sizeof RunHeader->SourceDEC            );
  gzwrite(fptr, &RunHeader->TelescopeRA,        sizeof RunHeader->TelescopeRA          );
  gzwrite(fptr, &RunHeader->TelescopeDEC,       sizeof RunHeader->TelescopeDEC         );
  gzwrite(fptr,  RunHeader->SourceEpochChar,    SIZE_SOURCE_EPOCH                      );
  gzwrite(fptr, &RunHeader->SourceEpochDate,    sizeof RunHeader->SourceEpochDate      );
  gzwrite(fptr, &RunHeader->NumChannels,        sizeof RunHeader->NumChannels          );
  gzwrite(fptr, &RunHeader->NumPixInChannel,    sizeof RunHeader->NumPixInChannel      );
#if (FORMAT_VERSION < 11)
  gzwrite(fptr, &RunHeader->Dummy,              sizeof RunHeader->Dummy                );
#endif
  gzwrite(fptr, &RunHeader->NumSamplesPerPixel, sizeof RunHeader->NumSamplesPerPixel   );
#if (FORMAT_VERSION >= 9)
  if (RunHeader->FormatVersion >= 9) {
    gzwrite(fptr, &RunHeader->NumSamplesRemovedHead, sizeof RunHeader->NumSamplesRemovedHead );
    gzwrite(fptr, &RunHeader->NumSamplesRemovedTail, sizeof RunHeader->NumSamplesRemovedTail );
  }
#endif
  gzwrite(fptr, &RunHeader->NumEvents,          sizeof RunHeader->NumEvents            );
#if (FORMAT_VERSION >= 9)
  if (RunHeader->FormatVersion >= 9) {
    gzwrite(fptr, &RunHeader->NumEventsRead,    sizeof RunHeader->NumEventsRead        );
  }
#endif
  gzwrite(fptr, &RunHeader->NumBytesPerSample,  sizeof RunHeader->NumBytesPerSample    );
  gzwrite(fptr, &RunHeader->SamplingFrequency,  sizeof RunHeader->SamplingFrequency    );
  gzwrite(fptr, &RunHeader->FadcResolution,     sizeof RunHeader->FadcResolution       );
  gzwrite(fptr, &RunHeader->StartYear,          sizeof RunHeader->StartYear            );
  gzwrite(fptr, &RunHeader->StartMonth,         sizeof RunHeader->StartMonth           );
  gzwrite(fptr, &RunHeader->StartDay,           sizeof RunHeader->StartDay             );
  gzwrite(fptr, &RunHeader->StartHour,          sizeof RunHeader->StartHour            );
  gzwrite(fptr, &RunHeader->StartMinute,        sizeof RunHeader->StartMinute          );
  gzwrite(fptr, &RunHeader->StartSecond,        sizeof RunHeader->StartSecond          );
  gzwrite(fptr, &RunHeader->EndYear,            sizeof RunHeader->EndYear              );
  gzwrite(fptr, &RunHeader->EndMonth,           sizeof RunHeader->EndMonth             );
  gzwrite(fptr, &RunHeader->EndDay,             sizeof RunHeader->EndDay               );
  gzwrite(fptr, &RunHeader->EndHour,            sizeof RunHeader->EndHour              );
  gzwrite(fptr, &RunHeader->EndMinute,          sizeof RunHeader->EndMinute            );
  gzwrite(fptr, &RunHeader->EndSecond,          sizeof RunHeader->EndSecond            );
  gzwrite(fptr,  RunHeader->PixAssignment,      sizeof *RunHeader->PixAssignment * nPix); 

  return 0;
}


run_header_str* fReadRunHeader(FILE *fptr) {
  int nPix;
  int nread = 0;

  run_header_str *RunHeader = malloc(RUN_HEADER_BASESIZE);

  nread += fread(&RunHeader->MagicNumber,        1, sizeof RunHeader->MagicNumber,        fptr);
  if (RunHeader->MagicNumber != MAGIC_NUMBER_FINISHED) {
    fprintf(stderr, "Found MagicNumber: 0x%8x (expect: 0x%8x)\n", RunHeader->MagicNumber, MAGIC_NUMBER_FINISHED);
    fprintf(stderr, " this is either a MAGIC rawdata file with on old format,\n");
    fprintf(stderr, "                the DAQ did not correctly close the file,\n");
    fprintf(stderr, "                or no MAGIC rawdata file at all\n");
    return NULL;
  }
  nread += fread(&RunHeader->FormatVersion,      1, sizeof RunHeader->FormatVersion,      fptr);
  if (RunHeader->FormatVersion < 6) {
    return NULL;
  }
#if (FORMAT_VERSION >= 11)
  if (RunHeader->FormatVersion >= 11) {
    nread += fread(&RunHeader->RunHeaderSize,     1, sizeof RunHeader->RunHeaderSize,     fptr);
    nread += fread(&RunHeader->EventHeaderSize,   1, sizeof RunHeader->EventHeaderSize,   fptr);
    nread += fread(&RunHeader->ChannelHeaderSize, 1, sizeof RunHeader->ChannelHeaderSize,  fptr);
  }
#endif
  nread += fread(&RunHeader->SoftVersion,        1, sizeof RunHeader->SoftVersion,        fptr);
  nread += fread(&RunHeader->FadcType,           1, sizeof RunHeader->FadcType,           fptr);
  nread += fread(&RunHeader->CameraVersion,      1, sizeof RunHeader->CameraVersion,      fptr);
  nread += fread(&RunHeader->TelescopeNumber,    1, sizeof RunHeader->TelescopeNumber,    fptr);
  nread += fread(&RunHeader->RunType,            1, sizeof RunHeader->RunType,            fptr);
  nread += fread(&RunHeader->RunNumber,          1, sizeof RunHeader->RunNumber,          fptr);
#if (FORMAT_VERSION >= 10)
  if (RunHeader->FormatVersion >=10 ) {
    nread += fread(&RunHeader->FileNumber,         1, sizeof RunHeader->FileNumber,         fptr);
  }
#endif
  nread += fread( RunHeader->ProjectName,        1, SIZE_PROJECT_NAME,                    fptr);
  nread += fread( RunHeader->SourceName,         1, SIZE_SOURCE_NAME,                     fptr);
  nread += fread(&RunHeader->ObservationMode,    1, SIZE_OBSERVATION_MODE,                fptr);
  nread += fread(&RunHeader->SourceRA,           1, sizeof RunHeader->SourceRA,           fptr);
  nread += fread(&RunHeader->SourceDEC,          1, sizeof RunHeader->SourceDEC,          fptr);
  nread += fread(&RunHeader->TelescopeRA,        1, sizeof RunHeader->TelescopeRA,        fptr);
  nread += fread(&RunHeader->TelescopeDEC,       1, sizeof RunHeader->TelescopeDEC,       fptr);
  nread += fread( RunHeader->SourceEpochChar,    1, SIZE_SOURCE_EPOCH,                    fptr);
  nread += fread(&RunHeader->SourceEpochDate,    1, sizeof RunHeader->SourceEpochDate,    fptr);
  nread += fread(&RunHeader->NumChannels,        1, sizeof RunHeader->NumChannels,        fptr);
  nread += fread(&RunHeader->NumPixInChannel,    1, sizeof RunHeader->NumPixInChannel,    fptr);
#if (FORMAT_VERSION < 11)
  nread += fread(&RunHeader->Dummy,              1, sizeof RunHeader->Dummy,              fptr);
#endif
  nread += fread(&RunHeader->NumSamplesPerPixel, 1, sizeof RunHeader->NumSamplesPerPixel, fptr);
#if (FORMAT_VERSION >= 9)
  if (RunHeader->FormatVersion >= 9) {
    nread += fread(&RunHeader->NumSamplesRemovedHead, 1, sizeof RunHeader->NumSamplesRemovedHead, fptr);
    nread += fread(&RunHeader->NumSamplesRemovedTail, 1, sizeof RunHeader->NumSamplesRemovedTail, fptr);
  }
#endif
  nread += fread(&RunHeader->NumEvents,          1, sizeof RunHeader->NumEvents,          fptr);
#if (FORMAT_VERSION >= 9)
  if (RunHeader->FormatVersion >= 9) {
    nread += fread(&RunHeader->NumEventsRead,    1, sizeof RunHeader->NumEventsRead,      fptr);
  }
#endif
  nread += fread(&RunHeader->NumBytesPerSample,  1, sizeof RunHeader->NumBytesPerSample,  fptr);
  nread += fread(&RunHeader->SamplingFrequency,  1, sizeof RunHeader->SamplingFrequency,  fptr);
  nread += fread(&RunHeader->FadcResolution,     1, sizeof RunHeader->FadcResolution,     fptr);
  nread += fread(&RunHeader->StartYear,          1, sizeof RunHeader->StartYear,          fptr);
  nread += fread(&RunHeader->StartMonth,         1, sizeof RunHeader->StartMonth,         fptr);
  nread += fread(&RunHeader->StartDay,           1, sizeof RunHeader->StartDay,           fptr);
  nread += fread(&RunHeader->StartHour,          1, sizeof RunHeader->StartHour,          fptr);
  nread += fread(&RunHeader->StartMinute,        1, sizeof RunHeader->StartMinute,        fptr);
  nread += fread(&RunHeader->StartSecond,        1, sizeof RunHeader->StartSecond,        fptr);
  nread += fread(&RunHeader->EndYear,            1, sizeof RunHeader->EndYear,            fptr);
  nread += fread(&RunHeader->EndMonth,           1, sizeof RunHeader->EndMonth,           fptr);
  nread += fread(&RunHeader->EndDay,             1, sizeof RunHeader->EndDay,             fptr);
  nread += fread(&RunHeader->EndHour,            1, sizeof RunHeader->EndHour,            fptr);
  nread += fread(&RunHeader->EndMinute,          1, sizeof RunHeader->EndMinute,          fptr);
  nread += fread(&RunHeader->EndSecond,          1, sizeof RunHeader->EndSecond,          fptr);

  nPix = RunHeader->NumPixInChannel * RunHeader->NumChannels;
  
  RunHeader = realloc(RunHeader, RUN_HEADER_BASESIZE + nPix * sizeof *RunHeader->PixAssignment);

  nread += fread( RunHeader->PixAssignment,      sizeof *RunHeader->PixAssignment,  nPix, fptr); 

  return RunHeader;
}


int PrintRunHeader(run_header_str *RunHeader, FILE *fptr) {
  int RAhour, RAmin, RAsec;
  int DEChour, DECmin, DECsec;
  int nPix, iPix;

  fprintf(fptr, "MagicNumber:     0x%x\n", RunHeader->MagicNumber);
  fprintf(fptr, "FormatVersion:   %u\n", RunHeader->FormatVersion);
#if (FORMAT_VERSION >= 11)
  if (RunHeader->FormatVersion >= 11) {
    fprintf(fptr, "RunHeaderSize:     %u\n", RunHeader->RunHeaderSize);
    fprintf(fptr, "EventHeaderSize:   %u\n", RunHeader->EventHeaderSize);
    fprintf(fptr, "ChannelHeaderSize: %u\n", RunHeader->ChannelHeaderSize);  
  }
#endif
  fprintf(fptr, "SoftVersion:     %u\n", RunHeader->SoftVersion);
  fprintf(fptr, "FadcType:        %u\n", RunHeader->FadcType);
  fprintf(fptr, "CameraVersion:   %u\n", RunHeader->CameraVersion);
  fprintf(fptr, "TelescopeNumber: %u\n", RunHeader->TelescopeNumber);
  fprintf(fptr, "RunType:         %u\n", RunHeader->RunType);
  fprintf(fptr, "RunNumber:       %u\n", RunHeader->RunNumber);        
#if (FORMAT_VERSION >= 10)
  fprintf(fptr, "FileNumber:      %u\n", RunHeader->FileNumber);        
#endif
  fprintf(fptr, "ProjectName:     %s\n", RunHeader->ProjectName);
  fprintf(fptr, "SourceName:      %s\n", RunHeader->SourceName);
  fprintf(fptr, "ObservationMode: %s\n", RunHeader->ObservationMode);

  RAsec = (int)(RunHeader->SourceRA)%60;
  RAmin = (int)(RunHeader->SourceRA/60)%60;
  RAhour = (int)(RunHeader->SourceRA/3600);
  DECsec = (int)(RunHeader->SourceDEC)%60;
  DECmin = (int)(RunHeader->SourceDEC/60)%60;
  DEChour = (int)(RunHeader->SourceDEC/3600);
  fprintf(fptr, "SourceRA:         %f (%d %d %d)\n",
          RunHeader->SourceRA, RAhour, RAmin, RAsec);
  fprintf(fptr, "SourceDEC:        %f (%d %d %d)\n",
          RunHeader->SourceDEC, DEChour, DECmin, DECsec);
  RAsec = (int)(RunHeader->TelescopeRA)%60;
  RAmin = (int)(RunHeader->TelescopeRA/60)%60;
  RAhour = (int)(RunHeader->TelescopeRA/3600);
  DECsec = (int)(RunHeader->TelescopeDEC)%60;
  DECmin = (int)(RunHeader->TelescopeDEC/60)%60;
  DEChour = (int)(RunHeader->TelescopeDEC/3600);
  fprintf(fptr, "TelescopeRA:      %f (%d %d %d)\n", 
	  RunHeader->TelescopeRA, RAhour, RAmin, RAsec);
  fprintf(fptr, "TelescopeDEC:     %f (%d %d %d)\n", 
	  RunHeader->TelescopeDEC, DEChour, DECmin, DECsec);

  fprintf(fptr, "SourceEpochChar:    %s\n", RunHeader->SourceEpochChar);
  fprintf(fptr, "SourceEpochDate:    %u\n", RunHeader->SourceEpochDate);
  fprintf(fptr, "NumChannels:        %u\n", RunHeader->NumChannels);
  fprintf(fptr, "NumPixInChannel:    %u\n", RunHeader->NumPixInChannel);
#if (FORMAT_VERSION < 11)
  fprintf(fptr, "Dummy:              %x\n", RunHeader->Dummy);  
#endif
  fprintf(fptr, "NumSamplesPerPixel: %u\n", RunHeader->NumSamplesPerPixel);
#if (FORMAT_VERSION >= 9)
  if (RunHeader->FormatVersion >= 9) {
    fprintf(fptr, "NumSamplesRemovedHead: %u\n", RunHeader->NumSamplesRemovedHead);
    fprintf(fptr, "NumSamplesRemovedTail: %u\n", RunHeader->NumSamplesRemovedTail);
  }
#endif
  fprintf(fptr, "NumEvents:          %u\n", RunHeader->NumEvents);
#if (FORMAT_VERSION >= 9)
  if (RunHeader->FormatVersion >= 9) {
    fprintf(fptr, "NumEventsRead:      %u\n", RunHeader->NumEventsRead);
  }
#endif
  fprintf(fptr, "NumBytesPerSample:  %u\n", RunHeader->NumBytesPerSample);
  fprintf(fptr, "SamplingFrequency:  %u\n", RunHeader->SamplingFrequency);
  fprintf(fptr, "FadcResolution:     %u\n", RunHeader->FadcResolution);
  fprintf(fptr, "StartYear:        %u\n", RunHeader->StartYear);
  fprintf(fptr, "StartMonth:       %u\n", RunHeader->StartMonth);
  fprintf(fptr, "StartDay:         %u\n", RunHeader->StartDay);
  fprintf(fptr, "StartHour:        %u\n", RunHeader->StartHour);
  fprintf(fptr, "StartMinute:      %u\n", RunHeader->StartMinute);
  fprintf(fptr, "StartSecond:      %u\n", RunHeader->StartSecond);
  fprintf(fptr, "EndYear:          %u\n", RunHeader->EndYear);
  fprintf(fptr, "EndMonth:         %u\n", RunHeader->EndMonth);
  fprintf(fptr, "EndDay:           %u\n", RunHeader->EndDay);
  fprintf(fptr, "EndHour:          %u\n", RunHeader->EndHour);
  fprintf(fptr, "EndMinute:        %u\n", RunHeader->EndMinute);
  fprintf(fptr, "EndSecond:        %u\n", RunHeader->EndSecond);

  nPix = RunHeader->NumPixInChannel * RunHeader->NumChannels;
  fprintf(fptr,"PixelAssignment: \n");
  for (iPix=0; iPix<nPix; iPix++) {
    fprintf(fptr, "%4d:%4d", iPix, (short)RunHeader->PixAssignment[iPix]);
    if (iPix%8 == 7) {
      fprintf(fptr, "\n");
    }
  }
  return 0;
} 


int fReadEventHeader (event_header_str *EventHeader, FILE *fptr) {
  int nread = 0;
 
  nread += fread(&EventHeader->EvtNumber,        1, sizeof EventHeader->EvtNumber,     fptr);
  nread += fread(&EventHeader->TimeSec,          1, sizeof EventHeader->TimeSec,          fptr);
  nread += fread(&EventHeader->TimeSubSec,       1, sizeof EventHeader->TimeSubSec,       fptr);
  nread += fread(&EventHeader->FstLvlTrigNumber, 1, sizeof EventHeader->FstLvlTrigNumber, fptr);
  nread += fread(&EventHeader->SecLvlTrigNumber, 1, sizeof EventHeader->SecLvlTrigNumber, fptr);
  nread += fread(&EventHeader->TrigPattern,      1, sizeof EventHeader->TrigPattern,      fptr);
  nread += fread(&EventHeader->CalPattern,       1, sizeof EventHeader->CalPattern,       fptr);
  nread += fread(&EventHeader->TriggerType,      1, sizeof EventHeader->TriggerType,      fptr);
 
  return nread;
}

int fWriteEventHeader (event_header_str *EventHeader, FILE *fptr) {

  fwrite(&EventHeader->EvtNumber,        sizeof EventHeader->EvtNumber,        1, fptr);
  fwrite(&EventHeader->TimeSec,          sizeof EventHeader->TimeSec,          1, fptr);
  fwrite(&EventHeader->TimeSubSec,       sizeof EventHeader->TimeSubSec,       1, fptr);
  fwrite(&EventHeader->FstLvlTrigNumber, sizeof EventHeader->FstLvlTrigNumber, 1, fptr);
  fwrite(&EventHeader->SecLvlTrigNumber, sizeof EventHeader->SecLvlTrigNumber, 1, fptr);
  fwrite(&EventHeader->TrigPattern,      sizeof EventHeader->TrigPattern,      1, fptr);
  fwrite(&EventHeader->CalPattern,       sizeof EventHeader->CalPattern,       1, fptr);
  fwrite(&EventHeader->TriggerType,      sizeof EventHeader->TriggerType,      1, fptr);
 
  return 0;
}


int PrintEventHeader(event_header_str *EventHeader, FILE *fptr) {
  int h,m,s,ns;

  decode_sec(EventHeader->TimeSec, &h, &m, &s);
  ns = decode_subsec(EventHeader->TimeSubSec);

  fprintf(fptr, "\nEvent Header:\n");
  fprintf(fptr, "=============\n");
  fprintf(fptr, "EvtNumber: %u\n",         EventHeader->EvtNumber);


  fprintf(fptr, "TimeSec: 0x%08x   ( %02d %02d %02d )\n",
	  EventHeader->TimeSec, h, m, s);
  fprintf(fptr, "TimeSubSec: 0x%06x  ( %09d )\n",
	  EventHeader->TimeSubSec, ns);

  fprintf(fptr, "FstLvlTrigNumber: %u\n", EventHeader->FstLvlTrigNumber);
  fprintf(fptr, "SecLvlTrigNumber: %u\n", EventHeader->SecLvlTrigNumber);


  fprintf(fptr, "TrigPattern: 0x%0x\n",   EventHeader->TrigPattern);

  fprintf(fptr, "CalPattern:  0x%0x\n",   EventHeader->CalPattern);
  fprintf(fptr, "TriggerType: 0x%0x\n",   EventHeader->TriggerType);

  return 0;
}

int fReadChannelHeader (channel_header_str *ChannelHeader, FILE *fptr) {
  int nread = 0;
 
  nread += fread(&ChannelHeader->DummyCrateNumber,   1, sizeof ChannelHeader->DummyCrateNumber,   fptr);
  nread += fread(&ChannelHeader->BoardNumber,        1, sizeof ChannelHeader->BoardNumber,   fptr);
  nread += fread(&ChannelHeader->ChannelNumber,      1, sizeof ChannelHeader->ChannelNumber, fptr);
  nread += fread(&ChannelHeader->ChannelEvtNumber,   1, sizeof ChannelHeader->ChannelEvtNumber, fptr);
  //  nread += fread(&ChannelHeader->FadcClockTickHi,    1, sizeof ChannelHeader->FadcClockTickHi, fptr);
  //  nread += fread(&ChannelHeader->FadcClockTickLo,    1, sizeof ChannelHeader->FadcClockTickLo, fptr);
  nread += fread(&ChannelHeader->FadcClockTick,      1, sizeof ChannelHeader->FadcClockTick, fptr);
  nread += fread(&ChannelHeader->FadcTrgNumber,      1, sizeof ChannelHeader->FadcTrgNumber, fptr);
  nread += fread(&ChannelHeader->TrigTimeInterpol,   1, sizeof ChannelHeader->TrigTimeInterpol, fptr);
  // diego CHANGE in the DATA FORMAT
  nread += fread(&ChannelHeader->FirstCapacitor,     1, sizeof ChannelHeader->FirstCapacitor, fptr);

  return nread;
}

int PrintChannelHeader(channel_header_str *ChannelHeader, FILE *fptr) {
 
  // Clock Tick in 100ns => according to 10MHz clock
  //  U32 ClockTick10MHz = (U32)(((double)(ChannelHeader->FadcClockTickHi)
  //			 * ((U32)(-1))+ChannelHeader->FadcClockTickLo)/1e5);

  //  fprintf(fptr, "Crate/Board/ChannelNumber: %3u %3u %3u\n", 
  //	  ChannelHeader->CrateNumber, ChannelHeader->BoardNumber, ChannelHeader->ChannelNumber);
  fprintf(fptr, "Board/ChannelNumber: %3u %3u\n", 
	  ChannelHeader->BoardNumber, ChannelHeader->ChannelNumber);
  //  fprintf(fptr, "CrateEvtNumber: %8u\n", ChannelHeader->CrateEvtNumber);
  //  fprintf(fptr, "FadcClockTickHi/Lo: 0x%x 0x%x ( 0x%x )\n", 
  //	  ChannelHeader->FadcClockTickHi, ChannelHeader->FadcClockTickLo, ClockTick10MHz);
  fprintf(fptr, "FadcClockTick: 0x%x  FadcTrgNumber: 0x%x\n", ChannelHeader->FadcClockTick,  ChannelHeader->FadcTrgNumber);
  fprintf(fptr, "TrigTimeInterpol: %8u (0x%x)\n", ChannelHeader->TrigTimeInterpol, ChannelHeader->TrigTimeInterpol);
  // diego CHANGE in the DATA FORMAT
  fprintf(fptr, "FirstCapacitor: %8u\n", ChannelHeader->FirstCapacitor);

  return 0;
}

int decode_sec(U32 pattern_sec, int *h, int *m, int *s) {
  
  pattern_sec =
    (pattern_sec>>7 & 0x00010000) |
    (pattern_sec>>5 & 0x00020000) |
    (pattern_sec>>3 & 0x00040000) |
    (pattern_sec>>1 & 0x00080000) |
    (pattern_sec    & 0x0000ffff);

  *h  = (pattern_sec>>18 & 0x3)*10 + (pattern_sec>>14 & 0xf);
  *m  = (pattern_sec>>11 & 0x7)*10 + (pattern_sec>> 7 & 0xf);
  *s  = (pattern_sec>> 4 & 0x7)*10 + (pattern_sec>> 0 & 0xf);

  return ((*h*60+*m)*60+*s);  /* seconds of day */
}


int decode_subsec(U32 pattern_subsec) {

  pattern_subsec = 
      (~(pattern_subsec) & 0x000000ff) |
      (pattern_subsec    & 0xffffff00);

  return pattern_subsec*200;  /* in nsec */
}
