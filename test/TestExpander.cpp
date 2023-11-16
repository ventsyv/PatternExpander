//
// Unit tests for the Expander class
//

#include <gtest/gtest.h>
#include "Expander.h"

using namespace ::testing;
using namespace std;

class TestExpander: public ::testing::Test
{
protected:
	PatternExpander::Expander underTest;
	PatternExpander::Expander altExpander;

	virtual void SetUp()
	{
		altExpander.setEscChar('#');
		altExpander.setGroupBegin('{');
		altExpander.setGroupEnd('}');
		altExpander.setRangeChar('>');
		altExpander.setQuote('^');
	}
};

TEST_F(TestExpander, testGetters)
{
	char result = underTest.getEscChar();
	ASSERT_EQ(PatternExpander::DEFAULT_ESC_SYM, result);

	result = underTest.getRangeChar();
	ASSERT_EQ(PatternExpander::DEFAULT_RANGE_SYM, result);

	result = underTest.getGroupBegin();
	ASSERT_EQ(PatternExpander::DEFAULT_GROUP_BEGIN_SYM, result);

	result = underTest.getGroupEnd();
	ASSERT_EQ(PatternExpander::DEFAULT_GROUP_END_SYM, result);

	result = underTest.getQuote();
	ASSERT_EQ(PatternExpander::DEFAULT_QUOTE_SYM, result);
}

TEST_F(TestExpander, testSetters)
{
	wchar_t input = '`';
	underTest.setEscChar(input);
	char result = underTest.getEscChar();
	ASSERT_EQ(input, result);

	input = '_';
	underTest.setRangeChar(input);
	result = underTest.getRangeChar();
	ASSERT_EQ(input, result);

	input = '{';
	underTest.setGroupBegin(input);
	result = underTest.getGroupBegin();
	ASSERT_EQ(input, result);

	input = '}';
	underTest.setGroupEnd(input);
	result = underTest.getGroupEnd();
	ASSERT_EQ(input, result);

	input = '^';
	underTest.setQuote(input);
	result = underTest.getQuote();
	ASSERT_EQ(input, result);

}
TEST_F(TestExpander, testValidation_valid)
{
	//A simple constant wstring
	std::wstring pattern = L"abcd";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Dash)
{
	//Static block - no need to escape the range
	wstring pattern = L"abc/-d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupBegin)
{
	//A pattern with valid escape seq
	wstring pattern = L"abc/[d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupEnd)
{
	//A pattern with valid escape seq
	wstring pattern = L"abc/]d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_EscChar)
{
	//No need to escape the escapet when the next val is not a valid esc sequence
	wstring pattern = L"abc//d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Quotes)
{
	//A pattern with valid escape seq
	wstring pattern = L"abc\"de\"";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupBegin)
{
	//A pattern with invalid group
	wstring pattern = L"abc[d";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupEnd)
{
	//A pattern with invalid group
	wstring pattern = L"abc]d";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_UnbalancedGroups)
{

	//A pattern with invalid group
	wstring pattern = L"abc[d[e]z";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testGenerate_StaticBlock)
{
	wstring pattern = L"abcd";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], pattern);
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpBegin)
{
	wstring pattern = L"abc/[d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"abc[d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpEnd)
{
	wstring pattern = L"abc/]d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"abc]d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscDash)
{
	//No need to escape dashes when they are in a static block
	wstring pattern = L"abc/-d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"abc/-d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscEscChar)
{

	//No need to escape / when the next char is not a valid esc sequence
	wstring pattern = L"abc//d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"abc/d");

}

TEST_F(TestExpander, testGenerate_VariableBlock_Range)
{
	wstring pattern = L"[a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"a");
	ASSERT_EQ(data[1], L"b");
	ASSERT_EQ(data[2], L"c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_Range2)
{
	wstring pattern = L"1[a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"1a");
	ASSERT_EQ(data[1], L"1b");
	ASSERT_EQ(data[2], L"1c");
}
TEST_F(TestExpander, testGenerate_VariableBlock_Range3)
{
	wstring pattern = L"[123][a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 9);
	ASSERT_EQ(data[0], L"1a");
	ASSERT_EQ(data[1], L"2a");
	ASSERT_EQ(data[2], L"3a");
	ASSERT_EQ(data[3], L"1b");
	ASSERT_EQ(data[4], L"2b");
	ASSERT_EQ(data[5], L"3b");
	ASSERT_EQ(data[6], L"1c");
	ASSERT_EQ(data[7], L"2c");
	ASSERT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_2Ranges)
{
	wstring pattern = L"[1-3][a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 9);
	ASSERT_EQ(data[0], L"1a");
	ASSERT_EQ(data[1], L"2a");
	ASSERT_EQ(data[2], L"3a");
	ASSERT_EQ(data[3], L"1b");
	ASSERT_EQ(data[4], L"2b");
	ASSERT_EQ(data[5], L"3b");
	ASSERT_EQ(data[6], L"1c");
	ASSERT_EQ(data[7], L"2c");
	ASSERT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_EmbededVarBlock)
{

	wstring pattern = L"[123[a-c]]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 9);
	ASSERT_EQ(data[0], L"1a");
	ASSERT_EQ(data[1], L"2a");
	ASSERT_EQ(data[2], L"3a");
	ASSERT_EQ(data[3], L"1b");
	ASSERT_EQ(data[4], L"2b");
	ASSERT_EQ(data[5], L"3b");
	ASSERT_EQ(data[6], L"1c");
	ASSERT_EQ(data[7], L"2c");
	ASSERT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testValidation_valid_alt)
{
	//A simple constant wstring
	std::wstring pattern = L"abcd";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Dash_alt)
{
	//A pattern with valid escape seq
	wstring pattern = L"abc#>d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupBegin_alt)
{
	//A pattern with valid escape seq
	wstring pattern = L"abc#{d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupEnd_alt)
{
	//A pattern with valid escape seq
	wstring pattern = L"abc#}d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_EscChar_alt)
{
	//A pattern with valid escape seq
	wstring pattern = L"abc##d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Quotes_alt)
{
	//A pattern with valid escape seq
	wstring pattern = L"abc\"de\"";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupBegin_alt)
{
	//A pattern with invalid group
	wstring pattern = L"abc{d";
	bool result = altExpander.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupEnd_alt)
{
	//A pattern with invalid group
	wstring pattern = L"abc}d";
	bool result = altExpander.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_UnbalancedGroups_alt)
{

	//A pattern with invalid group
	wstring pattern = L"abc{d{e}z";
	bool result = altExpander.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testGenerate_StaticBlock_alt)
{
	wstring pattern = L"abcd";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], pattern);
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpBegin_alt)
{
	wstring pattern = L"abc#{d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"abc{d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpEnd_alt)
{
	wstring pattern = L"abc#}d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"abc}d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscDash_alt)
{
	wstring pattern = L"abc#>d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"abc#>d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscEscChar_alt)
{

	wstring pattern = L"abc##d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"abc#d");

}

TEST_F(TestExpander, testGenerate_VariableBlock_Range_alt)
{
	wstring pattern = L"{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"a");
	ASSERT_EQ(data[1], L"b");
	ASSERT_EQ(data[2], L"c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_Range2_alt)
{
	wstring pattern = L"1{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"1a");
	ASSERT_EQ(data[1], L"1b");
	ASSERT_EQ(data[2], L"1c");
}
TEST_F(TestExpander, testGenerate_VariableBlock_Range3_alt)
{
	wstring pattern = L"{123}{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 9);
	ASSERT_EQ(data[0], L"1a");
	ASSERT_EQ(data[1], L"2a");
	ASSERT_EQ(data[2], L"3a");
	ASSERT_EQ(data[3], L"1b");
	ASSERT_EQ(data[4], L"2b");
	ASSERT_EQ(data[5], L"3b");
	ASSERT_EQ(data[6], L"1c");
	ASSERT_EQ(data[7], L"2c");
	ASSERT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_2Ranges_alt)
{
	wstring pattern = L"{1>3}{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 9);
	ASSERT_EQ(data[0], L"1a");
	ASSERT_EQ(data[1], L"2a");
	ASSERT_EQ(data[2], L"3a");
	ASSERT_EQ(data[3], L"1b");
	ASSERT_EQ(data[4], L"2b");
	ASSERT_EQ(data[5], L"3b");
	ASSERT_EQ(data[6], L"1c");
	ASSERT_EQ(data[7], L"2c");
	ASSERT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_EmbededVarBlock_alt)
{

	wstring pattern = L"{123{a>c}}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 9);
	ASSERT_EQ(data[0], L"1a");
	ASSERT_EQ(data[1], L"2a");
	ASSERT_EQ(data[2], L"3a");
	ASSERT_EQ(data[3], L"1b");
	ASSERT_EQ(data[4], L"2b");
	ASSERT_EQ(data[5], L"3b");
	ASSERT_EQ(data[6], L"1c");
	ASSERT_EQ(data[7], L"2c");
	ASSERT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testValidation_valid_Cyrilic)
{
	//A simple constant wstring
	std::wstring pattern = L"абвг";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Dash_Cyrilic)
{
	//A pattern with valid escape seq
	wstring pattern = L"абв/-г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupBegin_Cyrilic)
{
	//A pattern with valid escape seq
	wstring pattern = L"абв/[г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupEnd_Cyrilic)
{
	//A pattern with valid escape seq
	wstring pattern = L"абв/]г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_EscChar_Cyrilic)
{
	//A pattern with valid escape seq
	wstring pattern = L"абв//г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Quotes_Cyrilic)
{
	//A pattern with valid escape seq
	wstring pattern = L"абв\"гд\"";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupBegin_Cyrilic)
{
	//A pattern with invalid group
	wstring pattern = L"абв[г";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupEnd_Cyrilic)
{
	//A pattern with invalid group
	wstring pattern = L"абв]г";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_UnbalancedGroups_Cyrilic)
{

	//A pattern with invalid group
	wstring pattern = L"абв[г[д]е";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testGenerate_StaticBlock_Cyrilic)
{
	wstring pattern = L"абвг";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], pattern);
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpBegin_Cyrilic)
{
	wstring pattern = L"абв/[г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"абв[г");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpEnd_Cyrilic)
{
	wstring pattern = L"абв/]г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"абв]г");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscDash_Cyrilic)
{
	wstring pattern = L"абв/-г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"абв/-г");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscEscChar_Cyrilic)
{

	wstring pattern = L"абв//г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"абв/г");

}

TEST_F(TestExpander, testGenerate_VariableBlock_Range_Cyrilic)
{
	wstring pattern = L"[а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"а");
	ASSERT_EQ(data[1], L"б");
	ASSERT_EQ(data[2], L"в");
}

TEST_F(TestExpander, testGenerate_VariableBlock_Range2_Cyrilic)
{
	wstring pattern = L"1[а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"1а");
	ASSERT_EQ(data[1], L"1б");
	ASSERT_EQ(data[2], L"1в");
}
TEST_F(TestExpander, testGenerate_VariableBlock_Range3_Cyrilic)
{
	wstring pattern = L"[123][а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 9);
	ASSERT_EQ(data[0], L"1а");
	ASSERT_EQ(data[1], L"2а");
	ASSERT_EQ(data[2], L"3а");
	ASSERT_EQ(data[3], L"1б");
	ASSERT_EQ(data[4], L"2б");
	ASSERT_EQ(data[5], L"3б");
	ASSERT_EQ(data[6], L"1в");
	ASSERT_EQ(data[7], L"2в");
	ASSERT_EQ(data[8], L"3в");
}

TEST_F(TestExpander, testGenerate_VariableBlock_2Ranges_Cyrilic)
{
	wstring pattern = L"[1-3][а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 9);
	ASSERT_EQ(data[0], L"1а");
	ASSERT_EQ(data[1], L"2а");
	ASSERT_EQ(data[2], L"3а");
	ASSERT_EQ(data[3], L"1б");
	ASSERT_EQ(data[4], L"2б");
	ASSERT_EQ(data[5], L"3б");
	ASSERT_EQ(data[6], L"1в");
	ASSERT_EQ(data[7], L"2в");
	ASSERT_EQ(data[8], L"3в");
}

TEST_F(TestExpander, testGenerate_VariableBlock_EmbededVarBlock_Cyrilic)
{

	wstring pattern = L"[123[а-в]]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 9);
	ASSERT_EQ(data[0], L"1а");
	ASSERT_EQ(data[1], L"2а");
	ASSERT_EQ(data[2], L"3а");
	ASSERT_EQ(data[3], L"1б");
	ASSERT_EQ(data[4], L"2б");
	ASSERT_EQ(data[5], L"3б");
	ASSERT_EQ(data[6], L"1в");
	ASSERT_EQ(data[7], L"2в");
	ASSERT_EQ(data[8], L"3в");
}

TEST_F(TestExpander, testGenerate_VariableBlock_Beginning)
{

	wstring pattern = L"[1-3]abc";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"1abc");
	ASSERT_EQ(data[1], L"2abc");
	ASSERT_EQ(data[2], L"3abc");
}

TEST_F(TestExpander, testGenerate_VariableBlock_Middle)
{

	wstring pattern = L"ab[1-3]cd";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"ab1cd");
	ASSERT_EQ(data[1], L"ab2cd");
	ASSERT_EQ(data[2], L"ab3cd");
}

TEST_F(TestExpander, testGenerate_Static_Range_Beginning)
{

	wstring pattern = L"a-c[1-3]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"a-c1");
	ASSERT_EQ(data[1], L"a-c2");
	ASSERT_EQ(data[2], L"a-c3");
}


TEST_F(TestExpander, testGenerate_Static_Range_Middle)
{

	wstring pattern = L"[1-3]a-c[1-3]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 9);
	ASSERT_EQ(data[0], L"1a-c1");
	ASSERT_EQ(data[1], L"2a-c1");
	ASSERT_EQ(data[2], L"3a-c1");
	ASSERT_EQ(data[3], L"1a-c2");
	ASSERT_EQ(data[4], L"2a-c2");
	ASSERT_EQ(data[5], L"3a-c2");
	ASSERT_EQ(data[6], L"1a-c3");
	ASSERT_EQ(data[7], L"2a-c3");
	ASSERT_EQ(data[8], L"3a-c3");
}

TEST_F(TestExpander, testGenerate_Static_Range_End)
{

	wstring pattern = L"[1-3]a-c";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"1a-c");
	ASSERT_EQ(data[1], L"2a-c");
	ASSERT_EQ(data[2], L"3a-c");
}

TEST_F(TestExpander, testGenerate_QuotedString_InGroup)
{

	wstring pattern = L"a[b\"123\"c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"ab");
	ASSERT_EQ(data[1], L"a123");
	ASSERT_EQ(data[2], L"ac");
}

TEST_F(TestExpander, testGenerate_QuotedString_InGroup_AltQuote)
{

	wstring pattern = L"a{b^123^c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 3);
	//ASSERT_EQ(data[0], L"ab");
	//ASSERT_EQ(data[1], L"a123");
	//ASSERT_EQ(data[2], L"ac");
}

TEST_F(TestExpander, testGenerate_QuotedString)
{

	wstring pattern = L"\"[a-c]\"";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"[a-c]");
}

TEST_F(TestExpander, testGenerate_QuotedString_AltQuote)
{

	wstring pattern = L"^[a-c]^";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 1);
	ASSERT_EQ(data[0], L"[a-c]");
}


TEST_F(TestExpander, testGenerate_VariableBlock_EscRange)
{
	wstring pattern = L"a-c[1-3]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"a-c1");
	ASSERT_EQ(data[1], L"a-c2");
	ASSERT_EQ(data[2], L"a-c3");
}

TEST_F(TestExpander, testGenerate_VariableBlock_EscRange_InGroup)
{
	wstring pattern = L"[a/-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"a");
	ASSERT_EQ(data[1], L"-");
	ASSERT_EQ(data[2], L"c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_EscEscape)
{
	wstring pattern = L"a[b///-d]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 4);
	ASSERT_EQ(data[0], L"ab");
	ASSERT_EQ(data[1], L"a/");
	ASSERT_EQ(data[2], L"a-");
	ASSERT_EQ(data[3], L"ad");

}

TEST_F(TestExpander, testGenerate_VariableBlock_EscQuote)
{
	// a[/"b-d]
	wstring pattern = L"a[/\"b-d]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 4);
	ASSERT_EQ(data[0], L"a\"");
	ASSERT_EQ(data[1], L"ab");
	ASSERT_EQ(data[2], L"ac");
	ASSERT_EQ(data[3], L"ad");

}

TEST_F(TestExpander, testGenerate_VariableBlock_EscAltQuote)
{
	wstring pattern = L"a{#^b>d}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	ASSERT_EQ(data.size(), 4);
	ASSERT_EQ(data[0], L"a^");
	ASSERT_EQ(data[1], L"ab");
	ASSERT_EQ(data[2], L"ac");
	ASSERT_EQ(data[3], L"ad");

}

TEST_F(TestExpander, testGenerate_VariableBlock_EscQuote_OutsideGroup)
{
	// /"[a-c]/" Since the queotes are escaped [a-c] is expanded and you end up with : "a" "b" "c"
	wstring pattern = LR"(/"[a-c]/")";
	underTest.generate(pattern);
	auto data = underTest.getData();
	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data[0], L"\"a\"");
	ASSERT_EQ(data[1], L"\"b\"");
	ASSERT_EQ(data[2], L"\"c\"");

}

TEST_F(TestExpander, testLoadConfig)
{
	//If the file doesn't  exist the settings don't change
	//Assuming that no ~/.patexpconfig exists, those will be the default values
	underTest.loadConfig("./nofile.txt");

	ASSERT_EQ(underTest.getEscChar(), PatternExpander::DEFAULT_ESC_SYM);
	ASSERT_EQ(underTest.getGroupBegin(), PatternExpander::DEFAULT_GROUP_BEGIN_SYM);
	ASSERT_EQ(underTest.getGroupEnd(), PatternExpander::DEFAULT_GROUP_END_SYM);
	ASSERT_EQ(underTest.getRangeChar(), PatternExpander::DEFAULT_RANGE_SYM);

	altExpander.loadConfig("./nofile.txt");
	ASSERT_EQ(altExpander.getEscChar(), L'#');
	ASSERT_EQ(altExpander.getGroupBegin(), L'{');
	ASSERT_EQ(altExpander.getGroupEnd(), L'}');
	ASSERT_EQ(altExpander.getRangeChar(), L'>');
	ASSERT_EQ(altExpander.getQuote(), L'^');

}

TEST_F(TestExpander, testLoadConfig_altSettings)
{
	//Load the alt config file. All settings will change
	underTest.loadConfig("test_data/altConfig.txt");

	ASSERT_EQ(underTest.getEscChar(), L'#');
	ASSERT_EQ(underTest.getGroupBegin(), L'{');
	ASSERT_EQ(underTest.getGroupEnd(), L'}');
	ASSERT_EQ(underTest.getRangeChar(), L'>');
	ASSERT_EQ(underTest.getQuote(), L'^');

}

TEST_F(TestExpander, testSaveConfig)
{
	//Save the alt config to file, then load it in the default expander
	string configFileName = std::tmpnam(nullptr);

	altExpander.saveConfig(configFileName);
	underTest.loadConfig(configFileName);

	ASSERT_EQ(underTest.getEscChar(), L'#');
	ASSERT_EQ(underTest.getGroupBegin(), L'{');
	ASSERT_EQ(underTest.getGroupEnd(), L'}');
	ASSERT_EQ(underTest.getRangeChar(), L'>');
	ASSERT_EQ(underTest.getQuote(), L'^');


}



int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
