#ifndef PATTERN_EXPANDER
#define PATTERN_EXPANDER

#include <string>
#include <iostream>
#include <vector>

typedef unsigned int uint;

namespace PatternExpander
{

const char DEFAULT_ESC_SYM = '/';
const char DEFAULT_RANGE_SYM = '-';
const char GROUP_SYM_BEGIN = '[';
const char GROUP_SYM_END = ']';
const char SINGLE_QUOTE = '\'';
const char DOUBLE_QUOTE = '"';


class Expander
{
public:
	
	///Consructor
	explicit Expander(char esc = PatternExpander::DEFAULT_ESC_SYM, char range = PatternExpander::DEFAULT_RANGE_SYM,
             char grpBegin = PatternExpander::GROUP_SYM_BEGIN, char grpEnd = PatternExpander::GROUP_SYM_END);

	///The main function - it expands the pattern and generates are possible combinations
	void  generate(const std::string& pattern);

    ///Validate the pattern
    bool validate(const std::string &pattern);

	///prints out all generated patterns
	friend std::ostream& operator<<(std::ostream& os, const Expander& pg)
	{
		for (size_t i = 0; i < pg.data.size(); i++)
		{
			os<<pg.data[i]<<std::endl;
		}
		return os;
	}

    //Getters and setters
	char getEscChar() const {return escapeSymbol;}
	void setEscChar(char in) {escapeSymbol = in;}

	char getRangeChar() const {return rangeSymbol;}
	void setRangeChar(char in) {rangeSymbol = in;}

    char getGroupBegin() const {return groupBegin;}
    void setGroupBegin(char in) {groupBegin = in;}

    char getGroupEnd() const {return groupEnd;}
    void setGroupEnd(char in) {groupEnd = in;}

    std::vector<std::string> getData();

private:
	///used to escape other special symbols
	char escapeSymbol;
	char rangeSymbol;

    char groupBegin;
    char groupEnd;


	///the main data storage
	std::vector<std::string> data;
	
	///Used to find all opening / closing brackets
//	std::vector<uint> findAllInstances(const std::string& target, const std::string& item);

	///Looks for any ranges and expands them
	std::string expand(const std::string& pattern);

	///Returns true if a valid escape sequence is present at the given position
	bool isEscSeq(const std::string& pattern, uint position) const;
	
	/**A character set is a group of characters surrounded by []
	 * Characters can be listed individually - [abc]
	 * or as a range [a-c]. Special characters lose their meaning inside a set
	 */
	std::vector<std::string> handleCharacterSet(const std::string& pattern, uint);

	/***
	 *
	 */
	uint  getBlockElements(const std::string& pattern, uint& start, std::vector<std::string>& items);

    void getCombinations(std::vector<std::string> &data, std::vector<std::string> &newElements);
    void processGroup(const std::string &pattern, uint &i, uint &currentItem);

    bool getLoadBalance(const std::string& pattern);


};

}//end namespace

#endif
