/*
 * TestHelpers.cpp
 *
 *  Created on: Dec 13, 2023
 *      Author: ventsyv
 */

#include <gtest/gtest.h>
#include <codecvt>
#include <locale>
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

TEST_F(TestHelpers, testExecuteRunCommand)
{
	string args = R"([a-c][1-3] abc[1-3])";
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

	//Run the function and capture std out
	testing::internal::CaptureStdout();
	execute_run_command(temp.size(), argv);
	std::string output = testing::internal::GetCapturedStdout();

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
	wstring woutput = wcu8.from_bytes(output);

	wstringstream wss(woutput);

	vector<wstring> results;
	wstring result;
	while (wss >> result)
		results.push_back(result);


	ASSERT_EQ(results.size(), 12);

	ASSERT_EQ(results[0], L"a1");
	ASSERT_EQ(results[1], L"b1");
	ASSERT_EQ(results[2], L"c1");

	ASSERT_EQ(results[3], L"a2");
	ASSERT_EQ(results[4], L"b2");
	ASSERT_EQ(results[5], L"c2");

	ASSERT_EQ(results[6], L"a3");
	ASSERT_EQ(results[7], L"b3");
	ASSERT_EQ(results[8], L"c3");

	ASSERT_EQ(results[9], L"abc1");
	ASSERT_EQ(results[10], L"abc2");
	ASSERT_EQ(results[11], L"abc3");


	for (size_t i = 0; i < temp.size(); i++)
	{
		delete argv[i];
	}
	delete [] argv;


}

TEST_F(TestHelpers, testExecuteValidateCommand)
{
	string args = R"(1[a- 1[*-%])";
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

	//Run the function and capture std out
	testing::internal::CaptureStdout();
	testing::internal::CaptureStderr();
	execute_validate_command(temp.size(), argv);
	std::string output = testing::internal::GetCapturedStdout();
	std::string errorOut = testing::internal::GetCapturedStderr();

	//Validation does not produce any results
	ASSERT_EQ(output, "");

	//Now parse out the lines from std err
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
	wstring wErrOut = wcu8.from_bytes(errorOut);

	size_t pos = 0;
	std::wstring line;
	vector<wstring> errorLines;
	while ((pos = wErrOut.find(L"\n")) != std::string::npos) {
	    line = wErrOut.substr(0, pos);
	    wErrOut.erase(0, pos + 1);
	    errorLines.push_back(line);
	}

	//These are the expected error messages that will be generated
	ASSERT_EQ(errorLines[0], L"Error: Invalid range found");
	ASSERT_EQ(errorLines[1], L"Error: Invalid range found");
	ASSERT_EQ(errorLines[2], L"Error: Invalid non alpha-numerical range found");




	for (size_t i = 0; i < temp.size(); i++)
	{
		delete argv[i];
	}
	delete [] argv;


}

