#ifndef CFG_HPP
#define CFG_HPP

#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <ctime>
#include <set>
#include <map>

enum class State {
    TOKEN, COMMENT
};

class CFG {
    std::map< std::string, std::uniform_int_distribution<std::size_t> > choose;
    std::map< std::string, std::set<std::string> > R;
    std::set<std::string> V;
    std::set<std::string> E;
    std::string S;

    static std::size_t pline;

    static void Parse(CFG& cfg, const std::string& token);
public:
    static std::mt19937 random;

    CFG()  = default;
    ~CFG() = default;

    static bool Build(CFG& cfg, const std::string& path);

    std::size_t Emulate(const std::size_t& steps);
    void SetStart(const std::string& start);
    void Show() const;
};

#endif
