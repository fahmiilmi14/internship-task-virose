#include <iostream>
#include <fstream>
#include "MotionMovie.h"

int main (){
    int M;
    std::cin >> M;

    std::string path = "../../XL/motion_movie/" + std::to_string(M) + ".json";
    MotionMovie movie;
    movie.load(path);
}