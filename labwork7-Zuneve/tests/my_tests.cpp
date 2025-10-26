#include <unrolled_list.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <list>
#include <random>


TEST(MyTest, RandomErase) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        if (i % 3 == 0) {
            std_list.push_front(i);
            unrolled_list.push_front(i);
        } else if (i % 3 == 1) {
            std_list.push_back(i);
            unrolled_list.push_back(i);
        } else {
            auto std_it = std_list.begin();
            auto unrolled_it = unrolled_list.begin();

            std::advance(std_it, std_list.size() / 2);
            std::advance(unrolled_it, std_list.size() / 2);

            std_list.insert(std_it, i);
            unrolled_list.insert(unrolled_it, i);
        }
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    for (int i = 0; i < 500; ++i) {
        auto std_it = std_list.begin();
        auto unrolled_it = unrolled_list.begin();

        size_t rnd_ind = rand() % std_list.size();

        std::advance(std_it, rnd_ind);
        std::advance(unrolled_it, rnd_ind);

        std_list.erase(std_it);
        unrolled_list.erase(unrolled_it);
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(MyTest, SequentialErase) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        if (i % 3 == 0) {
            std_list.push_front(i);
            unrolled_list.push_front(i);
        } else if (i % 3 == 1) {
            std_list.push_back(i);
            unrolled_list.push_back(i);
        } else {
            auto std_it = std_list.begin();
            auto unrolled_it = unrolled_list.begin();

            std::advance(std_it, std_list.size() / 2);
            std::advance(unrolled_it, std_list.size() / 2);

            std_list.insert(std_it, i);
            unrolled_list.insert(unrolled_it, i);
        }
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    for (int i = 0; i < 900; ++i) {
        auto std_it = std_list.begin();
        auto unrolled_it = unrolled_list.begin();
        size_t erase_ind = unrolled_list.size() >> 1;
        std::advance(std_it, erase_ind);
        std::advance(unrolled_it, erase_ind);
        std_list.erase(std_it);
        unrolled_list.erase(unrolled_it);
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(MyTest, EraseException1) {
    constexpr size_t n = 7;
    unrolled_list<int> unrolled_list(n, 3);

    for (int i = 0; i < 100; ++i) {
        auto unrolled_it = unrolled_list.begin();

        size_t rnd_ind = rand() % unrolled_list.size();

        std::advance(unrolled_it, rnd_ind);

        unrolled_list.insert(unrolled_it, i);
    }

    EXPECT_NO_THROW(unrolled_list.erase(unrolled_list.end()));
}

TEST(MyTest, EraseException2) {
    constexpr int n = 7;
    unrolled_list<int> unrolled_list(n, 3);

    for (int i = 0; i < 100; ++i) {
        auto unrolled_it = unrolled_list.begin();

        size_t rnd_ind = rand() % unrolled_list.size();

        std::advance(unrolled_it, rnd_ind);

        unrolled_list.insert(unrolled_it, i);
    }

    EXPECT_NO_THROW(unrolled_list.erase(unrolled_list.end()));
}

TEST(MyTest, FrontBack) {
    constexpr size_t n = 7;
    unrolled_list<int> unrolled_list(n, 20);

    for (int i = 0; i < 100; ++i) {
        unrolled_list.push_back(i + 1);
        unrolled_list.push_front(99 - i);
    }

    ASSERT_EQ(unrolled_list.front(), 0);
    ASSERT_EQ(unrolled_list.back(), 100);
}

TEST(MyTest, Swap) {
    unrolled_list<int> ul1, ul2;

    ul1.push_back(1);
    ul2.push_back(2);

    ASSERT_EQ(ul1.front(), 1);
    ASSERT_EQ(ul2.front(), 2);

    swap(ul1, ul2);

    ASSERT_EQ(ul1.front(), 2);
    ASSERT_EQ(ul2.front(), 1);

    ul2.swap(ul1);

    ASSERT_EQ(ul1.front(), 1);
    ASSERT_EQ(ul2.front(), 2);
}

TEST(MyTest, EqualUnrolledLists) {
    unrolled_list<int> ul2;

    for (int i = 0; i < 10; ++i) {
        ul2.push_back(i * 10);
    }

    unrolled_list<int> ul1 = ul2;

    ASSERT_TRUE(ul1 == ul2);
    ASSERT_TRUE(ul1 == ul1);
}

TEST(MyTest, Insert1) {
    unrolled_list<int> ul(1, 1);

    auto it = ul.insert(ul.begin(), 2);

    ASSERT_EQ(*it, 2);
    ASSERT_EQ(ul.back(), 1);
}

TEST(MyTest, Insert2) {
    unrolled_list<int> ul1(1, 19937), ul2;
    auto it = ul1.insert(ul1.begin(), 3, 7);

    ++it;

    ASSERT_EQ(*it, 7);
    ASSERT_EQ(ul1.back(), 19937);

    auto it2 = ul2.insert(ul2.end(), 1703);
    ASSERT_EQ(ul2.back(), 1703);

    ++it;
    ul2.insert(ul2.begin(), it, ul1.end());

    std::list<int> std_list = {7, 19937, 1703};
    ASSERT_THAT(ul2, ::testing::ElementsAreArray(std_list));
}


TEST(MyTest, Insert3) {
    unrolled_list<int, 2> ul1, ul2;

    ul2.push_back(292);
    ul1.insert(ul1.end(), 2, 127);
    ul2.insert(ul2.begin(), ul1);

    std::list<int> std_list;
    for (int i = 0; i < 2; ++i) {
        std_list.push_back(127);
    }
    std_list.push_back(292);

    ASSERT_THAT(ul2, ::testing::ElementsAreArray(std_list));
}

TEST(MyTest, Assign1) {
    std::list<int> std_list = {1, 2, 3, 4, 5};
    unrolled_list<int> unrolled_list;

    unrolled_list.assign(std_list.begin(), std_list.end());

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(MyTest, AssignInitializerList) {
    std::initializer_list<int> il = {10, 20, 30, 40, 50};
    unrolled_list<int> unrolled_list;

    unrolled_list.assign(il);

    std::list<int> std_list(il);
    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(MyTest, AssignFill) {
    size_t n = 5;
    int value = 42;
    unrolled_list<int> unrolled_list;

    unrolled_list.assign(n, value);

    std::list<int> std_list(n, value);
    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(MyTest, EraseTest) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 10; ++i) {
        std_list.push_back(i);
        unrolled_list.push_back(i);
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    auto std_it = std_list.begin();
    auto unrolled_it = unrolled_list.begin();
    std::advance(std_it, 5);
    std::advance(unrolled_it, 5);

    std_list.erase(std_it);
    unrolled_list.erase(unrolled_it);

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    std_list.erase(std_list.begin());
    unrolled_list.erase(unrolled_list.begin());

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    std_list.erase(--std_list.end());
    unrolled_list.erase(--unrolled_list.end());

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(MyTest, EraseRangeTest) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 10; ++i) {
        std_list.push_back(i);
        unrolled_list.push_back(i);
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    auto std_it1 = std_list.begin();
    auto std_it2 = std_list.begin();
    auto unrolled_it1 = unrolled_list.begin();
    auto unrolled_it2 = unrolled_list.begin();

    std::advance(std_it1, 3);
    std::advance(std_it2, 6);
    std::advance(unrolled_it1, 3);
    std::advance(unrolled_it2, 6);

    std_list.erase(std_it1, std_it2);
    unrolled_list.erase(unrolled_it1, unrolled_it2);

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    std_it1 = std_list.begin();
    std_it2 = std_list.begin();
    unrolled_it1 = unrolled_list.begin();
    unrolled_it2 = unrolled_list.begin();

    std::advance(std_it1, 0);
    std::advance(std_it2, 3);
    std::advance(unrolled_it1, 0);
    std::advance(unrolled_it2, 3);

    std_list.erase(std_it1, std_it2);
    unrolled_list.erase(unrolled_it1, unrolled_it2);

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    std_list.erase(std_list.begin(), std_list.end());
    unrolled_list.erase(unrolled_list.begin(), unrolled_list.end());

    ASSERT_THAT(unrolled_list, ::testing::IsEmpty());
    ASSERT_THAT(std_list, ::testing::IsEmpty());
}

TEST(MyTest, PushPop) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    std_list.push_back(1);
    unrolled_list.push_back(1);

    std_list.push_front(0);
    unrolled_list.push_front(0);

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    std_list.pop_back();
    unrolled_list.pop_back();

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    std_list.pop_front();
    unrolled_list.pop_front();

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}
