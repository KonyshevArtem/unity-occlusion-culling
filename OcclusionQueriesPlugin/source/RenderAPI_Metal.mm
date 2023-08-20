
#include "RenderAPI.h"
#include "PlatformBase.h"


// Metal implementation of RenderAPI.


#if SUPPORT_METAL

#include "Unity/IUnityGraphicsMetal.h"
#import <Metal/Metal.h>


class RenderAPI_Metal : public RenderAPI
{
public:
	RenderAPI_Metal();
	virtual ~RenderAPI_Metal() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);

private:
	void CreateResources();

private:
	IUnityGraphicsMetal* m_MetalGraphics;
};


RenderAPI* CreateRenderAPI_Metal()
{
	return new RenderAPI_Metal();
}



void RenderAPI_Metal::CreateResources()
{
	id<MTLDevice> metalDevice = m_MetalGraphics->MetalDevice();
}


RenderAPI_Metal::RenderAPI_Metal()
{
}


void RenderAPI_Metal::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	if (type == kUnityGfxDeviceEventInitialize)
	{
		m_MetalGraphics = interfaces->Get<IUnityGraphicsMetal>();

		CreateResources();
	}
	else if (type == kUnityGfxDeviceEventShutdown)
	{
		//@TODO: release resources
	}
}

#endif // #if SUPPORT_METAL
