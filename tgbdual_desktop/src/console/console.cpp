//
// Created by chris on 02/04/17.
//

#include "console.h"
#include <iostream>

console::console(tgb::video_renderer *renderer, uint32_t width, uint32_t height, unhandled_command_func unhandledCommandFunc, time_provider timeProvider) :
        _renderer{renderer},
        _width{width},
        _height{height},
        _unhandledCommandFunc{unhandledCommandFunc},
        _timeProvider{timeProvider} {
}

void console::open() {
    _open = true;
}

void console::close() {
    _open = false;
}

void console::draw() {
    if (_open) {
        _renderer->fillRect(0, 0, _width, _height, 0xcc333333, 0xcc333333);
        _renderer->text(">", 3, _height - 10, 0xffffffff);
        _renderer->text("_", (int32_t) (10 + _cursorPos * 8), _height - 10, 0xffffffff);
        _renderer->text(_currentLine.c_str(), 10, _height -10, 0xffffffff);

        for (int i = 0; i < _history.size(); i++) {
            auto history = _history[i];
            int32_t historyLineY = (int32_t) ((_height - 10) - (10 * (_history.size() - i)));
            if (history.outputType() == OutputType::stdout) {
                _renderer->text(history.line().c_str(), 3, historyLineY, 0xffffffff);
            } else if (history.outputType() == OutputType::command) {
                _renderer->text(history.line().c_str(), 3, historyLineY, 0xff00ff00);
            } else {
                _renderer->text(history.line().c_str(), 3, historyLineY, 0xff0000ff);
            }
        }

        if (_timeProvider() > lastRepeat + keyRepeatDelay && keyRepeatDelay != 0) {
            handleKeyDown(keyToRepeat.first);
            keyRepeatDelay = 40;
            lastRepeat = _timeProvider();
        }
    }
}

bool console::isOpen() {
    return _open;
}

void console::key_down(char key) {
    handleKeyDown(key);
    queueRepeats(key);
}

void console::key_up(char key) {
    unqueueRepeats(key);
}

void console::queueRepeats(char key) {
    keyToRepeat.first = key;
    keyToRepeat.second = 0;
    keyRepeatDelay = 300;
    lastRepeat = _timeProvider();
}

void console::unqueueRepeats(char key) {
    keyRepeatDelay = 0;
}

void console::handleKeyDown(char key) {
    if (key == 2) {
        decrementCursorPosition();
    } else if (key == 3) {
        incrementCursorPosition();
    } else if (key == 7) {
        eraseCharacter();
    } else if (key == 5) {
        processLine();
    } else if (key == 6) {
        completeCommand();
    } else if (key == 0) {
        scrollUpHistory();
    } else if (key == 1) {
        scrollDownHistory();
    } else {
        insertKey(key);
    }
}

void console::insertKey(char key) {
    _currentLine.insert(_cursorPos, 1, key);
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