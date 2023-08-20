#include "RenderAPI.h"
#include "PlatformBase.h"

// OpenGL Core profile (desktop) or OpenGL ES (mobile) implementation of RenderAPI.
// Supports several flavors: Core, ES2, ES3


#if SUPPORT_OPENGL_UNIFIED


#include <assert.h>
#if UNITY_IOS || UNITY_TVOS
#	include <OpenGLES/ES2/gl.h>
#elif UNITY_ANDROID || UNITY_WEBGL
#	include <GLES2/gl2.h>
#elif UNITY_OSX
#	include <OpenGL/gl3.h>
#elif UNITY_WIN
// On Windows, use gl3w to initialize and load OpenGL Core functions. In principle any other
// library (like GLEW, GLFW etc.) can be used; here we use gl3w since it's simple and
// straightforward.
#	include "gl3w/gl3w.h"
#elif UNITY_LINUX
#	define GL_GLEXT_PROTOTYPES
#	include <GL/gl.h>
#elif UNITY_EMBEDDED_LINUX
#	include <GLES2/gl2.h>
#if SUPPORT_OPENGL_CORE
#	define GL_GLEXT_PROTOTYPES
#	include <GL/gl.h>
#endif
#else
#	error Unknown platform
#endif


class RenderAPI_OpenGLCoreES : public RenderAPI
{
public:
	RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType);
	virtual ~RenderAPI_OpenGLCoreES() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);

private:
	void CreateResources();

private:
	UnityGfxRenderer m_APIType;
};


RenderAPI* CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
{
	return new RenderAPI_OpenGLCoreES(apiType);
}


void RenderAPI_OpenGLCoreES::CreateResources()
{
#	if UNITY_WIN && SUPPORT_OPENGL_CORE
	if (m_APIType == kUnityGfxRendererOpenGLCore)
		gl3wInit();
#	endif
}


RenderAPI_OpenGLCoreES::RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
	: m_APIType(apiType)
{
}


void RenderAPI_OpenGLCoreES::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	if (type == kUnityGfxDeviceEventInitialize)
	{
		CreateResources();
	}
	else if (type == kUnityGfxDeviceEventShutdown)
	{
		//@TODO: release resources
	}
}

#endif // #if SUPPORT_OPENGL_UNIFIED
