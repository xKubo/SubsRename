// ResyncSubs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <time/TimeUtils.h>
#include "../SubsRename/SubsResync.h"
#include <regex>

using namespace std;

std::regex r{ R"((\d\d):(\d\d)(,(\d\d\d))?)" };

Time::Diff ParseTime(const std::string& TimePairString)
{
    std::smatch sm;
    if (!std::regex_match(TimePairString, sm, r))
        throw std::runtime_error("Cannot match regex:" + TimePairString);
    int Mins = std::stoi(sm[1]);
    int Secs = std::stoi(sm[2]);
    int MSecs = sm[3].matched ? std::stoi(sm[4]) : 0;
    Time::Diff d = Mins * Time::Minutes() + Secs * Time::Seconds() + MSecs * Time::Milliseconds();
    return d;
}

int main(int argc, const char* argv[])
{
    try
    {
        if (argc < 4)
            throw std::runtime_error("Usage: ResyncSubs in.srt out.srt \"00:00\" \"45:16\"");
        
        std::string FileIn(argv[1]);
        std::string FileOut(argv[2]);
        std::string FirstTime = "00:00";
        std::string SecondTime;
        if (argc == 4)
        {
            
            SecondTime = argv[3];
        }
        else
        {
            FirstTime = argv[3];
            SecondTime = argv[4];
        }

        std::cout << FileIn << " -> " << FileOut << endl;
        std::cout << "FirstTime: " << FirstTime << endl;
        std::cout << "SecondTime: " << SecondTime << endl;

        SubsResync::CConf c;
        c.SRTFileIn = FileIn;
        c.SRTFileOut = FileOut;
        c.FirstSub = ParseTime(FirstTime);
        c.LastSub = ParseTime(SecondTime);
        SubsResync::ResyncSRTFile(c);

    }
    catch (const std::exception& e)
    {
        cerr << e.what() << endl;
    }
}


