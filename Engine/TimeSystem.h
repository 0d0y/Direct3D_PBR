#include <Windows.h>

class TimeSystem {
public:
    TimeSystem() {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&startCounter);
    }

    void Update() {
        QueryPerformanceCounter(&endCounter);
        deltaTime = static_cast<float>(endCounter.QuadPart - startCounter.QuadPart) / frequency.QuadPart;
        startCounter = endCounter;
    }

    float GetDeltaTime() const {
        return deltaTime;
    }

private:
    LARGE_INTEGER frequency;
    LARGE_INTEGER startCounter;
    LARGE_INTEGER endCounter;
    float deltaTime;
};