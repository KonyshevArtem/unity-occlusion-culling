#include "Logger.h"

#include <stddef.h>

static IUnityLog* s_UnityLogPtr = NULL;

void InitLogger(IUnityLog* unityLogPtr)
{
    s_UnityLogPtr = unityLogPtr;
}

void LogError(const char* message)
{
    UNITY_LOG_ERROR(s_UnityLogPtr, message);
}
