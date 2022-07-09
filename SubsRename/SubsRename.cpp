// SubsRename.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <regex>
#include <vector>
#include <map>

using namespace std;
using namespace std::filesystem;
namespace fs = std::filesystem;



struct CPartID
{
    int Serie = 0, Episode = 0;
};

struct CFileInfo
{
    path File;
    CPartID Part;
};

struct CEpisode
{
    CPartID Part;
    CFileInfo Video, Sub, Out;
};

CFileInfo ParseFileName(path p)
{
    static const regex r{ ".+S(\\d{2})E(\\d{2}).+\.([^\.]+)" };
    return {};
}

int main()
{
    path p = fs::current_path();

    vector<string> VideoExts = { "mkv" };
    vector<string> SubsExts = { "srt" };

    vector<CFileInfo> Files;
    map<CPartID, CEpisode> Episodes;

    // find all files in dir
    for (auto it = directory_iterator(p); it != directory_iterator{}; ++it)
    {
        path p = *it;

        CFileInfo fi = ParseFileName(p);
        switch (fi.Type)
        {

        }
        if (IsVideo(fi))
            Episodes[fi.Part].Video = fi;
        else if (IsSub(fi))
            Episodes[fi.Part].Sub = fi;
    }

    
    // check ffmpeg exists on path
    // parse each file name - for serie/episode and extension
    // 
    std::cout << "Hello World!\n";
}

