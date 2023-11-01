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

TEST_F(TestExpander, testValidation_validEscSeqs)
{
    //A pattern with valid escape seq
    string pattern = "abc/-d";
    bool result = underTest.validate(pattern);
    EXPECT_TRUE(result);

    //A pattern with valid escape seq
    pattern = "abc/[d";
    result = underTest.validate(pattern);
    EXPECT_TRUE(result);

    //A pattern with valid escape seq
    pattern = "abc//d";
    result = underTest.validate(pattern);
    EXPECT_TRUE(result);

    //A pattern with valid escape seq
    pattern = "abc/]d";
    result = underTest.validate(pattern);
    EXPECT_TRUE(result);

    //A pattern with valid escape seq
    pattern = "abc\"de\"";
    result = underTest.validate(pattern);
    EXPECT_TRUE(result);
}

TEST_F(TestExpander, testValidation_invalidGroups)
{
    //A pattern with invalid group
    string pattern = "abc[d";
    bool result = underTest.validate(pattern);
    EXPECT_FALSE(result);

    //A pattern with invalid group
    pattern = "abc]d";
    result = underTest.validate(pattern);
    EXPECT_FALSE(result);

    //A pattern with invalid group
    pattern = "abc[d[e]z";
    result = underTest.validate(pattern);
    EXPECT_FALSE(result);
}

TEST_F(TestExpander, testGenerate_StaticBlock)
{
    string pattern = "abcd";
    underTest.generate(pattern);
    auto data = underTest.getData();
    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data[0], pattern);

    pattern = "abc/[d";
    underTest.generate(pattern);
    data = underTest.getData();
    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data[0], "abc[d");

    pattern = "abc/]d";
    underTest.generate(pattern);
    data = underTest.getData();
    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data[0], "abc]d");

    pattern = "abc/-d";
    underTest.generate(pattern);
    data = underTest.getData();
    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data[0], "abc-d");

    pattern = "abc//d";
    underTest.generate(pattern);
    data = underTest.getData();
    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data[0], "abc/d");

}

TEST_F(TestExpander, testGenerate_VariableBlock) {
    string pattern = "[a-c]";
    underTest.generate(pattern);
    auto data = underTest.getData();
    EXPECT_EQ(data.size(), 3);
    EXPECT_EQ(data[0], "a");
    EXPECT_EQ(data[1], "b");
    EXPECT_EQ(data[2], "c");

    pattern = "1[a-c]";
    underTest.generate(pattern);
    data = underTest.getData();
    EXPECT_EQ(data.size(), 3);
    EXPECT_EQ(data[0], "1a");
    EXPECT_EQ(data[1], "1b");
    EXPECT_EQ(data[2], "1c");

    pattern = "[123][a-c]";
    underTest.generate(pattern);
    data = underTest.getData();
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


    pattern = "[1-3][a-c]";
    underTest.generate(pattern);
    data = underTest.getData();
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


    pattern = "[123[a-c]]";
    underTest.generate(pattern);
    data = underTest.getData();
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



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}