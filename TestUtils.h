//
// Created by aleks311001 on 12.12.2020.
//

#include <gtest/gtest.h>

std::string listPairsToString(const std::list<NKA::Edge>& container) {
    std::string result = "[";
    for (auto& item: container) {
        result += "(" + item.word + ", " + std::to_string(item.final) + "),  ";
    }
    result += "]";

    return result;
}

testing::AssertionResult equalListsPair(const std::list<NKA::Edge>& left,
                                        const std::list<NKA::Edge>& right) {
    if (left.size() != right.size()) {
        return testing::AssertionFailure() << listPairsToString(left) << " not equal " << listPairsToString(right);
    }

    for (auto& left_item: left) {
        bool found = false;
        for (auto& right_item: right) {
            if (right_item.word == left_item.word && right_item.final == left_item.final) {
                found = true;
                break;
            }
        }
        if (!found) {
            return testing::AssertionFailure() << listPairsToString(left) << " not equal " << listPairsToString(right);
        }
    }

    return testing::AssertionSuccess() << listPairsToString(left) << " equal " << listPairsToString(right);
}

template<typename T>
std::string setToString(const std::set<T>& set) {
    std::string result = "{";
    for (auto& item: set) {
        result += std::to_string(item) + ", ";
    }
    result += "}";

    return result;
}

template<typename T>
testing::AssertionResult equalSets(const std::set<T>& left, const std::set<T>& right) {
    if (left.size() != right.size()) {
        return testing::AssertionFailure() << setToString(left) << " not equal " << setToString(right);
    }

    for (auto& itemLeft: left) {
        bool found = false;
        for (auto& itemRight: right) {
            if (itemLeft == itemRight) {
                found = true;
                break;
            }
        }
        if (!found) {
            return testing::AssertionFailure() << setToString(left) << " not equal " << setToString(right);
        }
    }

    return testing::AssertionSuccess() << setToString(left) << " equal " << setToString(right);
}
