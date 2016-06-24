#!/bin/bash

ls *.h *.c *.cpp | xargs clang-format -sort-includes -i -style=file
