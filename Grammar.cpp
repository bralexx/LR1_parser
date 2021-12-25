//
// Created by alex on 25.12.2021.
//

#include "Grammar.h"

Grammar::Grammar(const std::vector<char>& notTerms, const std::vector<char>& terms,
                 const std::vector<std::pair<char, std::string>>& rulesInput):lr_parser(*this) {
    symbols.push_back({-1, false});
    symbols.push_back({EPS, true});
    for (auto i:notTerms)
        symbols.push_back({i, false});
    for (auto i:terms)
        symbols.push_back({i, true});
    for (int i = 0; i < symbols.size(); ++i)
        char2SymbolTable[symbols[i].letter] = i;
    rules = std::vector<std::vector<Rule>>(notTerms.size() + 1);
    rules[0].push_back({0, {char2SymbolTable['S']}});
    for (auto i:rulesInput) {
        std::vector<int> right;
        for (auto j:i.second)
            right.push_back(char2SymbolTable[j]);
        rules[char2SymbolTable[i.first]].push_back({char2SymbolTable[i.first], right});
    }
}

bool Grammar::LR_parser::Item::operator<(const Grammar::LR_parser::Item& t2) const {
    if (rule != t2.rule)
        return rule < t2.rule;
    if (symbol != t2.symbol) return symbol < t2.symbol;
    return dotPos < t2.dotPos;
}

void Grammar::LR_parser::init_first_len1() {
    first_len1 = std::vector<std::set<int>>(grammar.symbols.size());
    for (int symbol = 0; symbol < grammar.symbols.size(); ++symbol) {
        if (grammar.symbols[symbol].term)
            first_len1[symbol].insert(symbol);
    }
    bool changed;
    do {
        changed = false;
        for (auto& gr_rules:grammar.rules)
            for (auto& rule:gr_rules) {
                int prev_set_size = first_len1[rule.left].size();
                int right_part_symbol = 0;
                do {
                    for (auto& symbol:first_len1[rule.right[right_part_symbol]])
                        if (symbol != EPS_INDEX) first_len1[rule.left].insert(symbol);
                } while (first_len1[rule.right[right_part_symbol]].find(EPS_INDEX) !=
                         first_len1[rule.right[right_part_symbol]].end() &&
                         ++right_part_symbol < rule.right.size());
                if (right_part_symbol == rule.right.size())
                    first_len1[rule.left].insert(EPS_INDEX);
                if (prev_set_size != first_len1[rule.left].size())
                    changed = true;
            }
    } while (changed);
}

std::set<int> Grammar::LR_parser::first(const std::vector<int>& str) {
    if (str.empty())
        return {EPS_INDEX};
    std::set<int> answer;
    for (int symbol:str) {
        for (auto c:first_len1[symbol])
            answer.insert(c);
        if (first_len1[symbol].find(EPS_INDEX) == first_len1[symbol].end())
            break;
    }
    return answer;
}

Grammar::LR_parser::SetOfItems Grammar::LR_parser::closure(const Grammar::LR_parser::SetOfItems& I) {
    auto closure = I;
    size_t prev_size;
    do {
        prev_size = closure.size();
        for (auto& item:closure) {
            int after_dot = grammar.rules[item.rule.first][item.rule.second].right[item.dotPos];
            if (!grammar.symbols[after_dot].term) {
                for (int rule = 0; rule < grammar.rules[after_dot].size(); ++rule) {
                    std::vector<int> sequence_after_dot;
                    for (int i = item.dotPos + 1;
                         i < grammar.rules[item.rule.first][item.rule.second].right.size(); ++i)
                        sequence_after_dot.push_back(grammar.rules[item.rule.first][item.rule.second].right[i]);
                    sequence_after_dot.push_back(item.symbol);
                    for (auto& symbol:first(sequence_after_dot)) {
                        Item new_item;
                        new_item.rule.first = after_dot;
                        new_item.rule.second = rule;
                        new_item.dotPos = 0;
                        new_item.symbol = symbol;
                        closure.insert(new_item);
                    }
                }
            }
        }
    } while (prev_size != closure.size());
    return closure;
}

Grammar::LR_parser::SetOfItems
Grammar::LR_parser::go_to(const Grammar::LR_parser::SetOfItems& I, int X) {
    SetOfItems answer;
    for (auto& item:I) {
        if (grammar.rules[item.rule.first][item.rule.second].right[item.dotPos] == X) {
            auto new_item = item;
            ++new_item.dotPos;
            answer.insert(new_item);
        }
    }
    return closure(answer);
}

Grammar::LR_parser::AutomatonNode::AutomatonNode(size_t alphabet_size) {
    edge = std::vector<int>(alphabet_size, -1);
}

void Grammar::LR_parser::build_automaton() {
    SetOfItems start_set = {Item({{START_RULE_INDEX_FIRST, START_RULE_INDEX_SECOND}, EPS_INDEX, 0})};
    start_set = closure(start_set);
    automaton.emplace_back(grammar.symbols.size());
    I_sets[start_set] = 0;
    size_t prev_size;
    do {
        prev_size = I_sets.size();
        for (auto it = I_sets.begin(); it != I_sets.end(); ++it) {
            for (int symbol = 0; symbol < grammar.symbols.size(); ++symbol) {
                if (automaton[it->second].edge[symbol] != -1)
                    continue;
                SetOfItems next_set = go_to(it->first, symbol);
                if (next_set.empty())
                    continue;
                auto I_sets_pos = I_sets.find(next_set);
                if (I_sets_pos == I_sets.end()) {
                    automaton.emplace_back(grammar.symbols.size());
                    auto insert_result = I_sets.insert({next_set, automaton.size() - 1});
                    I_sets_pos = insert_result.first;
                }
                automaton[it->second].edge[symbol] = I_sets_pos->second;
            }
        }
    } while (prev_size != I_sets.size());
}

void Grammar::LR_parser::setLR1CanonicalTable() {
    canonicalTable = std::vector<std::vector<Cell>>(I_sets.size(),
                                                    std::vector<Cell>(grammar.symbols.size()));
    for (auto it = I_sets.begin(); it != I_sets.end(); ++it) {
        int cur_row = it->second;
        for (auto& item:it->first) {
            if (item.rule.first == START_RULE_INDEX_FIRST
                && item.rule.second == START_RULE_INDEX_SECOND
                && item.symbol == EPS_INDEX
                && item.dotPos == grammar.rules[item.rule.first][item.rule.second].right.size()) {
                canonicalTable[cur_row][EPS_INDEX] = {Accept,-1,-1};
            } else if(item.dotPos == grammar.rules[item.rule.first][item.rule.second].right.size()) {
                canonicalTable[cur_row][item.symbol] = {Reduce, item.rule.first, item.rule.second};
            } else {
                int symbol_after_dot = grammar.rules[item.rule.first][item.rule.second].right[item.dotPos];
                if(grammar.symbols[symbol_after_dot].term) {
                    canonicalTable[cur_row][symbol_after_dot] =
                            {Shift, automaton[it->second].edge[symbol_after_dot], -1};
                }
            }
        }
    }
}

Grammar::LR_parser::LR_parser(const Grammar& grammar_):grammar(grammar_) {}

void Grammar::LR_parser::build_parser() {
    init_first_len1();
    build_automaton();
    setLR1CanonicalTable();
    built = true;
}

bool Grammar::LR_check(const std::string& str_) {
    if(!lr_parser.built)
        lr_parser.build_parser();
    std::vector<int> str;
    for(auto c:str_)
        str.push_back(char2SymbolTable[c]);
    str.push_back(EPS_INDEX);
    std::vector<int> stack;
    stack.push_back(0);
    for(int str_pos = 0; str_pos < str.size();) {
        int cur_state = stack.back();
        int cur_token = str[str_pos];
        LR_parser::Cell cell = lr_parser.canonicalTable[cur_state][cur_token];
        if(cell.type == LR_parser::Shift) {
            stack.push_back(cur_token);
            stack.push_back(cell.arg1);
            ++str_pos;
        } else if(cell.type == LR_parser::Reduce) {
            const Rule& rule = rules[cell.arg1][cell.arg2];
            for(int deleted = 0; deleted < rule.right.size(); ++deleted) {
                stack.pop_back();
                stack.pop_back();
            }
            stack.push_back(rule.left);
            stack.push_back(lr_parser.automaton[stack[stack.size()-2]].edge[rule.left]);
        } else if(cell.type == LR_parser::Accept) {
            return true;
        } else if(cell.type == LR_parser::Error) {
            throw std::runtime_error("Not LR(1)");
        }
    }
}

