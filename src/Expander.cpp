#include "Expander.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace PatternExpander;

Expander::Expander(wchar_t esc, wchar_t range, wchar_t grpBegin, wchar_t grpEnd) :
		escapeSymbol(esc), rangeSymbol(range), groupBegin(grpBegin), groupEnd(
				grpEnd)
{
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

	//This will expand all range (a-b) expressions
	wstring expandedPattern = expand(pattern);
	size_t pLength = expandedPattern.length();
	if (pLength == 0)
	{
		return;
	}

	//Increment when unescaped [ is reached, decrement when ]
	uint load = 0;
	bool escSeqReached = false;

	//When entering a new group, the partial patters are saved off in temp
	bool isFirstInGroup = true;
	vector<wstring> partials;

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
		else if (expandedPattern[i] == groupBegin && !escSeqReached)
		{
			isFirstInGroup = true;
			load++;
			continue;
		}
		else if (expandedPattern[i] == groupEnd && !escSeqReached)
		{
			load--;
			continue;
		}
		else if (expandedPattern[i] == DOUBLE_QUOTE && !escSeqReached)
		{
			//Just skip everything thats in quotes
			uint startIndex = ++i;
			while (expandedPattern[i] != DOUBLE_QUOTE && i < pLength)
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

uint Expander::getBlockElements(const wstring &pattern, uint &start,
		vector<wstring> &items)
{
	uint currentIndx = start; //remember we are going from right to left

	//Find the end of the block
	while (currentIndx != 0) //we are going backwards
	{
		if (pattern[currentIndx] == groupBegin
				|| pattern[currentIndx] == groupEnd)
			break; //reached the end of the block
		currentIndx--;
	}
	uint endIndx = currentIndx;
	uint itemCount = start - endIndx;

	//Find all escape sequences
	for (currentIndx = endIndx + 1; currentIndx <= start; currentIndx++)
	{
		//count one less character since escape sequences (two character) are counted as one character
		if (isEscSeq(pattern, currentIndx, true))
			itemCount--;
		//Starting a constant block - multiple character are counted as one item
		else if (pattern[currentIndx] == '"')
		{
			currentIndx++; //move inside the quote
			while (currentIndx <= start)
			{
				itemCount--;
				if (pattern[currentIndx] == '"')
					break;
				currentIndx++;
			}
		}
	} //end for
	  //We should have the actual number of items
	  //*items = new wstring[itemCount];

	  //Extract the items
	uint i = 0;
	wstring temp;
	for (currentIndx = endIndx + 1; currentIndx <= start; currentIndx++) //now we move left to right
	{

		if (pattern[currentIndx] == '"')
		{
			uint strt = ++currentIndx; //move inside the quote
			while (currentIndx < start)
			{
				if (pattern[currentIndx] == '"')
					break;
				currentIndx++;
			}
			//Extract the quote
			temp = pattern.substr(strt, currentIndx - strt);
			items[i] = temp;
			i++;
		}
		else if (!isEscSeq(pattern, currentIndx, true)) //just normal characters
		{
			//TODO: Check if we really need to have an exclude symbol
//			if (pattern[currentIndx] != excludeSymbol)
			items[i] = pattern[currentIndx];
//			else
//				(*items)[i] = "";
//			i++;
		}
		else if (isEscSeq(pattern, currentIndx, true))
		{
			currentIndx++; //get the escaped character
			items[i] = pattern[currentIndx];
			i++;
		}
	}
	start = endIndx; //that's where the pattern processing will resume
	return itemCount;
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

		if (second == escapeSymbol || second == groupBegin || second == groupEnd || second == DOUBLE_QUOTE)
		{
			result = true;
		}
		else
		{
			result = false;
		}
}
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
		else if (pattern[i] == DOUBLE_QUOTE)
		{
			//Just skip everything thats in quotes
			i++;
			while (pattern[i] != DOUBLE_QUOTE && i < size)
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
		else if (pattern[i] == rangeSymbol && load > 0) //range symbol reached
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
				//i += 2;
			}
		}
		//result = "";
	} //end for

	if (result == L"")
		result = pattern;
	return result;
}

bool Expander::validate(const wstring &pattern)
{
	int loadBrackets = 0;
	uint loadQuotes = 0;

	for (uint i = 0; i < pattern.size(); i++)
	{

		if (isEscSeq(pattern, i, loadBrackets > 0))
		{
			i++;
		}
		else
		{
			if (pattern[i] == groupBegin)
				loadBrackets++;
			else if (pattern[i] == groupEnd)
				loadBrackets--;
			else if (pattern[i] == DOUBLE_QUOTE)
				loadQuotes++;

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

void Expander::getCombinations(vector<wstring> &data,
		vector<wstring> &newElements)
{
	vector<wstring> original = data;
	vector<wstring> temp = data;

	auto numNewElements = newElements.size();
	auto numOrgElements = original.size();

	//Nothing to do, just return
	if (numNewElements == 0)
	{
		return;
	}

	//If the initial list is empty, just add the first entity of the new list
	if (numOrgElements == 0)
	{
		data.push_back(newElements[0]);
	}
	else
	{

		for (size_t i = 0; i < numOrgElements; i++)
		{
			data[i] = data[i] + newElements[0];
		}
	}

	for (size_t i = 1; i < numNewElements; i++)
	{
		for (size_t j = 0; j < numOrgElements; j++)
		{
			temp[j] = temp[j] + newElements[i];
		}
		data.insert(std::end(data), std::begin(temp), std::end(temp));
		temp = original;
	}
}

void Expander::processGroup(const wstring &pattern, uint &i, uint &currentItem)
{
	//wstring* newItems = NULL;
	vector<wstring> newItems;
	int eCount = getBlockElements(pattern, i, newItems);
	if (currentItem == 0) //data is empty
	{
		for (int indx = 0; indx < eCount; indx++)
		{
			data[currentItem] = newItems[indx];
			currentItem++;
		}
	}
	else //partial patterns already present
	{
		int nDataElements = currentItem; //number of partial patters
		//after combination total number of patters will be nDataElements*eCount
		//since the data vector already contains 1 set of elemens
		//those need to be copied (nDataElements-1)*eCount times
		for (int el = 0; el < eCount - 1; el++)
		{
			for (int dataEl = 0; dataEl < nDataElements; dataEl++)
			{
				data[currentItem] = data[dataEl];
				currentItem++;
				//resizeResult(currentItem);
			}
		}
		//do the actual combination
		int indx = 0; //current block element
		for (uint k = 0; k < currentItem; k++)
		{

			if (k != 0 && k % nDataElements == 0)
				indx++; //move to next block element
			//all block elements were processed
			if (indx >= eCount)
				break;
			data[k] = newItems[indx] + data[k];
		}
	}
	//delete[] newItems; //clean up temporary resource
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
	}

}

void Expander::saveConfig(const std::string& filePath)
{
	wofstream out(filePath, ios::out);

	out << L"range " << getRangeChar() << std::endl;
	out << L"escape " << getEscChar() << endl;

	out << L"group.begin " << getGroupBegin() << endl;
	out << L"group.end " << getGroupEnd() << endl;

}
