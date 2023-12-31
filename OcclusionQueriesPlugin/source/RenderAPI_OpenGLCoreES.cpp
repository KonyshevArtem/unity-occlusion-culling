#include "RenderAPI.h"
#include "PlatformBase.h"
#include "Logger.h"

#include <string>

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
#   include <OpenGL/OpenGL.h>
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

const int CUBE_VERTEX_COUNT = 3 * 2 * 6;

const char* VERTEX_SHADER_SOURCE =                                        \
    "#version 330\n"                                                 \
    "in highp vec3 pos;\n"                                        \
    "layout(std140) uniform Matrices\n"                    \
    "{\n"                                                           \
    "   mat4 vpMatrix;\n"                                           \
    "};\n"                                                           \
    "void main()\n"                                                    \
    "{\n"                                                            \
    "    gl_Position = vpMatrix * vec4(pos,1);\n"                     \
    "}\n";

const char* FRAGMENT_SHADER_SOURCE =                        \
    "#version 330\n"                                  \
    "out lowp vec4 fragColor;\n"                       \
    "void main()\n"                                    \
    "{\n"                                            \
    "   fragColor = vec4(1, 1, 1, 1);\n"                        \
    "}\n";


class RenderAPI_OpenGLCoreES : public RenderAPI
{
public:
	RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType);
	virtual ~RenderAPI_OpenGLCoreES() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces) override;
    
    virtual void Test() override;
    virtual void PrepareRenderAPI(const void* matricesBuffer) override;

private:
	void CreateResources();
    void ReleaseResources();

private:
	UnityGfxRenderer m_APIType;
    GLuint m_VAO;
    GLuint m_VertexBuffer;
    GLuint m_Program;
    GLuint m_Query;

#if UNITY_OSX
    CGLContextObj m_ContextObject = NULL;
#endif
};


RenderAPI* CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
{
	return new RenderAPI_OpenGLCoreES(apiType);
}

GLuint CompileShader(GLenum shaderType, const char* shaderSource)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        char* msg = new char[maxLength];
        glGetShaderInfoLog(shader, maxLength, &maxLength, &msg[0]);
        LogError(msg);
        delete[] msg;
    }
    
    return shader;
}

void RenderAPI_OpenGLCoreES::CreateResources()
{
    const float vertices[CUBE_VERTEX_COUNT * 3] = {
        // front face
        -1, 1, -1,    1, 1, -1,    -1, -1, -1,
        1, 1, -1,     1, -1, -1,   -1, -1, -1,
        
        // left face
        -1, 1, 1,     -1, 1, -1,   -1, -1, 1,
        -1, 1, -1,    -1, -1, -1,  -1, -1, 1,
        
        // back face
        1, 1, 1,      -1, 1, 1,    -1, -1, 1,
        1, 1, 1,      -1, -1, 1,   1, -1, 1,
        
        // right face
        1, 1, -1,     1, 1, 1,     1, -1, -1,
        1, 1, 1,      1, -1, 1,    1, -1, -1,
        
        // top face
        -1, 1, 1,     1, 1, 1,     -1, 1, -1,
        1, 1, 1,      1, 1, -1,    -1, 1, -1,
        
        // bottom face
        -1, -1, -1,   1, -1, -1,   1, -1, 1,
        -1, -1, -1,   1, -1, 1,    -1, -1, 1
    };
    
#	if UNITY_WIN && SUPPORT_OPENGL_CORE
	if (m_APIType == kUnityGfxRendererOpenGLCore)
		gl3wInit();
#	endif
    
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    
    glGenBuffers(1, &m_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, CUBE_VERTEX_COUNT * 3 * 4, &vertices, GL_STREAM_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindVertexArray(0);
    
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);
    
    m_Program = glCreateProgram();
    glAttachShader(m_Program, vertexShader);
    glAttachShader(m_Program, fragmentShader);
    glLinkProgram(m_Program);
    
    GLuint index = glGetUniformBlockIndex(m_Program, "Matrices");
    glUniformBlockBinding(m_Program, index, 0);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    glGenQueries(1, &m_Query);
}


void RenderAPI_OpenGLCoreES::ReleaseResources()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteProgram(m_Program);
    glDeleteQueries(1, &m_Query);
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
        ReleaseResources();
	}
}

void RenderAPI_OpenGLCoreES::Test()
{
    glBeginQuery(GL_SAMPLES_PASSED, m_Query);
    glDrawArrays(GL_TRIANGLES, 0, CUBE_VERTEX_COUNT);
    glEndQuery(GL_SAMPLES_PASSED);
    
    int samples;
    glGetQueryObjectiv(m_Query, GL_QUERY_RESULT, &samples);
    LogError(std::to_string(samples).c_str());
}

void RenderAPI_OpenGLCoreES::PrepareRenderAPI(const void* matricesBuffer)
{
#if UNITY_OSX
    CGLContextObj context = CGLGetCurrentContext();
    if (m_ContextObject != context)
    {
        m_ContextObject = context;
        glDeleteQueries(1, &m_Query);
        glGenQueries(1, &m_Query);
    }
#endif
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    
    glBindVertexArray(m_VAO);
    glUseProgram(m_Program);
    
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, (GLuint)(size_t)matricesBuffer);
}

#endif // #if SUPPORT_OPENGL_UNIFIED
