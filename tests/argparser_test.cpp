#include "lib/arg_parser/store/store.hpp"
#include <sstream>

#include <gtest/gtest.h>
#include <lib/labwork_adapter/ArgParser.hpp>

using namespace ArgumentParser;

/*
    Функция принимает в качество аргумента строку, разделяет ее по "пробелу"
    и возвращает вектор полученных слов
*/
std::vector<std::string> SplitString(const std::string& str) {
    std::istringstream iss(str);

    return {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
}
// #define BANED_TEST
#ifndef BANED_TEST

TEST(ArgParserTestSuite, EmptyTest) {
    ArgParserLabwork parser("My Empty Parser");

    ASSERT_TRUE(parser.Parse(SplitString("app")));
}


TEST(ArgParserTestSuite, StringTest) {
    ArgParserLabwork parser("My Parser");
    parser.AddStringArgument("param1");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1")));
    ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}

TEST(ArgParserTestSuite, ShortNameTest) {
    ArgParserLabwork parser("My Parser");
    parser.AddStringArgument('p', "param1");

    ASSERT_TRUE(parser.Parse(SplitString("app -p=value1")));
    ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}

TEST(ArgParserTestSuite, DefaultTest) {
    ArgParserLabwork parser("My Parser");
    parser.AddStringArgument("param1").Default<std::string>("value1");

    ASSERT_TRUE(parser.Parse(SplitString("app")));
    ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}


TEST(ArgParserTestSuite, NoDefaultTest) {
    ArgParserLabwork parser("My Parser");
    parser.AddStringArgument("param1");

    ASSERT_FALSE(parser.Parse(SplitString("app")));
}


TEST(ArgParserTestSuite, StoreValueTest) {
    ArgParserLabwork parser("My Parser");
    std::string value;
    parser.AddStringArgument("param1").StoreValue(value);

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1")));
    ASSERT_EQ(value, "value1");
}


TEST(ArgParserTestSuite, MultiStringTest) {
    ArgParserLabwork parser("My Parser");
    std::string value;
    parser.AddStringArgument("param1").StoreValue(value);
    parser.AddStringArgument('a', "param2");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1 --param2=value2")));
    ASSERT_EQ(parser.GetStringValue("param2"), "value2");
}


TEST(ArgParserTestSuite, IntTest) {
    ArgParserLabwork parser("My Parser");
    parser.AddIntArgument("param1");

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=100500")));
    ASSERT_EQ(parser.GetIntValue("param1"), 100500);
}


TEST(ArgParserTestSuite, MultiValueTest) {
    ArgParserLabwork parser("My Parser");
    std::vector<int> int_values;
    parser.AddIntArgument('p', "param1").MultiValue<int>().StoreValues(int_values);

    ASSERT_TRUE(parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
    ASSERT_EQ(parser.GetIntValue("param1", 0), 1);
    ASSERT_EQ(int_values[1], 2);
    ASSERT_EQ(int_values[2], 3);
}


TEST(ArgParserTestSuite, MinCountMultiValueTest) {
    ArgParserLabwork parser("My Parser");
    std::vector<int> int_values;
    size_t MinArgsCount = 10;
    parser.AddIntArgument('p', "param1").MultiValue<int>(MinArgsCount).StoreValues(int_values);

    ASSERT_FALSE(parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
}


TEST(ArgParserTestSuite, FlagTest) {
    ArgParserLabwork parser("My Parser");
    parser.AddFlag('f', "flag1");

    ASSERT_TRUE(parser.Parse(SplitString("app --flag1")));
    ASSERT_TRUE(parser.GetFlag("flag1"));
}


TEST(ArgParserTestSuite, FlagsTest) {
    ArgParserLabwork parser("My Parser");
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
    ArgParserLabwork parser("My Parser");
    std::vector<int> values;
    parser.AddIntArgument("Param1").MultiValue<int>(1).Positional().StoreValues(values);

    ASSERT_TRUE(parser.Parse(SplitString("app 1 2 3 4 5")));
    ASSERT_EQ(values[0], 1);
    ASSERT_EQ(values[2], 3);
    ASSERT_EQ(values.size(), 5);
}
#endif

#ifndef BANED_TEST

TEST(ArgParserTestSuite, PositionalAndNormalArgTest) {
    ArgParserLabwork parser("My Parser");
    std::vector<int> values;
    parser.AddFlag('f', "flag", "Flag");
    parser.AddIntArgument('n', "number", "Some Number");
    parser.AddIntArgument("Param1").MultiValue<int>(1).Positional().StoreValues(values);

    ASSERT_TRUE(parser.Parse(SplitString("app -n 0 1 2 3 4 5 -f")));
    ASSERT_TRUE(parser.GetFlag("flag"));
    ASSERT_EQ(parser.GetIntValue("number"), 0);
    ASSERT_EQ(values[0], 1);
    ASSERT_EQ(values[2], 3);
    ASSERT_EQ(values.size(), 5);
}



TEST(ArgParserTestSuite, RepeatedParsingTest) {
    ArgParserLabwork parser("My Parser");
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
    ArgParserLabwork parser("My Parser");
    parser.AddHelp('h', "help", "Some Description about program");

    ASSERT_TRUE(parser.Parse(SplitString("app --help")));
    ASSERT_TRUE(parser.Help());
}


TEST(ArgParserTestSuite, HelpStringTest) {
    ArgParserLabwork parser("My Parser");
    parser.AddHelp('h', "help", "Some Description about program");
    parser.AddStringArgument('i', "input", "File path for input file").MultiValue<std::string>(1);
    parser.AddFlag('s', "flag1", "Use some logic").Default(true);
    parser.AddFlag('p', "flag2", "Use some logic");
    parser.AddIntArgument("numer", "Some Number");


    ASSERT_TRUE(parser.Parse(SplitString("app --help")));
    std::cout << "\n\n" << parser.HelpDescription() << std::endl;
    // Проверка закоментирована намеренно. Ождиается, что результат вызова функции будет приблизительно такой же,
    // но не с точностью до символа

    // ASSERT_EQ(
    //     parser.HelpDescription(),
    //     "My Parser\n"
    //     "Some Description about program\n"
    //     "\n"
    //     "-i,  --input=<string>,  File path for input file [repeated, min args = 1]\n"
    //     "-s,  --flag1,  Use some logic [default = true]\n"
    //     "-p,  --flag2,  Use some logic\n"
    //     "     --number=<int>,  Some Number\n"
    //     "\n"
    //     "-h, --help Display this help and exit\n"
    // );
}
#endif

TEST(ArgParserTestSuite, StoreIsEqToGetValue) {
    std::vector<std::string> str_store;

    ArgParserLabwork parser("TestParser");
    parser.AddHelp('h', "help", "help description");
    parser.AddStringArgument('s', "string", "positional multivalue test").MultiValue<std::string>(5).Positional().StoreValues(str_store);

    ASSERT_TRUE(parser.Parse(SplitString("app 1 2 3 4 5 a b c d")));

    auto string_res = parser.GetStringValues("string");

    for (auto beg_store = str_store.begin(), end_store = str_store.end(),
        beg_res = string_res.begin(), end_res = string_res.end();
        beg_store != end_store && beg_res != end_res; ++beg_store, ++beg_res) {
        bool cmp_res = *beg_store == *beg_res;
        ASSERT_TRUE(cmp_res);
    }
}

TEST(ArgParserTestSuite, MoreOneMultivalue) {
    ArgParserLabwork parser("TestParser");
    parser.AddHelp('h', "help", "help description");

    std::vector<int> int_store;
    parser.AddIntArgument("int", "positional multivalue string test").MultiValue<int>().Positional().StoreValues(int_store);

    std::vector<std::string> str_store;
    parser.AddStringArgument('s', "string", "positional multivalue string test").MultiValue<std::string>(5).Positional().StoreValues(str_store);

    ASSERT_TRUE(parser.Parse(SplitString("app 1 2 3 4 5 a b c d e")));
    auto string_res = parser.GetStringValues("string");

    const int correct_int_arr[] = {1, 2, 3, 4, 5};
    const char* correct_str_arr[] = {"a", "b", "c", "d", "e"};

    std::size_t correct_arr_ind = 0;
    for (auto&& elem : str_store) {
        bool cmp_res = elem == correct_str_arr[correct_arr_ind];
        if (!cmp_res)
            std::cout << elem << " != " << correct_str_arr[correct_arr_ind] << std::endl;
        ASSERT_TRUE(cmp_res);
        ++correct_arr_ind;
    }

    correct_arr_ind = 0;
    for (auto&& elem : int_store) {
        bool cmp_res = elem == correct_int_arr[correct_arr_ind];
        if (!cmp_res)
            std::cout << elem << " != " << correct_int_arr[correct_arr_ind] << std::endl;
        ASSERT_TRUE(cmp_res);
        ++correct_arr_ind;
    }
}

TEST(ArgParserTestSuite, MoreOnePositional) {
    ArgParserLabwork parser("TestParser");

    std::string fst_store;
    std::string scd_store;
    parser.AddStringArgument('a', "string1", "positional string test").Positional().StoreValue(fst_store);
    parser.AddStringArgument('b', "string2", "positional string test").Positional().StoreValue(scd_store);

    ASSERT_TRUE(parser.Parse(SplitString("app bububu bebebe")));

    ASSERT_EQ(fst_store, "bububu");
    ASSERT_EQ(scd_store, "bebebe");
}

TEST(ArgParserTestSuite, DefaultRedefinition) {
    ArgParserLabwork parser("TestParser");

    int store;
    parser.AddIntArgument('a', "def_arg", "redef test").Default(239).Positional().StoreValue(store);

    std::string str_store;
    parser.AddStringArgument('b', "def_arg2", "redef test").Default<std::string>("gdzg").Positional().StoreValue(str_store);

    ASSERT_TRUE(parser.Parse(SplitString("app 30 cgsg_forever")));

    std::cout << parser.HelpDescription() << std::endl;

    ASSERT_EQ(store, 30);
    ASSERT_EQ(str_store, "cgsg_forever");
}

TEST(ArgParserTestSuite, MultivalueSeparation) {
    std::vector<std::string> str_store;

    ArgParserLabwork parser("TestParser");
    parser.AddStringArgument('s', "string", "positional multivalue test").MultiValue<std::string>(4).Positional().StoreValues(str_store);
    parser.AddStringArgument('a', "non_pos_string", "positional multivalue test");

    ASSERT_TRUE(parser.Parse(SplitString("app one two --non_pos_string cgsg_forever four five")));

    ASSERT_EQ(parser.GetStringValue("non_pos_string"), "cgsg_forever");

    const char* correct_arr[] = {"one", "two", "four", "five"};
    std::size_t correct_arr_ind = 0;

    ASSERT_EQ(sizeof(correct_arr) / sizeof(correct_arr[0]), str_store.size());
    for (auto beg_store = str_store.begin(), end_store = str_store.end();
        beg_store != end_store ; ++beg_store, ++correct_arr_ind) {
        bool cmp_res = *beg_store == correct_arr[correct_arr_ind];
        if (!cmp_res)
            std::cout << *beg_store << " != " << correct_arr[correct_arr_ind] << std::endl;
        ASSERT_TRUE(cmp_res);
    }
}

TEST(ArgParserTestSuite, MultivalueDefinition) {
    std::vector<std::string> str_store;

    ArgParserLabwork parser("TestParser");
    parser.AddStringArgument('s', "string", "positional multivalue test").Positional().MultiValue<std::string>().StoreValues(str_store);

    ASSERT_TRUE(parser.Parse(SplitString("app -s one --string=two -s=four --string five 30ka")));

    const char* correct_arr[] = {"one", "two", "four", "five", "30ka"};
    std::size_t correct_arr_ind = 0;

    ASSERT_EQ(sizeof(correct_arr) / sizeof(correct_arr[0]), str_store.size());
    for (auto beg_store = str_store.begin(), end_store = str_store.end();
        beg_store != end_store ; ++beg_store, ++correct_arr_ind) {
        bool cmp_res = *beg_store == correct_arr[correct_arr_ind];
        if (!cmp_res)
            std::cout << *beg_store << " != " << correct_arr[correct_arr_ind] << std::endl;
        ASSERT_TRUE(cmp_res);
    }
}

TEST(ArgParserTestSuite, NotAdapterTrueTest) {
    argument_parser::ArgParser parser_device;

    argument_parser::Argument arg("store_arg");
    arg.SetStore<int>(nullptr);
    arg.SetStore(new argument_parser::Store<int>());

    int store;
    arg.SetPtrStore(&store);

    parser_device.registrate(
        argument_parser::make_argument<std::vector<int>>("integer").SetMultiValueStore(new argument_parser::MultiValueStore<std::vector<int>>()),
        argument_parser::make_argument<bool>("flag").SetStore(new argument_parser::Store<bool>()),
        argument_parser::make_argument<std::string>("str").SetMultiValueStore(new argument_parser::MultiValueStore<std::vector<std::string>>()).Positional(),
        argument_parser::make_argument<double>("dbl").SetStore(new argument_parser::Store<double>()),
        arg
    );

  std::vector<std::string_view> argv_test = {
    "name_of_prog",
    "--integer=0", "-1", "2", "3", "4",
    "--flag",
    "privet_mir",
    "hello",
    "world",
    "--dbl", "30.239",
    "--store_arg", "30",
  };

ASSERT_TRUE(parser_device.parse(argv_test));

#if 0
    if (parser_device.parse(argv_test)) {
        std::cout << "main:   PARSING SUCCESSFULLY" << std::endl;
    } else {
        std::cout << "main:   PARSING FAIL" << std::endl;
    }
#endif
}

TEST(ArgParserTestSuite, NotAdapterFalseTest) {
    argument_parser::ArgParser parser_device;

    argument_parser::Argument arg("store_arg");
    arg.SetStore<int>(nullptr);
    arg.SetStore(new argument_parser::Store<int>());

    int store;
    arg.SetPtrStore(&store);

    auto arg_integer = std::move(argument_parser::make_argument<int>("integer2").SetStore(new argument_parser::Store<int>(5)));
    arg_integer.WasInitialize();

    parser_device.registrate(
        argument_parser::make_argument<std::vector<int>>("integer").SetMultiValueStore(new argument_parser::MultiValueStore<std::vector<int>>()),
        arg_integer,
        argument_parser::make_argument<bool>("flag").SetStore(new argument_parser::Store<bool>()),
        argument_parser::make_argument<std::string>("str").SetMultiValueStore(new argument_parser::MultiValueStore<std::vector<std::string>>()).Positional(),
        argument_parser::make_argument<double>("dbl").SetStore(new argument_parser::Store<double>()),
        arg
    );


  std::vector<std::string_view> argv_test0 = {
    "name_of_prog",
  };

  std::vector<std::string_view> argv_test1 = {
    "name_of_prog",
    "--integer=0", "-1", "2", "3", "--dbl=pupupuuuuuu",
    "privet_mir",
    "hello",
    "world",
    "--dbl", "30.239",
    "--store_arg", "30",
  };
  std::vector<std::string_view> argv_test2 = {
    "name_of_prog",
    "--intege=0", "-1", "2", "3", "4",
    "--flag",
    "privet_mir",
    "hello",
    "world",
    "--dbl", "30.239",
    "--store_arg", "30",
  };
  std::vector<std::string_view> argv_test3 = {
    "name_of_prog",
    "--integer=0", "-1", "2", "3", "4",
    "--not_flag",
    "privet_mir",
    "hello",
    "world",
    "--dbl", "30.239",
    "--store_arg", "30",
  };

ASSERT_FALSE(parser_device.parse(argv_test0));
ASSERT_FALSE(parser_device.parse(argv_test1));
ASSERT_FALSE(parser_device.parse(argv_test2));
ASSERT_FALSE(parser_device.parse(argv_test3));

#if 0
    if (parser_device.parse(argv_test)) {
        std::cout << "main:   PARSING SUCCESSFULLY" << std::endl;
    } else {
        std::cout << "main:   PARSING FAIL" << std::endl;
    }
#endif
}