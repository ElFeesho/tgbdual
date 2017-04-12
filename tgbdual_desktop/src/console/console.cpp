//
// Created by chris on 02/04/17.
//

#include "console.h"
#include <SDL/SDL_gfxPrimitives.h>
#include <iostream>

console::console(console::unhandled_command_func unhandledCommandFunc) : _unhandledCommandFunc{unhandledCommandFunc} {

}

void console::open() {
    _open = true;
}

void console::close() {
    _open = false;
}

void console::draw(SDL_Surface *screen) {
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

        if (SDL_GetTicks() > lastRepeat + keyRepeatDelay && keyRepeatDelay != 0) {
            handleKeyDown(keyToRepeat.first, keyToRepeat.second);
            keyRepeatDelay = 40;
            lastRepeat = SDL_GetTicks();
        }
    }
}

bool console::isOpen() {
    return _open;
}

void console::key_down(SDLKey key, SDLMod mod) {
    handleKeyDown(key, mod);
    queueRepeats(key, mod);
}

void console::key_up(SDLKey key, SDLMod mod) {
    unqueueRepeats(key, mod);
}

void console::queueRepeats(SDLKey key, SDLMod mod) {
    keyToRepeat.first = key;
    keyToRepeat.second = mod;
    keyRepeatDelay = 300;
    lastRepeat = SDL_GetTicks();
}

void console::unqueueRepeats(SDLKey key, SDLMod mod) {
    keyRepeatDelay = 0;
}

void console::handleKeyDown(SDLKey &key, SDLMod &mod) {
    if (key == SDLK_LEFT) {
        decrementCursorPosition();
    } else if (key == SDLK_RIGHT) {
        incrementCursorPosition();
    } else if (key == SDLK_BACKSPACE) {
        eraseCharacter();
    } else if (key == SDLK_RETURN) {
        processLine();
    } else if ((key >= SDLK_0 && key <= SDLK_z) || key == SDLK_SPACE || key == SDLK_MINUS || key == SDLK_PERIOD) {
        insertKey(key, mod);
    } else if (key == SDLK_TAB) {
        completeCommand();
    } else if (key == SDLK_UP) {
        scrollUpHistory();
    } else if (key == SDLK_DOWN) {
        scrollDownHistory();
    }
}

void console::insertKey(SDLKey &key, SDLMod &mod) {
    if (key == SDLK_MINUS && mod == 1) {
        key = SDLK_UNDERSCORE;
        mod = KMOD_NONE;
    }
    _currentLine.insert(_cursorPos, 1, (char) (key - (32 * mod != 0 ? 1 : 0)));
    _cursorPos++;
}

void console::scrollDownHistory() {
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

void console::scrollUpHistory() {
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
}

void console::completeCommand() {
    if (_cursorPos > 0) {
        std::string replacement;
        int potentialMatches = 0;
        std::vector<std::string> allMatches;
        for (auto &pair : _cmds) {
            if (pair.first.find(_currentLine) == 0) {
                potentialMatches++;
                if (replacement == "") {
                    replacement = pair.first;
                } else if (pair.first.length() < replacement.length()) {
                    replacement = pair.first;
                }

                std::string newReplacement(replacement.begin(), std::mismatch(replacement.begin(), replacement.end(), pair.first.begin()).first);
                if (newReplacement.length() > _currentLine.length()) {
                    replacement = newReplacement;
                }
            }
        }

        if (replacement.length() > 0) {
            if (potentialMatches == 1) {
                replacement += " ";
            }
            _currentLine = replacement;
            _cursorPos = _currentLine.length();
        }
    }
}

void console::processLine() {
    if (_currentLine.length() > 0) {
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
                std::vector<std::string> vector = console_cmd::splitArguments(args);
                if (!_unhandledCommandFunc(command, vector)) {
                    addError("Command " + command + " not found");
                } else {
                    addHistory(_currentLine);
                }
            }
        }
        _currentLine = "";
        _cursorPos = 0;
        _historyIndex = 0;
    }
}

void console::eraseCharacter() {
    if (_cursorPos > 0) {
        _currentLine.erase(_cursorPos - 1, 1);
        _cursorPos--;
    }
}

void console::incrementCursorPosition() {
    _cursorPos++;
    if (_cursorPos > _currentLine.length()) {
        _cursorPos = _currentLine.length();
    }
}

void console::decrementCursorPosition() {
    if (_cursorPos > 0) {
        _cursorPos--;
    }
}

void console::addOutput(std::string output) {
    _history.push_back(console::HistoryLine{output});
}

void console::addHistory(std::string output) {
    _history.push_back(console::HistoryLine{output, OutputType::command});
}

void console::addError(std::string error) {
    _history.push_back(console::HistoryLine{error, OutputType::stderr});
}

void console::addCommand(const std::string &command, std::function<void(std::vector<std::string>)> commandFunc) {
    _cmds.emplace(command, std::unique_ptr<console_cmd>(new console_cmd(command, commandFunc)));
}

void console::removeCommand(const std::string &command) {
    if (_cmds.find(command) != _cmds.end()) {
        _cmds.erase(_cmds.find(command));
    }
}

console::HistoryLine::HistoryLine(std::string line, console::OutputType outputType) : _line(line),
                                                                                      _outputType(outputType) {

}

const std::string &console::HistoryLine::line() {
    return _line;
}

console::OutputType console::HistoryLine::outputType() {
    return _outputType;
}