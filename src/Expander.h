#ifndef PATTERN_EXPANDER
#define PATTERN_EXPANDER

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

typedef unsigned int uint;

namespace PatternExpander
{

// Default reserved symbols. Those can be changed via the set methods
const wchar_t DEFAULT_ESC_SYM = '/';
const wchar_t DEFAULT_RANGE_SYM = '-';
const wchar_t DEFAULT_GROUP_BEGIN_SYM = '[';
const wchar_t DEFAULT_GROUP_END_SYM = ']';
const wchar_t DEFAULT_QUOTE_SYM = '"';

class Expander
{
public:

	/**
	 * Constructor with default values allows the Expander class to be configured at construction time
	 * @param esc      - the escape symbol. Default: DEFAULT_ESC_SYM
	 * @param range    - the range symbol. Default: DEFAULT_RANGE_SYM
	 * @param grpBegin - the group begin symbol. Default: DEFAULT_GROUP_BEGIN_SYM
	 * @param grpEnd   - the group end symbol. Default: DEFAULT_GROUP_END_SYM
	 * @param quote    - the quote symbol. Default: DEFAULT_QUOTE_SYM
	 */
	explicit Expander(wchar_t esc = PatternExpander::DEFAULT_ESC_SYM,
			wchar_t range = PatternExpander::DEFAULT_RANGE_SYM,
			wchar_t grpBegin = PatternExpander::DEFAULT_GROUP_BEGIN_SYM,
			wchar_t grpEnd = PatternExpander::DEFAULT_GROUP_END_SYM,
			wchar_t quote = PatternExpander::DEFAULT_QUOTE_SYM);

	/**
	 * The main function - it expands the pattern and generates are possible combinations
	 * @param pattern - the pattern to be processed
	 */
	void generate(const std::wstring &pattern);

	/**
	 * Validates the provided pattern. Returns false if pattern is invalid
	 * Writes an error message to the output stream (see below)
	 * @param pattern - the pattern to be processed
	 */
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


	/// Returns the set of generated patterns
	std::vector<std::wstring> getData();

	///The error stream. Used by the validate method to write error messages
	std::wstringstream output;

	/**
	 * Loads the reserved symbols from a file.
	 * @param filePath: Default config file is ~/.patexpconfig
	 */
	void loadConfig(const std::string& filePath="~/.patexpconfig");

	/**
	 * Writes the reserved symbols to a file
	 * @param filePath: Default config file is ~/.patexpconfig
	 */
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

	/**
	 * Looks for any ranges in the given pattern and expands them.
	 * For example: a[1-3] will return a[123] and 1[c-a] will return 1[cba]
	 */
	std::wstring expand(const std::wstring &pattern);

	///Returns true if a valid escape sequence is present at the given position
	bool isEscSeq(const std::wstring &pattern, uint position, bool isInGroup) const;

	/**
	 * Appends a static string to all existing results. Usually this wil be a single character at a time
	 * @param results - the existing result from expanding the pattern so far
	 * @param newData - the newly added string
	 */
	void append(std::vector<std::wstring> &results,
			const std::wstring &newData);

	/**
	 * Appends strings from a group to the existing results.
	 * For example, if the pattern is abc[123], the [123] will be appended by this function.
	 * The first time the function is ran the pre-existing results need to be saved (in this example abc)
	 * Each item in the group is added to the partials, then all of them are added to the result set
	 * @param isFirstInGroup - boolean flag to check if we are starting to process a new group.
	 * @param newData - the strings that need to be appended. In this example, this will be 1,2,3
	 * @param partials - the pre-existing results before the group begins processing
	 * @param results - the combined results after each item is added
	 */
	void appendGroup(bool &isFirstInGroup, const std::wstring &newData, std::vector<std::wstring> &partials, std::vector<std::wstring> &results);
};

} //end namespace

#endif
