//
// Created by chris on 02/04/17.
//

#pragma once

#include <string>
#include <vector>
#include <SDL/SDL.h>
#include <map>
#include <memory>
#include <functional>
#include "ConsoleCmd.h"

class Console {
public:
    using unhandled_command_func = std::function<bool(std::string&, std::vector<std::string> &)>;
    Console(unhandled_command_func unhandledCommandFunc);

    void open();
    void update(SDLKey param, SDLMod mod);
    void close();

    void draw(SDL_Surface *screen);

    void addOutput(std::string output);
    void addHistory(std::string output);
    void addError(std::string error);

    bool isOpen();

    void addCommand(ConsoleCmd *consoleCmd);

    void processLine();
private:
    enum class OutputType {
        stdout,
        stderr,
        command
    };
    class HistoryLine {
    public:
        HistoryLine(std::string line, OutputType outputType = OutputType::stdout);
        OutputType outputType();
        const std::string &line();
    private:
        std::string _line;
        OutputType _outputType;
    };
    bool _open { false };
    std::string _currentLine { "" };
    unsigned long _cursorPos {0 };
    std::vector<HistoryLine> _history;
    int _historyIndex{0};

    std::map<std::string, std::unique_ptr<ConsoleCmd>> _cmds;

    unhandled_command_func _unhandledCommandFunc;
};
