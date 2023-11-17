#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <algorithm>
#include <filesystem>
#include "Expander.h"

using namespace std;

const string DEFAULT_CONFIG_FILE_NAME = ".pathexpconfig";

void run_command(int argc, char *argv[])
{
	wstring pattern;
	PatternExpander::Expander exp;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;

	string command(argv[1]);
	std::transform(command.begin(), command.end(), command.begin(), ::tolower);

	if (argc > 2)
	{
		for (int currentParam = 2; currentParam < argc; currentParam++)
		{
			pattern = wcu8.from_bytes(argv[currentParam]);
			if (command == "validate")
			{
				exp.validate(pattern);
				wcerr << exp.output.str();
			}
			else if (command == "run")
			{
				exp.generate(pattern);
			}

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
			if (command == "validate")
			{
				exp.validate(pattern);
				wcerr << exp.output.str();
			}
			else if (command == "run")
			{
				exp.generate(pattern);
			}
		}
		// Validate will not produce any data output
		wcout << exp;
	}
}

void setConfig(int argc, char *argv[], const string& filePath)
{
	PatternExpander::Expander exp;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
	int i = 2;
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

		exp.saveConfig(filePath);

	}
}

int main(int argc, char *argv[])
{

	std::setlocale(LC_CTYPE, "C.UTF8");

	string usage = R"(usage: patexp [-h | --help] <command> [<args>]
	commands:
	run [<pat1> <pat2> .. ] | <stdin> : Generates strings from a list of patterns
	validate [<pat1> <pat2> .. ] | <stdin> : Validates if the patterns provided are syntaxically correct
	configure: Sets the various configuration options
		-b, --group_begin <val> : Sets the group begin symbol. Default: '['
		-e, --esc         <val> : Sets the escape symbol. Default: '/'
		-q, --quote       <val> : Sets the quote symbols. Default: '"'
		-n, --group_end   <val> : Sets the group end symbol. Default: ']'
		-r, --range       <val> : Sets the range symbol. Default: '-'
)";

	if (argc > 1)
	{
		string command(argv[1]);
		std::transform(command.begin(), command.end(), command.begin(),
				::tolower);
		if (command == "-h" || command == "--help")
		{
			cout << usage << endl;
			exit(0);
		}
		else if (command == "run" || command == "validate")
		{
			run_command(argc, argv);

		}
		else if (command == "config" || command == "configure")
		{
			filesystem::path path(getenv("HOME"));
			path.append(DEFAULT_CONFIG_FILE_NAME);

			setConfig(argc, argv, path);
		}
		else
		{
			cerr << "Error: unknown command" << endl;
			cout << usage << endl;
		}

	}
	else
	{
		// command not provided
		cerr << "Error: no command was provided" << endl;
		cout << usage << endl;
	}

	return 0;
}

