#include <gtest/gtest.h>
#include <lib/ArgParser.h>

#include <sstream>
#include <fstream>


using namespace ArgumentParser;

/*
    Функция принимает в качество аргумента строку, разделяет ее по "пробелу"
    и возвращает вектор полученных слов
*/
std::vector<std::string> SplitString(const std::string& str) {
    std::istringstream iss(str);

    return {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
}


TEST(ArgParserTestSuite, EmptyTest) {
    ArgParser parser("My Empty Parser");

    ASSERT_TRUE(parser.Parse(SplitString("app")));
}


TEST(ArgParserTestSuite, StringTest) {
    ArgParser parser("My Parser");
    parser.AddStringArgument("param1");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1")));
    ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}


TEST(ArgParserTestSuite, ShortNameTest) {
    ArgParser parser("My Parser");
    parser.AddStringArgument('p', "param1");

    ASSERT_TRUE(parser.Parse(SplitString("app -p=value1")));
    ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}


TEST(ArgParserTestSuite, DefaultTest) {
    ArgParser parser("My Parser");
    parser.AddStringArgument("param1").Default("value1");

    ASSERT_TRUE(parser.Parse(SplitString("app")));
    ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}


TEST(ArgParserTestSuite, NoDefaultTest) {
    ArgParser parser("My Parser");
    parser.AddStringArgument("param1");

    ASSERT_FALSE(parser.Parse(SplitString("app")));
}


TEST(ArgParserTestSuite, StoreValueTest) {
    ArgParser parser("My Parser");
    std::string value;
    parser.AddStringArgument("param1").StoreValue(value);

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1")));
    ASSERT_EQ(value, "value1");
}


TEST(ArgParserTestSuite, MultiStringTest) {
    ArgParser parser("My Parser");
    std::string value;
    parser.AddStringArgument("param1").StoreValue(value);
    parser.AddStringArgument('a', "param2");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1 --param2=value2")));
    ASSERT_EQ(parser.GetStringValue("param2"), "value2");
}


TEST(ArgParserTestSuite, IntTest) {
    ArgParser parser("My Parser");
    parser.AddIntArgument("param1");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=100500")));
    ASSERT_EQ(parser.GetIntValue("param1"), 100500);
}


TEST(ArgParserTestSuite, MultiValueTest) {
    ArgParser parser("My Parser");
    std::vector<int> int_values;
    parser.AddIntArgument('p', "param1").MultiValue().StoreValues(int_values);

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
    ASSERT_EQ(parser.GetIntValue("param1", 0), 1);
    ASSERT_EQ(int_values[1], 2);
    ASSERT_EQ(int_values[2], 3);
}


TEST(ArgParserTestSuite, MinCountMultiValueTest) {
    ArgParser parser("My Parser");
    std::vector<int> int_values;
    size_t MinArgsCount = 10;
    parser.AddIntArgument('p', "param1").MultiValue(MinArgsCount).StoreValues(int_values);

    ASSERT_FALSE(parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
}


TEST(ArgParserTestSuite, FlagTest) {
    ArgParser parser("My Parser");
    parser.AddFlag('f', "flag1");

    ASSERT_TRUE(parser.Parse(SplitString("app --flag1")));
    ASSERT_TRUE(parser.GetFlag("flag1"));
}


TEST(ArgParserTestSuite, FlagsTest) {
    ArgParser parser("My Parser");
    bool flag3 ;
    parser.AddFlag('a', "flag1");
    parser.AddFlag('b', "flag2").Default(true);
    parser.AddFlag('c', "flag3").StoreValue(flag3);

    ASSERT_TRUE(parser.Parse(SplitString("app -ac")));
    ASSERT_TRUE(parser.GetFlag("flag1"));
    ASSERT_TRUE(parser.GetFlag("flag2"));
    ASSERT_TRUE(flag3);
}


TEST(ArgParserTestSuite, PositionalArgTest) {
    ArgParser parser("My Parser");
    std::vector<int> values;
    parser.AddIntArgument("Param1").MultiValue(1).Positional().StoreValues(values);

    ASSERT_TRUE(parser.Parse(SplitString("app 1 2 3 4 5")));
    ASSERT_EQ(values[0], 1);
    ASSERT_EQ(values[2], 3);
    ASSERT_EQ(values.size(), 5);
}


TEST(ArgParserTestSuite, PositionalAndNormalArgTest) {
    ArgParser parser("My Parser");
    std::vector<int> values;
    parser.AddFlag('f', "flag", "Flag");
    parser.AddIntArgument('n', "number", "Some Number");
    parser.AddIntArgument("Param1").MultiValue(1).Positional().StoreValues(values);

    ASSERT_TRUE(parser.Parse(SplitString("app -n 0 1 2 3 4 5 -f")));
    ASSERT_TRUE(parser.GetFlag("flag"));
    ASSERT_EQ(parser.GetIntValue("number"), 0);
    ASSERT_EQ(values[0], 1);
    ASSERT_EQ(values[2], 3);
    ASSERT_EQ(values.size(), 5);
}


TEST(ArgParserTestSuite, RepeatedParsingTest) {
    ArgParser parser("My Parser");
    parser.AddHelp('h', "help", "Some Description about program");
    parser.AddStringArgument('i', "input", "File path for input file");
    parser.AddStringArgument('o', "output", "File path for output directory");
    parser.AddFlag('s', "flag1", "Read first number");
    parser.AddFlag('p', "flag2", "Read second number");
    parser.AddIntArgument("number", "Some Number");

    ASSERT_TRUE(parser.Parse(SplitString("app --number 2 -s -i test -o=test")));

    if (parser.GetFlag("flag1")) {
      parser.AddIntArgument("first", "First Number");
    } else if (parser.GetFlag("flag2")) {
      parser.AddIntArgument("second", "Second Number");
    }

    ASSERT_TRUE(parser.Parse(SplitString("app --number 2 -s -i test -o=test --first=52")));
    ASSERT_EQ(parser.GetIntValue("first"), 52);
}


TEST(ArgParserTestSuite, HelpTest) {
    ArgParser parser("My Parser");
    parser.AddHelp('h', "help", "Some Description about program");

    ASSERT_TRUE(parser.Parse(SplitString("app --help")));
    ASSERT_TRUE(parser.Help());
}


TEST(ArgParserTestSuite, HelpStringTest) {
    ArgParser parser("My Parser");
    parser.AddHelp('h', "help", "Some Description about program");
    parser.AddStringArgument('i', "input", "File path for input file").MultiValue(1);
    parser.AddFlag('s', "flag1", "Use some logic").Default(true);
    parser.AddFlag('p', "flag2", "Use some logic");
    parser.AddIntArgument("numer", "Some Number");


    ASSERT_TRUE(parser.Parse(SplitString("app --help")));
    // Проверка закоментирована намеренно. Ождиается, что результат вызова функции будет приблизительно такой же,
    // но не с точностью до символа

     ASSERT_EQ(
         parser.HelpDescription(),
         "My Parser\n"
         "Some Description about program\n"
         "\n"
         "     --numer=<int>,  Some Number\n"
         "-p,  --flag2,  Use some logic\n"
         "-s,  --flag1,  Use some logic\n"
         "-i,  --input=<string>,  File path for input file [repeated, min args = 1]\n"
         "\n"
         "-h, --help Display this help and exit\n"
     );
}




TEST(ArgParserTestSuite, MyTest18) {
    ArgParser parser("My Parser");
    std::string value;
    parser.AddStringArgument("param3").StoreValue(value);

    ASSERT_TRUE(parser.Parse(SplitString("app --param3=val --param3=zuneve")));
    ASSERT_EQ(value, "zuneve");
}

TEST(ArgParserTestSuite, MyTest19) {
    ArgParser parser("My Parser");
    parser.AddIntArgument("param1");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=666999")));
    ASSERT_EQ(parser.GetIntValue("param1"), 666999);
    ASSERT_FALSE(parser.GetIntValue("param1") == 999666);
    ASSERT_FALSE(parser.GetIntValue("param1") == 666998);
}

TEST(ArgParserTestSuite, MyTest20) {
    ArgParser parser("My Parser");
    std::vector<int> int_values;
    size_t MinArgsCount = 3;
    parser.AddIntArgument('p', "param1").MultiValue(MinArgsCount).StoreValues(int_values);

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
}

TEST(ArgParserTestSuite, MyTest21) {
    ArgParser parser("My Parser");
    std::vector<int> int_values;
    size_t MinArgsCount = 2;
    parser.AddIntArgument('p', "param1").MultiValue(MinArgsCount).StoreValues(int_values);

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3 --param1=4")));
    ASSERT_EQ(int_values.size(), 4);
}

TEST(ArgParserTestSuite, MyTest22) {
    ArgParser parser("My Parser");
    parser.AddStringArgument("param1").Default("ZOV");

    ASSERT_TRUE(parser.Parse(SplitString("app")));
    ASSERT_EQ(parser.GetStringValue("param1"), "ZOV");
}

TEST(ArgParserTestSuite, MyTest23) {
    ArgParser parser("My Parser");
    parser.AddStringArgument("param1").Default("ZOV");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1 goida")));
    ASSERT_EQ(parser.GetStringValue("param1"), "goida");
}

TEST(ArgParserTestSuite, MyTest24) {
    
    ArgParser parser("My Parser");
    std::vector<std::string> string_values;
    size_t MinArgsCount = 3;
    parser.AddStringArgument('p', "param1").MultiValue(MinArgsCount).StoreValues(string_values).Positional();

    ASSERT_TRUE(parser.Parse(SplitString("app 12 2 3")));
    ASSERT_EQ(string_values[0], "12");
}

TEST(ArgParserTestSuite, MyTest25) {
    ArgParser parser("My Parser");
    parser.AddFlag('f', "flag", "Flag");
    parser.AddFlag('w', "Write", "Just simple flag").Default(true);
    parser.AddFlag('u', "unite").Default(false);
    parser.AddIntArgument('n', "number", "Some Number");

    ASSERT_TRUE(parser.Parse(SplitString("app -n 29230239 -f")));
    ASSERT_TRUE(parser.GetFlag("flag"));
    ASSERT_TRUE(parser.GetFlag("Write"));
    ASSERT_FALSE(parser.GetFlag('u'));
}
