// SubsRename.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <regex>
#include <vector>
#include <map>

//#include <boost/algorithm/string.hpp>

using namespace std;
using namespace std::filesystem;
namespace fs = std::filesystem;



struct CPartID
{
    int Serie = 0, Episode = 0;
    string str() const
    {
        std::ostringstream oss;
        oss << "S" << setw(2) << setfill('0') << Serie << setw(2) << setfill('0') << Episode;
        return oss.str();
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
    FileType Type = FileType::Unknown;
    path File;
    CPartID Part;
};

struct CEpisode
{
    CFileInfo Video, Sub, Out;
};

bool IsEpisodeOK(const CPartID& Part, const CEpisode& e) 
{
    return Part.Episode != 0 && Part.Serie != 0 && e.Video.Type == FileType::Video && e.Sub.Type == FileType::Sub;
}



vector<string> VideoExts = { "mkv" };
vector<string> SubsExts = { "srt" };

bool Contains(const string& Body, const string& Text)
{
    auto it = std::search(Body.begin(), Body.end(), Text.begin(), Text.end());
    return it != Body.end();
}

CFileInfo ParseFileName(path p)
{
    static const regex r{ ".+S(\\d{2})E(\\d{2}).*\\.([^\\.]+)", std::regex_constants::icase };
    CFileInfo fi;
    smatch sm;
    string s = p.string();
    if (Contains(p.string(), ".subs."))      // we skip already processed files
        return fi;
    if (regex_match(s, sm, r))
    {
        fi.Part.Serie = stoi(sm[1]);
        fi.Part.Episode = stoi(sm[2]);
        fi.File = p;
        auto it = Exts.find(sm[3]);
        if (it == Exts.end())
            throw std::runtime_error("Invalid extension:" + sm[3].str());
        fi.Type = it->second;
    }
    return fi;
}

// C:ffmpeg -i Unforgotten.S01E01.mkv -f srt -i Unforgotten.S01E01.srt -map 0:0 -map 0:1 -map 1:0 -c:v copy -c:a copy -c:s srt out.mkv

void GenMuxedFile(const path& Video, const path& Sub, const path& Out)
{
    std::ostringstream oss;
    oss << "C:\\tools\\ffmpeg\\ffmpeg -i \"" << Video << "\" -f srt -i \"" << Sub << "\" -map 0:0 -map 0:1 -map 1:0 -c:v copy -c:a copy -c:s srt \"" << Out << "\"";
    system(oss.str().c_str());
}

using CParams = std::map<string, string>;

template <typename ... TArgs>
inline bool Check(bool res, const TArgs&... args...)
{
    if (res == true)
        return res;
    std::ostringstream oss;
    (oss << ... << args);
    throw std::runtime_error("Check failed: " + oss.str());
}

CParams LoadParams(int argc, char* argv[])
{
    CParams res;

    Check(argc % 2 == 1, "Even num of params:", argc);


    for (int i = 1; i < argc; i+=2)
    {
        string Key = argv[i];
        Check(Key[0] == '-', "ParStartsWith-");
        Key.erase(0, 1);
        res[Key] = argv[i + 1];
    }

    return res;

}

int main(int argc, char* argv[]) try
{

    CParams pars = LoadParams(argc, argv);
    path p = pars["d"];
    Check(!p.empty(), "Specify dir with -d");
    string NamePrefix = pars["n"];
    Check(!NamePrefix.empty(), "Specify name prefix with -n");

    path OrigFilesDir = p / "orig";
    fs::create_directory(OrigFilesDir);
    
    cout << "Name prefix: " << NamePrefix << endl;

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
            break;
        case FileType::Sub:
            Episodes[fi.Part].Sub = fi;
            break;
        }            
    }

    for (const auto& ep : Episodes)
    {
        if (!IsEpisodeOK(ep.first, ep.second))
        {
            cout << "Skipping episode: " << ep.first << endl;
            continue;
        }
        const auto& e = ep.second;
        const auto& vf = e.Video.File;
        const auto& sf = e.Sub.File;
        const auto& Part = ep.first;
        fs::path VFName = OrigFilesDir / (NamePrefix + "." + Part.str() + vf.extension().string());
        fs::rename(vf, VFName);
        cout << vf << endl;
        cout << " -> " << VFName << endl;
        fs::path SubName = OrigFilesDir / (NamePrefix + "." + Part.str() + sf.extension().string());
        fs::rename(sf, SubName);
        cout << sf << endl; 
        cout << " -> " << SubName << endl;
        fs::path OutName = p / (VFName.stem().string() + ".subs" + VFName.extension().string());

        GenMuxedFile(VFName, SubName, OutName);

    }
    cout << "Done" << endl;
}
catch (const std::exception& e)
{
    cerr << "Error: " << e.what() << endl;
}