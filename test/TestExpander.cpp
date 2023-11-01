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
	char input = '`';
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
	//A simple constant string
	std::string pattern = "abcd";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Dash) {
	//A pattern with valid escape seq
	string pattern = "abc/-d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupBegin) {
	//A pattern with valid escape seq
	string pattern = "abc/[d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupEnd) {
	//A pattern with valid escape seq
	string pattern = "abc/]d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_EscChar) {
	//A pattern with valid escape seq
	string pattern = "abc//d";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Quotes) {
	//A pattern with valid escape seq
	string pattern = "abc\"de\"";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupBegin) {
	//A pattern with invalid group
	string pattern = "abc[d";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupEnd) {
	//A pattern with invalid group
	string pattern = "abc]d";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_UnbalancedGroups) {

	//A pattern with invalid group
	string pattern = "abc[d[e]z";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testGenerate_StaticBlock) {
	string pattern = "abcd";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], pattern);
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpBegin) {
	string pattern = "abc/[d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "abc[d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpEnd) {
	string pattern = "abc/]d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "abc]d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscDash) {
	string pattern = "abc/-d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "abc-d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscEscChar) {

	string pattern = "abc//d";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "abc/d");

}

TEST_F(TestExpander, testGenerate_VariableBlock_Range) {
	string pattern = "[a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], "a");
	EXPECT_EQ(data[1], "b");
	EXPECT_EQ(data[2], "c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_Range2) {
	string pattern = "1[a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], "1a");
	EXPECT_EQ(data[1], "1b");
	EXPECT_EQ(data[2], "1c");
}
TEST_F(TestExpander, testGenerate_VariableBlock_Range3) {
	string pattern = "[123][a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], "1a");
	EXPECT_EQ(data[1], "2a");
	EXPECT_EQ(data[2], "3a");
	EXPECT_EQ(data[3], "1b");
	EXPECT_EQ(data[4], "2b");
	EXPECT_EQ(data[5], "3b");
	EXPECT_EQ(data[6], "1c");
	EXPECT_EQ(data[7], "2c");
	EXPECT_EQ(data[8], "3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_2Ranges) {
	string pattern = "[1-3][a-c]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], "1a");
	EXPECT_EQ(data[1], "2a");
	EXPECT_EQ(data[2], "3a");
	EXPECT_EQ(data[3], "1b");
	EXPECT_EQ(data[4], "2b");
	EXPECT_EQ(data[5], "3b");
	EXPECT_EQ(data[6], "1c");
	EXPECT_EQ(data[7], "2c");
	EXPECT_EQ(data[8], "3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_EmbededVarBlock) {

	string pattern = "[123[a-c]]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], "1a");
	EXPECT_EQ(data[1], "2a");
	EXPECT_EQ(data[2], "3a");
	EXPECT_EQ(data[3], "1b");
	EXPECT_EQ(data[4], "2b");
	EXPECT_EQ(data[5], "3b");
	EXPECT_EQ(data[6], "1c");
	EXPECT_EQ(data[7], "2c");
	EXPECT_EQ(data[8], "3c");
}

TEST_F(TestExpander, testValidation_valid_alt)
{
	//A simple constant string
	std::string pattern = "abcd";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Dash_alt) {
	//A pattern with valid escape seq
	string pattern = "abc#>d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupBegin_alt) {
	//A pattern with valid escape seq
	string pattern = "abc#{d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupEnd_alt) {
	//A pattern with valid escape seq
	string pattern = "abc#}d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_EscChar_alt) {
	//A pattern with valid escape seq
	string pattern = "abc##d";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Quotes_alt) {
	//A pattern with valid escape seq
	string pattern = "abc\"de\"";
	bool result = altExpander.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupBegin_alt) {
	//A pattern with invalid group
	string pattern = "abc{d";
	bool result = altExpander.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupEnd_alt) {
	//A pattern with invalid group
	string pattern = "abc}d";
	bool result = altExpander.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_UnbalancedGroups_alt) {

	//A pattern with invalid group
	string pattern = "abc{d{e}z";
	bool result = altExpander.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testGenerate_StaticBlock_alt) {
	string pattern = "abcd";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], pattern);
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpBegin_alt) {
	string pattern = "abc#{d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "abc{d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpEnd_alt) {
	string pattern = "abc#}d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "abc}d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscDash_alt) {
	string pattern = "abc#>d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "abc>d");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscEscChar_alt) {

	string pattern = "abc##d";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "abc#d");

}

TEST_F(TestExpander, testGenerate_VariableBlock_Range_alt) {
	string pattern = "{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], "a");
	EXPECT_EQ(data[1], "b");
	EXPECT_EQ(data[2], "c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_Range2_alt) {
	string pattern = "1{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], "1a");
	EXPECT_EQ(data[1], "1b");
	EXPECT_EQ(data[2], "1c");
}
TEST_F(TestExpander, testGenerate_VariableBlock_Range3_alt) {
	string pattern = "{123}{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], "1a");
	EXPECT_EQ(data[1], "2a");
	EXPECT_EQ(data[2], "3a");
	EXPECT_EQ(data[3], "1b");
	EXPECT_EQ(data[4], "2b");
	EXPECT_EQ(data[5], "3b");
	EXPECT_EQ(data[6], "1c");
	EXPECT_EQ(data[7], "2c");
	EXPECT_EQ(data[8], "3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_2Ranges_alt) {
	string pattern = "{1>3}{a>c}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], "1a");
	EXPECT_EQ(data[1], "2a");
	EXPECT_EQ(data[2], "3a");
	EXPECT_EQ(data[3], "1b");
	EXPECT_EQ(data[4], "2b");
	EXPECT_EQ(data[5], "3b");
	EXPECT_EQ(data[6], "1c");
	EXPECT_EQ(data[7], "2c");
	EXPECT_EQ(data[8], "3c");
}

TEST_F(TestExpander, testGenerate_VariableBlock_EmbededVarBlock_alt) {

	string pattern = "{123{a>c}}";
	altExpander.generate(pattern);
	auto data = altExpander.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], "1a");
	EXPECT_EQ(data[1], "2a");
	EXPECT_EQ(data[2], "3a");
	EXPECT_EQ(data[3], "1b");
	EXPECT_EQ(data[4], "2b");
	EXPECT_EQ(data[5], "3b");
	EXPECT_EQ(data[6], "1c");
	EXPECT_EQ(data[7], "2c");
	EXPECT_EQ(data[8], "3c");
}

TEST_F(TestExpander, testValidation_valid_Cyrilic)
{
	//A simple constant string
	std::string pattern = "абвг";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Dash_Cyrilic) {
	//A pattern with valid escape seq
	string pattern = "абв/-г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupBegin_Cyrilic) {
	//A pattern with valid escape seq
	string pattern = "абв/[г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_GroupEnd_Cyrilic) {
	//A pattern with valid escape seq
	string pattern = "абв/]г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_EscChar_Cyrilic) {
	//A pattern with valid escape seq
	string pattern = "абв//г";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_validEscSeqs_Quotes_Cyrilic) {
	//A pattern with valid escape seq
	string pattern = "абв\"гд\"";
	bool result = underTest.validate(pattern);
	EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupBegin_Cyrilic) {
	//A pattern with invalid group
	string pattern = "абв[г";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_GroupEnd_Cyrilic) {
	//A pattern with invalid group
	string pattern = "абв]г";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups_UnbalancedGroups_Cyrilic) {

	//A pattern with invalid group
	string pattern = "абв[г[д]е";
	bool result = underTest.validate(pattern);
	EXPECT_FALSE(result);
}

TEST_F(TestExpander, testGenerate_StaticBlock_Cyrilic) {
	string pattern = "абвг";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], pattern);
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpBegin_Cyrilic) {
	string pattern = "абв/[г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "абв[г");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscGrpEnd_Cyrilic) {
	string pattern = "абв/]г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "абв]г");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscDash_Cyrilic) {
	string pattern = "абв/-г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "абв-г");
}

TEST_F(TestExpander, testGenerate_StaticBlock_EscEscChar_Cyrilic) {

	string pattern = "абв//г";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 1);
	EXPECT_EQ(data[0], "абв/г");

}

TEST_F(TestExpander, testGenerate_VariableBlock_Range_Cyrilic) {
	string pattern = "[а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], "а");
	EXPECT_EQ(data[1], "б");
	EXPECT_EQ(data[2], "в");
}

TEST_F(TestExpander, testGenerate_VariableBlock_Range2_Cyrilic) {
	string pattern = "1[а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 3);
	EXPECT_EQ(data[0], "1а");
	EXPECT_EQ(data[1], "1б");
	EXPECT_EQ(data[2], "1в");
}
TEST_F(TestExpander, testGenerate_VariableBlock_Range3_Cyrilic) {
	string pattern = "[123][а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], "1а");
	EXPECT_EQ(data[1], "2а");
	EXPECT_EQ(data[2], "3а");
	EXPECT_EQ(data[3], "1б");
	EXPECT_EQ(data[4], "2б");
	EXPECT_EQ(data[5], "3б");
	EXPECT_EQ(data[6], "1в");
	EXPECT_EQ(data[7], "2в");
	EXPECT_EQ(data[8], "3в");
}

TEST_F(TestExpander, testGenerate_VariableBlock_2Ranges_Cyrilic) {
	string pattern = "[1-3][а-в]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], "1а");
	EXPECT_EQ(data[1], "2а");
	EXPECT_EQ(data[2], "3а");
	EXPECT_EQ(data[3], "1б");
	EXPECT_EQ(data[4], "2б");
	EXPECT_EQ(data[5], "3б");
	EXPECT_EQ(data[6], "1в");
	EXPECT_EQ(data[7], "2в");
	EXPECT_EQ(data[8], "3в");
}

TEST_F(TestExpander, testGenerate_VariableBlock_EmbededVarBlock_Cyrilic) {

	string pattern = "[123[а-в]]";
	underTest.generate(pattern);
	auto data = underTest.getData();
	EXPECT_EQ(data.size(), 9);
	EXPECT_EQ(data[0], "1а");
	EXPECT_EQ(data[1], "2а");
	EXPECT_EQ(data[2], "3а");
	EXPECT_EQ(data[3], "1б");
	EXPECT_EQ(data[4], "2б");
	EXPECT_EQ(data[5], "3б");
	EXPECT_EQ(data[6], "1в");
	EXPECT_EQ(data[7], "2в");
	EXPECT_EQ(data[8], "3в");
}



int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
