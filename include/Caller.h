#pragma once

#include <RunAST.h>

class RunAST;
struct Result;

class Callable {
    public:
        virtual Result call(RunAST* code, std::list<Result> args) const;
        virtual int SizeOfArgs() const {return 0;}
};

class Func : public Callable {
    protected:
        FuncDef declare;
    public:
        Func(FuncDef declare) : declare(declare){}
        Result call(RunAST*code, std::list<Result> args) const;
        int SizeOfArgs() const;
};