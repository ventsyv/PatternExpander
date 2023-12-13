/*
 * TestHelpers.cpp
 *
 *  Created on: Dec 13, 2023
 *      Author: ventsyv
 */

#include <gtest/gtest.h>
#include "Expander.h"
#include "Helpers.h"

class TestHelpers: public ::testing::Test
{
protected:
	PatternExpander::Expander underTest;
	PatternExpander::Expander altExpander;

	virtual void SetUp()
	{
		altExpander.setEscChar('#');
		altExpander.setGroupBegin('{');
		altExpander.setGroupEnd('}');
		altExpander.setRangeChar('>');
		altExpander.setQuote('^');
	}
};

using namespace ::testing;
using namespace std;

TEST_F(TestHelpers, testSetConfig)
{
	string configFileName = std::tmpnam(nullptr);

	string args = R"(-e # -r > -b { -n } -q ^)";
	stringstream ss(args);
	vector<string> temp;
	string item;
	while (ss >> item)
		temp.push_back(item);

	char** argv = new char*[temp.size()];
	for (size_t i = 0; i < temp.size(); i++)
	{
		argv[i] = new char[temp[i].length()];
		strcpy(argv[i], temp[i].c_str());
	}

	setConfig(temp.size(), argv, configFileName);

	//Load the alt config file. All settings will change
	underTest.loadConfig(configFileName);

	ASSERT_EQ(underTest.getEscChar(), L'#');
	ASSERT_EQ(underTest.getGroupBegin(), L'{');
	ASSERT_EQ(underTest.getGroupEnd(), L'}');
	ASSERT_EQ(underTest.getRangeChar(), L'>');
	ASSERT_EQ(underTest.getQuote(), L'^');


	for (size_t i = 0; i < temp.size(); i++)
	{
		delete argv[i];
	}
	delete [] argv;


}
