#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

char *E_Utils_LoadFile(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f)
    {
        printf("error: could not open %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buf = (char*)malloc(size + 1);
    if (!buf) {
        printf("error: out of memory\n");
        fclose(f);
        return NULL;
    }

    long read = fread(buf, 1, size, f);
    buf[read] = '\0';

    fclose(f);
    return buf;
}

int g_readyToStart = 0;

SYSTEMTIME engineStartTime;
BOOL timerInitialized = FALSE;

double fuz_engineDeltaTime = 0.0;
double g_LastTime = 0.0;

double E_Utils_getElapsedTimeInSeconds()
{
    if (!g_readyToStart) return 0.0;

    SYSTEMTIME currentTime;
    GetLocalTime(&currentTime);

    double currentInSeconds = currentTime.wHour * 3600 +
                              currentTime.wMinute * 60 +
                              currentTime.wSecond +
                              currentTime.wMilliseconds / 1000.0;

    if (!timerInitialized)
    {
        engineStartTime  = currentTime;
        timerInitialized = TRUE;
        g_LastTime       = 0.0;
        fuz_engineDeltaTime = 0.0;
        return 0.0;
    }

    double startInSeconds = engineStartTime.wHour * 3600 +
                            engineStartTime.wMinute * 60 +
                            engineStartTime.wSecond +
                            engineStartTime.wMilliseconds / 1000.0;

    double elapsed = currentInSeconds - startInSeconds;

    fuz_engineDeltaTime = elapsed - g_LastTime;
    g_LastTime          = elapsed;

    return elapsed;
}

float E_Utils_lerpBetweenTime(float now, float t0, float t1, float v0, float v1)
{
    if (now <= t0) return v0;
    if (now >= t1) return v1;
    float t = (now - t0) / (t1 - t0);
    t = t * t * (3.0f - 2.0f * t); // smoothstep ease
    return v0 + t * (v1 - v0);
}
