#include "Expander.h"
#include <string>
#include <iomanip>

using namespace std;
using namespace PatternExpander;

Expander::Expander(char esc, char range, char grpBegin, char grpEnd) :
        escapeSymbol(esc), rangeSymbol(range), groupBegin(grpBegin), groupEnd(grpEnd) {}

void Expander::generate(const string &pattern)
{
    data.clear();
    vector<string> results;

    //This will expand all range (a-b) expressions
    string expandedPattern = expand(pattern);
    size_t pLength = expandedPattern.length();
    if (pLength == 0) {
        return;
    }

    uint currentItem = 0; //current pattern in the data array
    //Increment when unescaped [ is reached, decrement when ]
    uint load = 0;
    bool escSeqReached = false;

    //When entering a new group, the partial patters are saved off in temp
    bool isFirstInGroup = true;
    vector<string> partials;


    for (uint i = 0; i < pLength; i++) {
        if (expandedPattern[i] == escapeSymbol) //escape character reached
        {
            if (isEscSeq(expandedPattern, i)) //check if the next character is a valid escape sequence
            {
                escSeqReached = true;
                continue;
            }

        } else if (expandedPattern[i] == '[' && !escSeqReached) {
            isFirstInGroup = true;
            load++;
            continue;
        } else if (expandedPattern[i] == ']' && !escSeqReached) {
            load--;
            continue;
        }

        //it is a part of the pattern
        if (load == 0) //constant character - escape sequences here are disregarded
        {
            if (currentItem == 0) //data array is empty
            {
                results.push_back(string(1,expandedPattern[i]));
				currentItem++;
			} else //variations are present.
			{
				//add the constant character to all variations
				//Just appending the constant character to all patterns
				for (uint j = 0; j < currentItem; j++)
				{
					results[j] = results[j] + pattern[i];
				}
			}
		} else if (load > 0) //character inside a variable block
		{
			//when we first enter the variable block, we save the
            //existing patterns
			if (isFirstInGroup)
			{
				partials = results;
                results.clear();
				isFirstInGroup = false;
			}

            //It's possible the variable block appears first in the pattern
            //That means that the result set is empty
            //If that's the case, just add the first item from the varible block
            if (partials.empty())
                results.push_back(string(1,expandedPattern[i]));
            else {
                //The result set is not empty
                //Append the first item of the variable block to the existing patterns,
                //then add it the result set.
                for (uint item = 0; item < partials.size(); item++) {
                    results.push_back(partials[item] + expandedPattern[i]);
                }
            }

		}
		escSeqReached = false;
	} //end for


	data.insert(data.end(), results.begin(), results.end());
}

uint Expander::getBlockElements(const string& pattern, uint& start,
		vector<string>& items)
{
	uint currentIndx = start; //remember we are going from right to left

	//Find the end of the block
	while (currentIndx >= 0) //we are going backwards
	{
		if (pattern[currentIndx] == '[' || pattern[currentIndx] == ']')
			break; //reached the end of the block
		currentIndx--;
	}
	uint endIndx = currentIndx;
	uint itemCount = start - endIndx;

	//Find all escape sequences
	for (currentIndx = endIndx + 1; currentIndx <= start; currentIndx++)
	{
		//count one less character since escape sequences (two characters) are counted as one character
		if (isEscSeq(pattern, currentIndx))
			itemCount--;
		//Starting a constant block - multiple characters are counted as one item
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
	//*items = new string[itemCount];

	//Extract the items
	uint i = 0;
	string temp;
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
		else if (!isEscSeq(pattern, currentIndx)) //just normal characters
		{
            //TODO: Check if we really need to have an exclude symbol
//			if (pattern[currentIndx] != excludeSymbol)
				items[i] = pattern[currentIndx];
//			else
//				(*items)[i] = "";
//			i++;
		}
		else if (isEscSeq(pattern, currentIndx))
		{
			currentIndx++; //get the escaped character
			items[i] = pattern[currentIndx];
			i++;
		}
	}
	start = endIndx; //that's where the pattern processing will resume
	return itemCount;
}

inline bool Expander::isEscSeq(const std::string& pattern, uint position) const
{
	bool result = false;
	if (position + 1 <= pattern.length() && pattern[position] == escapeSymbol) //first char. in sequence is indeed escape char.
	{
		//check second char in sequence
        char second = pattern[position + 1];
		if (second == escapeSymbol || second == rangeSymbol || second == groupBegin || second == groupEnd) {
            result = true;
        } else {
				result = false;
		}
	}
	return result;
}

std::string Expander::expand(const std::string& pattern)
{
    int size = pattern.length();
    string result;
    string expanded;

    for (int i = 0; i < size; i++)
    {
        if (isEscSeq(pattern, i))
        {
            i++;
            continue;
        }
        else if (pattern[i] == rangeSymbol) //range symbol reached
        {
            if (i - 1 < 0 || i + 1 >= size)
                return "";
            else if (!isEscSeq(pattern, i)) //the character was not escaped
            {
                string preStr = pattern.substr(0, i - 1); //the presiding str
                string postStr = pattern.substr(i + 2, size - (i + 2)); //the rest of the original str

                char startRange = pattern[i - 1];
                char endRange = pattern[i + 1];

                //if both are alphabetical characters
                if ((isalpha(startRange) && isalpha(endRange))
                    || (isdigit(startRange) && isdigit(endRange)))
                {
                    if ((int) startRange < (int) endRange) //inorder
                    {
                        for (int j = startRange; j <= endRange; j++)
                        {
                            expanded += (char) j;
                        }
                    }
                    else //reverse order
                    {
                        for (int j = startRange; j >= endRange; j--)
                        {
                            expanded += (char) j;
                        }
                    }
                }
                else
                    return ""; //range does not seem valid
                result = preStr + expanded + postStr;
                size = pattern.length();
                i += 2;
            }
        }
        //result = "";
    } //end for

    if (result == "")
        result = pattern;
    return result;
}



bool Expander::validate(const string& pattern)
{
    int loadBrackets = 0;
    uint loadQuotes = 0;

	for (uint i = 0; i < pattern.size(); i++)
	{
		if (pattern[i] == escapeSymbol) {
            if (!isEscSeq(pattern, i)) {
                output << "Error: Invalid escape sequence detected" << endl;
                output << pattern << endl;
                output << std::setw(i) << "^" << endl;
                return false;
            }
            else
            {
                i++;
            }
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

    return ((loadQuotes % 2 == 0) && (loadBrackets % 2 == 0));
}

void Expander::getCombinations(vector<string>& data, vector<string>& newElements)
{
	vector<string> original = data;
	vector<string> temp = data;

	uint numNewElements = newElements.size();
	uint numOrgElements = original.size();

	//Nothing to do, just return
	if (numNewElements == 0)
	{
		return;
	}

	//If the initial list is empty, just add the first entity of the new list
	if(numOrgElements == 0)
	{
		data.push_back(newElements[0]);
	}
	else
	{

		for (int i= 0; i < numOrgElements; i++)
		{
			data[i] = data[i] + newElements[0];
		}
	}

	for (int i = 1; i < numNewElements; i++)
	{
		for (int j = 0; j < numOrgElements; j++)
		{
			temp[j] = temp[j] + newElements[i];
		}
		data.insert(std::end(data), std::begin(temp), std::end(temp));
		temp = original;
	}
}

void Expander::processGroup(const string& pattern, uint& i,
		uint& currentItem)
{
	//string* newItems = NULL;
    vector<string> newItems;
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
		for (int k = 0; k < currentItem; k++)
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

std::vector<std::string> Expander::getData() {
    return data;
}
