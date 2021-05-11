#ifndef SORTNODE_H
#define SORTNODE_H

#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include "tracker.h"

namespace sortnode
{
    class SortNode
    {
    public:
        int kMinHits = 3;
        int kMaxAge = 1;
        int kMaxCoastCycles = 1;
        float kIoUThreshold = 0.3;
        float kMinConfidence = 0.6;
        int frame_index = 0;
        Tracker tracker;

        SortNode(int kMinHits, int kMaxAge, int kMaxCoastCycles, float kIoUThreshold, float kMinConfidence, int frame_index);
        float* update(float* detectionList, float *landmarksList, float *scoresList, int detectionLen, int landmarkLen);

    };
} // namespace sortnode
#endif