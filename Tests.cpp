//
// Created by aleks311001 on 01.11.2020.
//

#include <gtest/gtest.h>
#include "Tests.h"

TEST_F(TestNKA, testAdd) {
    testAdd();
}

TEST_F(TestNKA, testReplaceMultiSymbolEdges) {
    testReplaceMultiSymbolEdges();
}

TEST_F(TestNKA, testEpsilonOperations) {
    testEpsilonOperations();
}

TEST_F(TestNKA, testDelUselessConfigurations) {
    testDelUselessConfigurations();
}

TEST_F(TestNKA, testGetMapConfigurationToNumber) {
    testGetMapConfigurationToNumber();
}

TEST_F(TestNKA, testMakeConfigurationFromOthers) {
    testMakeConfigurationFromOthers();
}

TEST_F(TestNKA, testCheckSetConfigsOnAccepting) {
    testCheckSetConfigsOnAccepting();
}

TEST_F(TestNKA, testCheckNumberOfConfigurations) {
    testCheckNumberOfConfigurations();
}

TEST_F(TestNKA, testFindAllTransitionsFromSetConfigurations) {
    testFindAllTransitionsFromSetConfigurations();
}

TEST_F(TestNKA, testMakeExplicitWays) {
    testMakeExplicitWays();
}

TEST_F(TestNKA, testMakeDKA) {
    testMakeDKA();
}

TEST_F(TestNKA, testMakeOneAcceptingConfiguration) {
    testMakeOneAcceptingConfiguration();
}

TEST_F(TestNKA, testMakeReverse) {
    testMakeReverse();
}

TEST_F(TestNKA, testUnionNKA) {
    testUnionNKA();
}

TEST_F(TestNKA, testMulNKA) {
    testMulNKA();
}

TEST_F(TestNKA, testAddNKA) {
    testAddNKA();
}

TEST_F(TestNKA, testStarNKA) {
    testStarNKA();
}

TEST_F(TestNKA, testLenMaxPrefOf) {
    testLenMaxPrefOf();
}

TEST_F(TestNKA, testMakeNKA) {
    testMakeNKA();
}

TEST(TestTask14, testTask14) {
    std::string regular1 = "ab+c.aba.*.bac.+.+*";
    ASSERT_EQ(task14(regular1, ""), 0);
    ASSERT_EQ(task14(regular1, "babc"), 2);
    ASSERT_EQ(task14(regular1, "asdasbabc"), 2);
    ASSERT_EQ(task14(regular1, "bababac"), 6);
    ASSERT_EQ(task14(regular1, "acbbab"), 2);

    std::string regular2 = "acb..bab.c.*.ab.ba.+.+*a.";
    ASSERT_EQ(task14(regular2, ""), 0);
    ASSERT_EQ(task14(regular2, "cbaa"), 1);
    ASSERT_EQ(task14(regular2, "baacba"), 4);
    ASSERT_EQ(task14(regular2, "cbabcaba"), 7);
    ASSERT_EQ(task14(regular2, "bcbcaacbbaba"), 7);
}
