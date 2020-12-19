//
// Created by aleks311001 on 22.09.2020.
//

#ifndef FORMAL__NKA_H_
#define FORMAL__NKA_H_

#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <unordered_map>
#include <queue>
#include <exception>
#include <limits>
#include <fstream>
#include <cmath>

const std::string EPS = ""; ///< Epsilon word

class BuilderNKA;

/**
 * @brief NKA class
 * @details This class can make DKA and calculate max len of word's prefix, which in L(NKA/DKA)
 */
class NKA {
public:
    using ConfigurationType = long long;

private:
    using TransitionsType = std::unordered_map<ConfigurationType, std::unordered_map<std::string, std::set<ConfigurationType>>>;

    friend class TestNKA;
    friend class BuilderNKA;

    std::set<ConfigurationType> configurations_;
    std::set<char> alphabet_;
    TransitionsType transitions_;
    ConfigurationType q0_;
    std::set<ConfigurationType> acceptingConfigurations_;

    /**
     * Check, that start and finish in configurations_ set and word in alphabet^*.
     * After it call addTransitionWithoutCheck_
     * @param [in] left start of edge configuration
     * @param [in] word value on edge
     * @param [in] right finish of edge configuration
     * @throw invalid_argument if added configuration or word does not contains in configurations or alphabet
     */
    void addTransition_(ConfigurationType left, const std::string& word, ConfigurationType right);
    /**
     * Call addTransition(left, string(word), right)
     * @param [in] left start of edge configuration
     * @param [in] word value on edge
     * @param [in] right finish of edge configuration
     * @throw invalid_argument if added configuration or word does not contains in configurations or alphabet
     */
    void addTransition_(ConfigurationType left, char word, ConfigurationType right);

    /**
     * Generate and add unique configuration
     * @param [in] min generated configuration greater then this
     * @return added configuration
     */
    ConfigurationType addNewConfiguration_(ConfigurationType min = 0);


    /**
     * Add edge from left to right with value word. But it doesn't check, that start and finish in configurations_ set
     * and word in alphabet^*
     * @param [in] left start of edge configuration
     * @param [in] word value on edge
     * @param [in] right finish of edge configuration
     */
    void addTransitionWithoutCheck_(ConfigurationType left, const std::string& word, ConfigurationType right);

public:
    struct Edge {
        std::string word;
        ConfigurationType final;

        Edge(const std::string& word, ConfigurationType final): word(word), final(final) {}
    };

    struct relationConfigurations {
        ConfigurationType newConf;
        std::set<ConfigurationType> oldConf;

        relationConfigurations(ConfigurationType newConf, const std::set<ConfigurationType>& oldConf):
                newConf(newConf),
                oldConf(oldConf) {}
    };

private:
    /**
     * @details Find all ends configurations of ways from start, that has epsilon transitions,
     * but last transition is not epsilon. start-EPS->q_1-EPS->q_2-EPS->....-EPS->q_n-a->q in endWayConfigurations
     * @param [in] start starting configuration
     * @param [in, out] wasIn set of configurations, that has way from start
     * @param [out] endWayConfigurations all ends configurations of ways with
     * @param [out] hasAcceptingConf true if exist epsilon way from start to accepting configuration
     */
    void findAllWaysOnEpsEdges_(ConfigurationType start,
                               std::set<ConfigurationType>& wasIn,
                               std::list<Edge>& endWayConfigurations,
                               bool& hasAcceptingConf);
    /**
     * Add all edges, that can replace epsilon edges for ways from start.
     * If exist way: start-EPS->q_1-EPS->q_2-EPS->....-EPS->q_n-a->q we add start-a->q
     * Also that function make configurations accepting, if exist epsilon way from it to accepting configuration.
     * @param [in] start starting configuration
     * @param [in, out] wasIn set of configurations, that has way from start
     */
    void addTransitionsInEpsWays_(ConfigurationType start, std::set<ConfigurationType>& wasIn);
    /**
     * Remove all epsilon transitions
     */
    void removeEpsilonTransitions_();

    /**
     * Find all configurations, which can way from q0
     * @return set of these configurations
     */
    std::set<ConfigurationType> getSetUsefulConfigurations_();
    /**
     * For each configuration make a unique number in [0, number of Configurations)
     * @return map: configuration -> number
     */
    std::unordered_map<long long, size_t> getMapConfigurationToNumber_();
    /**
     * Make big configuration for new NKA, as a subset of set all configurations
     * @param [in] configurations subset all configurations
     * @param [in] indexesConfigs map: configuration -> number
     * @return new configuration
     */
    static ConfigurationType makeConfigurationFromOthers_(const std::set<ConfigurationType>& configurations,
                                                          const std::unordered_map<ConfigurationType, size_t>& indexesConfigs);
    /**
     * Check that set of configurations has accepting configuration
     * @param [in] configurations checked configurations
     * @return result of check
     */
    bool checkSetConfigsOnAccepting_(const std::set<ConfigurationType>& configurations);

    /// Create new configurations and edges from old accepting configurations to this
    void makeOneAcceptingConfiguration_();

    /// Check that number of configurations less log_2(max(ConfigurationType)), for building DKA
    void checkNumberOfConfigurations_();
    /**
     * Find all edges, which has start configuration in set configurations
     * @param [in] configurations set of start configurations
     * @return map of founded edges
     */
    std::unordered_map<std::string, std::set<long long>>
    findAllTransitionsFromSetConfigurations_(const std::set<ConfigurationType>& configurations);
    /**
     * Add edge from nowConf in newNKA (for building DKA)
     * @param [int, out] newNKA DKA
     * @param [in] nowConf pair of edge's start configuration in newNKA and set of configurations in this
     * @param [in] numsConfigurations function from configurations to indexes
     * @param [in] configurationsQueue queue for insert next configuration
     */
    void addTransitionsInBuildingDKA_(NKA& newNKA,
                                      const std::unordered_map<ConfigurationType, size_t>& numsConfigurations,
                                      std::queue<relationConfigurations>& configurationsQueue);

public:
    /**
     * Build NKA from regular expression
     * @param [in] regular expression in polish notation
     * @return made NKA
     */
    friend NKA makeNKA(const std::string& regular);

    NKA();

    NKA(const NKA& other);
    NKA(NKA&& other);
    NKA& operator= (const NKA& other);
    NKA& operator= (NKA&& other);

    /// Replace multi symbols edge on single-symbol edges
    void replaceMultiSymbolsEdges();

    /// Change all epsilon transitions on one-letter.
    void changeEpsTransitions();

    /// Delete All configurations which has not edges
    void delUselessConfigurations();

    /**
     * Transform this in NKA with a single edge type from each state
     */
    void makeExplicitWays();

    /**
     * Make DKA. Call:
     * @code
     *     replaceMultiSymbolsEdges();
     *     changeEpsTransitions();
     *     makeExplicitWays();
     * @endcode
     */
    void makeDKA();

    /**
     * Union two non-intersecting NKAs and return new start configuration second NKA
     * @param [in] other second NKA
     * @return start configuration second NKA
     */
    ConfigurationType unionNKA(const NKA& other);
    /**
     * Make new NKA: L(new NKA) = L(this) . L(other)
     * @param [in] other second NKA
     */
    void mul(const NKA& other);
    /**
     * Make new NKA: L(new NKA) = L(this) + L(other)
     * @param [in] other second NKA
     */
    void add(const NKA& other);
    /**
     * Make new NKA: L(new NKA) = L(this)*
     */
    void star();

    /**
     * Make reverse NKA: L(new NKA) = L(this)^R
     */
    void makeReverse();
    /**
     * Calculate max len of word's prefix, which in L(this NKA)
     * @param [in] word
     * @return len
     */
    size_t lenMaxPrefOf(const std::string& word);
};

#endif //FORMAL__NKA_H_
