#pragma once

#include <codegen/Result_MC.h>

class Builder_MC {
    public:
        Builder_MC();
        Result_MC* AddIntrBinary(Result_MC* L, Result_MC* R);
        Result_MC* SubIntrBinary(Result_MC* L, Result_MC* R);
        Result_MC* MulIntrBinary(Result_MC* L, Result_MC* R);
        Result_MC* DivIntrBinary(Result_MC* L, Result_MC* R);

        void dump();
};