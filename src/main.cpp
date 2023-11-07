#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include "Expander.h"

using namespace std;

int main(int argc, char *argv[])
{
	wstring pattern;
    PatternExpander::Expander exp;

	if (argc > 1)
	{
		for (int currentParam = 1; currentParam < argc; currentParam++)
		{
			
			
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;				
			wstring pattern = wcu8.from_bytes(argv[currentParam]);

			exp.generate(pattern);
		}
	}
	else
	{
		while (wcin >> pattern)
		{
			exp.generate(pattern);
		}
	}
	
	//Output the results
	wcout<<exp;
	
	

	return 0;
}

