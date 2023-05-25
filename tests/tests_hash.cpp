#include "gtest/gtest.h"

#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <map>
#include <lib/hash.h>

#include "lib/hash.h"


TEST(PerformanceHashMapTest, InsertTest) {
    std::cout << "InsertTest\n";
    mtl::hash_map<int, int> MTL_MAP(5e6);
    std::unordered_map<int, int> STD_Map(5e6);
    std::vector<std::pair<int, int>> tests;
    for (int i = 0; i < 1e6; ++i) {
        tests.emplace_back(rand(), rand());
    }
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 1e6; ++i) {
        MTL_MAP.Insert(tests[i].first, tests[i].second);
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 1e6; ++i) {
        STD_Map[tests[i].first] = tests[i].second;
    }
    end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
}

TEST(PerformanceHashMapTest, ContainsTest) {
    std::cout << "ContainsTest\n";
    mtl::hash_map<int, int> MTL_MAP(5e6);
    std::unordered_map<int, int> STD_Map(5e6);
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 1e6; ++i) {
        auto key = rand(), value = rand();
        MTL_MAP.Insert(key, value);
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 1e6; ++i) {
        auto key = rand(), value = rand();
        STD_Map[key] = value;
    }
    end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
}


TEST(PerformanceHashMapTest, DeleteTest) {
    std::cout << "DeleteTest\n";
    mtl::hash_map<int, int> MTL_MAP(5e6);
    std::unordered_map<int, int> STD_Map(5e6);
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 1e6; ++i) {
        auto key = rand(), value = rand();
        MTL_MAP.Insert(key, value);
        if (i % 10 == 0) {
            key = rand();
            MTL_MAP.Delete(key);
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 1e6; ++i) {
        auto key = rand(), value = rand();
        STD_Map[key] = value;
        if (i % 10 == 0) {
            key = rand();
            STD_Map.erase(key);
        }
    }
    end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
}

TEST(FunctionalHashMapTest, BigInsertTest) {
    mtl::hash_map<int, int> MTL_Map(5e6);
    std::unordered_map<int, int> STD_Map(5e6);
    for (int i = 0; i < 1e6; ++i) {
        auto key = rand(), value = rand();
        STD_Map[key] = value;
        MTL_Map.Insert(key, value);
    }
    for (auto j:STD_Map) {
        ASSERT_EQ(j.second, MTL_Map[j.first]);
    }
    for (auto j:MTL_Map) {
        ASSERT_EQ(j.second, STD_Map[j.first]);
    }
}

TEST(FunctionalHashMapTest, BigContainsTest) {
    mtl::hash_map<int, int> MTL_MAP(5e6);
    std::unordered_map<int, int> STD_Map(5e6);
    for (int i = 0; i < 1e6; ++i) {
        auto key = rand(), value = rand();
        STD_Map[key] = value;
        MTL_MAP.Insert(key, value);
    }
    for (auto j:STD_Map) {
        ASSERT_TRUE(MTL_MAP.Contains(j.first));
    }
}

TEST(FunctionalHashMapTest, BigDeleteTest) {
    mtl::hash_map<int, int> MTL_MAP(5e6);
    std::unordered_map<int, int> STD_Map(5e6);
    for (int i = 0; i < 1e6; ++i) {
        auto key = rand(), value = rand();
        STD_Map[key] = value;
        MTL_MAP.Insert(key, value);
        if (i % 5 == 0) {
            key = rand();
            STD_Map.erase(key);
            MTL_MAP.Delete(key);
        }
    }
    for (auto j:STD_Map) {
        ASSERT_EQ(j.second, MTL_MAP[j.first]);
    }
}

TEST(FunctionalHashMapTest, SimpleOperationsTest) {
    mtl::hash_map<std::string, int> MTL_Map;
    MTL_Map.Insert("hello", 20);
    MTL_Map.Insert("world", 45);
    MTL_Map.Insert("abc", 200);
    MTL_Map.Insert("a", 8);
    MTL_Map.Insert("b", 8);
    MTL_Map.Insert("c", 8);
    ASSERT_TRUE(MTL_Map.Contains("hello"));
    ASSERT_FALSE(MTL_Map.Contains("wrold"));
    MTL_Map.Insert("hello", 28);
    ASSERT_EQ(MTL_Map["hello"], 28);
    MTL_Map.Delete("hello");
    ASSERT_FALSE(MTL_Map.Contains("hello"));
}

TEST(FunctionalHashSetTest, BigContainsTest) {
    mtl::hash_set<std::string> MTL_Set(5e6);
    std::unordered_set<std::string> STD_Set(5e6);
    for (int i = 0; i < 1e6; ++i) {
        std::string value;
        for (int i = 0; i < rand() % 50; ++i) {
            value += (char) (rand() % CHAR_MAX);
        }
        STD_Set.insert(value);
        MTL_Set.Insert(value);
    }
    for (auto j:STD_Set) {
        ASSERT_TRUE(MTL_Set.Contains(j));
    }
}

TEST(FunctionalHashSetTest, BigDeleteTest) {
    mtl::hash_set<int> MTL_Set(5e6);
    std::unordered_set<int> STD_Set(5e6);
    for (int i = 0; i < 1e6; ++i) {
        auto value = rand();
        STD_Set.insert(value);
        MTL_Set.Insert(value);
        if (i % 5 == 0) {
            value = rand();
            STD_Set.erase(value);
            MTL_Set.Delete(value);
        }
    }
    for (auto j:STD_Set) {
        //ASSERT_TRUE(MTL_Set.Contains(j));
    }
    for (auto j:MTL_Set) {
        ASSERT_TRUE(STD_Set.contains(j));
    }
}

TEST(FunctionalHashSetTest, SimpleOperationsTest) {
    mtl::hash_set<std::string> MTL_Set;
    MTL_Set.Insert("hello");
    MTL_Set.Insert("world");
    MTL_Set.Insert("abc");
    MTL_Set.Insert("a");
    MTL_Set.Insert("b");
    MTL_Set.Insert("c");
    ASSERT_TRUE(MTL_Set.Contains("hello"));
    ASSERT_FALSE(MTL_Set.Contains("wrold"));
    MTL_Set.Insert("hello");
    ASSERT_TRUE(MTL_Set.Contains("hello"));
    MTL_Set.Delete("hello");
    ASSERT_FALSE(MTL_Set.Contains("hello"));
}

TEST(FunctionalHashMultiSetTest, BigContainsTest) {
    mtl::hash_multiset<std::string> MTL_Set(5e6);
    std::unordered_multiset<std::string> STD_Set(5e6);
    for (int i = 0; i < 1e6; ++i) {
        std::string value;
        for (int i = 0; i < rand() % 50; ++i) {
            value += (char) (rand() % CHAR_MAX);
        }
        STD_Set.insert(value);
        MTL_Set.Insert(value);
    }
    for (auto j:STD_Set) {
        ASSERT_TRUE(MTL_Set.Contains(j));
    }
}

TEST(FunctionalHashMultiSetTest, BigDeleteTest) {
    mtl::hash_multiset<int> MTL_Set(5e6);
    std::unordered_multiset<int> STD_Set(5e6);
    for (int i = 0; i < 1e6; ++i) {
        auto value = rand();
        STD_Set.insert(value);
        MTL_Set.Insert(value);
        if (i % 5 == 0) {
            value = rand();
            STD_Set.erase(value);
            MTL_Set.Delete(value);
        }
    }
    for (auto j:MTL_Set) {
        ASSERT_TRUE(STD_Set.contains(j.first));
    }
}

TEST(FunctionalHashMultiSetTest, SimpleOperationsTest) {
    mtl::hash_multiset<std::string> MTL_Set;
    MTL_Set.Insert("hello");
    MTL_Set.Insert("world");
    MTL_Set.Insert("abc");
    MTL_Set.Insert("a");
    MTL_Set.Insert("b");
    MTL_Set.Insert("c");
    ASSERT_TRUE(MTL_Set.Contains("hello"));
    ASSERT_FALSE(MTL_Set.Contains("wrold"));
    MTL_Set.Insert("hello");
    ASSERT_TRUE(MTL_Set.Contains("hello"));
    MTL_Set.Delete("hello");
    ASSERT_FALSE(MTL_Set.Contains("hello"));
}
