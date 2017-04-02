#include "Settings.h"
#include <opencv2/core/core.hpp>
#include <iostream>

Settings::Settings()
{
}

Settings::~Settings()
{
}

bool Settings::Read(const std::string& rs)
{
    bool result = false;
    cv::FileStorage fs;
    fs.open(rs, cv::FileStorage::READ);
    if (fs.isOpened())
    {
        cv::FileNode n;
        
        n = fs["app"];
        app.zoom_steps = n["zoom_steps"];
        app.zoom_max = n["zoom_max"];
        app.com_port = n["com_port"];
        app.rec_path = n["rec_path"];
        app.cascade_path = n["cascade_path"];

        n = fs["loop"];
        loop.max_level = n["max_level"];
        loop.inh_time = n["inh_time"];
        loop.norm_time = n["norm_time"];
        loop.warn_time = n["warn_time"];
        loop.act_time = n["act_time"];

        n = fs["phrase"];
        phrase.wait_time = n["wait_time"];
        phrase.rec_time = n["rec_time"];
        phrase.spk_time = n["spk_time"];
        phrase.file_name = n["file_name"];

        result = true;
    }
    return result;
}

bool Settings::Write(const std::string& rs) const
{
    bool result = false;
    cv::FileStorage fs;
    fs.open(rs, cv::FileStorage::WRITE);
    if (fs.isOpened())
    {
        fs << "app" << "{";
        fs << "zoom_steps" << app.zoom_steps;
        fs << "zoom_max" << app.zoom_max;
        fs << "com_port" << app.com_port;
        fs << "rec_path" << app.rec_path;
        fs << "cascade_path" << app.cascade_path;
        fs << "}";

        fs << "loop" << "{";
        fs << "max_level" << loop.max_level;
        fs << "inh_time" << loop.inh_time;
        fs << "norm_time" << loop.norm_time;
        fs << "warn_time" << loop.warn_time;
        fs << "act_time" << loop.act_time;
        fs << "}";

        fs << "phrase" << "{";
        fs << "wait_time" << phrase.wait_time;
        fs << "rec_time" << phrase.rec_time;
        fs << "spk_time" << phrase.spk_time;
        fs << "file_name" << phrase.file_name;
        fs << "}";

        result = true;
    }
    return result;
}

void Settings::ApplyDefaults(void)
{
    // times are all in seconds
    
    app.zoom_steps = 20;
    app.zoom_max = 4;
    app.com_port = "COM2";
    app.rec_path = "C:\\work\\movie";
    app.cascade_path = "C:\\opencv-3.2.0\\opencv\\build\\etc\\haarcascades\\";

    loop.max_level = 10;
    loop.inh_time = 5;
    loop.norm_time = 4;
    loop.warn_time = 3;
    loop.act_time = 5;

    phrase.wait_time = 10;
    phrase.rec_time = 25;
    phrase.spk_time = 10;
    phrase.file_name = "phrases.txt";
}
