cmake_minimum_required(VERSION 3.5)
project(WuZiQiDemo)

set(CMAKE_CXX_STANDARD 11)

# 这里告诉 CLion，这三个文件要一起编译
add_executable(WuZiQiDemo main.cpp Board.cpp Console.cpp Player.cpp Board.h Console.h Player.h Global.h
        Referee.h
        Referee.cpp
        AIPlayer.h
        AIPlayer.cpp
        NeuralNet.h
        NeuralNet.cpp
        StaticAI.h
        StaticAI.cpp)