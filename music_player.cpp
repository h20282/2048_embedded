#include "music_player.h"

#include <thread>
#include <unistd.h>
#include <fstream>
#include <iostream>

#include "Img.h"
#include "bmp.h"

MusicPlayer::MusicPlayer() {
    
    std::ifstream file("music/musics.txt", std::ios::in);
    if (!file.is_open()) {
        std::cout << "open `music/musics.txt` failed" << std::endl;
        exit(0);
        return ;
    }
    std::string s;
    while(file>>s){
        names_.push_back("music/" + s);
    }
    for (auto &item : names_) {
        imgs_.push_back(Img((item+".bmp").c_str()));
    }
    std::thread([&](){
        while(1){
            if (playing_) {
                Img t = Rotate(imgs_[curr_idx_], angle_);
                bmp_display(t, 541, 226);
                angle_ -= 3.0/360*(2*3.14);
                usleep(1000*1000 / 60);
            }
        }
    }).detach();
    Start();
}

void MusicPlayer::Start() {
    std::thread([&](){
        system( ("madplay " + names_[curr_idx_] + ".mp3").c_str() );
    }).detach();
}

void MusicPlayer::Pause() {
    playing_ = false;
    system("killall -STOP madplay &");
}

void MusicPlayer::Continue() {
    playing_ = true;
    system("killall -CONT madplay &");
}

void MusicPlayer::Next() {
    Stop();
    angle_ = 0;
    curr_idx_ = (curr_idx_+1) % names_.size();
    Start();
    playing_ = true;
}

void MusicPlayer::Pre() {
    Stop();
    angle_ = 0;
    curr_idx_ = (curr_idx_ + names_.size() - 1) % names_.size();
    Start();
    playing_ = true;
}

void MusicPlayer::Toggle(){
    playing_ ? Pause() : Continue();
}

void MusicPlayer::Stop() {
    system("killall -9 madplay");
}
