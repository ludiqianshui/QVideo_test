//
// UFilterUtils.h
//

#ifndef __H_UFilterUtils__
#define __H_UFilterUtils__

class UFilterUtils
{
public:
	UFilterUtils() {};
	~UFilterUtils() {};

	// NOT outstanding reference
	static IPin * GetPin(IBaseFilter * inFilter, BOOL inInput, 	const char * inPartialName = NULL);
	static IPin * GetUnconnectedPin(IBaseFilter * inFilter, BOOL inInput);
	static BOOL GetPinCount(IBaseFilter * inFilter, int & outInputPin, 	int & outOutputPin);
	static BOOL IsCompletelyConnected(IBaseFilter * inFilter);
};

#endif // __H_UFilterUtils__