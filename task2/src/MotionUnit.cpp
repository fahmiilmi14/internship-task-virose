#include "MotionUnit.h"
#include <iostream>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>

namespace rj = rapidjson;

void MotionUnit::loadDanSimpan(const std::string& path, int id, int ulang){
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

    const rj::Value& total = doc["total_frame"];
    int maks = total.GetInt();

    std::cout << id << std::endl;
    const auto& frames = doc["motion_frame"];
    for (int i = 0; i < ulang; i++){
        for (const auto& frame : frames.GetArray()){
            for (const auto& val : frame.GetArray()){
                std::cout << val.GetInt() << " ";
            }
            std::cout<< std::endl;
        }
    }
}