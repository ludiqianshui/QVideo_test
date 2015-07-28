//
// UDsUtils.cpp
//

/*-----------------------------------------------------*\
			HQ Tech, Make Technology Easy!       
 More information, please go to http://hqtech.nease.net.
/*-----------------------------------------------------*/

#include "stdafx.h"
#include <streams.h>
#include "UDsUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/*
BOOL UDsUtils::QueryVideoCaptureDevices(DEVICE_LIST & outDevices)
{
	return QueryDeviceCategory(CLSID_VideoInputDeviceCategory, outDevices);
}

BOOL UDsUtils::QueryAudioCaptureDevices(DEVICE_LIST & outDevices)
{
	return QueryDeviceCategory(CLSID_AudioInputDeviceCategory, outDevices);
}
*/
BOOL UDsUtils::QueryDeviceCategory(GUID inCategory, char ppDisplayName[][1024],char* pFriendlyName)
{
	HRESULT hr = NOERROR;
	ICreateDevEnum * enumHardware = NULL;
	int i=0;
	
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_ALL,	IID_ICreateDevEnum, (void**)&enumHardware);
	if (FAILED(hr))
	{
		return FALSE;
	}

	IEnumMoniker *  enumMoniker = NULL;
	hr = enumHardware->CreateClassEnumerator(inCategory, &enumMoniker, 0);
	if (enumMoniker)
	{
		enumMoniker->Reset();

		char  displayName[1024];
		char  friendlyName[256];
		ULONG fetched = 0;
		IMoniker * moniker = NULL;

		while(SUCCEEDED(enumMoniker->Next(1, &moniker, &fetched)) && fetched && i<2)
		{
			if (moniker)
			{
				WCHAR * wzDisplayName      = NULL;
				IPropertyBag * propertyBag = NULL;
				IBaseFilter *  filter      = NULL;
				VARIANT        name;

				hr = moniker->GetDisplayName(NULL, NULL, &wzDisplayName);
				if (SUCCEEDED(hr))
				{					
					WideCharToMultiByte(CP_ACP, 0, wzDisplayName, -1, displayName, 1024, "", NULL);
					CoTaskMemFree(wzDisplayName);
					hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propertyBag);
				}				
				if (SUCCEEDED(hr))
				{
					name.vt = VT_BSTR;
					hr = propertyBag->Read(L"FriendlyName", &name, NULL);
				}
				if (SUCCEEDED(hr))
				{
					friendlyName[0] = 0;
					WideCharToMultiByte(CP_ACP, 0, name.bstrVal, -1,friendlyName, 256, NULL, NULL);
					hr = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&filter);
					if(strcmp(friendlyName,pFriendlyName)==0)
					{
						strcpy(ppDisplayName[i],displayName);
						i++;
					}
				}

				// Release interfaces
				if (filter)
				{
					filter->Release();
					filter = NULL;
				}
				if (propertyBag)
				{
					propertyBag->Release();
					propertyBag = NULL;
				}
				moniker->Release();
			}
		}
		enumMoniker->Release();
	}
	enumHardware->Release();

	return TRUE;
}


/*
Device_Type UDsUtils::DecideDeviceType(IBaseFilter * inFilter)
{
	// Check if DV device
	IAMExtTransport * pAMExtTransPost = NULL;
	inFilter->QueryInterface(IID_IAMExtTransport, (void **)&pAMExtTransPost);
	if (pAMExtTransPost)
	{
		pAMExtTransPost->Release();
		return DT_DV;
	}

	// Check if WDM analog
	IAMAnalogVideoDecoder * pDecoder = NULL;
	inFilter->QueryInterface(IID_IAMAnalogVideoDecoder, (void **)&pDecoder);
	if (pDecoder)
	{
		pDecoder->Release();
		return DT_Analog_WDM;
	}

	// Check if VFW analog
	IAMVfwCaptureDialogs * pVfwDlgs = NULL;
	inFilter->QueryInterface(IID_IAMVfwCaptureDialogs, (void **)&pVfwDlgs);
	if (pVfwDlgs)
	{
		pVfwDlgs->Release();
		return DT_Analog_VFW;
	}

	// Check if audio capture device
	IAMAudioInputMixer * pAudioMixer = NULL;
	inFilter->QueryInterface(IID_IAMAudioInputMixer, (void **)&pAudioMixer);
	if (pAudioMixer)
	{
		pAudioMixer->Release();
		return DT_Analog_Audio;
	}
	 
	return DT_Unknown;
}
*/

/*
IBaseFilter * UDsUtils::CreateVideoDevice(const char * inFriendlyName)
{
	//return CreateHardwareFilter(CLSID_VideoInputDeviceCategory, inFriendlyName);
	return CreateHardwareFilter(CLSID_VideoInputDeviceCategory,inFriendlyName,NULL );
}

IBaseFilter * UDsUtils::CreateAudioDevice(const char * inFriendlyName)
{
	return CreateHardwareFilter2(CLSID_AudioInputDeviceCategory, inFriendlyName);

}

// Create device according to the display name
IBaseFilter * UDsUtils::CreateDevice(BOOL inIsVideo, const char * inDisplayName,
									 char * outFriendlyName)
{
	GUID guid = inIsVideo ? CLSID_VideoInputDeviceCategory : CLSID_AudioInputDeviceCategory;
	return CreateHardwareFilter(guid, inDisplayName, outFriendlyName);
}

IBaseFilter * UDsUtils::CreateCompressor(BOOL inIsVideo, const char * inName)
{
	GUID guid = inIsVideo ? CLSID_VideoCompressorCategory : CLSID_AudioCompressorCategory;
	return CreateHardwareFilter2(guid, inName);
}

IBaseFilter * UDsUtils::CreateHardwareFilter2(GUID inCategory, const char * inFriendlyName)
{
	ICreateDevEnum * enumHardware = NULL;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_ALL,
		IID_ICreateDevEnum, (void**)&enumHardware);
	if (FAILED(hr))
	{
		return NULL;
	}

	IBaseFilter * hardwareFilter = NULL;
	IEnumMoniker * enumMoniker   = NULL;
	hr = enumHardware->CreateClassEnumerator(inCategory, &enumMoniker, 0);
	if (enumMoniker)
	{
		enumMoniker->Reset();

		ULONG      fetched = 0;
		IMoniker * moniker = NULL;
		char friendlyName[256];

		while (!hardwareFilter && SUCCEEDED(enumMoniker->Next(1, &moniker, &fetched)) && fetched)
		{
			if (moniker)
			{
				IPropertyBag * propertyBag = NULL;
				VARIANT        name;
				friendlyName[0] = 0;
				hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propertyBag);
				if (SUCCEEDED(hr))
				{					
					name.vt = VT_BSTR;
					hr = propertyBag->Read(L"FriendlyName", &name, NULL);
				}
				if (SUCCEEDED(hr))
				{
					WideCharToMultiByte(CP_ACP, 0, name.bstrVal, -1,
						friendlyName, 256, NULL, NULL);
					if (strcmp(friendlyName, inFriendlyName) == 0) 
					{
						moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&hardwareFilter);
					}
				}

				// Release interfaces
				if (propertyBag)
				{
					propertyBag->Release();
					propertyBag = NULL;
				}
				moniker->Release();
			}			
		}
		enumMoniker->Release();
	}
	enumHardware->Release();

	return hardwareFilter;
}*/


IBaseFilter * UDsUtils::CreateHardwareFilter(GUID inCategory, 
											  const char * inDisplayName,
											  char * outFriendlyName)
{
	
	ICreateDevEnum * enumHardware = NULL;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_ALL,
		IID_ICreateDevEnum, (void**)&enumHardware);
	if (FAILED(hr))
	{
		return NULL;
	}

	IBaseFilter * hardwareFilter = NULL;
	IEnumMoniker * enumMoniker   = NULL;
	hr = enumHardware->CreateClassEnumerator(inCategory, &enumMoniker, 0);
	if (enumMoniker&&SUCCEEDED(hr))
	{
		enumMoniker->Reset();

		ULONG      fetched = 0;
		IMoniker * moniker = NULL;
		char displayName[1024];

		while (!hardwareFilter && SUCCEEDED(enumMoniker->Next(1, &moniker, &fetched)) && fetched)
		{
			if (moniker)
			{
				WCHAR * wzDisplayName = NULL;
				// Get display name
				hr = moniker->GetDisplayName(NULL, NULL, &wzDisplayName);
				if (SUCCEEDED(hr))
				{					
					WideCharToMultiByte(CP_ACP, 0, wzDisplayName, -1, displayName, 1024, "", NULL);
					CoTaskMemFree(wzDisplayName);

					if (IsSameDevice(displayName, inDisplayName)) 
					{
						moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&hardwareFilter);
					}
				}

				// Get the friendly name
  				if (outFriendlyName && hardwareFilter)
				{
					IPropertyBag * propertyBag = NULL;
					outFriendlyName[0] = 0;
					if (SUCCEEDED(moniker->BindToStorage(0, 0, IID_IPropertyBag, 
						(void **)&propertyBag)))
					{
						VARIANT name;
						name.vt = VT_BSTR;
						if (SUCCEEDED(propertyBag->Read(L"FriendlyName", &name, NULL)))
						{
							WideCharToMultiByte(CP_ACP, 0, name.bstrVal, -1,
								outFriendlyName, 1024, NULL, NULL);
						}
						propertyBag->Release();
					}
				}
				moniker->Release();
			}			
		}
		enumMoniker->Release();
	}
	enumHardware->Release();
	
	return hardwareFilter;
}


// Given the displayname of devices, determine if they are the same one
BOOL UDsUtils::IsSameDevice(const char * inDeviceName1, const char * inDeviceName2)
{
	CString temp1, temp2, deviceName1, deviceName2;

	temp1 = inDeviceName1;
	temp2 = inDeviceName2;

	int backSlash1 = temp1.Find("\\\\", 0);
	int backSlash2 = temp2.Find("\\\\", 0);

	if (backSlash1 >= 0 && backSlash2 >= 0)
	{
		deviceName1 = temp1.Right(temp1.GetLength() - backSlash1);
		deviceName2 = temp2.Right(temp2.GetLength() - backSlash2);

		return (deviceName1.CompareNoCase(deviceName2) == 0);
	}
	else
	{
		return (temp1.CompareNoCase(temp2) == 0);
	}
	return FALSE;
}

/*
IBaseFilter * UDsUtils::FindFilterByCLSID(IGraphBuilder * inGraph, REFCLSID inClsid)
{
	IBaseFilter *  foundFilter = NULL;

	IEnumFilters * pFilters = NULL;
	inGraph->EnumFilters(&pFilters);
	if (pFilters)
	{
		IBaseFilter *  pFilter  = NULL;
		IPersist *     pPersist = NULL;
		CLSID          clsid;
		ULONG fetchCount = 0;
		while (S_OK == pFilters->Next(1, &pFilter, &fetchCount) && fetchCount && !foundFilter)
		{
			HRESULT hr = pFilter->QueryInterface(IID_IPersist, (void**)&pPersist);
			if (SUCCEEDED(hr))
			{
				pPersist->GetClassID(&clsid);
				pPersist->Release();
				if (inClsid == clsid)
				{
					pFilter->AddRef();
					foundFilter = pFilter;
				}
			}
			pFilter->Release();
		}
		pFilters->Release();
	}

	return foundFilter;
}*/


// Determine if the specified filter has been registered
BOOL UDsUtils::IsFilterRegistered(CLSID inFilterId)
{
	IBaseFilter * pFilter = NULL;
	if (SUCCEEDED(CoCreateInstance(inFilterId, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void **)&pFilter)))
	{
		pFilter->Release();
		return TRUE;
	}
	return FALSE;
}

// DllRegisterServer
BOOL UDsUtils::RegisterFilter(const char * inFilterAx)
{
	typedef int (WINAPI * REGISTER_FUNC) (void);
	REGISTER_FUNC   MyFunc = NULL;

	HMODULE hModule = ::LoadLibrary(inFilterAx);
	if (hModule)
	{
		MyFunc = (REGISTER_FUNC) GetProcAddress(hModule, "DllRegisterServer");
		BOOL pass = (MyFunc != NULL);
		if (pass)
		{
			MyFunc();
		}
		::FreeLibrary(hModule);
		return pass;
	}

	return FALSE;
}


void UDsUtils::NukeDownstream(IGraphBuilder * inGraph, IBaseFilter * inFilter) 
{
	if (!inGraph || !inFilter)
	{
		return ;
	}

	IEnumPins * pinEnum = NULL;
	if (SUCCEEDED(inFilter->EnumPins(&pinEnum)))
	{
		pinEnum->Reset();

		IPin * pin     = NULL;
		ULONG  fetched = 0;
		while (SUCCEEDED(pinEnum->Next(1, &pin, &fetched)) && fetched)
		{
			if (pin)
			{
				IPin * connectedPin = NULL;
				pin->ConnectedTo(&connectedPin);
				if (connectedPin) 
				{
					PIN_INFO pininfo;
					if (SUCCEEDED(connectedPin->QueryPinInfo(&pininfo)))
					{
						pininfo.pFilter->Release();
						if (pininfo.dir == PINDIR_INPUT) 
						{
							NukeDownstream(inGraph, pininfo.pFilter);
							inGraph->Disconnect(connectedPin);
							inGraph->Disconnect(pin);
							inGraph->RemoveFilter(pininfo.pFilter);
						}
					}
					connectedPin->Release();
				}
				pin->Release();
			}
		}
		pinEnum->Release();
	}
}

void UDsUtils::NukeUpstream(IGraphBuilder * inGraph, IBaseFilter * inFilter) 
{
	if (inGraph && inFilter)
	{
		IEnumPins * pinEnum = NULL;
		if (SUCCEEDED(inFilter->EnumPins(&pinEnum)))
		{
			pinEnum->Reset();

			IPin * pin     = NULL;
			ULONG  fetched = 0;
			BOOL   pass    = TRUE;
			while (pass && SUCCEEDED(pinEnum->Next(1, &pin, &fetched)) && fetched)
			{
				if (pin)
				{
					IPin * connectedPin = NULL;
					pin->ConnectedTo(&connectedPin);
					if(connectedPin) 
					{
						PIN_INFO pininfo;
						if (SUCCEEDED(connectedPin->QueryPinInfo(&pininfo)))
						{
							if(pininfo.dir == PINDIR_OUTPUT) 
							{
								NukeUpstream(inGraph, pininfo.pFilter);
								inGraph->Disconnect(connectedPin);
								inGraph->Disconnect(pin);
								inGraph->RemoveFilter(pininfo.pFilter);
							}
							pininfo.pFilter->Release();
						}
						connectedPin->Release();
					}
					pin->Release();
				}
				else
				{
					pass = FALSE;
				}
			}
			pinEnum->Release();
		}
	}
}


BOOL UDsUtils::ShowFilterPropertyPage(IBaseFilter * inFilter)
{
	if (!inFilter)
	{
		return FALSE;
	}

	ISpecifyPropertyPages * pSpecify;
	HRESULT hr = inFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);
	if (SUCCEEDED(hr))
	{
		CAUUID   caGUID;
		pSpecify->GetPages(&caGUID);
		pSpecify->Release();

		OleCreatePropertyFrame(
			::GetActiveWindow(),        // Parent window
			0,                          // x (Reserved)
			0,                          // y (Reserved)
			NULL,                       // Caption for the dialog box
			1,                          // Number of filters
			(IUnknown **) &inFilter,    // Pointer to the filter
			caGUID.cElems,              // Number of property pages
			caGUID.pElems,              // Pointer of property page CLSIDs
			0,                          // Locale identifier
			0,                          // Reserved
			NULL                        // Reserved
			);
		CoTaskMemFree(caGUID.pElems);
		return TRUE;
	}
	return FALSE;
}




















