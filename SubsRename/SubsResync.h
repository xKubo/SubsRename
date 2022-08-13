#pragma once

#include <filesystem>
#include <time/TimeUtils.h>

namespace SubsResync
{
	namespace fs = std::filesystem;

	struct CConf
	{
		fs::path SRTFileIn, SRTFileOut;
		Time::Diff FirstSub;
		Time::Diff LastSub;
	};

	void ResyncSRTFile(const CConf& c);

	struct CSRTLine
	{
		int No = 0;
		Time::Diff From, To;
		std::string Text;
	};

	using CSRTLines = std::vector<CSRTLine>;

	CSRTLines ResyncSRTLines(const CSRTLines& ls, Time::Diff RealFirst, Time::Diff RealLast);

	std::string ToSRTFormat(Time::Diff d);
	Time::Diff FromSRTFormat(std::string f);
}