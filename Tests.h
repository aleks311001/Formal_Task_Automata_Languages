//
// Created by aleks311001 on 01.11.2020.
//

#ifndef FORMALPRACTICE1__TESTS_H_
#define FORMALPRACTICE1__TESTS_H_

#include <gtest/gtest.h>
#include "Builder.h"
#include "task14.h"
#include "TestUtils.h"

class TestNKA: public ::testing::Test {
protected:
    BuilderNKA builderNKA;

    testing::AssertionResult inTransition(long long left, const std::string& word, long long right) {
        std::string transit = "((" + std::to_string(left) + ", " + word + ") -> " + std::to_string(right) + ")";

        NKA& nka = builderNKA.getNKA();

        if (nka.transitions_.contains(left) &&
            nka.transitions_[left].contains(word) &&
            nka.transitions_[left][word].contains(right)) {
            return testing::AssertionSuccess() << transit << " in transitions";
        }
        return testing::AssertionFailure() << transit << " not in transitions";
    }

    void testBuilderMakeDefault() {
        builderNKA.makeDefaultNKA();
        NKA& nka = builderNKA.getNKA();

        EXPECT_TRUE(equalSets(nka.alphabet_, {'a', 'b', 'c'}));
        EXPECT_TRUE(equalSets(nka.configurations_, {0}));
        ASSERT_EQ(nka.q0_, 0);
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {}));
        ASSERT_TRUE(nka.transitions_.empty());
    }

    void testBuilderMake() {
        NKA& nka = builderNKA.getNKA();

        ASSERT_THROW(builderNKA.makeNKA(27, 100), std::range_error);

        builderNKA.makeNKA(5, 4);

        EXPECT_TRUE(equalSets(nka.alphabet_, {'a', 'b', 'c', 'd', 'e'}));
        EXPECT_TRUE(equalSets(nka.configurations_, {0, 1, 2, 3}));
        ASSERT_EQ(nka.q0_, 0);
        EXPECT_TRUE(nka.acceptingConfigurations_.empty());
        ASSERT_TRUE(nka.transitions_.empty());
    }

    void testBuilderAdds() {
        NKA& nka = builderNKA.getNKA();

        builderNKA.addConfigurations({2, 3, 4})
                  .addAlphabet({'a', 'b', 'd'})
                  .setQ0(2)
                  .addAcceptingConfigurations({4, 3});
        EXPECT_TRUE(equalSets(nka.alphabet_, {'a', 'b', 'd'}));
        EXPECT_TRUE(equalSets(nka.configurations_, {2, 3, 4}));
        ASSERT_EQ(nka.q0_, 2);
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {4, 3}));
        ASSERT_TRUE(nka.transitions_.empty());

        builderNKA.addConfigurations({0, 1, 3})
                  .addAlphabet({'c', 'd'})
                  .addAcceptingConfigurations({1})
                  .setQ0(0);
        EXPECT_TRUE(equalSets(nka.alphabet_, {'a', 'b', 'c', 'd'}));
        EXPECT_TRUE(equalSets(nka.configurations_, {0, 1, 2, 3, 4}));
        ASSERT_EQ(nka.q0_, 0);
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {1, 3, 4}));
        ASSERT_TRUE(nka.transitions_.empty());

        long long conf = nka.addNewConfiguration_(6);
        ASSERT_TRUE(nka.configurations_.contains(conf));
        ASSERT_GE(conf, 6);
    }

    void testAddTransitions() {
        builderNKA.makeNKA(3, 5);
        NKA& nka = builderNKA.getNKA();

        ASSERT_TRUE(nka.transitions_.empty());
        builderNKA.addTransition(1, "ab", 2)
                  .addTransition(1, 'c', 0);
        EXPECT_TRUE(inTransition(1, "c", 0));
        EXPECT_TRUE(inTransition(1, "ab", 2));

        ASSERT_THROW(builderNKA.addTransition(4, "ab", 5), std::invalid_argument);
        ASSERT_FALSE(nka.transitions_.contains(4));
        ASSERT_THROW(builderNKA.addTransition(2, 'a', 7), std::invalid_argument);
        ASSERT_FALSE(nka.transitions_.contains(2));

        nka.addTransition_(1, "ab", 0);
        ASSERT_FALSE(nka.transitions_.contains(0));
        ASSERT_FALSE(nka.transitions_[1].contains("b"));
        builderNKA.addConfigurations({5});
        nka.addTransition_(0, EPS, 5);
        EXPECT_TRUE(inTransition(1, "ab", 0));
        EXPECT_TRUE(inTransition(0, EPS, 5));
        EXPECT_FALSE(inTransition(0, "b", 5));
    }

    void testReplaceMultiSymbolEdges() {
        builderNKA.makeDefaultNKA()
                  .addConfigurations({2, 3})
                  .addAcceptingConfigurations({1});
        NKA& nka = builderNKA.getNKA();

        builderNKA.addTransition(0, "abca", 2);
        nka.replaceMultiSymbolsEdges();

        EXPECT_TRUE(inTransition(0, "a", 1));
        EXPECT_TRUE(inTransition(1, "b", 4));
        EXPECT_TRUE(inTransition(4, "c", 5));
        EXPECT_TRUE(inTransition(5, "a", 2));
        EXPECT_EQ(nka.transitions_.size(), 4);
    }

    void testFindAllWaysOnEpsEdges() {
        builderNKA.makeNKA(3, 6).addAcceptingConfigurations({3});
        NKA& nka = builderNKA.getNKA();

        builderNKA.addTransition(1, EPS, 2);
        builderNKA.addTransition(2, EPS, 3);

        std::set<long long> wasIn;
        std::list<NKA::Edge> endWayConfigurations;
        bool hasAcceptingConf = false;

        nka.findAllWaysOnEpsEdges_(0, wasIn, endWayConfigurations, hasAcceptingConf);
        ASSERT_TRUE(endWayConfigurations.empty());
        ASSERT_FALSE(hasAcceptingConf);

        builderNKA.addTransition(2, EPS, 4)
                  .addTransition(3, EPS, 4)
                  .addTransition(2, "a", 0)
                  .addTransition(4, "c", 5)
                  .addTransition(0, EPS, 1)
                  .addTransition(4, "b", 3);

        wasIn.clear();
        nka.findAllWaysOnEpsEdges_(0, wasIn, endWayConfigurations, hasAcceptingConf);
        ASSERT_TRUE(hasAcceptingConf);
        std::list<NKA::Edge> rightEndWayConf = {{"a", 0}, {"c", 5}, {"b", 3}};
        EXPECT_TRUE(equalListsPair(endWayConfigurations, rightEndWayConf));
    }

    void testRemoveEpsilonTransitions() {
        builderNKA.makeNKA(3, 6).addAcceptingConfigurations({3});

        builderNKA.addTransition(1, EPS, 2)
                  .addTransition(2, EPS, 3)
                  .addTransition(2, EPS, 4)
                  .addTransition(3, EPS, 4)
                  .addTransition(2, "a", 0)
                  .addTransition(4, "c", 5)
                  .addTransition(0, EPS, 1)
                  .addTransition(4, "b", 3);

        NKA& nka = builderNKA.getNKA();
        std::set<long long> wasIn;

        wasIn.clear();
        nka.addTransitionsInEpsWays_(0, wasIn);
        EXPECT_TRUE(inTransition(0, "a", 0));
        EXPECT_TRUE(inTransition(0, "c", 5));
        EXPECT_TRUE(inTransition(0, "b", 3));
        EXPECT_TRUE(inTransition(3, "c", 5));

        nka.removeEpsilonTransitions_();
        EXPECT_FALSE(inTransition(0, EPS, 1));
        EXPECT_FALSE(inTransition(2, EPS, 4));
        EXPECT_FALSE(inTransition(3, EPS, 4));
        EXPECT_FALSE(inTransition(1, EPS, 2));
        EXPECT_FALSE(inTransition(2, EPS, 3));
    }

    void testChangeEpsTransitions() {
        builderNKA.makeNKA(3, 6)
                  .addAcceptingConfigurations({3})
                  .addTransition(1, EPS, 2)
                  .addTransition(2, EPS, 3)
                  .addTransition(2, EPS, 4)
                  .addTransition(3, EPS, 4)
                  .addTransition(2, "a", 0)
                  .addTransition(4, "c", 5)
                  .addTransition(0, EPS, 1)
                  .addTransition(4, "b", 3);

        NKA& nka = builderNKA.getNKA();

        nka.changeEpsTransitions();
        EXPECT_TRUE(inTransition(0, "a", 0));
        EXPECT_TRUE(inTransition(0, "c", 5));
        EXPECT_TRUE(inTransition(0, "b", 3));
        EXPECT_TRUE(inTransition(3, "c", 5));
        EXPECT_FALSE(inTransition(0, EPS, 1));
        EXPECT_FALSE(inTransition(2, EPS, 4));
        EXPECT_FALSE(inTransition(3, EPS, 4));
        EXPECT_FALSE(inTransition(1, EPS, 2));
        EXPECT_FALSE(inTransition(2, EPS, 3));
    }

    void testDelUselessConfigurations() {
        builderNKA.makeNKA(3, 8).addAcceptingConfigurations({1, 3, 6, 7});
        NKA& nka = builderNKA.getNKA();

        builderNKA.addTransition(0, "a", 1)
                  .addTransition(0, EPS, 2)
                  .addTransition(2, "ab", 5)
                  .addTransition(2, "ccc", 6)
                  .addTransition(3, "a", 7)
                  .addTransition(7, "b", 4)
                  .addTransition(7, "a", 0)
                  .addTransition(5, "a", 0);

        auto usefulConfs = nka.getSetUsefulConfigurations_();
        ASSERT_EQ(usefulConfs.size(), 5);
        ASSERT_TRUE(usefulConfs.contains(0));
        ASSERT_TRUE(usefulConfs.contains(1));
        ASSERT_TRUE(usefulConfs.contains(2));
        ASSERT_TRUE(usefulConfs.contains(5));
        ASSERT_TRUE(usefulConfs.contains(6));

        nka.delUselessConfigurations();
        ASSERT_EQ(nka.configurations_.size(), 5);
        ASSERT_FALSE(nka.configurations_.contains(3));
        ASSERT_FALSE(nka.configurations_.contains(7));
        ASSERT_FALSE(nka.configurations_.contains(4));
        ASSERT_FALSE(nka.acceptingConfigurations_.contains(7));
        ASSERT_FALSE(nka.acceptingConfigurations_.contains(3));
    }

    void testGetMapConfigurationToNumber() {
        builderNKA.addAlphabet({'a', 'b', 'c'})
                  .addConfigurations({1, 3, 5, 7})
                  .addAcceptingConfigurations({1, 3})
                  .setQ0(3);
        NKA& nka = builderNKA.getNKA();

        builderNKA.addTransition(3, "a", 5)
                  .addTransition(1, "bc", 5);

        auto map = nka.getMapConfigurationToNumber_();
        ASSERT_EQ(map.size(), 4);
        for (auto& pair: map) {
            ASSERT_TRUE(nka.configurations_.contains(pair.first));
            ASSERT_LT(pair.second, map.size());
        }
    }

    void testMakeConfigurationFromOthers() {
        std::unordered_map<long long, size_t> indexes;
        indexes.emplace(1, 2);
        indexes.emplace(2, 1);
        indexes.emplace(3, 4);
        indexes.emplace(4, 6);
        indexes.emplace(5, 3);
        indexes.emplace(8, 5);

        long long conf = NKA::makeConfigurationFromOthers_({1, 3, 8}, indexes);
        ASSERT_EQ(conf, 0b110100);
        conf = NKA::makeConfigurationFromOthers_({2, 4, 8, 1}, indexes);
        ASSERT_EQ(conf, 0b1100110);
        conf = NKA::makeConfigurationFromOthers_({5, 4, 3, 2, 1}, indexes);
        ASSERT_EQ(conf, 0b1011110);
    }

    void testCheckSetConfigsOnAccepting() {
        builderNKA.addAlphabet({'a', 'b', 'c'})
                  .addConfigurations({1, 3, 5, 7})
                  .setQ0(3)
                  .addAcceptingConfigurations({1, 3});
        NKA& nka = builderNKA.getNKA();

        ASSERT_TRUE(nka.checkSetConfigsOnAccepting_({1, 2, 0, 6}));
        ASSERT_TRUE(nka.checkSetConfigsOnAccepting_({1, 3}));
        ASSERT_FALSE(nka.checkSetConfigsOnAccepting_({0, 2, 4}));
        ASSERT_TRUE(nka.checkSetConfigsOnAccepting_({3, 4, 5, 6, 7}));
        ASSERT_FALSE(nka.checkSetConfigsOnAccepting_({10, 7, 4, 5}));
    }

    void testCheckNumberOfConfigurationsNoThrow() {
        builderNKA.makeNKA(3, std::numeric_limits<long long>::digits)
                  .addAcceptingConfigurations({1});
        NKA& nka = builderNKA.getNKA();

        for (int i = 0; i < 10; ++i) {
            builderNKA.addTransition(i, "a", i + 1);
        }
        ASSERT_NO_THROW(nka.checkNumberOfConfigurations_());
    }
    void testCheckNumberOfConfigurationsThrow() {
        builderNKA.makeNKA(3, std::numeric_limits<long long>::digits)
                  .addAcceptingConfigurations({1});
        NKA& nka = builderNKA.getNKA();

        for (int i = 0; i < std::numeric_limits<long long>::digits - 1; ++i) {
            builderNKA.addTransition(i, "a", i + 1);
        }
        ASSERT_THROW(nka.checkNumberOfConfigurations_(), std::range_error);
    }

    void testFindAllTransitionsFromSetConfigurations() {
        builderNKA.makeNKA(3, 7)
                  .addAcceptingConfigurations({1, 3});
        NKA& nka = builderNKA.getNKA();

        builderNKA.addTransition(0, "a", 2)
                  .addTransition(3, "b", 5)
                  .addTransition(3, "c", 1)
                  .addTransition(1, "a", 5)
                  .addTransition(2, "cc", 6)
                  .addTransition(5, "bc", 3)
                  .addTransition(5, "b", 4);

        auto transitions = nka.findAllTransitionsFromSetConfigurations_({2, 3, 5});
        EXPECT_TRUE(equalSets(transitions["b"], {4, 5}));
        EXPECT_TRUE(equalSets(transitions["c"], {1}));
        EXPECT_TRUE(equalSets(transitions["cc"], {6}));
        EXPECT_TRUE(equalSets(transitions["bc"], {3}));
        ASSERT_EQ(transitions.size(), 4);
    }

    void testMakeExplicitWays() {
        builderNKA.makeNKA(3, 3)
                  .addAcceptingConfigurations({0});
        NKA& nka = builderNKA.getNKA();

        builderNKA.addTransition(0, "a", 1)
                  .addTransition(0, "a", 2)
                  .addTransition(0, "b", 2)
                  .addTransition(0, "b", 1)
                  .addTransition(1, "a", 1)
                  .addTransition(1, "a", 2)
                  .addTransition(1, "b", 2)
                  .addTransition(1, "c", 0)
                  .addTransition(2, "c", 2)
                  .addTransition(2, "c", 0);

        nka.makeExplicitWays();

        ASSERT_EQ(nka.q0_, 1);
        EXPECT_TRUE(equalSets(nka.configurations_, {0b001, 0b100, 0b101, 0b110}));
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {0b001, 0b101}));

        ASSERT_EQ(nka.transitions_.size(), 4);
        ASSERT_EQ(nka.transitions_[1].size(), 2);
        ASSERT_EQ(nka.transitions_[4].size(), 1);
        EXPECT_TRUE(equalSets(nka.transitions_[1]["a"], {6}));
        EXPECT_TRUE(equalSets(nka.transitions_[1]["b"], {6}));
        EXPECT_TRUE(equalSets(nka.transitions_[4]["c"], {5}));
        EXPECT_TRUE(equalSets(nka.transitions_[5]["a"], {6}));
        EXPECT_TRUE(equalSets(nka.transitions_[5]["b"], {6}));
        EXPECT_TRUE(equalSets(nka.transitions_[5]["c"], {5}));
        EXPECT_TRUE(equalSets(nka.transitions_[6]["a"], {6}));
        EXPECT_TRUE(equalSets(nka.transitions_[6]["b"], {4}));
        EXPECT_TRUE(equalSets(nka.transitions_[6]["c"], {5}));
    }

    void testMakeDKA() {
        builderNKA.makeNKA(3, 4)
                  .addAcceptingConfigurations({1});
        NKA& nka = builderNKA.getNKA();

        builderNKA.addTransition(0, "b", 0)
                  .addTransition(0, EPS, 1)
                  .addTransition(0, "ca", 2)
                  .addTransition(2, EPS, 1)
                  .addTransition(1, "a", 1)
                  .addTransition(1, "a", 2)
                  .addTransition(1, "a", 3)
                  .addTransition(1, "c", 3)
                  .addTransition(3, "bc", 2);

        nka.makeDKA();

        ASSERT_EQ(nka.q0_, 1);
        EXPECT_TRUE(equalSets(nka.configurations_, {0b000001, 0b000100, 0b001000, 0b001110, 0b011000, 0b100000}));
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {1, 4, 14}));
        EXPECT_TRUE(equalSets(nka.transitions_[1]["a"], {14}));
        EXPECT_TRUE(equalSets(nka.transitions_[1]["b"], {1}));
        EXPECT_TRUE(equalSets(nka.transitions_[1]["c"], {24}));
        EXPECT_TRUE(equalSets(nka.transitions_[4]["a"], {14}));
        EXPECT_TRUE(equalSets(nka.transitions_[4]["b"], {}));
        EXPECT_TRUE(equalSets(nka.transitions_[4]["c"], {8}));
        EXPECT_TRUE(equalSets(nka.transitions_[8]["a"], {}));
        EXPECT_TRUE(equalSets(nka.transitions_[8]["b"], {32}));
        EXPECT_TRUE(equalSets(nka.transitions_[8]["c"], {}));
        EXPECT_TRUE(equalSets(nka.transitions_[14]["a"], {14}));
        EXPECT_TRUE(equalSets(nka.transitions_[14]["b"], {32}));
        EXPECT_TRUE(equalSets(nka.transitions_[14]["c"], {8}));
        EXPECT_TRUE(equalSets(nka.transitions_[24]["a"], {4}));
        EXPECT_TRUE(equalSets(nka.transitions_[24]["b"], {32}));
        EXPECT_TRUE(equalSets(nka.transitions_[24]["c"], {}));
        EXPECT_TRUE(equalSets(nka.transitions_[32]["a"], {}));
        EXPECT_TRUE(equalSets(nka.transitions_[32]["b"], {}));
        EXPECT_TRUE(equalSets(nka.transitions_[32]["c"], {4}));
    }

    void testMakeOneAcceptingConfiguration() {
        builderNKA.makeNKA(3, 3)
                  .addAcceptingConfigurations({1, 2});
        NKA& nka = builderNKA.getNKA();

        builderNKA.addTransition(0, "ab", 1)
                  .addTransition(0, EPS, 2)
                  .addTransition(1, "c", 1);

        nka.makeOneAcceptingConfiguration_();

        EXPECT_TRUE(equalSets(nka.transitions_[1][EPS], {3}));
        EXPECT_TRUE(equalSets(nka.transitions_[2][EPS], {3}));
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {3}));
        EXPECT_TRUE(equalSets(nka.configurations_, {0, 1, 2, 3}));
    }

    void testMakeReverse() {
        builderNKA.makeNKA(3, 3)
                  .addAcceptingConfigurations({1, 2});
        NKA& nka = builderNKA.getNKA();

        builderNKA.addTransition(0, "c", 0)
                  .addTransition(0, "ab", 1)
                  .addTransition(0, "c", 2)
                  .addTransition(1, "bc", 1)
                  .addTransition(2, "bc", 1)
                  .addTransition(2, "bc", 0);

        nka.makeReverse();
        ASSERT_EQ(nka.q0_, 3);
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {0}));
        EXPECT_TRUE(equalSets(nka.configurations_, {0, 1, 2, 3}));
        EXPECT_TRUE(equalSets(nka.transitions_[3][EPS], {1, 2}));
        EXPECT_TRUE(equalSets(nka.transitions_[2]["c"], {0}));
        EXPECT_TRUE(equalSets(nka.transitions_[1]["cb"], {1, 2}));
        EXPECT_TRUE(equalSets(nka.transitions_[1]["ba"], {0}));
        EXPECT_TRUE(equalSets(nka.transitions_[0]["cb"], {2}));
        EXPECT_TRUE(equalSets(nka.transitions_[0]["c"], {0}));
    }

    NKA makeNKAforTestUnion() {
        builderNKA.makeDefaultNKA()
                  .addConfigurations({0, 1, 3, 5})
                  .addAcceptingConfigurations({0, 3});

        builderNKA.addTransition(0, "a", 1)
                  .addTransition(1, "b", 3)
                  .addTransition(3, "a", 5)
                  .addTransition(5, "b", 0);

        BuilderNKA builderAddedNKA;
        builderAddedNKA.makeNKA(3, 3)
                       .addAcceptingConfigurations({1, 2});

        builderAddedNKA.addTransition(0, "c", 1)
                       .addTransition(1, "c", 2)
                       .addTransition(2, "c", 0);

        return builderAddedNKA.getNKA();
    }

    void testUnionNKA() {
        auto addedNKA = makeNKAforTestUnion();

        NKA& nka = builderNKA.getNKA();
        long long start2 = nka.unionNKA(addedNKA);

        ASSERT_EQ(nka.q0_, 0);
        ASSERT_EQ(start2, 2);
        EXPECT_TRUE(equalSets(nka.configurations_, {0, 1, 2, 3, 4, 5, 6}));
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {0, 3, 4, 6}));

        EXPECT_TRUE(equalSets(nka.transitions_[0]["a"], {1}));
        EXPECT_TRUE(equalSets(nka.transitions_[1]["b"], {3}));
        EXPECT_TRUE(equalSets(nka.transitions_[3]["a"], {5}));
        EXPECT_TRUE(equalSets(nka.transitions_[5]["b"], {0}));

        EXPECT_TRUE(equalSets(nka.transitions_[2]["c"], {4}));
        EXPECT_TRUE(equalSets(nka.transitions_[4]["c"], {6}));
        EXPECT_TRUE(equalSets(nka.transitions_[6]["c"], {2}));
    }

    void testMulNKA() {
        auto mulledNKA = makeNKAforTestUnion();
        NKA& nka = builderNKA.getNKA();
        nka.mul(mulledNKA);

        ASSERT_EQ(nka.q0_, 0);
        EXPECT_TRUE(equalSets(nka.configurations_, {0, 1, 2, 3, 4, 5, 6, 7}));
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {6, 7}));

        EXPECT_TRUE(equalSets(nka.transitions_[2][EPS], {4}));
        EXPECT_TRUE(equalSets(nka.transitions_[0][EPS], {2}));
        EXPECT_TRUE(equalSets(nka.transitions_[3][EPS], {2}));

        EXPECT_TRUE(equalSets(nka.transitions_[0]["a"], {1}));
        EXPECT_TRUE(equalSets(nka.transitions_[1]["b"], {3}));
        EXPECT_TRUE(equalSets(nka.transitions_[3]["a"], {5}));
        EXPECT_TRUE(equalSets(nka.transitions_[5]["b"], {0}));

        EXPECT_TRUE(equalSets(nka.transitions_[4]["c"], {6}));
        EXPECT_TRUE(equalSets(nka.transitions_[6]["c"], {7}));
        EXPECT_TRUE(equalSets(nka.transitions_[7]["c"], {4}));
    }

    void testAddNKA() {
        auto addedNKA = makeNKAforTestUnion();
        NKA& nka = builderNKA.getNKA();
        nka.add(addedNKA);

        ASSERT_EQ(nka.q0_, 7);
        EXPECT_TRUE(equalSets(nka.configurations_, {0, 1, 2, 3, 4, 5, 6, 7}));
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {0, 3, 4, 6}));

        EXPECT_TRUE(equalSets(nka.transitions_[7][EPS], {0, 2}));

        EXPECT_TRUE(equalSets(nka.transitions_[0]["a"], {1}));
        EXPECT_TRUE(equalSets(nka.transitions_[1]["b"], {3}));
        EXPECT_TRUE(equalSets(nka.transitions_[3]["a"], {5}));
        EXPECT_TRUE(equalSets(nka.transitions_[5]["b"], {0}));

        EXPECT_TRUE(equalSets(nka.transitions_[2]["c"], {4}));
        EXPECT_TRUE(equalSets(nka.transitions_[4]["c"], {6}));
        EXPECT_TRUE(equalSets(nka.transitions_[6]["c"], {2}));
    }

    void testStarNKA() {
        builderNKA.makeNKA(3, 3)
                  .addAcceptingConfigurations({1});
        NKA& nka = builderNKA.getNKA();

        builderNKA.addTransition(0, "a", 1)
                  .addTransition(1, "b", 2)
                  .addTransition(2, "c", 0);

        nka.star();

        ASSERT_EQ(nka.q0_, 3);
        EXPECT_TRUE(equalSets(nka.configurations_, {0, 1, 2, 3}));
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {3}));

        EXPECT_TRUE(equalSets(nka.transitions_[3][EPS], {0}));
        EXPECT_TRUE(equalSets(nka.transitions_[0]["a"], {1}));
        EXPECT_TRUE(equalSets(nka.transitions_[1]["b"], {2}));
        EXPECT_TRUE(equalSets(nka.transitions_[2]["c"], {0}));
        EXPECT_TRUE(equalSets(nka.transitions_[1][EPS], {3}));
    }

    void testLenMaxPrefOf() {
        builderNKA.makeNKA(3, 3)
                  .addAcceptingConfigurations({2});
        NKA& nka = builderNKA.getNKA();

        builderNKA.addTransition(0, "ab", 0)
                  .addTransition(0, "aa", 0)
                  .addTransition(0, EPS, 1)
                  .addTransition(1, "c", 1)
                  .addTransition(1, "c", 2)
                  .addTransition(2, "a", 2);

        NKA nka2 = nka;
        ASSERT_EQ(nka.lenMaxPrefOf("ababaa"), 0);
        ASSERT_EQ(nka.lenMaxPrefOf("abaaccababccc"), 7);
        ASSERT_EQ(nka2.lenMaxPrefOf("cccbabc"), 3);
        ASSERT_EQ(nka2.lenMaxPrefOf(EPS), 0);
    }

    void testMakeNKA() {
        NKA& nka = builderNKA.getNKA();

        ASSERT_THROW(makeNKA("ab+."), std::range_error);
        ASSERT_THROW(makeNKA("ab.+"), std::range_error);
        ASSERT_THROW(makeNKA("abc*."), std::range_error);

        nka = makeNKA("abc*+.");

        ASSERT_EQ(nka.q0_, 0);
        EXPECT_TRUE(equalSets(nka.configurations_, {0, 1, 2, 3, 4, 5, 6, 7}));
        EXPECT_TRUE(equalSets(nka.acceptingConfigurations_, {3, 6}));

        EXPECT_TRUE(equalSets(nka.transitions_[0]["a"], {1}));
        EXPECT_TRUE(equalSets(nka.transitions_[1][EPS], {7}));
        EXPECT_TRUE(equalSets(nka.transitions_[7][EPS], {2, 6}));
        EXPECT_TRUE(equalSets(nka.transitions_[2]["b"], {3}));
        EXPECT_TRUE(equalSets(nka.transitions_[6][EPS], {4}));
        EXPECT_TRUE(equalSets(nka.transitions_[4]["c"], {5}));
        EXPECT_TRUE(equalSets(nka.transitions_[5][EPS], {6}));
    }
};

#endif //FORMALPRACTICE1__TESTS_H_
