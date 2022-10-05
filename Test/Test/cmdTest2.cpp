////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN Stopscan command
//

#include "StdAfx.h"
#include "TestPlugIn.h"
#include "../../smart-scan/SmartScanService/inc/SmartScanService.h"
#include "../../smart-scan/SmartScanService/inc/Point3.h"

#pragma region Stopscan command

class CCommandStopscan : public CRhinoCommand {
public:
	CCommandStopscan() = default;
	UUID CommandUUID() override {
		// {E6F6A722-BB47-4AD1-91F0-BB6E66D9E1C9}
		static const GUID StopscanCommand_UUID =
		{ 0xE6F6A722, 0xBB47, 0x4AD1, { 0x91, 0xF0, 0xBB, 0x6E, 0x66, 0xD9, 0xE1, 0xC9 } };
		return StopscanCommand_UUID;
	}
	const wchar_t* EnglishCommandName() override { return L"Stopscan"; }
	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

// The one and only CCommandStopscan object
static class CCommandStopscan theStopscanCommand;

extern SmartScan::SmartScanService obj;
int Stopcommand=0;

CRhinoCommand::result CCommandStopscan::RunCommand(const CRhinoCommandContext& context) {
	Stopcommand = 1;
	obj.StopScan();

	return CRhinoCommand::success;
}

#pragma endregion

//
// END Stopscan command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
