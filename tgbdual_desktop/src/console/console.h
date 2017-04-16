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
#include <rendering/video_renderer.h>
#include "console_cmd.h"

class console {
public:
    using time_provider = std::function<long()>;
    using unhandled_command_func = std::function<bool(std::string&, std::vector<std::string> &)>;
    console(tgb::video_renderer *renderer, uint32_t width, uint32_t height, unhandled_command_func unhandledCommandFunc, time_provider timeProvider);

    void open();
    void key_down(char key);
    void key_up(char key);
    void close();

    void draw();

    void addOutput(std::string output);
    void addHistory(std::string output);
    void addError(std::string error);

    bool isOpen();

    void addCommand(const std::string &command, std::function<void(std::vector<std::string>)> commandFunc);

    void removeCommand(const std::string &command);

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
    tgb::video_renderer *_renderer;
    uint32_t _width, _height;
    bool _open { false };
    std::string _currentLine { "" };
    unsigned long _cursorPos {0 };
    std::vector<HistoryLine> _history;
    int _historyIndex{0};

    std::map<std::string, std::unique_ptr<console_cmd>> _cmds;

    unhandled_command_func _unhandledCommandFunc;

    std::pair<char, int> keyToRepeat;
    int keyRepeatDelay{ 0 };
    long lastRepeat { 0 };

    time_provider _timeProvider;

    void decrementCursorPosition();
    void incrementCursorPosition();
    void eraseCharacter();
    void processLine();
    void completeCommand();
    void scrollUpHistory();
    void scrollDownHistory();
    void insertKey(char key);
    void handleKeyDown(char key);
    void unqueueRepeats(char key);
    void queueRepeats(char key);
};
