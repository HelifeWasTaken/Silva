#!/bin/bash
g++ -o tests -W -Wall -Werror -Wextra -O3 -lgtest Tests.cpp && ./tests ; rm tests
