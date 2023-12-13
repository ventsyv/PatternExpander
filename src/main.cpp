#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <algorithm>
#include <filesystem>
#include "Helpers.h"
#include "Expander.h"

using namespace std;

const string DEFAULT_CONFIG_FILE_NAME = ".pathexpconfig";


int main(int argc, char *argv[])
{

	std::setlocale(LC_CTYPE, "C.UTF8");

	string usage = R"(usage: patexp [-h | --help] <command> [<args>]
	commands:
	run [<pat1> <pat2> .. ] | <stdin> : Generates strings from a list of patterns
	validate [<pat1> <pat2> .. ] | <stdin> : Validates if the patterns provided are syntaxically correct
	configure: Sets the various configuration options
		[-b, --group_begin <val>] : Sets the group begin symbol. Default: '['
		[-e, --esc         <val>] : Sets the escape symbol. Default: '/'
		[-q, --quote       <val>] : Sets the quote symbols. Default: '"'
		[-n, --group_end   <val>] : Sets the group end symbol. Default: ']'
		[-r, --range       <val>] : Sets the range symbol. Default: '-'
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
		else if (command == "run")
		{
			//The first arg is the name of the executable so start at the one after
			execute_run_command(argc - 1, argv + 1);
		}
		else if (command == "validate")
		{
			execute_validate_command(argc - 1, argv + 1)

		}
		else if (command == "config" || command == "configure")
		{
			filesystem::path path(getenv("HOME"));
			path.append(DEFAULT_CONFIG_FILE_NAME);

			//The first arg is the name of the executable
			//The second argument is the name of the command (in this case "config")
			//We need to skip both of those
			setConfig(argc - 2, argv + 2, path);
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

