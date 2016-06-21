#!/bin/bash

ls *.h *.c *.cpp | xargs clang-format -style=file - -i style=file
