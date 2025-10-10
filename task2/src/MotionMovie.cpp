#include "MotionMovie.h"
#include <iostream>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>

namespace rj = rapidjson;

void MotionMovie::load(const std::string& path){
    std::ifstream ifs(path);
    if (!ifs.is_open())
    {
        std::cout << "error opening file";
        return;
    }

    rj::IStreamWrapper isw(ifs);
    rj::Document doc;

    doc.ParseStream(isw);
    if(doc.HasParseError())
    {
      std::cout <<"JSON parse ERRor";
    }
    
    int id = -1;
    int loop = 0;
    const auto& arr = doc["motion_unit"].GetArray();
    for (const auto& obj : arr){
      if (obj.HasMember("id") && obj["id"].IsInt()){
        id = obj["id"].GetInt();
        loop = obj["loop"].GetInt();
      }
    }

    std::string path_baru= std::string("../../XL/motion_unit/") + std::to_string(id) + ".json";
    MotionUnit mu;
    mu.loadDanSimpan(path_baru, id + 1, loop);
}
