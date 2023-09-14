// SubsRenameTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BOOST_TEST_MODULE SubsRename
#include <boost/test/included/unit_test.hpp>

#include <variant>
#include <string>

#include "../SubsRename/SubsResync.h"

using namespace Time;
using namespace SubsResync;

void CheckSRTFormat(HMS t, const std::string &s)
{
	BOOST_REQUIRE_EQUAL(ToSRTFormat(Time::Diff{ t }), s);
}

BOOST_AUTO_TEST_SUITE(TimeFormat)



BOOST_AUTO_TEST_CASE(BasicTest)
{
	CheckSRTFormat({ 1, 0, 0 }, "01:00:00,000");
	CheckSRTFormat({ 0, 1, 0 }, "00:01:00,000");
	CheckSRTFormat({ 0, 0, 1 }, "00:00:01,000");
	CheckSRTFormat({ 0, 0, 1, 333 }, "00:00:01,333");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Interpolation)

BOOST_AUTO_TEST_CASE(BugNegativeTo)
{
	Time::Diff SRTFirst{ 0 };
	Time::Diff SRTLast{ 27466000000 };
	Time::Diff RealFirst{ 0 };
	Time::Diff RealLast{ 27160000000 };
	Time::Diff To{ 348000000 };

	auto NewTo = SubsResync::lerp(SRTFirst, SRTLast, RealFirst, RealLast, To);
	BOOST_REQUIRE(NewTo.Get() >= 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SRTLines)

BOOST_AUTO_TEST_CASE(BasicTest)
{
	struct CPoint
	{
		int x = 0, y = 0;
		std::string name;
	};
	using namespace std;
	std::variant<int, std::string, CPoint> x; 
	x = CPoint{ 2,3 };
	cout << std::get<int>(x) << endl;
}

BOOST_AUTO_TEST_SUITE_END()