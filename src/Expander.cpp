#include "Expander.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace PatternExpander;

Expander::Expander(wchar_t esc, wchar_t range, wchar_t grpBegin, wchar_t grpEnd, wchar_t quote) :
		escapeSymbol(esc), rangeSymbol(range), groupBegin(grpBegin), groupEnd(
				grpEnd), quote(quote)
{
	//If the config file exists, load it. If it does not, the constructor arguments are used
	loadConfig();
}

void Expander::append(vector<wstring> &results, const wstring &newData)
{
	if (results.size() == 0) //data array is empty
	{
		results.push_back(newData);
	}
	else //variations are present.
	{
		//add the constant character to all variations
		//Just appending the constant character to all patterns
		for (size_t j = 0; j < results.size(); j++)
		{
			results[j] = results[j] + newData;
		}
	}
}

void Expander::appendGroup(bool &isFirstInGroup, const wstring &newData, vector<wstring> &partials, vector<wstring> &results)
{
	//when we first enter the variable block, we save the
	//existing patterns
	if (isFirstInGroup)
	{
		partials = results;
		//The variable block is not optional, so we need to clear the result set
		//For example: 1[a-c] is expected to produce 1a, 1b, 1c
		//Without the clear, it produces 1, 1a, 1b, 1c
		results.clear();
		isFirstInGroup = false;
	}
	//It's possible the variable block appears first in the pattern
	//That means that the result set is empty
	//If that's the case, just add the first item from the varible block
	if (partials.empty())
		results.push_back(newData);
	else
	{
		//The result set is not empty
		//Append the first item of the variable block to the existing patterns,
		//then add it the result set.
		for (uint item = 0; item < partials.size(); item++)
		{
			results.push_back(partials[item] + newData);
		}
	}
}

void Expander::generate(const wstring &pattern)
{
	vector<wstring> results;

	//Check if the pattern is valid
	if (!validate(pattern))
		return;

	//This will expand all range (a-b) expressions
	wstring expandedPattern = expand(pattern);
	size_t pLength = expandedPattern.length();

	//Increment when un-escaped [ is reached, decrement when ]
	uint load = 0;
	//Flag that indicates an escape sequence has been reached
	bool escSeqReached = false;

	//Flags indicates that we've entered a new group.
	bool isFirstInGroup = true;
	//When entering a new group, the partial patterns are saved off in temp variable
	vector<wstring> partials;

	//Loop over the epanded pattern
	for (uint i = 0; i < pLength; i++)
	{
		//check if a new escape sequence is reached
		if (expandedPattern[i] == escapeSymbol && !escSeqReached)
		{
			if (isEscSeq(expandedPattern, i, load > 0)) //check if the next character is a valid escape sequence
			{
				escSeqReached = true;
				continue;
			}

		}
		else if (expandedPattern[i] == groupBegin && !escSeqReached) //un-escaped group start reached
		{
			isFirstInGroup = true;
			load++;
			continue;
		}
		else if (expandedPattern[i] == groupEnd && !escSeqReached) //un-escaped group end reached
		{
			load--;
			continue;
		}
		else if (expandedPattern[i] == quote && !escSeqReached) //un-escaped string literal reached
		{
			//Just skip everything thats in quotes
			uint startIndex = ++i;
			while (expandedPattern[i] != quote && i < pLength)
			{
				i++;
			}

			uint numChars = i - startIndex;
			wstring quotedStr = expandedPattern.substr(startIndex, numChars);
			if (load == 0) //constant character - escape sequences here are disregarded
			{
				append(results, quotedStr);
			}
			else if (load > 0) //character inside a variable block
			{
				appendGroup(isFirstInGroup, quotedStr, partials, results);
			}
			//Skip the trailing quote
			continue;
		}

		if (load == 0) //constant character - escape sequences here are disregarded
		{
			append(results, wstring(1,expandedPattern[i]));
		}
		else if (load > 0) //character inside a variable block
		{
			auto newData = wstring(1,expandedPattern[i]);
			appendGroup(isFirstInGroup, newData, partials, results);
		}
		escSeqReached = false;
	} //end for

	data.insert(data.end(), results.begin(), results.end());
}


inline bool Expander::isEscSeq(const std::wstring &pattern, uint position, bool isInGroup) const
{
	bool result = false;
	if (position + 1 <= pattern.length() && pattern[position] == escapeSymbol) //first char. in sequence is indeed escape char.
	{
		//check second char in sequence
		wchar_t second = pattern[position + 1];

		//Range symbols are only valid in a group so no need to escape if we are not in one
		if (isInGroup)
		{
			if (second == rangeSymbol)
				return true;

		}

		if (second == escapeSymbol || second == groupBegin || second == groupEnd || second == quote)
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}//end if
	return result;
}

std::wstring Expander::expand(const std::wstring &pattern)
{
	int size = pattern.length();
	wstring result = pattern;
	wstring expanded;

	uint load = 0;
	for (int i = 0; i < size; i++)
	{
		if (isEscSeq(pattern, i, load > 0))
		{
			i++;
			continue;
		}
		else if (pattern[i] == quote)
		{
			//Just skip everything thats in quotes
			i++;
			while (pattern[i] != quote && i < size)
			{
				i++;
			}
		}
		else if (pattern[i] == groupBegin)
		{
			load++;
		}
		else if (pattern[i] == groupEnd)
		{
			load--;
		}
		else if (pattern[i] == rangeSymbol && load > 0) //range symbol reached. Ranges are valid in groups only, not in static portions of the pattern
		{
			expanded = L"";
			if (i - 1 < 0 || i + 1 >= size)
				return L"";
			else if (!isEscSeq(pattern, i, load > 0)) //the character was not escaped
			{
				wstring preStr = result.substr(0, i - 1); //the presiding str
				wstring postStr = result.substr(i + 2, size - (i + 2)); //the rest of the original str

				wchar_t startRange = pattern[i - 1];
				wchar_t endRange = pattern[i + 1];

				std::locale loc2("C.UTF8");

				//if both are alphabetical chars
				if ((isalpha(startRange, loc2) && isalpha(endRange, loc2))
						|| (isdigit(startRange, loc2) && isdigit(endRange, loc2)))
				{
					if ((int) startRange < (int) endRange) //inorder
					{
						for (int j = startRange; j <= endRange; j++)
						{
							expanded += (wchar_t) j;
						}
					}
					else //reverse order
					{
						for (int j = startRange; j >= endRange; j--)
						{
							expanded += (wchar_t) j;
						}
					}
				}
				else
					return L""; //range does not seem valid
				result = preStr + expanded + postStr;
				size = pattern.length();
			}
		}
	} //end for

	if (result == L"")
		result = pattern;
	return result;
}

bool Expander::validate(const wstring &pattern)
{
	int loadBrackets = 0;
	uint loadQuotes = 0;

	for (int i = 0; i < (int)pattern.size(); i++)
	{

		if (isEscSeq(pattern, i, loadBrackets > 0))
		{
			i++;//escape sequence - just keep going
		}
		else
		{
			if (pattern[i] == groupBegin)
				loadBrackets++;
			else if (pattern[i] == groupEnd)
				loadBrackets--;
			else if (pattern[i] == quote)
				loadQuotes++;
			else if (pattern[i] == rangeSymbol && loadBrackets > 0) //range symbol reached. Ranges are valid in groups only, not in static portions of the pattern
			{
				if (i - 1 < 0 || i + 1 >= (int)pattern.size())
				{
					output << "Error: Invalid range found"<<endl;
					return false;
				}

				wchar_t startRange = pattern[i - 1];
				wchar_t endRange = pattern[i + 1];

				std::locale loc2("C.UTF8");

				//if either the start on end character are not alphanumeric return an error
				//This might need to be removed if support for East Asian languages is ever to be added
				//Not sure how that works in logographic languages
				if ((!isalpha(startRange, loc2) && !isdigit(startRange, loc2))
						|| (!isalpha(endRange, loc2) && !isdigit(endRange, loc2)))
				{
					output << "Error: Invalid non alpha-numerical range found"<<endl;
					return false;
				}
			}

			if (loadBrackets < 0)
			{
				output << "Error: Invalid group sequence detected" << endl;
				output << pattern << endl;
				output << std::setw(i) << "^" << endl;
				return false;
			}

		}
	}

	if (loadBrackets != 0)
	{
		output << "Error: Unclosed group bracket" << endl;
	}

	if (loadQuotes %2 != 0)
	{
		output << "Error: Unclosed quote detected" << endl;
	}

	return ((loadQuotes % 2 == 0) && (loadBrackets % 2 == 0));
}



std::vector<std::wstring> Expander::getData()
{
	return data;
}

void Expander::loadConfig(const std::string& filePath)
{

	wifstream in(filePath, ios::in);
	while (in)
	{
		wstring key;
		wstring val;
		in >> key >> val;

		std::transform(key.begin(), key.end(), key.begin(), ::tolower);

		if (key == L"range")
		{
			setRangeChar(val[0]);
		}
		else if (key == L"escape")
		{
			setEscChar(val[0]);
		}
		else if (key == L"group.begin")
		{
			setGroupBegin(val[0]);
		}
		else if (key == L"group.end")
		{
			setGroupEnd(val[0]);
		}
		else if (key == L"quote")
		{
			setQuote(val[0]);
		}
	}

}

void Expander::saveConfig(const std::string& filePath)
{
	wofstream out(filePath, ios::out);

	out << L"range " << getRangeChar() << std::endl;
	out << L"escape " << getEscChar() << endl;

	out << L"group.begin " << getGroupBegin() << endl;
	out << L"group.end " << getGroupEnd() << endl;
	out << L"quote " << getQuote() << endl;

	out.close();

}
