//
// Created by alex on 25.12.2021.
//

#include "Grammar.h"

Grammar::Grammar(const std::vector<char>& notTerms, const std::vector<char>& terms,
                 const std::vector<std::pair<char, std::string>>& rulesInput) : lr_parser(*this) {
    symbols.push_back({-1, false});
    symbols.push_back({EPS, true});
    for (auto i:notTerms)
        symbols.push_back({i, false});
    for (auto i:terms)
        symbols.push_back({i, true});
    for (int i = 0; i < symbols.size(); ++i)
        char2SymbolTable[symbols[i].letter] = i;
    rules = std::vector<std::vector<Rule>>(notTerms.size() + SPECIAL_CHAR_NUM);
    rules[0].push_back({0, {char2SymbolTable['S']}, -1});
    rules_src = rulesInput;
    for (int rule = 0; rule < rulesInput.size(); ++rule) {
        std::vector<int> right;
        for (auto c:rulesInput[rule].second)
            if(c != EPS)
                right.push_back(char2SymbolTable[c]);
        rules[char2SymbolTable[rulesInput[rule].first]].push_back(
                {char2SymbolTable[rulesInput[rule].first], right, rule});
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
                if(rule.right.empty()) {
                    first_len1[rule.left].insert(EPS_INDEX);
                    continue;
                }
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
            if (grammar.rules[item.rule.first][item.rule.second].right.size() <= item.dotPos)
                continue;
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
        if (item.dotPos < grammar.rules[item.rule.first][item.rule.second].right.size()
            && grammar.rules[item.rule.first][item.rule.second].right[item.dotPos] == X) {
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
    for (auto& I_set : I_sets) {
        int cur_row = I_set.second;
        for (auto& item:I_set.first) {
            if (item.rule.first == START_RULE_INDEX_FIRST
                && item.rule.second == START_RULE_INDEX_SECOND
                && item.symbol == EPS_INDEX
                && item.dotPos == grammar.rules[item.rule.first][item.rule.second].right.size()) {
                if (canonicalTable[cur_row][EPS_INDEX].type != Error
                    && canonicalTable[cur_row][EPS_INDEX].type != Accept)
                    throw std::runtime_error("Not LR(1)!");
                canonicalTable[cur_row][EPS_INDEX] = {Accept, -1, -1};
            } else if (item.dotPos == grammar.rules[item.rule.first][item.rule.second].right.size()) {
                Cell new_cell({Reduce, item.rule.first, item.rule.second});
                if (canonicalTable[cur_row][item.symbol].type != Error
                    && canonicalTable[cur_row][item.symbol] != new_cell)
                    throw std::runtime_error("Not LR(1)!");
                canonicalTable[cur_row][item.symbol] = new_cell;
            } else {
                int symbol_after_dot = grammar.rules[item.rule.first][item.rule.second].right[item.dotPos];
                if (grammar.symbols[symbol_after_dot].term) {
                    Cell new_cell({Shift, automaton[I_set.second].edge[symbol_after_dot], -1});
                    if (canonicalTable[cur_row][symbol_after_dot].type != Error
                        && canonicalTable[cur_row][symbol_after_dot] != new_cell)
                        throw std::runtime_error("Not LR(1)!");
                    canonicalTable[cur_row][symbol_after_dot] = new_cell;
                }
            }
        }
    }
}

Grammar::LR_parser::LR_parser(const Grammar& grammar_) : grammar(grammar_) {}

void Grammar::LR_parser::build_parser() {
    init_first_len1();
    build_automaton();
    setLR1CanonicalTable();
    built = true;
}

std::vector<std::pair<char, std::string>> Grammar::LR_check(const std::string& str_) {
    if (!lr_parser.built)
        lr_parser.build_parser();
    std::vector<int> str;
    for (auto c:str_)
        str.push_back(char2SymbolTable[c]);
    str.push_back(EPS_INDEX);
    std::vector<std::pair<char, std::string>> rules_seq;
    std::vector<int> stack;
    stack.push_back(0);
    for (int str_pos = 0; str_pos < str.size();) {
        int cur_state = stack.back();
        int cur_token = str[str_pos];
        LR_parser::Cell cell = lr_parser.canonicalTable[cur_state][cur_token];
        if (cell.type == LR_parser::Shift) {
            stack.push_back(cur_token);
            stack.push_back(cell.arg1);
            ++str_pos;
        } else if (cell.type == LR_parser::Reduce) {
            const Rule& rule = rules[cell.arg1][cell.arg2];
            rules_seq.push_back(rules_src[rule.src_pos]);
            for (int deleted = 0; deleted < rule.right.size(); ++deleted) {
                stack.pop_back();
                stack.pop_back();
            }
            stack.push_back(rule.left);
            stack.push_back(lr_parser.automaton[stack[stack.size() - 2]].edge[rule.left]);
        } else if (cell.type == LR_parser::Accept) {
            std::reverse(rules_seq.begin(), rules_seq.end());
            return rules_seq;
        } else if (cell.type == LR_parser::Error) {
            return {};
        }
    }
    return {};
}

bool Grammar::LR_parser::Cell::operator!=(const Grammar::LR_parser::Cell& other) {
    return type != other.type || arg1 != other.arg1 || arg2 != other.arg2;
}
