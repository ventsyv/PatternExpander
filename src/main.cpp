#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <algorithm>
#include "Expander.h"

using namespace std;

int main(int argc, char *argv[])
{
	wstring pattern;
    PatternExpander::Expander exp;
    
    std::setlocale(LC_CTYPE, "C.UTF8"); 
    
    wstring usage = LR"(usage: patexp [-h | --help] <command> [<args>]
	commands:
		run: Generates strings from a list of patterns
		verify: Verify if the patterns provided are syntaxically correct
		configure: Sets the various configuration options)";

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
	if (argc > 1)
	{
		string command(argv[1]);
		std::transform(command.begin(), command.end(), command.begin(), ::tolower);
		if (command == "-h" || command=="--help")
		{
			wcout << usage << endl;
			exit(0);
		}
		else if (command == "run")
		{
			if (argc > 2)
			{
				for (int currentParam = 2; currentParam < argc; currentParam++)
				{
					pattern = wcu8.from_bytes(argv[currentParam]);
					exp.generate(pattern);
				}
			}
			else
			{
				string temp;
				while (cin >> temp)
				{
					pattern = wcu8.from_bytes(temp);
					exp.generate(pattern);
				}
			}
	
			//Output the results
			wcout<<exp;
		}
		else if (command == "verify") 
		{
			if (argc > 2)
			{
				for (int currentParam = 2; currentParam < argc; currentParam++)
				{
					pattern = wcu8.from_bytes(argv[currentParam]);
					exp.validate(pattern);
				}
			}
			else
			{
				string temp;
				while (cin >> temp)
				{
					pattern = wcu8.from_bytes(temp);
					exp.validate(pattern);
				}
			}
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

