#include <gtest/gtest.h>
#include "../Grammar.h"
#include "test_functions.h"

TEST(AllTests, test2) {
    std::vector<char> notTerm = {'S', 'A'};
    std::vector<char> term = {'(', ')', 'b'};
    for (int i = 0; i < 10; ++i) term.push_back(i + '0');
    std::vector<std::pair<char, std::string>> rules = {{'S', "Ab"},
                                                       {'A', "(bA)"},
                                                       {'A', "(A)"},
                                                       {'A', "~"}
    };
    Grammar g(notTerm, term, rules);
    EXPECT_EQ(try_rule_sequence(g.LR_check("(b(b))b")), "(b(b))b");
    EXPECT_EQ(try_rule_sequence(g.LR_check("(b(b(b(b))))b")), "(b(b(b(b))))b");
    std::list<char> list;
    list.push_back('b');
    auto pos = list.begin();
    for(int len = 0; len < 1000; ++len) {
        std::string s = list2str(list);
        EXPECT_EQ(try_rule_sequence(g.LR_check(s)), s);
        int rule = rand()%2;
        if(rule == 0) {
            list.insert(pos, '(');
            list.insert(pos, 'b');
            list.insert(pos, ')');
            --pos;
        } else {
            list.insert(pos, '(');
            list.insert(pos, ')');
            --pos;
        }
    }
}