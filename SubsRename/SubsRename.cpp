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
    string str() const
    {
        return "S" + std::to_string(Serie) + "E" + std::to_string(Episode);
    }
};

bool operator<(CPartID p1, CPartID p2)
{
    if (p1.Serie != p2.Serie)
        return p1.Serie < p2.Serie;
    return p1.Episode < p2.Episode;
}

ostream& operator<<(ostream& o, CPartID p)
{
    o << p.str();
    return o;
}

enum FileType
{
    Unknown,
    Video,
    Sub,
};

map<string, FileType> Exts = {
    {"mkv", FileType::Video},
    {"srt", FileType::Sub},
};

struct CFileInfo
{
    FileType Type;
    path File;
    CPartID Part;
};

struct CEpisode
{
    bool IsOK() const
    {
        return Part.Episode != 0 && Part.Serie != 0 && Video.Type == FileType::Video && Sub.Type == FileType::Sub;
    }

    CPartID Part;
    CFileInfo Video, Sub, Out;
};

vector<string> VideoExts = { "mkv" };
vector<string> SubsExts = { "srt" };

CFileInfo ParseFileName(path p)
{
    static const regex r{ ".+S(\\d{2})E(\\d{2}).+\\.([^\\.]+)" };
    CFileInfo fi;
    smatch sm;
    string s = p.string();
    if (regex_match(s, sm, r))
    {
        fi.Part.Serie = stoi(sm[1]);
        fi.Part.Episode = stoi(sm[2]);
        auto it = Exts.find(sm[3]);
        if (it == Exts.end())
            throw std::runtime_error("Invalid extension:" + sm[3].str());
    }
    return fi;
}

// C:ffmpeg -i Unforgotten.S01E01.mkv -f srt -i Unforgotten.S01E01.srt -map 0:0 -map 0:1 -map 1:0 -c:v copy -c:a copy -c:s srt out.mkv

void GenMuxedFile(const CEpisode& e)
{

}

int main() try
{
    path p = fs::current_path();



    vector<CFileInfo> Files;
    map<CPartID, CEpisode> Episodes;

    // find all files in dir
    for (auto it = directory_iterator(p); it != directory_iterator{}; ++it)
    {
        path p = *it;

        CFileInfo fi = ParseFileName(p);
        switch (fi.Type)
        {
        case FileType::Unknown:
            continue;
        case FileType::Video:
            Episodes[fi.Part].Video = fi;
        case FileType::Sub:
            Episodes[fi.Part].Sub = fi;
        }            
    }

    for (const auto& e : Episodes)
    {
        if (!e.second.IsOK())
            throw std::runtime_error("Episode " + e.first.str() +" is not OK");
        GenMuxedFile(e.second);
    }
}
catch (const std::exception& e)
{
    cerr << "Error: " << e.what() << endl;
}