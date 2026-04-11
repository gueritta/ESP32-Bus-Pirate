#pragma once

#include <string>
#include <vector>
#include "Enums/ModeEnum.h"

struct MenuCommand {
    std::string name;
    std::string description;
    bool requiresParameter;
    std::vector<MenuCommand> subcommands;

    MenuCommand(std::string n = "", std::string d = "", bool reqParam = false, std::vector<MenuCommand> sub = {})
        : name(n), description(d), requiresParameter(reqParam), subcommands(sub) {}
};

class MenuTree {
public:
    static std::vector<MenuCommand> getGeneralCommands();
    static std::vector<MenuCommand> getProtocolCommands(ModeEnum mode);
};
