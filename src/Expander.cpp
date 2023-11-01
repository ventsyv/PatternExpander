#include "Expander.h"
#include <string>
#include <iomanip>

using namespace std;
using namespace PatternExpander;

Expander::Expander(char esc, char range, char grpBegin, char grpEnd) :
        escapeSymbol(esc), rangeSymbol(range), groupBegin(grpBegin), groupEnd(grpEnd) {}

void Expander::generate(const string &pattern) { std::ignore = pattern; }

//    vector<string> results;
//
//    //This will expand all range (a-b) expressions
//    string expandedPattern = expand(pattern);
//    size_t pLength = expandedPattern.length();
//    if (pLength == 0) {
//        return;
//    }
//
//    uint currentItem = 0; //current pattern in the data array
//    //Increment when unescaped [ is reached, decrement when ]
//    uint load = 0;
//    bool escSeqReached = false;
//
//    //When entering a new group, the partial patters are saved off in temp
//    bool isFirstInGroup = true;
//    vector<string> partials;
//
//
//    for (uint i = 0; i < pLength; i++) {
//        if (expandedPattern[i] == escapeSymbol) //escape character reached
//        {
//            if (!isEscSeq(expandedPattern, i)) //check if the next character is a valid escape sequence
//            {
//                return;
//            } else {
//                escSeqReached = true;
//            }
//            continue;
//        } else if (expandedPattern[i] == '[' && !escSeqReached) {
//            isFirstInGroup = true;
//            load++;
//            continue;
//        } else if (expandedPattern[i] == ']' && !escSeqReached) {
//            load--;
//            continue;
//        }
//
//        //it is a part of the pattern
//        if (load == 0) //constant character - escape sequences here are disregarded
//        {
//            if (currentItem == 0) //data array is empty
//            { //No need to resize here, the array is SUPPOSED to be empty ...
//                results.push_back(to_string(expandedPattern[i]));
//				currentItem++;
//			} else //variations are present.
//			{
//				//add the constant character to all variations
//				//No need to resize, not adding new items, just adding const. char. to current patterns
//				for (int j = 0; j < currentItem; j++)
//				{
//					results[j] = results[j] + pattern[i];
//				}
//			}
//		} else if (load > 0) //character inside a variable block
//		{
//			//handleVariableBlock(pattern, (uint) i, currentItem);
//			if (isFirstInGroup)
//			{
//				partials = results;
//				isFirstInGroup = false;
//			}
//
//			for (uint item=0; item < temp.size(); item++)
//			{
//				results.push_back(partials[item] + expandedPattern[i]);
//			}
//
//		}
//		escSeqReached = false;
//	} //end for
//
//
//	data.insert(data.end(), results.begin(), results.end());
//}
//
//uint Expander::getBlockElements(const string& pattern, uint& start,
//		const vector<string>& items)
//{
//	uint currentIndx = start; //remember we are going from right to left
//
//	//Find the end of the block
//	while (currentIndx >= 0) //we are going backwards
//	{
//		if (pattern[currentIndx] == '[' || pattern[currentIndx] == ']')
//			break; //reached the end of the block
//		currentIndx--;
//	}
//	uint endIndx = currentIndx;
//	uint itemCount = start - endIndx;
//
//	//Find all escape sequences
//	for (currentIndx = endIndx + 1; currentIndx <= start; currentIndx++)
//	{
//		//count one less character since escape sequences (two characters) are counted as one character
//		if (isEscSeq(pattern, currentIndx))
//			itemCount--;
//		//Starting a constant block - multiple characters are counted as one item
//		else if (pattern[currentIndx] == '"')
//		{
//			currentIndx++; //move inside the quote
//			while (currentIndx <= start)
//			{
//				itemCount--;
//				if (pattern[currentIndx] == '"')
//					break;
//				currentIndx++;
//			}
//		}
//	} //end for
//	  //We should have the actual number of items
//	//*items = new string[itemCount];
//
//	//Extract the items
//	uint i = 0;
//	string temp;
//	for (currentIndx = endIndx + 1; currentIndx <= start; currentIndx++) //now we move left to right
//	{
//
//		if (pattern[currentIndx] == '"')
//		{
//			uint strt = ++currentIndx; //move inside the quote
//			while (currentIndx < start)
//			{
//				if (pattern[currentIndx] == '"')
//					break;
//				currentIndx++;
//			}
//			//Extract the quote
//			temp = pattern.substr(strt, currentIndx - strt);
//			(*items)[i] = temp;
//			i++;
//		}
//		else if (!isEscSeq(pattern, currentIndx)) //just normal characters
//		{
//            //TODO: Check if we really need to have an exclude symbol
////			if (pattern[currentIndx] != excludeSymbol)
//				(*items)[i] = pattern[currentIndx];
////			else
////				(*items)[i] = "";
////			i++;
//		}
//		else if (isEscSeq(pattern, currentIndx))
//		{
//			currentIndx++; //get the escaped character
//			(*items)[i] = pattern[currentIndx];
//			i++;
//		}
//	}
//	start = endIndx; //that's where the pattern processing will resume
//	return itemCount;
//}

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

//std::string Expander::expand(const std::string& pattern)
//{
//	uint size = pattern.length();
//	string result;
//
//	uint position = pattern.find(rangeSymbol);
//	while ( position >= 0)
//	{
//		if (!isEscSeq(pattern, position)) {
//
//            // A valid range symbol will have left and right operands
//            if (position - 1 < 0 || position + 1 >= size)
//                return "";
//
//            //the preceeding string
//            string preStr = pattern.substr(0, position - 1);
//            //the rest of the original str
//            string postStr = pattern.substr(position + 2, size - (position + 2));
//
//            char startRange = pattern[position - 1];
//            char endRange = pattern[position + 1];
//
//            //if both are alphabetical characters
//            if ((isalpha(startRange) && isalpha(endRange))
//                || (isdigit(startRange) && isdigit(endRange))) {
//                if (static_cast<uint>(startRange) < static_cast<uint>(endRange)) //inorder
//                {
//                    for (uint j = static_cast<uint>(startRange); j <= static_cast<uint>(endRange); j++) {
//                        result += (char) j;
//                    }
//                } else //reverse order
//                {
//                    for (uint j = static_cast<uint>(startRange); j >= static_cast<uint>(endRange); j--) {
//                        result += (char) j;
//                    }
//                }
//            } else
//                return ""; //range does not seem valid
//
//            result = preStr + result + postStr;
//            size = pattern.length();
//            //Search the rest of the original pattern
//            position = pattern.find(rangeSymbol, preStr.length() + result.length());
//        }
//	} //end while
//	return result;
//}



bool Expander::validate(const string& pattern)
{
    int loadBrackets = 0;
    uint loadQuotes = 0;

	for (uint i = 0; i < pattern.size(); i++)
	{
		if (pattern[i] == escapeSymbol) {
            if (!isEscSeq(pattern, i)) {
                cerr << "Error: Invalid escape sequence detected" << endl;
                cerr << pattern << endl;
                cerr << std::setw(i) << "^" << endl;
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
                cerr << "Error: Invalid group sequence detected" << endl;
                cerr << pattern << endl;
                cerr << std::setw(i) << "^" << endl;
                return false;
            }

        }
			
	}

    return ((loadQuotes % 2 == 0) && (loadBrackets % 2 == 0));
}

//void Expander::getCombinations(vector<string>& data, vector<string>& newElements)
//{
//	vector<string> original = data;
//	vector<string> temp = data;
//
//	uint numNewElements = newElements.size();
//	uint numOrgElements = original.size();
//
//	//Nothing to do, just return
//	if (numNewElements == 0)
//	{
//		return;
//	}
//
//	//If the initial list is empty, just add the first entity of the new list
//	if(numOrgElements == 0)
//	{
//		data.push_back(newElements[0]);
//	}
//	else
//	{
//
//		for (int i= 0; i < numOrgElements; i++)
//		{
//			data[i] = data[i] + newElements[0];
//		}
//	}
//
//	for (int i = 1; i < numNewElements; i++)
//	{
//		for (int j = 0; j < numOrgElements; j++)
//		{
//			temp[j] = temp[j] + newElements[i];
//		}
//		data.insert(std::end(data), std::begin(temp), std::end(temp));
//		temp = original;
//	}
//}
//
//void Expander::processGroup(const string& pattern, uint& i,
//		uint& currentItem)
//{
//	//string* newItems = NULL;
//    vector<string> newItems;
//	int eCount = getBlockElements(pattern, i, newItems);
//	if (currentItem == 0) //data is empty
//	{
//		for (int indx = 0; indx < eCount; indx++)
//		{
//			data[currentItem] = newItems[indx];
//			currentItem++;
//		}
//	}
//	else //partial patterns already present
//	{
//		int nDataElements = currentItem; //number of partial patters
//		//after combination total number of patters will be nDataElements*eCount
//		//since the data vector already contains 1 set of elemens
//		//those need to be copied (nDataElements-1)*eCount times
//		for (int el = 0; el < eCount - 1; el++)
//		{
//			for (int dataEl = 0; dataEl < nDataElements; dataEl++)
//			{
//				data[currentItem] = data[dataEl];
//				currentItem++;
//				//resizeResult(currentItem);
//			}
//		}
//		//do the actual combination
//		int indx = 0; //current block element
//		for (int k = 0; k < currentItem; k++)
//		{
//
//			if (k != 0 && k % nDataElements == 0)
//				indx++; //move to next block element
//			//all block elements were processed
//			if (indx >= eCount)
//				break;
//			data[k] = newItems[indx] + data[k];
//		}
//	}
//	//delete[] newItems; //clean up temporary resource
//}

