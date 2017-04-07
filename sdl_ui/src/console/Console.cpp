//
// Created by chris on 02/04/17.
//

#include "Console.h"
#include <SDL/SDL_gfxPrimitives.h>
#include <iostream>

Console::Console(Console::unhandled_command_func unhandledCommandFunc) : _unhandledCommandFunc{unhandledCommandFunc} {

}

void Console::open() {
    _open = true;
}

void Console::close() {
    _open = false;
}

void Console::draw(SDL_Surface *screen) {
    if (_open) {
        Sint16 x[] = {0, 0, (Sint16) screen->w, (Sint16) screen->w};
        Sint16 y[] = {0, (Sint16) (screen->h / 2), (Sint16) (screen->h / 2), 0};
        filledPolygonRGBA(screen, x, y, 4, 33, 33, 33, 220);
        stringRGBA(screen, 3, (Sint16) (screen->h / 2 - 10), ">", 255, 255, 255, 255);

        stringRGBA(screen, (Sint16) (10 + _cursorPos * 8), (Sint16) (screen->h / 2 - 10), "_", 255, 255, 255,
                   (Uint8) (SDL_GetTicks() % 255));
        stringRGBA(screen, 10, (Sint16) (screen->h / 2 - 10), _currentLine.c_str(), 255, 255, 255, 255);

        for (int i = 0; i < _history.size(); i++) {
            auto history = _history[i];
            if (history.outputType() == OutputType::stdout) {
                stringRGBA(screen, 3, (Sint16) ((Sint16) (screen->h / 2 - 10) - (10 * (_history.size() - i))),
                           history.line().c_str(), 255, 255, 255, 255);
            } else if (history.outputType() == OutputType::command) {
                stringRGBA(screen, 3, (Sint16) ((Sint16) (screen->h / 2 - 10) - (10 * (_history.size() - i))),
                           history.line().c_str(), 0, 255, 0, 255);
            } else {
                stringRGBA(screen, 3, (Sint16) ((Sint16) (screen->h / 2 - 10) - (10 * (_history.size() - i))),
                           history.line().c_str(), 255, 0, 0, 255);
            }
        }

    }
}

bool Console::isOpen() {
    return _open;
}

void Console::update(SDLKey key, SDLMod mod) {
    if (key == SDLK_LEFT) {
        if (_cursorPos > 0) {
            _cursorPos--;
        }
    } else if (key == SDLK_RIGHT) {
        _cursorPos++;
        if (_cursorPos > _currentLine.length()) {
            _cursorPos = _currentLine.length();
        }
    } else if (key == SDLK_BACKSPACE && _cursorPos > 0) {
        _currentLine.erase(_cursorPos - 1, 1);
        _cursorPos--;
    } else if (key == SDLK_RETURN && _currentLine.length() > 0) {
        processLine();
        _currentLine = "";
        _cursorPos = 0;
        _historyIndex = 0;
    } else if ((key >= SDLK_0 && key <= SDLK_z) || key == SDLK_SPACE || key == SDLK_MINUS || key == SDLK_PERIOD) {
        if (key == SDLK_MINUS && mod == 1) {
            key = SDLK_UNDERSCORE;
            mod = KMOD_NONE;
        }
        _currentLine.insert(_cursorPos, 1, (char) (key - (32 * mod != 0 ? 1 : 0)));
        _cursorPos++;
    } else if (key == SDLK_TAB) {
        std::string replacement;
        for (auto &pair : _cmds) {
            if (pair.first.find(_currentLine) == 0) {
                if (replacement == "") {
                    replacement = pair.first + " ";
                } else if (pair.first.length() < replacement.length()) {
                    replacement = pair.first + " ";
                }
            }
        }

        if (replacement.length() > 0) {
            _currentLine = replacement;
            _cursorPos = _currentLine.length();
        }
    } else if (key == SDLK_UP) {
        _historyIndex++;
        bool found = false;
        for (int i = 0, j = 0; i < _history.size(); i++) {
            unsigned long index = (_history.size() - 1) - i;
            if (_history[index].outputType() == OutputType::command) {
                j++;
                if (j == _historyIndex) {
                    found = true;
                    _currentLine = _history[index].line();
                    _cursorPos = _currentLine.length();
                    break;
                }
            }
        }
        if (!found) {
            _historyIndex--;
        }
    } else if (key == SDLK_DOWN) {
        _historyIndex--;
        if (_historyIndex <= 0) {
            _historyIndex = 0;
            _currentLine = "";
            _cursorPos = 0;
        } else {
            for (int i = 0, j = 0; i < _history.size(); i++) {
                unsigned long index = (_history.size() - 1) - i;
                if (_history[index].outputType() == OutputType::command) {
                    j++;
                    if (j == _historyIndex) {
                        _currentLine = _history[index].line();
                        _cursorPos = _currentLine.length();
                        break;
                    }
                }
            }
        }
    }
}

void Console::addOutput(std::string output) {
    _history.push_back(Console::HistoryLine{output});
}

void Console::addHistory(std::string output) {
    _history.push_back(Console::HistoryLine{output, OutputType::command});
}

void Console::addError(std::string error) {
    _history.push_back(Console::HistoryLine{error, OutputType::stderr});
}

void Console::addCommand(ConsoleCmd *command) {
    _cmds.emplace(command->name(), std::unique_ptr<ConsoleCmd>(command));
}

void Console::processLine() {
    if (_currentLine.find(" ") == std::string::npos) {
        if (_cmds.find(_currentLine) != _cmds.end()) {
            addHistory(_currentLine);
            _cmds[_currentLine]->invoke();
        } else {
            std::vector<std::string> args;
            if (!_unhandledCommandFunc(_currentLine, args)) {
                addError("Command " + _currentLine + " not found");
            } else {
                addHistory(_currentLine);
            }
        }
    } else {
        std::string command = _currentLine.substr(0, _currentLine.find(' '));
        std::string args = _currentLine.substr(_currentLine.find(' ') + 1);
        if (_cmds.find(command) != _cmds.end()) {
            addHistory(_currentLine);
            _cmds[command]->invoke(args);
        } else {
            std::vector<std::string> vector = ConsoleCmd::splitArguments(args);
            if (!_unhandledCommandFunc(command, vector)) {
                addError("Command " + command + " not found");
            } else {
                addHistory(_currentLine);
            }
        }
    }
}

Console::HistoryLine::HistoryLine(std::string line, Console::OutputType outputType) : _line(line),
                                                                                      _outputType(outputType) {

}

const std::string &Console::HistoryLine::line() {
    return _line;
}

Console::OutputType Console::HistoryLine::outputType() {
    return _outputType;
}