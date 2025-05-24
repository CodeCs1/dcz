#include <Caller.h>
#include <ErrorHandler.h>

int Func::SizeOfArgs() const {
    return this->declare.pars.size();
}

Result Callable::call(RunAST* code, std::list<Result> args) const {
    return Result();
}

Result Func::call(RunAST* code, std::list<Result> args) const {
    if (code->globals.Size() == 0) code->UpdateGlobal();
    Environment envi(code->globals);
    if (args.size() != SizeOfArgs()) {
        printf("[line %d] Expect %d, got %d\n", args.begin()->line, SizeOfArgs(), args.size());
        exit(1);
    }
    auto args_l = args.begin();
    int i=0;
    for (auto x : this->declare.pars) {
        envi.define(x.datatype.token, x.pars.token, *args_l);
        i++;
        std::advance(args_l, i);
    }
    return code->ExecuteBlock(declare.body, envi);
}
