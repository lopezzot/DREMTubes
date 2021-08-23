#include <stdio.h>
#include <CAENDigitizer.h>
#include "WaveDump.h"

int32_t ParseConfigFile(const char* ConfigFileName, WaveDumpConfig_t* WDcfg) 
{
	char str[400], str1[400];
	int32_t i;

        /* Open and parse configuration file */
        FILE * f_ini = fopen(ConfigFileName, "r");
        if (f_ini == NULL ) return 1;

	/* Default settings */
	memset ( WDcfg, 0, sizeof( *WDcfg));

	WDcfg->AcquisitionMode = CAEN_DGTZ_SW_CONTROLLED;
	WDcfg->RecordLength = (1024*16);
	WDcfg->PostTrigger = 80;
	WDcfg->NumEvents = 1;
	WDcfg->ChannelEnableMask = 0x1FF;
	WDcfg->GroupEnableMask= 0x01;
	WDcfg->GWn = 0;

	for(i=0; i<MAX_CH; i++) {
		WDcfg->DCoffset[i] = 0x8000;
	}

	/* read config file and assign parameters */
	while(!feof(f_ini)) {
		int32_t dummy;
		int32_t read;
		read= fscanf(f_ini, "%s", str);
		if( !read|| ( read== EOF)) {
			continue;
		}
		if ((str[0] == '#')|| !strlen( str)) {
			fgets(str, 1000, f_ini);
			continue;
		}
		// OPEN: read the details of physical path to the digitizer
		if (strstr(str, "OPEN")!=NULL) {
			fscanf(f_ini, "%s", str1);
			if (strcmp(str1, "USB")==0)
				WDcfg->LinkType = CAEN_DGTZ_USB;
			else if (strcmp(str1, "PCI")==0)
				WDcfg->LinkType = CAEN_DGTZ_PCI_OpticalLink;
			else
				return -1; 
			fscanf(f_ini, "%d", &WDcfg->LinkNum);
			fscanf(f_ini, "%d", &WDcfg->ConetNode);
			fscanf(f_ini, "%x", &WDcfg->BaseAddress);
			continue;
		}

		// Generic VME Write
		if ((strstr(str, "WRITE_REGISTER")!=NULL) && (WDcfg->GWn < MAX_GW)) {
			fscanf(f_ini, "%x", (int32_t*)&WDcfg->GWaddr[WDcfg->GWn]);
			fscanf(f_ini, "%x", (int32_t*)&WDcfg->GWdata[WDcfg->GWn]);
			WDcfg->GWn++;
			continue;
		}

		// Acquisiton mode
		if (strstr(str, "ACQUISITION_MODE")!=NULL) {
			fscanf(f_ini, "%s", str1);
			if (strcmp(str1, "CAEN_DGTZ_SW_CONTROLLED")==0)
				WDcfg->AcquisitionMode = CAEN_DGTZ_SW_CONTROLLED;
			else if (strcmp(str1, "CAEN_DGTZ_S_IN_CONTROLLED")==0)
				WDcfg->AcquisitionMode = CAEN_DGTZ_S_IN_CONTROLLED;
			continue;
		}

		// DC offset
		if (strstr(str, "DC_OFFSET")!=NULL) {
			int32_t ch;
			fscanf(f_ini, "%d", &ch);
			fscanf(f_ini, "%d", &WDcfg->DCoffset[ch]);
			continue;
		}

		// Acquisition Record Length (in number of samples)
		if (strstr(str, "RECORD_LENGTH")!=NULL) {
			fscanf(f_ini, "%d", &WDcfg->RecordLength);
			continue;
		}
		// Max. number of events for a block transfer
		if (strstr(str, "MAX_NUM_EVENTS_BLT")!=NULL) {
			fscanf(f_ini, "%d", &WDcfg->NumEvents);
			continue;
		}
		// Post Trigger (percent of the acquisition window)
		if (strstr(str, "POST_TRIGGER")!=NULL) {
			fscanf(f_ini, "%d", &WDcfg->PostTrigger);
			continue;
		}
		// Channel Enable Mask (or Group enable mask for the V1740)
		if (strstr(str, "CHANNEL_ENABLE")!=NULL) {
			fscanf(f_ini, "%x", &dummy);
			WDcfg->ChannelEnableMask= (uint16_t)(dummy & 0xffff);
			WDcfg->GroupEnableMask= (uint16_t)(dummy & 0xffff);
			continue;
		}
		// Save output file in BINARY format (otherwise the format is ASCII)
		if (strstr(str, "BINARY_OUTPUT_FILE")!=NULL) {
			uint32_t q = WDcfg->OutFileFlags | OFF_BINARY;
			WDcfg->OutFileFlags = static_cast<OUTFILE_FLAGS>(q);
			// WDcfg->OutFileFlags|= OFF_BINARY;
			continue;
		}
		// Save raw data into output file (otherwise only samples data will be saved)
		if (strstr(str, "SAVE_CH_HEADER")!=NULL) {
			uint32_t q = WDcfg->OutFileFlags | OFF_HEADER;
			WDcfg->OutFileFlags = static_cast<OUTFILE_FLAGS>(q);
			// WDcfg->OutFileFlags|= OFF_HEADER;
			continue;
		}
		// Use wait for interrupt
		/*
			syntax: USE_INTERRUPT level status_id event_number mode mask timeout
			where :
				level : the interrupt level (meaningful for VME devices only )
				status_id : the status id (meaningful for VME devices only )
				event_number : the number of events to wait for (>= 1)
				mode : 0 = RORA , 1 = ROAK
				mask : wait mask (hex format) (meaningful for VME devices only )
				timeout : wait timeout (msec)
		*/
		if (strstr(str, "USE_INTERRUPT")!=NULL) {
			WDcfg->Interrupt.Enable= 1;
			// level
			fscanf(f_ini, "%d", &WDcfg->Interrupt.Level);
			// status_id
			fscanf(f_ini, "%d", &WDcfg->Interrupt.StatusId);
			// event_number
			fscanf(f_ini, "%d", &WDcfg->Interrupt.EventNumber);
			// mode
			fscanf(f_ini, "%d", &dummy);
			WDcfg->Interrupt.Mode= dummy? CAEN_DGTZ_IRQ_MODE_ROAK: CAEN_DGTZ_IRQ_MODE_RORA;
			// mask
			fscanf(f_ini, "%x", &dummy);
			WDcfg->Interrupt.Mask= dummy;
			// Timeout
			fscanf(f_ini, "%d", &WDcfg->Interrupt.Timeout);
			continue;
		}
	}

        fclose(f_ini);

	return 0;
}
