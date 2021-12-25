#include <iostream>
#include "Grammar.h"

int main() {
    int n;
    std::cin >> n;
    std::vector<char> notTerms(n);
    for(int i = 0; i < n; ++i) std::cin >> notTerms[i];
    std::cin >> n;
    std::vector<char> terms(n);
    for(int i = 0; i < n; ++i) std::cin >> terms[i];
    std::cin >> n;
    std::vector<std::pair<char, std::string>> rules(n);
    for(int i = 0; i < n; ++i) std::cin >> rules[i].first >> rules[i].second;

    Grammar g(notTerms, terms, rules);

    std::string w;
    std::cin >> w;
    std::cout << (g.LR_check(w) ? "true" : "false") << std::endl;
    return 0;
}