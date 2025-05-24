#pragma once

#include <string>

class ErrorHandler {
    public:
        static void pError(std::string message, std::string filename, int line, int current);
        static void pWarning(std::string message, std::string filename, int line, int current);
};