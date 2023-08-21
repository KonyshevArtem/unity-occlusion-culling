#pragma once

#include "Unity/IUnityLog.h"

void InitLogger(IUnityLog* unityLogPtr);
void LogError(const char* message);
