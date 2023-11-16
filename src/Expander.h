#ifndef PATTERN_EXPANDER
#define PATTERN_EXPANDER

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

typedef unsigned int uint;

namespace PatternExpander
{

const wchar_t DEFAULT_ESC_SYM = '/';
const wchar_t DEFAULT_RANGE_SYM = '-';
const wchar_t DEFAULT_GROUP_BEGIN_SYM = '[';
const wchar_t DEFAULT_GROUP_END_SYM = ']';
const wchar_t DEFAULT_QUOTE_SYM = '"';

class Expander
{
public:

	///Consructor
	explicit Expander(wchar_t esc = PatternExpander::DEFAULT_ESC_SYM,
			wchar_t range = PatternExpander::DEFAULT_RANGE_SYM,
			wchar_t grpBegin = PatternExpander::DEFAULT_GROUP_BEGIN_SYM,
			wchar_t grpEnd = PatternExpander::DEFAULT_GROUP_END_SYM,
			wchar_t quote = PatternExpander::DEFAULT_QUOTE_SYM);

	///The main function - it expands the pattern and generates are possible combinations
	void generate(const std::wstring &pattern);

	///Validate the pattern
	bool validate(const std::wstring &pattern);

	///prints out all generated patterns
	friend std::wostream& operator<<(std::wostream &os, const Expander &pg)
	{
		for (size_t i = 0; i < pg.data.size(); i++)
		{
			os << pg.data[i] << std::endl;
		}
		return os;
	}

	//Getters and setters
	wchar_t getEscChar() const
	{
		return escapeSymbol;
	}
	void setEscChar(wchar_t in)
	{
		escapeSymbol = in;
	}

	wchar_t getRangeChar() const
	{
		return rangeSymbol;
	}
	void setRangeChar(wchar_t in)
	{
		rangeSymbol = in;
	}

	wchar_t getGroupBegin() const
	{
		return groupBegin;
	}
	void setGroupBegin(wchar_t in)
	{
		groupBegin = in;
	}

	wchar_t getGroupEnd() const
	{
		return groupEnd;
	}
	void setGroupEnd(wchar_t in)
	{
		groupEnd = in;
	}

	wchar_t getQuote() const
	{
		return quote;
	}
	void setQuote(wchar_t in)
	{
		quote = in;
	}

	std::vector<std::wstring> getData();

	std::wstringstream output;

	void loadConfig(const std::string& filePath="~/.patexpconfig");

	void saveConfig(const std::string& filePath="~/.patexpconfig");

private:
	///used to escape other special symbols
	wchar_t escapeSymbol;
	wchar_t rangeSymbol;

	wchar_t groupBegin;
	wchar_t groupEnd;

	wchar_t quote;

	///the main data storage
	std::vector<std::wstring> data;

	///Used to find all opening / closing brackets
	//	std::vector<uint> findAllInstances(const std::wstring& target, const std::wstring& item);

	//Looks for any ranges and expands them
	std::wstring expand(const std::wstring &pattern);

	///Returns true if a valid escape sequence is present at the given position
	bool isEscSeq(const std::wstring &pattern, uint position, bool isInGroup) const;

	/**A character set is a group of characters surrounded by []
	 * Characters can be listed individually - [abc]
	 * or as a range [a-c]. Special characters lose their meaning inside a set
	 */
	std::vector<std::wstring> handleCharacterSet(const std::wstring &pattern,
			uint);

	/***
	 *
	 */
	uint getBlockElements(const std::wstring &pattern, uint &start,
			std::vector<std::wstring> &items);

	void getCombinations(std::vector<std::wstring> &data,
			std::vector<std::wstring> &newElements);
	void processGroup(const std::wstring &pattern, uint &i, uint &currentItem);

	bool getLoadBalance(const std::wstring &pattern);
	void append(std::vector<std::wstring> &results,
			const std::wstring &newData);
	void appendGroup(bool &isFirstInGroup, const std::wstring &newData, std::vector<std::wstring> &partials, std::vector<std::wstring> &results);
};

} //end namespace

#endif
