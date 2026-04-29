#pragma once

#include "PCH.h"

class ThreadSafetyManager {
public:
    static void Install();
    static void PrepareForSave();
    static void FinalizeAfterSave();

private:
    static void MonitorScrapHeap();
};
