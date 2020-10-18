if [ ! -d ./bin ]; then 
    mkdir bin
else
    if [ ! -z "$(ls -A ./bin)" ]; then
        rm -rf bin/*
    fi
fi
clang++ -std=c++1z -Wall -Wextra -Wpedantic -O0 -g GMusicMan.cpp -o bin/GMusicMan.exe
