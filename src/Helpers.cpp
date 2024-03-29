/*
 * Helpers.cpp
 *
 *  Created on: Nov 17, 2023
 *      Author: ventsyv
 */

#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <algorithm>
#include "Expander.h"
#include "Helpers.h"

using namespace std;



void execute_run_command(int argc, char *argv[])
{
	wstring pattern;
	PatternExpander::Expander exp;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;

	if (argc > 0)
	{
		for (int currentParam = 0; currentParam < argc; currentParam++)
		{
			pattern = wcu8.from_bytes(argv[currentParam]);
			exp.generate(pattern);

		}
		// Validate will not produce any data output
		wcout << exp;
	}
	else
	{
		string temp;
		while (cin >> temp)
		{
			pattern = wcu8.from_bytes(temp);
			exp.generate(pattern);
		}
		// Validate will not produce any data output
		wcout << exp;
	}
}

void execute_validate_command(int argc, char *argv[])
{
	wstring pattern;
	PatternExpander::Expander exp;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;

	if (argc > 0)
	{
		for (int currentParam = 0; currentParam < argc; currentParam++)
		{
			pattern = wcu8.from_bytes(argv[currentParam]);
			exp.validate(pattern);
			wcerr << exp.output.str();
		}
		// Validate will not produce any data output
		wcout << exp;
	}
	else
	{
		string temp;
		while (cin >> temp)
		{
			pattern = wcu8.from_bytes(temp);
			exp.validate(pattern);
			wcerr << exp.output.str();
		}
		// Validate will not produce any data output
		wcout << exp;
	}
}

void setConfig(int argc, char *argv[], const string& filePath)
{
	PatternExpander::Expander exp;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
	int i = 0;
	while (i < argc)
	{
		string option(argv[i++]);
		if (i >= argc)
		{
			cerr << "Skipping option '" << option << "' - no value provided" << endl;
			return;
		}
		string temp(argv[i++]);
		wstring val =  wcu8.from_bytes(temp);
		if (val.length() != 1)
		{
			cerr << "Skipping option '" << option << "' due to invalid value '" << temp << "'. Values are single characters only." << endl;
			continue;
		}

		std::transform(option.begin(), option.end(), option.begin(),
						::tolower);
		if (option == "-q" || option == "--quote")
			exp.setQuote(val[0]);
		else if (option == "-e" || option == "--esc")
			exp.setEscChar(val[0]);
		else if (option == "-b" || option == "--group_begin" )
			exp.setGroupBegin(val[0]);
		else if (option == "-n" || option == "--group_end")
			exp.setGroupEnd(val[0]);
		else if (option == "-r" || option == "range")
			exp.setRangeChar(val[0]);
		else
			cout << "Skipping unknown option: "<< option << endl;
	}
	exp.saveConfig(filePath);
}
