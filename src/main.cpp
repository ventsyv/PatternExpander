#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <algorithm>
#include "Expander.h"

using namespace std;

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
				exp.validate(pattern);
			else if (command == "run")
				exp.generate(pattern);
				
			wcout << exp;
				
		}
	}
	else
	{
		string temp;
		while (cin >> temp)
		{
			pattern = wcu8.from_bytes(temp);
			if (command == "validate")
				exp.validate(pattern);
			else if (command == "run")
				exp.generate(pattern);
		}
		
		wcout << exp;
	}
}

int main(int argc, char *argv[])
{
    
    std::setlocale(LC_CTYPE, "C.UTF8"); 
    
    wstring usage = LR"(usage: patexp [-h | --help] <command> [<args>]
	commands:
		run [<pat1> <pat2> .. ] | <stdin> : Generates strings from a list of patterns
		validate [<pat1> <pat2> .. ] | <stdin> : Validates if the patterns provided are syntaxically correct
		configure: Sets the various configuration options)";

	if (argc > 1)
	{
		string command(argv[1]);
		std::transform(command.begin(), command.end(), command.begin(), ::tolower);
		if (command == "-h" || command=="--help")
		{
			wcout << usage << endl;
			exit(0);
		}
		else if (command == "run" || command == "validate")
		{
			run_command(argc, argv);
			
		}
		else if ( command == "config" || command == "configure")
		{
			wcout << "Configure is currently not implemented"<<endl;
			exit(1);
		}
		else
		{
			wcerr << "Error: unknown command" << endl;
			wcout << usage << endl;
		}
			
	}
	else
	{
		// command not provided
		wcerr << "Error: no command was provided" << endl;
		wcout << usage << endl;
	}
	
	
	

	return 0;
}

