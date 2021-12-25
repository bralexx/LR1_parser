#include <gtest/gtest.h>
#include "../Grammar.h"
#include "test_functions.h"
#include <string>

TEST(AllTests, test3) {
    std::vector<char> notTerm = {'S', 'B', 'C'};
    std::vector<char> term = {'a', 'b', 'c'};
    std::vector<std::pair<char, std::string>> rules = {{'S', "Aa"},
                                                       {'S', "Cc"},
                                                       {'S', "Bb"},
                                                       {'B', "a"},
                                                       {'B', "b"},
                                                       {'B', "c"},
                                                       {'C', "a"},
                                                       {'C', "b"},
                                                       {'C', "c"},
                                                       {'A', "a"},
                                                       {'A', "b"},
                                                       {'A', "c"}
    };
    Grammar g(notTerm, term, rules);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            std::string s;
            s.push_back('a' + i);
            s.push_back('a' + j);
            EXPECT_EQ(try_rule_sequence(g.LR_check(s)), s);
        }
}