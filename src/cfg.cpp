#include "cfg.hpp"

std::size_t CFG::pline;
std::mt19937 CFG::random;

void CFG::Parse(CFG& cfg, const std::string& token) {
    if(token.at(0) != '<') {
        std::cerr << "Line " << CFG::pline << ": invalid variable" << std::endl;
        return;
    }

    std::string left = "";

    std::size_t rstart;
    for(rstart = 0; rstart < token.length() && token.at(rstart) != '>'; ++rstart) left += token.at(rstart);
    ++rstart;
    left = left.substr(1, left.length() - 1);

    if(token.substr(rstart, 3) != "::=") {
        std::cerr << "Line " << CFG::pline << ": invalid rule symbol " << token.substr(rstart, 3) << std::endl;
        return;
    }

    rstart += 3;

    std::string word = "", rule = "";

    if(cfg.R.find("<" + left + ">") == cfg.R.end()) {
        cfg.R.insert(
            std::make_pair( "<" + left + ">", std::set<std::string>() )
        );
    }

    cfg.V.insert("<" + left + ">");
    if(cfg.V.size() == 1) cfg.SetStart(*cfg.V.begin());

    for(; rstart < token.length(); ++rstart) {
        const char& c = token.at(rstart);
        switch(c) {
            case '"': {
                for(++rstart; rstart < token.length() && token.at(rstart) != '"'; ++rstart) {
                    word += token.at(rstart);
                }
                cfg.E.insert(word);
                rule += word;
                word = "";
            } break;
            case '<': {
                for(++rstart; rstart < token.length() && token.at(rstart) != '>'; ++rstart) {
                    word += token.at(rstart);
                }
                cfg.V.insert("<" + word + ">");
                rule += "<" + word + ">";
                word = "";
            } break;
            case '|': {
                cfg.R.at("<" + left + ">").insert(rule);
                rule = "";
                break;
            }
        }
    }

    cfg.R.at("<" + left + ">").insert(rule);
}

bool CFG::Build(CFG& cfg, const std::string& path) {
    CFG::pline = 0;

    std::ifstream in(path);

    if(!in.is_open()) {
        std::cerr << "File at " << path << " could not be found." << std::endl;
        in.close();
        return false;
    }

    State state = State::TOKEN;
    std::string token = "";

    for(char c; !in.eof(); ) {
        switch(state) {
            case State::TOKEN: {
                c = in.get();
                if(c == ';') state = State::COMMENT;
                else if(!std::isspace(c)) token += c;
                else if(c == '\n') {
                    ++CFG::pline;
                    if(!token.empty()) CFG::Parse(cfg, token);
                    token = "";
                }
            } break;
            case State::COMMENT: {
                for(c = in.get(); !in.eof() && c != '\n'; ) c = in.get();
                ++CFG::pline;
                state = State::TOKEN;
            } break;
        }
    }

    if(!token.empty()) CFG::Parse(cfg, token);

    for(const auto& var : cfg.V) {
        if(cfg.R.find(var) == cfg.R.end()) continue;
        cfg.choose.insert(
            std::make_pair(var, std::uniform_int_distribution<std::size_t>(0, cfg.R.at(var).size() - 1))
        );
    }

    in.close();
    return true;
}

std::size_t CFG::Emulate(const std::size_t& steps) {
    std::string result = this->S;

    std::size_t i;

    bool any_var = true;
    for(i = 0; i < steps && any_var; ++i) {
        any_var = false;

        for(const auto& var : this->V) {
            bool found = true;
            for(std::size_t j = 0, k; found; ) {
                if((k = result.find(var, j)) == std::string::npos) {
                    found = false;
                    break;
                }

                any_var = true;
                auto it = this->R.at(var).begin();
                std::advance(it, this->choose.at(var)(CFG::random));
                result = result.replace(k, var.length(), *it);
                j = k + (*it).length();
            }
        }

    }

    std::cout << result << std::endl;

    return i;
}

void CFG::SetStart(const std::string& start) {
    this->S = start;
}

void CFG::Show() const {
    std::cout << "Start: " << this->S << std::endl;

    std::cout << "Variables: {" << std::endl;
    for(const auto& s : this->V) {
        std::cout << "\t" << s << std::endl;
    }
    std::cout << "}" << std::endl;

    std::cout << "Terminals: {" << std::endl;
    for(const auto& s : this->E) {
        std::cout << "\t" << s << std::endl;
    }
    std::cout << "}" << std::endl;

    std::cout << "Rules: {" << std::endl;
    for(const auto& var_set : this->R) {
        for(const auto& rule : var_set.second) {
            std::cout << "\t" << var_set.first << " <- " << rule << std::endl;
        }
    }
    std::cout << "}" << std::endl;
}
