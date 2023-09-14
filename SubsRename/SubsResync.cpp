#include "SubsResync.h"

#include <fstream>
#include <iomanip>
#include <regex>

namespace SubsResync
{
	std::string CRLF = "\n";

	std::string ToSRTFormat(Time::Diff d)
	{
		using namespace Time;
		using namespace std;
		std::ostringstream oss;

		// HH:MM:SS.mmm
		i64_t Hours = d.InHours();
		Diff dHour(Hours * OneHour);
		d -= dHour;

		i64_t Mins = d.InMinutes();
		Diff dMin(Mins * OneMinute);
		d -= dMin;

		i64_t Secs = d.InSecs();
		Diff dSec(Secs * OneSecond);
		d -= dSec;

		i64_t MSecs = d.InMillisecs();

		oss << setw(2) << setfill('0') << Hours << ":";
		oss << setw(2) << setfill('0') << Mins << ":";
		oss << setw(2) << setfill('0') << Secs << ",";
		oss << setw(3) << setfill('0') << MSecs;
		
		return oss.str();
	}

	

	int ToInt(const std::smatch& sm, int Index)
	{
		size_t Idx = 0;
		std::string s = sm[Index].str();
		int res = std::stoi(s, &Idx);
		if (Idx != s.size())
			throw std::runtime_error(" Cannot parse int from : " + s);
		return res;
	}

	
	std::regex SRTFormatParser{ R"((\d{2}):(\d{2}):(\d{2}),(\d{3}))" };
	Time::Diff FromSRTFormat(std::string s)
	{	
		using namespace std;
		smatch sm;
		if (!regex_match(s, sm, SRTFormatParser))
			throw std::runtime_error("Cannot parse SRT format string: " + s);

		Time::Diff d =
			Time::Hours() * ToInt(sm, 1) +
			Time::Minutes() * ToInt(sm, 2) +
			Time::Seconds() * ToInt(sm, 3) +
			Time::Milliseconds() * ToInt(sm, 4);
		
		return d;
	}

	std::regex r{ "(.+) --> (.+)" };

	CSRTLine Parse(std::ifstream &f)
	{
		CSRTLine l;
		std::string LineNo;
		std::getline(f, LineNo);
		if (LineNo.empty())
			return {};
		l.No = std::stoi(LineNo);
		std::string s;
		std::smatch sm;
		std::getline(f, s);
		if (!std::regex_match(s, sm, r))
			throw std::runtime_error("Cannot parse SRT line: " + s);
		l.From = FromSRTFormat(sm[1]);
		l.To = FromSRTFormat(sm[2]);
		bool First = true;
		for (;;)
		{
			if (First)
			{
				First = false;
			}
			else
			{
				l.Text += CRLF;
			}
			std::getline(f, s);
			if (s.empty())
				return l;
			l.Text += s;
		}
	}

	void Write(const CSRTLine& l, std::ofstream& f)
	{
		f << l.No << CRLF <<  ToSRTFormat(l.From) << " --> " << ToSRTFormat(l.To) << CRLF << l.Text << CRLF << CRLF;
	}

	Time::Diff lerp(Time::Diff SRTFirst, Time::Diff SRTLast, Time::Diff RealFirst, Time::Diff RealLast, Time::Diff t)
	{
		long long l = t.Get() - SRTFirst.Get();
		long long SRTLength = SRTLast.InMillisecsInt() - SRTFirst.InMillisecsInt();
		long long RealLen = RealLast.InMillisecsInt() - RealFirst.InMillisecsInt();
		long long res = RealFirst.Get() + l * RealLen / SRTLength;
		return Time::Diff{ res };
	}

	CSRTLines ResyncSRTLines(const CSRTLines& ls, Time::Diff RealFirst, Time::Diff RealLast)
	{
		CSRTLines res;
		if (ls.empty())
			throw std::runtime_error("No lines read");


		Time::Diff SRTFirst = ls.front().From;
		Time::Diff SRTLast = ls.back().From;

		for (auto l : ls)
		{
			std::cout << "SRT: " << SRTFirst << " - " << SRTLast << std::endl;
			std::cout << "Real: " << RealFirst << " - " << RealLast << std::endl;
			auto NewFrom = lerp(SRTFirst, SRTLast, RealFirst, RealLast, l.From);
			auto NewTo = lerp(SRTFirst, SRTLast, RealFirst, RealLast, l.To);
			std::cout << "From: " << l.From << " NewFrom: " << NewFrom << std::endl;
			l.From = NewFrom;
			l.To = NewTo;
			res.push_back(l);
		}
		

		return res;
	}

	void ResyncSRTFile(const CConf& c)
	{
		std::ifstream fIn(c.SRTFileIn);
		std::ofstream fOut(c.SRTFileOut);

		if (fIn.fail())
			throw std::runtime_error("Cannot open input file");

		if (fOut.fail())
			throw std::runtime_error("Cannot open output file");

		CSRTLines In;
		while (fIn)
		{
			auto l = Parse(fIn);
			if (!l.IsValid())
				break;
			In.push_back(l);
		}

		CSRTLines Out = ResyncSRTLines(In, c.FirstSub, c.LastSub);

		for (const auto& o : Out)
		{
			Write(o, fOut);
		}

	}
}