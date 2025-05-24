#include <ErrorHandler.h>
#include <iostream>
using namespace std;

void ErrorHandler::pError(std::string message, std::string filename, int line, int current) {
    cerr << filename << ":" << line << ":" << current << " error: " << message << endl;
}
void ErrorHandler::pWarning(std::string message, std::string filename, int line, int current) {
    cerr << filename << ":" << line << ":" << current << " warning: " << message << endl;
}