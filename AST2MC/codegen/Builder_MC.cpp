#include <codegen/Builder_MC.h>

Builder_MC::Builder_MC()
{
}

Result_MC* Builder_MC::AddIntrBinary(Result_MC* L, Result_MC* R)
{
    if (L->rType() == NUM && R->rType() == NUM) {
        return new Constant_MC(L->v().n.value() + R->v().n.value());
    }
    //TODO: Implement with variable and function call
    /*For Variable and func call:

        int a;
        a + 1;
        -> mov a; inc a (for optimization)
    */
    return nullptr;
}

Result_MC* Builder_MC::SubIntrBinary(Result_MC* L, Result_MC* R)
{
    if (L->rType() == NUM && R->rType() == NUM) {
        return new Constant_MC(L->v().n.value() - R->v().n.value());
    }
    //TODO: Implement with variable and function call
    return nullptr;
}

Result_MC* Builder_MC::MulIntrBinary(Result_MC* L, Result_MC* R)
{
    
    if (L->rType() == NUM && R->rType() == NUM) {
        return new Constant_MC(L->v().n.value() * R->v().n.value());
    }
    //TODO: Implement with variable and function call
    return nullptr;
}

Result_MC* Builder_MC::DivIntrBinary(Result_MC* L, Result_MC* R)
{
    
    if (L->rType() == NUM && R->rType() == NUM) {
        return new Constant_MC(L->v().n.value() / R->v().n.value());
    }
    //TODO: Implement with variable and function call
    return nullptr;
}

void Builder_MC::dump()
{
}
