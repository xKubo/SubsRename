// SubsRenameTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BOOST_TEST_MODULE SubsRename
#include <boost/test/included/unit_test.hpp>

#include "../SubsRename/SubsResync.h"

BOOST_AUTO_TEST_SUITE(TimeFormat)



BOOST_AUTO_TEST_CASE(BasicTest)
{
	Time::Diff t("")
	SubsResync::ToSRTFormat()
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SRTLines)

BOOST_AUTO_TEST_CASE(BasicTest)
{

}

BOOST_AUTO_TEST_SUITE_END()