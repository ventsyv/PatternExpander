#include <iostream>
#include <string>
#include "Expander.h"

using namespace std;

int main(int argc, char *argv[])
{
	string pattern;
    PatternExpander::Expander exp;

	if (argc > 1)
	{
		for (int currentParam = 1; currentParam < argc; currentParam++)
		{
			string pattern = string(argv[currentParam]);

			exp.generate(pattern);
		}
	}
	else
	{
		while (cin >> pattern)
		{
			exp.generate(pattern);
		}
	}
	
	//Output the results
	cout<<exp;
	
	

	return 0;
}

