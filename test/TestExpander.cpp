//
// Unit tests for the Expander class
//

#include "gtest/gtest.h"
#include "Expander.h"

using namespace ::testing;
using namespace std;

class TestExpander  : public ::testing::Test {
protected:
	PatternExpander::Expander underTest;
	PatternExpander::Expander altExpander;


	virtual void SetUp() {
		altExpander.setEscChar('#');
		altExpander.setGroupBegin('{');
		altExpander.setGroupEnd('}');
		altExpander.setRangeChar('>');
	}
};

TEST_F(TestExpander, testGetters)
{
	char result = underTest.getEscChar();
	EXPECT_EQ(PatternExpander::DEFAULT_ESC_SYM, result);

	result = underTest.getRangeChar();
	EXPECT_EQ(PatternExpander::DEFAULT_RANGE_SYM, result);

	result = underTest.getGroupBegin();
	EXPECT_EQ(PatternExpander::GROUP_SYM_BEGIN, result);

	result = underTest.getGroupEnd();
	EXPECT_EQ(PatternExpander::GROUP_SYM_END, result);
}

TEST_F(TestExpander, testSetters)
{
	wchar_t input = '`';
	underTest.setEscChar(input);
	char result = underTest.getEscChar();
	EXPECT_EQ(input, result);

	input = '_';
	underTest.setRangeChar(input);
	result = underTest.getRangeChar();
	EXPECT_EQ(input, result);

	input = '{';
	underTest.setGroupBegin(input);
	result = underTest.getGroupBegin();
	EXPECT_EQ(input, result);

	input = '}';
	underTest.setGroupEnd(input);
	result = underTest.getGroupEnd();
	EXPECT_EQ(input, result);


}
TEST_F(TestExpander, testValidation_valid)
{
	//A simple constant wstring
	std::wstring pattern = L"abcd";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Dash) {
	//A pattern with valid escape seq
	wstring pattern = L"abc/-d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupBegin) {
	//A pattern with valid escape seq
	wstring pattern = L"abc/[d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupEnd) {
	//A pattern with valid escape seq
	wstring pattern = L"abc/]d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_EscChar) {
	//A pattern with valid escape seq
	wstring pattern = L"abc//d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Quotes) {
	//A pattern with valid escape seq
	wstring pattern = L"abc\"de\"";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupBegin) {
	//A pattern with invalid group
	wstring pattern = L"abc[d";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupEnd) {
	//A pattern with invalid group
	wstring pattern = L"abc]d";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_UnbalancedGroups) {

	//A pattern with invalid group
	wstring pattern = L"abc[d[e]z";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testGenerate_StaticBlock) {
	wstring pattern = L"abcd";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], pattern);
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpBegin) {
	wstring pattern = L"abc/[d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"abc[d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpEnd) {
	wstring pattern = L"abc/]d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"abc]d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscDash) {
	wstring pattern = L"abc/-d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"abc-d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscEscChar) {

	wstring pattern = L"abc//d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"abc/d");

}

TEST_F(TestExpander, testGenerate_VariableBlock_Range) {
	wstring pattern = L"[a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], L"a");
	EXPECT_EQ(data[1], L"b");
	EXPECT_EQ(data[2], L"c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_Range2) {
	wstring pattern = L"1[a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], L"1a");
	EXPECT_EQ(data[1], L"1b");
	EXPECT_EQ(data[2], L"1c");
}
TEST_F(TestExpander, testGenerate_VariableBlock_Range3) {
	wstring pattern = L"[123][a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], L"1a");
	EXPECT_EQ(data[1], L"2a");
	EXPECT_EQ(data[2], L"3a");
	EXPECT_EQ(data[3], L"1b");
	EXPECT_EQ(data[4], L"2b");
	EXPECT_EQ(data[5], L"3b");
	EXPECT_EQ(data[6], L"1c");
	EXPECT_EQ(data[7], L"2c");
	EXPECT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_2Ranges) {
	wstring pattern = L"[1-3][a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], L"1a");
	EXPECT_EQ(data[1], L"2a");
	EXPECT_EQ(data[2], L"3a");
	EXPECT_EQ(data[3], L"1b");
	EXPECT_EQ(data[4], L"2b");
	EXPECT_EQ(data[5], L"3b");
	EXPECT_EQ(data[6], L"1c");
	EXPECT_EQ(data[7], L"2c");
	EXPECT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_EmbededVarBlock) {

	wstring pattern = L"[123[a-c]]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], L"1a");
	EXPECT_EQ(data[1], L"2a");
	EXPECT_EQ(data[2], L"3a");
	EXPECT_EQ(data[3], L"1b");
	EXPECT_EQ(data[4], L"2b");
	EXPECT_EQ(data[5], L"3b");
	EXPECT_EQ(data[6], L"1c");
	EXPECT_EQ(data[7], L"2c");
	EXPECT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testValidation_valid_alt)
{
	//A simple constant wstring
	std::wstring pattern = L"abcd";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Dash_alt) {
	//A pattern with valid escape seq
	wstring pattern = L"abc#>d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupBegin_alt) {
	//A pattern with valid escape seq
	wstring pattern = L"abc#{d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupEnd_alt) {
	//A pattern with valid escape seq
	wstring pattern = L"abc#}d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_EscChar_alt) {
	//A pattern with valid escape seq
	wstring pattern = L"abc##d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Quotes_alt) {
	//A pattern with valid escape seq
	wstring pattern = L"abc\"de\"";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupBegin_alt) {
	//A pattern with invalid group
	wstring pattern = L"abc{d";
	bool result = altExpander.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupEnd_alt) {
	//A pattern with invalid group
	wstring pattern = L"abc}d";
	bool result = altExpander.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_UnbalancedGroups_alt) {

	//A pattern with invalid group
	wstring pattern = L"abc{d{e}z";
	bool result = altExpander.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testGenerate_StaticBlock_alt) {
	wstring pattern = L"abcd";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], pattern);
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpBegin_alt) {
	wstring pattern = L"abc#{d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"abc{d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpEnd_alt) {
	wstring pattern = L"abc#}d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"abc}d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscDash_alt) {
	wstring pattern = L"abc#>d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"abc>d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscEscChar_alt) {

	wstring pattern = L"abc##d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"abc#d");

}

TEST_F(TestExpander, testGenerate_VariableBlock_Range_alt) {
	wstring pattern = L"{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], L"a");
	EXPECT_EQ(data[1], L"b");
	EXPECT_EQ(data[2], L"c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_Range2_alt) {
	wstring pattern = L"1{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], L"1a");
	EXPECT_EQ(data[1], L"1b");
	EXPECT_EQ(data[2], L"1c");
}
TEST_F(TestExpander, testGenerate_VariableBlock_Range3_alt) {
	wstring pattern = L"{123}{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], L"1a");
	EXPECT_EQ(data[1], L"2a");
	EXPECT_EQ(data[2], L"3a");
	EXPECT_EQ(data[3], L"1b");
	EXPECT_EQ(data[4], L"2b");
	EXPECT_EQ(data[5], L"3b");
	EXPECT_EQ(data[6], L"1c");
	EXPECT_EQ(data[7], L"2c");
	EXPECT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_2Ranges_alt) {
	wstring pattern = L"{1>3}{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], L"1a");
	EXPECT_EQ(data[1], L"2a");
	EXPECT_EQ(data[2], L"3a");
	EXPECT_EQ(data[3], L"1b");
	EXPECT_EQ(data[4], L"2b");
	EXPECT_EQ(data[5], L"3b");
	EXPECT_EQ(data[6], L"1c");
	EXPECT_EQ(data[7], L"2c");
	EXPECT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_EmbededVarBlock_alt) {

	wstring pattern = L"{123{a>c}}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], L"1a");
	EXPECT_EQ(data[1], L"2a");
	EXPECT_EQ(data[2], L"3a");
	EXPECT_EQ(data[3], L"1b");
	EXPECT_EQ(data[4], L"2b");
	EXPECT_EQ(data[5], L"3b");
	EXPECT_EQ(data[6], L"1c");
	EXPECT_EQ(data[7], L"2c");
	EXPECT_EQ(data[8], L"3c");
}

TEST_F(TestExpander, testValidation_valid_Cyrilic)
{
	//A simple constant wstring
	std::wstring pattern = L"абвг";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Dash_Cyrilic) {
	//A pattern with valid escape seq
	wstring pattern = L"абв/-г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupBegin_Cyrilic) {
	//A pattern with valid escape seq
	wstring pattern = L"абв/[г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupEnd_Cyrilic) {
	//A pattern with valid escape seq
	wstring pattern = L"абв/]г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_EscChar_Cyrilic) {
	//A pattern with valid escape seq
	wstring pattern = L"абв//г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Quotes_Cyrilic) {
	//A pattern with valid escape seq
	wstring pattern = L"абв\"гд\"";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupBegin_Cyrilic) {
	//A pattern with invalid group
	wstring pattern = L"абв[г";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupEnd_Cyrilic) {
	//A pattern with invalid group
	wstring pattern = L"абв]г";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_UnbalancedGroups_Cyrilic) {

	//A pattern with invalid group
	wstring pattern = L"абв[г[д]е";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testGenerate_StaticBlock_Cyrilic) {
	wstring pattern = L"абвг";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], pattern);
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpBegin_Cyrilic) {
	wstring pattern = L"абв/[г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"абв[г");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpEnd_Cyrilic) {
	wstring pattern = L"абв/]г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"абв]г");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscDash_Cyrilic) {
	wstring pattern = L"абв/-г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"абв-г");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscEscChar_Cyrilic) {

	wstring pattern = L"абв//г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], L"абв/г");

}

TEST_F(TestExpander, testGenerate_VariableBlock_Range_Cyrilic) {
	wstring pattern = L"[а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], L"а");
	EXPECT_EQ(data[1], L"б");
	EXPECT_EQ(data[2], L"в");
}

TEST_F(TestExpander, testGenerate_VariableBlock_Range2_Cyrilic) {
	wstring pattern = L"1[а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], L"1а");
	EXPECT_EQ(data[1], L"1б");
	EXPECT_EQ(data[2], L"1в");
}
TEST_F(TestExpander, testGenerate_VariableBlock_Range3_Cyrilic) {
	wstring pattern = L"[123][а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], L"1а");
	EXPECT_EQ(data[1], L"2а");
	EXPECT_EQ(data[2], L"3а");
	EXPECT_EQ(data[3], L"1б");
	EXPECT_EQ(data[4], L"2б");
	EXPECT_EQ(data[5], L"3б");
	EXPECT_EQ(data[6], L"1в");
	EXPECT_EQ(data[7], L"2в");
	EXPECT_EQ(data[8], L"3в");
}

TEST_F(TestExpander, testGenerate_VariableBlock_2Ranges_Cyrilic) {
	wstring pattern = L"[1-3][а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], L"1а");
	EXPECT_EQ(data[1], L"2а");
	EXPECT_EQ(data[2], L"3а");
	EXPECT_EQ(data[3], L"1б");
	EXPECT_EQ(data[4], L"2б");
	EXPECT_EQ(data[5], L"3б");
	EXPECT_EQ(data[6], L"1в");
	EXPECT_EQ(data[7], L"2в");
	EXPECT_EQ(data[8], L"3в");
}

TEST_F(TestExpander, testGenerate_VariableBlock_EmbededVarBlock_Cyrilic) {

	wstring pattern = L"[123[а-в]]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], L"1а");
	EXPECT_EQ(data[1], L"2а");
	EXPECT_EQ(data[2], L"3а");
	EXPECT_EQ(data[3], L"1б");
	EXPECT_EQ(data[4], L"2б");
	EXPECT_EQ(data[5], L"3б");
	EXPECT_EQ(data[6], L"1в");
	EXPECT_EQ(data[7], L"2в");
	EXPECT_EQ(data[8], L"3в");
}



int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
