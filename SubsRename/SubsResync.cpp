#include "SubsResync.h"

#include <fstream>
#include <iomanip>

namespace SubsResync
{
	std::string CRLF = "\r\n";

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
		oss << setw(2) << setfill('0') << Minutes << ":";
		oss << setw(2) << setfill('0') << Seconds << ",";
		oss << setw(3) << setfill('0') << MSecs;
		
		return oss.str();
	}

	CSRTLine Parse(std::ifstream &f)
	{
		CSRTLine l;
		f >> l.No;
		std::string s;
		std::getline(f, s);
		std::getline(f, l.Text);
		return l;
	}

	void Write(const CSRTLine& l, std::ofstream& f)
	{
		f << l.No << CRLF <<  ToSRTFormat(l.From) << " --> " << ToSRTFormat(l.To) << CRLF << l.Text << CRLF;
	}

	Time::Diff lerp(Time::Diff SRTFirst, Time::Diff SRTLast, Time::Diff RealFirst, Time::Diff RealLast, Time::Diff t)
	{
		long long l = (t.Get() - SRTFirst.Get());
		return RealFirst + l * (RealLast - RealFirst) / (SRTLast.Get() - SRTFirst.Get());
	}

	

	CSRTLines AdjustLineTimes(const CSRTLines& ls, Time::Diff RealFirst, Time::Diff RealLast)
	{
		CSRTLines res;
		if (ls.empty())
			throw std::runtime_error("No lines read");


		Time::Diff SRTFirst = ls.front().From;
		Time::Diff SRTLast = ls.back().From;

		for (auto l : ls)
		{
			l.From = lerp(SRTFirst, SRTLast, RealFirst, RealLast, l.From);
			l.To = lerp(SRTFirst, SRTLast, RealFirst, RealLast, l.To);
		}

		return res;
	}

	void ResyncSRTFile(const CConf& c)
	{
		std::ifstream fIn(c.SRTFileIn);
		std::ofstream fOut(c.SRTFileOut);

		CSRTLines In;
		while (fIn)
		{
			auto l = Parse(fIn);
			In.push_back(l);
		}

		CSRTLines Out = AdjustLineTimes(In, c.FirstSub, c.LastSub);

		for (const auto& o : Out)
		{
			Write(o, fOut);
		}

	}
}