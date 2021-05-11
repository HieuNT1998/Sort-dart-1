#include "sort_node.h"
#include<iostream>
#include <stdint.h>
using namespace std;

namespace sortnode{
    extern "C"
    SortNode* Navtive_GetSort (int kMinHits, int kMaxAge, int kMaxCoastCycles, float kIoUThreshold, float kMinConfidence, int frame_index){
        // cout << "C lib Kmin: " << kMinConfidence <<"\n";
        return new SortNode(kMinHits, kMaxAge, kMaxCoastCycles, kIoUThreshold, kMinConfidence, frame_index);
    }

    extern "C"
    float* Navtive_Update(SortNode *sortNode, float* detectionList, float *landmarksList, float *scoresList, int detectionLen, int landmarkLen){
        // for(int i=0; i<detectionLen;i++){
        //     for(int j = 0; j<4;j++){
        //         cout << *( detectionList + i*4 +j ) << "  ";
        //     } 
        //     cout << "\n";
        // }
        
        // for(int i=0; i<detectionLen;i++){
        //     for(int j = 0; j < 6;j++){
        //         cout << *( landmarksList + i*6 + j ) << "  ";
        //     } 
        //     cout << "\n";
        // }
        return sortNode->update(detectionList, landmarksList, scoresList, detectionLen, landmarkLen);
    }


    extern "C"
    float* Navtive_InitFloatList(int len){
        float* floatList = (float*) malloc(len*sizeof(float));
        return floatList;
    }

    extern "C"
    int* Navtive_InitIntList(int len){
        int* intList = (int*) malloc(len*sizeof(int));
        return intList;
    }
}









