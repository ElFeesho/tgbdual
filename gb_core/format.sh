#!/bin/bash

ls *.h *.cpp | xargs clang-format -sort-includes -i -style=file
