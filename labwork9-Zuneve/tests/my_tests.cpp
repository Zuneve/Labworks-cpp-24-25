#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <numeric>
#include <string>

#include "lib/scheduler.h"

struct AddNumber {
    float add(float a) const {
        return a + number;
    }

    float add2(float a, float b) const {
        return a + b + number;
    }

    AddNumber(float num) : number(num) {
    }

    float number;
};


TEST(TTaskSchedulerTest, ExampleTest) {
    float a = 1;
    float b = -2;
    float c = 0;
    AddNumber add(3);

    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](float a, float c) { return -4 * a * c; }, a, c);

    auto id2 = scheduler.add([](float b, float v) { return b * b + v; }, b, scheduler.getFutureResult<float>(id1));

    auto id3 = scheduler.add([](float b, float d) { return -b + std::sqrt(d); }, b,
                             scheduler.getFutureResult<float>(id2));

    auto id4 = scheduler.add([](float b, float d) { return -b - std::sqrt(d); }, b,
                             scheduler.getFutureResult<float>(id2));

    auto id5 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id3));

    auto id6 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id4));

    auto id7 = scheduler.add(&AddNumber::add, add, scheduler.getFutureResult<float>(id6));

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<float>(id5), 2);
    ASSERT_EQ(scheduler.getResult<float>(id6), 0);
    ASSERT_EQ(scheduler.getResult<float>(id7), 3);
}

TEST(TTaskSchedulerTest, MemberFunctionTest) {
    TTaskScheduler scheduler;
    AddNumber add_number(5.0f);

    auto id = scheduler.add(&AddNumber::add, std::ref(add_number), 3.5f);
    scheduler.executeAll();

    ASSERT_NEAR(scheduler.getResult<float>(id), 8.5f, 1e-5f);
}

TEST(TTaskSchedulerTest, ErrorTest1) {
    TTaskScheduler scheduler;

    auto id = scheduler.add([]() -> int {
        throw std::runtime_error("error: runtime error");
    });

    ASSERT_THROW(scheduler.executeAll(), std::runtime_error);
}

TEST(TTaskSchedulerTest, SimpleTest42_43) {
    TTaskScheduler scheduler;
    int value = 42;
    auto id = scheduler.add([](int v) { return v + 1; }, value);
    scheduler.executeAll();
    int result = scheduler.getResult(scheduler.getFutureResult<int>(id));
    ASSERT_EQ(result, 43);
}

TEST(TTaskSchedulerTest, TasksWithDependencies) {
    TTaskScheduler scheduler;
    int a = 239, b = 7;
    auto id1 = scheduler.add([](int x) { return x * 2; }, a);
    auto id2 = scheduler.add([](int x, int y) { return x + y; }, b, scheduler.getFutureResult<int>(id1));

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult(scheduler.getFutureResult<int>(id2)), 485);
}

TEST(TTaskSchedulerTest, SimpleAdderTest) {
    TTaskScheduler scheduler;
    AddNumber adder(10);
    auto id = scheduler.add(&AddNumber::add2, adder, 10, 15);

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult(scheduler.getFutureResult<float>(id)), 35);
}


TEST(TTaskSchedulerTest, ConsequenseTest) {
    TTaskScheduler scheduler;
    auto id1 = scheduler.add([]() { return 1; });
    auto id2 = scheduler.add([](int x) { return x + 2; }, scheduler.getFutureResult<int>(id1));
    auto id3 = scheduler.add([](int x) { return x + 3; }, scheduler.getFutureResult<int>(id2));
    auto id4 = scheduler.add([](int x) { return x + 4; }, scheduler.getFutureResult<int>(id3));
    auto id5 = scheduler.add([](int x) { return x + 5; }, scheduler.getFutureResult<int>(id4));

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult(scheduler.getFutureResult<int>(id5)), 15);
}

TEST(TTaskSchedulerTest, FutureResultTest239) {
    TTaskScheduler scheduler;
    auto id = scheduler.add([]() { return 239; });
    auto future = scheduler.getFutureResult<int>(id);

    auto id1 = scheduler.add([](int x) { return x + 30; }, future);
    auto id2 = scheduler.add([](int x) { return x + 566; }, future);
    auto id3 = scheduler.add([](int x, int y) { return x * y; },
                             scheduler.getFutureResult<int>(id1), scheduler.getFutureResult<int>(id2));

    scheduler.executeAll();
    int result = scheduler.getResult(scheduler.getFutureResult<int>(id3));
    ASSERT_EQ(result, (239 + 30) * (239 + 566));
}

TEST(TTaskSchedulerTest, STDRefTest) {
    TTaskScheduler scheduler;
    int a = 1;
    auto id = scheduler.add([](int& ref) {
        ref += 10;
        return ref;
    }, std::ref(a));

    scheduler.executeAll();

    ASSERT_EQ(a, 11);
    ASSERT_EQ(scheduler.getResult(scheduler.getFutureResult<int>(id)), 11);
}

TEST(TTaskSchedulerExtraTest, ExecuteAllTwiceTest) {
    TTaskScheduler scheduler;
    int counter = 0;
    auto id = scheduler.add([&]() {
        ++counter;
        return counter;
    });

    scheduler.executeAll();
    scheduler.executeAll(); // nothing to do

    ASSERT_EQ(counter, 1);
    ASSERT_EQ(scheduler.getResult(scheduler.getFutureResult<int>(id)), 1);
}

TEST(TTaskSchedulerExtraTest, CycleTest) {
    TTaskScheduler scheduler;
    auto id = scheduler.add([] { return 1; });

    for (int i = 0; i < 238; ++i) {
        id = scheduler.add([](int x) { return x + 1; }, scheduler.getFutureResult<int>(id));
    }

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult(scheduler.getFutureResult<int>(id)), 239);
}

TEST(TTaskSchedulerExtraTest, ErrorTest2) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([]() -> int {
        throw std::runtime_error("");
    });

    auto id2 = scheduler.add([](int x) { return x + 1; }, scheduler.getFutureResult<int>(id1));

    ASSERT_THROW(scheduler.executeAll(), std::runtime_error);
}

TEST(TTaskSchedulerExtraTest, DifferentTypesTest1) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([]() -> int { return 52; });
    auto id2 = scheduler.add([]() -> std::string { return "itmo"; });
    auto id3 = scheduler.add([](int a, const std::string& s) {
        return s + " " + std::to_string(a);
    }, scheduler.getFutureResult<int>(id1), scheduler.getFutureResult<std::string>(id2));

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult(scheduler.getFutureResult<std::string>(id3)), "itmo 52");
}

TEST(TTaskSchedulerExtraTest, CyclicDependencyTest) {
    TTaskScheduler scheduler;

    ASSERT_ANY_THROW(scheduler.add([](int x) { return x + 1; },
        scheduler.getFutureResult<int>(0)););
}

TEST(TTaskSchedulerExtraTest, VoidTaskExecution) {
    TTaskScheduler scheduler;

    bool flag = false;
    scheduler.add([&flag]() { flag = true; });

    scheduler.executeAll();

    ASSERT_TRUE(flag);
}

TEST(TTaskSchedulerExtraTest, TwoDependencies) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([]() { return 292; });
    auto id2 = scheduler.add([](int x) { return x + 3; },
                             scheduler.getFutureResult<int>(id1));
    auto id3 = scheduler.add([](int y) { return y * 10; },
                             scheduler.getFutureResult<int>(id2));

    scheduler.executeAll();
    ASSERT_EQ(scheduler.getResult<int>(id3), 2950);
}

TEST(TTaskSchedulerExtraTest, StringTest1) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](const std::string& str) { return str + " world"; },
                             std::string("Hello,"));
    auto id2 = scheduler.add([](const std::string& str) { return str + "!"; },
                             scheduler.getFutureResult<std::string>(id1));

    scheduler.executeAll();
    ASSERT_EQ(scheduler.getResult<std::string>(id2), "Hello, world!");
}


TEST(TaskShedulerTestSuit, StringTest2) {
    TTaskScheduler scheduler;

    auto id = scheduler.add([](std::string str) {
        return str.size();
    }, std::string("zuneve_labwork9"));

    EXPECT_EQ(scheduler.getResult<std::size_t>(id), 15);
}

TEST(TTaskSchedulerExtraTest, ArrayTest) {
    TTaskScheduler scheduler;

    std::array<int, 3> arr1 = {1, 2, 3};

    auto id1 = scheduler.add([](const std::array<int, 3>& arr) {
        return std::accumulate(arr.begin(), arr.end(), 0);
    }, arr1);

    auto id2 = scheduler.add([](int sum) { return sum * 2; },
                             scheduler.getFutureResult<int>(id1));

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<int>(id2), 12);
}

TEST(TTaskSchedulerExtraTest, StructTest) {
    struct Employee {
        std::string name;
        int age;
    };

    TTaskScheduler scheduler;

    Employee employee{"John Doe", 30};

    auto id1 = scheduler.add([](const Employee& emp) {
        return emp.age + 10;
    }, employee);

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<int>(id1), 40);
}

TEST(TTaskSchedulerExtraTest, BizzBuzzTest) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int x) {
        if (x % 3 == 0 && x % 5 == 0) return std::string("FizzBuzz");
        if (x % 3 == 0) return std::string("Fizz");
        if (x % 5 == 0) return std::string("Buzz");
        return std::to_string(x);
    }, 15);

    auto id2 = scheduler.add([](int x) {
        if (x % 3 == 0 && x % 5 == 0) return std::string("FizzBuzz");
        if (x % 3 == 0) return std::string("Fizz");
        if (x % 5 == 0) return std::string("Buzz");
        return std::to_string(x);
    }, 3);

    auto id3 = scheduler.add([](int x) {
        if (x % 3 == 0 && x % 5 == 0) return std::string("FizzBuzz");
        if (x % 3 == 0) return std::string("Fizz");
        if (x % 5 == 0) return std::string("Buzz");
        return std::to_string(x);
    }, 5);

    auto id4 = scheduler.add([](int x) {
        if (x % 3 == 0 && x % 5 == 0) return std::string("FizzBuzz");
        if (x % 3 == 0) return std::string("Fizz");
        if (x % 5 == 0) return std::string("Buzz");
        return std::to_string(x);
    }, 7);

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<std::string>(id1), "FizzBuzz");
    ASSERT_EQ(scheduler.getResult<std::string>(id2), "Fizz");
    ASSERT_EQ(scheduler.getResult<std::string>(id3), "Buzz");
    ASSERT_EQ(scheduler.getResult<std::string>(id4), "7");
}

TEST(TTaskSchedulerExtraTest, ReferenceWrapperTest) {
    TTaskScheduler scheduler;

    int value = 10;
    auto id1 = scheduler.add([](int& x) { return x + 5; }, std::ref(value));

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<int>(id1), 15);
}

TEST(TTaskSchedulerExtraTest, MultipleFutureResultUseTest) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([]() { return 5; });

    auto future = scheduler.getFutureResult<int>(id1);

    auto id2 = scheduler.add([](int x) { return x + 10; }, future);
    auto id3 = scheduler.add([](int x) { return x * 2; }, future);

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<int>(id2), 15);
    ASSERT_EQ(scheduler.getResult<int>(id3), 10);
}

TEST(TTaskSchedulerExtraTest, DifferentTypesTest2) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int a, double b) {
        return a + b;
    }, 1, 2.5f);

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<double>(id1), 3.5);
}

