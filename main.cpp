#include "src/cfg.hpp"

int main(int argc, char ** argv) {
    if(argc < 3) {
        std::cerr << "\t\tContext-Free Grammar Emulator\n";
        std::cerr << "\tUsage:\n";
        std::cerr << "\t\tcfg [config-file] [steps]\n\n";
        return 0;
    }

    CFG::random.seed((std::size_t)time(nullptr));

    CFG cfg = {};

    if(!CFG::Build(cfg, std::string(argv[1]))) {
        return 1;
    }

    cfg.Show();
    cfg.Emulate(std::stoi(argv[2]));

    return 0;
}
