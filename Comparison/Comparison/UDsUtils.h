//
// UDsUtils.h
//

#ifndef __H_UDsUtils__
#define __H_UDsUtils__

//#include "CDSDevice.h"
#include <streams.h>
class UDsUtils
{
public:
	UDsUtils() {};
	~UDsUtils() {};

//	static BOOL QueryVideoCaptureDevices(DEVICE_LIST & outDevices);
//	static BOOL QueryAudioCaptureDevices(DEVICE_LIST & outDevices);
//	static Device_Type DecideDeviceType(IBaseFilter * inFilter);
	// DV: "Microsoft DV Camera and VCR"
//	static IBaseFilter * CreateVideoDevice(const char * inFriendlyName);
//	static IBaseFilter * CreateAudioDevice(const char * inFriendlyName);
	// Create device according to the display name
	// As a bonus, return the friendly name too.
//	static IBaseFilter * CreateDevice(BOOL inIsVideo, const char * inDisplayame,
//		char * outFriendlyName = NULL);
//	static IBaseFilter * CreateCompressor(BOOL inIsVideo, const char * inName);
//	static IBaseFilter * CreateHardwareFilter2(GUID inCategory, 
//		const char * inFriendlyName);

	static BOOL IsSameDevice(const char * inDeviceName1, const char * inDeviceName2);
//	static IBaseFilter * FindFilterByCLSID(IGraphBuilder * inGraph, REFCLSID inClsid);

	static BOOL IsFilterRegistered(CLSID inFilterId);
	static BOOL RegisterFilter(const char * inFilterAx);

	static void NukeDownstream(IGraphBuilder * inGraph, IBaseFilter * inFilter);
	static void NukeUpstream(IGraphBuilder * inGraph, IBaseFilter * inFilter);
	static BOOL ShowFilterPropertyPage(IBaseFilter * inFilter);

	static BOOL QueryDeviceCategory(GUID inCategory,char ppDisplayName[][1024],char* pFriendlyName);
	static IBaseFilter * CreateHardwareFilter(GUID inCategory, const char * inDisplayName, char * outFriendlyName);
};

#endif // __H_UDsUtils__