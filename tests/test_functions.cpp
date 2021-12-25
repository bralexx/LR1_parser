#include "test_functions.h"
#include <algorithm>

std::string list2str(const std::list<char>& list) {
    std::string answer;
    for (auto it = list.begin(); it != list.end(); ++it)
        answer.push_back(*it);
    return answer;
}

std::string try_rule_sequence(const std::vector<std::pair<char, std::string>>& rules) {
    std::list<char> str;
    str.push_back('S');
    auto pos = str.begin();
    for (auto& rule:rules) {
        while (pos != str.end() && *pos != rule.first)
            ++pos;
        if (*pos != rule.first)
            break;
        ++pos;
        for (char c:rule.second) {
            if (c == '~')
                break;
            str.insert(pos, c);
            --pos;
        }
        auto pos_copy = pos;
        --pos_copy;
        str.erase(pos_copy);
    }
    std::reverse(str.begin(), str.end());
    return list2str(str);
}