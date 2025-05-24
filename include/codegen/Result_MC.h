#pragma once

#include <Syntax.h>
#include <optional>

enum ResultType {
    NUM,
    STR,
    LST,
    NONE
};

struct Values {
    std::optional<double> n;
    std::optional<std::string> str;
};

class Result_MC {
    public:
        Result_MC(){}
        virtual ResultType rType() const {return NONE;}
        virtual Values v() const {return {0,"NOT IMPLEMENTED"};};
        ~Result_MC() =default;
        virtual void dump() {};

        void* operator new(size_t sz) {
            return ::operator new(sz);
        }
};

class Constant_MC : public Result_MC {
    private:
        Values V;
    public:
        Constant_MC(double n) {V.n = n;}
        Constant_MC(std::string str) {V.str = str;}
        ResultType rType() const override { 
            if (V.n.has_value()) return ResultType::NUM;
            else if(V.str.has_value()) return ResultType::STR;
            return NONE;
        }
        Values v() const override {return V;}
        void dump() override {
            if (v().n.has_value()) {
                printf("number: %f\n", v().n.value());
            }
            else if(v().str.has_value()) {
                printf("string: %s\n", v().str.value().c_str());
            }
        }
};
class Function_MC : public Result_MC {
    public:
        Function_MC();
};