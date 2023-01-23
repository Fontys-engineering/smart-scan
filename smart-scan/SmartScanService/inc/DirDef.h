#pragma once

#include <filesystem>
#include <fstream>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~Directory Definitions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define DIR_FILTER_PLUGINS			"./plugins/"
#define DIR_CONFIG					"./configs/"
#define DIR_SETTINGS				"./configs/settings.ini"
#define DIR_TEMP					"./temp/"
#define DIR_SCANS					"./scans/"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~INI sections~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define SECTION_SYSTEM				"system"
#define SECTION_SENSOR				"sensor"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~INI parameters~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define PARAM_SYS_TRANS_ID			"transmitterID"
#define PARAM_SYS_MEAS_RATE			"measurementRate"
#define PARAM_SYS_PWR_LINE_FREQ		"powerLineFreq"
#define PARAM_SYS_MAX_RANGE			"maxRangeInch"

#define PARAM_SNSR_REF_SERIAL		"reference"
#define PARAM_SNSR_THUMB_SERIAL		"thumb"
#define PARAM_SNSR_INDEX_SERIAL		"index"
#define PARAM_SNSR_MIDDLE_SERIAL	"middle"
#define PARAM_SNSR_AZIMUTH_OFFSET	"offsetAzimuth"
#define PARAM_SNSR_ELEVATION_OFFSET	"offsetElevation"
#define PARAM_SNSR_ROLL_OFFSET		"offsetRoll"