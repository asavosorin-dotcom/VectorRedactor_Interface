#!/usr/bin/env bash
# check-tidy.sh
# Using: ./check-tidy.sh "Compiler Options"

COMPILER_OPTIONS="${1:-}"

find -type f -and \( -name '*.hpp' -or -name '*.cpp' \) -exec clang-tidy {} -- $COMPILER_OPTIONS \;
