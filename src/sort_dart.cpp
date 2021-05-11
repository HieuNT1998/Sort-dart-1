#include "sort_node.h"
#include<iostream>
#include <stdint.h>
using namespace std;


namespace sortnode
{
    SortNode::SortNode(int kMinHits, int kMaxAge, int kMaxCoastCycles, float kIoUThreshold, float kMinConfidence, int frame_index){
        this->kMinHits = int(kMinHits);
        this->kMaxAge = int(kMaxAge);
        this->kMaxCoastCycles = int(kMaxCoastCycles);
        this->kIoUThreshold = float(kIoUThreshold);
        this->kMinConfidence = float(kMinConfidence);
        this->tracker = Tracker(); // init tracker;
    }

    float* SortNode::update(float* detectionList, float *landmarksList, float *scoresList, int detectionLen, int landmarkLen){
        // cout << "---------------------------SORT DART---------------------------------------------\n";
        this->frame_index++;

        // detection format  [ xmin, ymin, height, witdh, score, leftEye, rightEye, leftCheck, rightCheck, nose, mouth ]
        int row = detectionLen;
        int detectCol = 4; 
        int landmarkCol = landmarkLen;
        int scoreCol = 1;

        // convert array1d to bbox 
        std::vector<std::pair<cv::Rect, std::vector<float>>> bbox_per_frame;
        for(int i=0; i < row ;i++){
            // cout << "Scores : " << *(scoresList + i) << "\n";
            // cout << "Kmin : " << this->kMinConfidence <<"\n";
            if(*(scoresList + i) > kMinConfidence){
                // cout << "pass\n";
                float bbox_rec[4];
                std::vector<float> landmarks;
                for(int j=0; j < detectCol; j++){
                    bbox_rec[j] = *(detectionList + i*detectCol + j);
                }
                cv::Rect rect{bbox_rec[0],bbox_rec[1],bbox_rec[2],bbox_rec[3]};           
                for(int j=0; j < landmarkCol;j++){
                    landmarks.push_back(*(landmarksList + i*landmarkCol + j));
                }

                // for(int j=0; j<4; j++){
                //     cout << bbox_rec[j] << " ";
                // }
                // cout << "\n";

                // for(int j=0; j<6; j++){
                //     cout << landmarks[j] << " ";
                // }
                // cout << "\n";

                bbox_per_frame.push_back(std::make_pair(rect, landmarks));
            }
        }

        // cout << "Input done\n";
        


        // run sorttracker 
        this->tracker.Run(bbox_per_frame, this->kMaxAge, this->kIoUThreshold);
        const auto tracks = this->tracker.GetTracks();

        // Convert results from cv::Rect to normal float vector
        std::vector<std::vector<int> > res;
        std::vector<std::vector<float>> res_landmarksList;

        for (auto &trk : tracks)
        {
            const auto &bbox = trk.second.GetStateAsBbox();
            // Note that we will not export coasted tracks
            // If we export coasted tracks, the total number of false negative will decrease (and maybe ID switch)
            // However, the total number of false positive will increase more (from experiments),
            // which leads to MOTA decrease
            // Developer can export coasted cycles if false negative tracks is critical in the system
            if (trk.second.coast_cycles_ < this->kMaxCoastCycles && (trk.second.hit_streak_ >= this->kMinHits || this->frame_index < this->kMinHits))
            {
                std::vector<int> current_object{bbox.tl().x, bbox.tl().y, bbox.width, bbox.height, trk.first};
                // Last value is track id
                std::vector<float> landmarksList = trk.second.landmarks;
                res.push_back(current_object);
                res_landmarksList.push_back(landmarksList);
            }
        }

        //return 
        int resultCol = 5 + landmarkLen;
        float *tmpDartResult = (float*) malloc ( res.size() * resultCol * sizeof(float));
        // cout << "---------convert output---------\n";
        // cout << "res size: " << res.size() <<'\n';
        for(uint32_t i = 0; i < res.size(); i++){
            // cout << "i=" << i << "\n";
            auto bbox = res[i];
            auto landmarks = res_landmarksList[i];
            int bboxSize = bbox.size();
            // cout << "bboxsize = " << bboxSize << "\n";

            for(int j = 0; j < bboxSize; j++)
            {   
                *(tmpDartResult + i*resultCol + j) = bbox[j];
            }
            
            // for(int j = 0; j < bboxSize; j++)
            // {   
            //     cout << *(tmpDartResult + i*resultCol + j) << " ";
            // }
            // cout << "\n";
            
            for(uint32_t j = 0 ; j < landmarks.size(); j++){
                *(tmpDartResult + i*resultCol + j + bboxSize) = (float)landmarks[j];
            }

            // cout << "landmark";
            // for(uint32_t j = 0 ; j < landmarks.size(); j++){
            //     cout << (int)landmarks[j] << " ";
            // }
            // cout << "\n";

        }

        float *dartResult = (float*) malloc ((res.size() * resultCol + 1) * sizeof(float));
        dartResult[0] = res.size();
        for(int i=0 ; i < (resultCol * res.size()) ;i++ ){
            *(dartResult + 1 + i) = *(tmpDartResult +i);
        }

        // for(int i=0 ; i < (resultCol * sizeof(float) + 1) ;i++ ){
        //     cout << *(dartResult+i) << "\n";
        // }

        // cout << "\nOutput: ";
        // for(uint32_t i=0; i < (res.size() * resultCol); i++){
        //     cout << *( tmpDartResult + i ) << " ";
        // }
        // cout << "\n";

        // cout << "result res: " << (res.size() * resultCol + 1) << "\n";
        return dartResult;
    }
} // namespace sortnode