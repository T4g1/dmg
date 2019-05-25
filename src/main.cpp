#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <string>
#include <vector>

#include "log.h"
#include "dmg.h"

#include "main.h"


void show_usage()
{
    std::cerr << "Usage: dmg <option(s)> ROM\n"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-b,--boot BOOT\tSpecifies BOOT ROM\n"
              << "\t-p,--palette PALETTE\tndex of color palette to use\n";
}


int main(int argc, char *argv[])
{
    info("DMG emulation\n");

    if (argc < 2 || argc > 6) {
        show_usage();
    }

    std::string rom;
    std::string boot = "";
    std::string palette = "0";

    for (int i=1; i<argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage();
            return EXIT_SUCCESS;
        } else if ((arg == "-b") || (arg == "--boot")) {
            if (i + 1 < argc) {
                boot = argv[++i];
            } else {
                error("--boot option requires one argument\n");
                show_usage();
                return EXIT_FAILURE;
            }
        } else if ((arg == "-p") || (arg == "--palette")) {
            if (i + 1 < argc) {
                palette = argv[++i];
            } else {
                error("--palette option requires one argument\n");
                show_usage();
                return EXIT_FAILURE;
            }
        } else if (i == argc - 1) {
            rom = argv[i];
        } else {
            error("Option not recognized: %s\n", argv[i]);
            show_usage();
            return EXIT_FAILURE;
        }
    }

    DMG *dmg = new DMG();
    if (!dmg->init(boot.c_str(), rom.c_str())) {
        return EXIT_FAILURE;
    }

    uint8_t palette_id = palette.c_str()[0] - '0';
    dmg->set_palette(palette_id);

    int status = dmg->run();

    delete dmg;

    return status;
}
