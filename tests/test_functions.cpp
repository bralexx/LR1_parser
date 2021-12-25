#include "test_functions.h"

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
            str.insert(pos, c);
            --pos;
        }
        auto pos_copy = pos;
        --pos_copy;
        str.erase(pos_copy);
    }
    std::string answer;
    for (auto it = str.rbegin(); it != str.rend(); ++it)
        answer.push_back(*it);
    return answer;
}