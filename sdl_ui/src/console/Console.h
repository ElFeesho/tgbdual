//
// Created by chris on 02/04/17.
//

#pragma once

#include <string>
#include <vector>
#include <SDL/SDL.h>

class Console {
public:
    Console();

    void open();
    void update(SDLKey param);
    void close();

    void draw(SDL_Surface *screen);

    void addhOutput(std::string output);
    void addError(std::string error);

    bool isOpen();
private:
    enum class OutputType {
        stdout,
        stderr
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
    int _cursorPos { 0 };
    std::vector<HistoryLine> _history;
};
