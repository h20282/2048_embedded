#pragma once

#include <vector>
#include "Img.h"

class MusicPlayer {

public:
    MusicPlayer();

    void Start();
    void Pause();
    void Continue();
    void Next();
    void Pre();
    void Toggle();
private:
    void Stop();

private:
    bool playing_ = true;
    int curr_idx_ = 0;
    double angle_ = 0;
    std::vector<Img> imgs_;
    std::vector<std::string> names_;
    int sleep_time_;
};
