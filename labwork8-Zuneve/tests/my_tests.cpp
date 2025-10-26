#include <processing.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <list>
#include <random>
#include <expected>


TEST(MyTest, MultiTransform) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    std::stringstream file_emulator;
    auto result = AsDataFlow(input) |
                  Filter([](int i) { return i != 3; })
                  | Filter([](int i) { return i != 3; })
    | Transform([](int i) { return i * 10; })
    | Transform([](int i) { return i + 10; })
    | Transform([](int i) { return std::to_string(i * 10); });
    result | Write(file_emulator, '|');
    result | Write(file_emulator, '|');

    std::string real_ans = "200|300|500|600|200|300|500|600|";
    ASSERT_EQ(file_emulator.str(), real_ans);
}

TEST(MyTest, DoubleEvenNumbers) {
    std::vector<int> input = {1, 2, 3, 4, 5, 6};
    auto result = AsDataFlow(input)
                | Filter([](int x) { return x % 2 == 0; })
                | Transform([](int x) { return x * 2; })
                | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(4, 8, 12));
}

TEST(MyTest, BasicEquality) {
    KV<int, std::string> kv1{1, "a"};
    KV<int, std::string> kv2{1, "a"};
    KV<int, std::string> kv3{2, "b"};
    ASSERT_EQ(kv1, kv2);
    ASSERT_NE(kv1, kv3);
}

TEST(MyTest, OutputToStream) {
    std::vector<std::string> input = {"a", "b", "c"};
    std::stringstream out;
    AsDataFlow(input) | Out(out, "");
    ASSERT_EQ(out.str(), "abc");
}

TEST(MyTest, RecursiveDir) {
    std::filesystem::path tmp_dir = std::filesystem::temp_directory_path() / "dataflow_test_dir";
    std::filesystem::create_directories(tmp_dir);
    std::ofstream(tmp_dir / "a.txt") << "file A";
    std::ofstream(tmp_dir / "b.txt") << "file B";

    bool recursive = true;
    auto result = Dir(tmp_dir.string(), recursive) | OpenFiles() | Transform([](std::ifstream& f) {
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }) | AsVector();

    ASSERT_THAT(result, testing::UnorderedElementsAre("file A", "file B"));

    std::filesystem::remove_all(tmp_dir);
}

TEST(MyTest, SplitValidAndErrors) {
    std::vector<std::expected<int, std::string>> input = {
        1, std::unexpected("error1"), 2, std::unexpected("error2"), 3
    };

    auto [unexpected_flow, good_flow] = AsDataFlow(input) | SplitExpected();

    std::stringstream unexpected_file;
    unexpected_flow | Write(unexpected_file, '.');

    auto expected_result = good_flow | AsVector();
    ASSERT_EQ(unexpected_file.str(), "error1.error2.");
    ASSERT_THAT(expected_result, testing::ElementsAre(1, 2, 3));
}

TEST(MyTest, AddStrPrefix) {
    std::vector<std::string> input = {"one", "two", "three"};
    auto result = AsDataFlow(input)
                | Transform([](const std::string& s) { return "num:" + s; })
                | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("num:one", "num:two", "num:three"));
}

TEST(MyTest, EmptyRightSide) {
    std::vector<KV<int, std::string>> left = {{1, "a"}, {2, "b"}};
    std::vector<KV<int, std::string>> right = {};

    auto result = AsDataFlow(left)
                | Join(AsDataFlow(right))
                | AsVector();

    ASSERT_THAT(
        result,
        testing::ElementsAre(
            JoinResult<std::string, std::string>{"a", std::nullopt},
            JoinResult<std::string, std::string>{"b", std::nullopt}
        )
    );
}

TEST(MyTest, MultipleDelimiters) {
    std::vector<std::stringstream> files(1);
    files[0] << "1,2;3,4;5";
    auto result = AsDataFlow(files) | Split(",;") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("1", "2", "3", "4", "5"));
}

TEST(MyTest, AllErrors) {
    std::vector<std::expected<int, std::string>> input = {
        std::unexpected("fail1"),
        std::unexpected("fail2")
    };

    auto [unexpected_flow, good_flow] = AsDataFlow(input) | SplitExpected();
    auto unexpected_result = unexpected_flow | AsVector();
    auto expected_result = good_flow | AsVector();

    ASSERT_TRUE(expected_result.empty());
    ASSERT_THAT(unexpected_result, testing::ElementsAre("fail1", "fail2"));
}

TEST(MyTest, RepeatedLeftKeys) {
    std::vector<KV<int, std::string>> left = {{1, "a"}, {1, "b"}, {2, "c"}};
    std::vector<KV<int, std::string>> right = {{1, "x"}, {2, "y"}};

    auto result = AsDataFlow(left)
                | Join(AsDataFlow(right))
                | AsVector();

    ASSERT_THAT(result, testing::ElementsAre(
        JoinResult<std::string, std::string>{"a", "x"},
        JoinResult<std::string, std::string>{"b", "x"},
        JoinResult<std::string, std::string>{"c", "y"}
    ));
}

TEST(MyTest, RepeatedKeys) {
    std::vector<KV<int, std::string>> left = {{1, "a"}, {1, "b"}};
    std::vector<KV<int, std::string>> right = {{1, "x"}};

    auto result = AsDataFlow(left)
                | Join(AsDataFlow(right))
                | AsVector();

    ASSERT_THAT(result, testing::ElementsAre(
        JoinResult<std::string, std::string>{"a", "x"},
        JoinResult<std::string, std::string>{"b", "x"}
    ));
}

TEST(MyTest, SimpleJoinResult) {
    JoinResult<std::string, int> jr{"value", 42};
    ASSERT_EQ(jr.base, "value");
    ASSERT_EQ(jr.joined, 42);
}

TEST(MyTest, EmptyLeftInput) {
    std::vector<KV<int, std::string>> left = {};
    std::vector<KV<int, std::string>> right = {{1, "x"}};

    auto result = AsDataFlow(left)
                | Join(AsDataFlow(right))
                | AsVector();

    ASSERT_TRUE(result.empty());
}

TEST(MyTest, EqualityComparison) {
    JoinResult<std::string, int> r1{"one", 1};
    JoinResult<std::string, int> r2{"one", 1};
    JoinResult<std::string, int> r3{"two", 2};
    EXPECT_EQ(r1, r2);
    EXPECT_NE(r1, r3);
}


TEST(MyTest, MultipleDelimitersInRow) {
    std::vector<std::stringstream> input(1);
    input[0] << "a,,b,,c";
    auto result = AsDataFlow(input) | Split(",") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("a", "", "b", "", "c"));
}

TEST(MyTest, EndsWithDelimiter) {
    std::vector<std::stringstream> input(1);
    input[0] << "1|2|3|";
    auto result = AsDataFlow(input) | Split("|") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("1", "2", "3"));
}

TEST(MyTest, FilterTransformPipeline) {
    std::vector<int> input = {1, 2, 3, 4, 5, 6};

    auto result = AsDataFlow(input)
        | Filter([](int x) { return x % 2 == 0; }) // 2, 4, 6
        | Transform([](int x) { return x * 2; }) // 4, 8, 12
        | Filter([](int x) { return x > 5; }) // 8, 12
        | Transform([](int x) { return x + 1; }) // 9, 13
        | AsVector();

    ASSERT_THAT(result, testing::ElementsAre(9, 13));
}

TEST(MyTest, StringsLength) {
    std::vector<std::string> input = {
        "a", "ab", "abc",
        "abcd", "abacaba"
    };

    auto result = AsDataFlow(input)
        | Filter([](const std::string& s) {
            return s.size() >= 2;
        })
        | Transform([](const std::string& s) {
            return s + "!";
        })
        | Filter([](const std::string& s) {
            return s.size() <= 3;
        })
        | Transform([](const std::string& s) {
            return "[" + s + "]";
        })
    | AsVector();

    ASSERT_THAT(result, testing::ElementsAre("[ab!]"));
}

TEST(Mytest, SplitBySpaceAndFilter) {
    std::vector<std::stringstream> s(1);
    s[0] << "one two three four";

    auto result = AsDataFlow(s)
        | Split(" ")
        | Filter([](const std::string& word) { return word.size() > 3; })
        | AsVector();

    ASSERT_THAT(result, testing::ElementsAre("three", "four"));
}


TEST(MyTest, TransformAfterDrop) {
    std::vector<std::optional<int>> input = {1, std::nullopt, 3};
    auto result = AsDataFlow(input)
        | DropNullopt()
        | Transform([](int x) { return x * 2; })
        | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(2, 6));
}

TEST(MyTest, SumByKey) {
    std::vector<std::pair<std::string, int>> input = {
        {"a", 1}, {"b", 2}, {"a", 3}, {"b", 4}, {"c", 5}
    };
    auto result = AsDataFlow(input)
        | AggregateByKey(
            0,
            [](const auto& pair, int& acc) { acc += pair.second; },
            [](const auto& pair) { return pair.first; }
        )
        | AsVector();

    ASSERT_THAT(result, ::testing::UnorderedElementsAre(
        std::make_pair("a", 4),
        std::make_pair("b", 6),
        std::make_pair("c", 5)
    ));
}

TEST(MyTest, CharCounting) {
    std::string input = "abcdaabd";

    auto result =
        AsDataFlow(input)
        | AggregateByKey(
            int{0},
            [](char, int& acc) { ++acc; },
            [](char c) { return c; }
        )
        | AsVector();

    ASSERT_THAT(result, ::testing::UnorderedElementsAre(
        std::make_pair('a', 3),
        std::make_pair('b', 2),
        std::make_pair('c', 1),
        std::make_pair('d', 2)
    ));
}

TEST(MyTest, NullOptTransform) {
    std::vector<std::optional<std::string>> input = {
        std::make_optional("Alice"),
        std::nullopt,
        std::make_optional("Bob")
    };

    auto result = AsDataFlow(input)
        | Transform([](const std::optional<std::string>& arg) {
            return arg.has_value() ? arg.value() : std::string("Unknown");
        })
        | AsVector();

    ASSERT_THAT(result, ::testing::ElementsAre("Alice", "Unknown", "Bob"));
}


TEST(MyTest, StructTransform) {
    struct Record {
        int id;
        std::string name;
    };
    std::vector<Record> input = {
        {1, "Alice"},
        {2, "Bob"},
        {3, "Charlie"}
    };

    auto result =
        AsDataFlow(input)
        | Transform([](const Record& r) { return r.name; })
        | AsVector();

    ASSERT_THAT(result, ::testing::ElementsAre("Alice", "Bob", "Charlie"));
}
