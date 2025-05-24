#include <iostream>
#include <fstream>
#include <Token.h>
#include <Syntax.h>
#include <RunAST.h>
#include <AST2MC.h>
#include <sstream>
#include <cstring>

#include <CLI/CLI.hpp>
#include <functional>

using namespace std;

#define VERSION "1.0"

int main(int argc, char* argv[]) {
    
    CLI::App app{"A dcz (duca compiler zcript) compiler + interpreter"};
    argv = app.ensure_utf8(argv);
    string filename;
    string output="out.a";
    bool VerboseEnabled=false;
    bool Compile=false;
    int memoryorigin =0;
    string mstr;
    std::string arch="8086";
    app.add_option("filename", filename, ".dcz filename");
    app.add_flag("-v,--verbose", VerboseEnabled, "Enable verbose");
    app.add_flag_function("-V,--version",[](int64_t a) { cout << VERSION <<endl; exit(0); } ,"Show version and exit");
    app.add_option("-o,--output", output, "Output file");
    auto compileopt = app.add_option_group("Compile option");
    compileopt->add_flag("-c,--compile", Compile, "Enable Compile (default is interpreter)");
    compileopt->add_flag("-a,--arch", arch, "Binary architecture (8086, x86_64)");
    compileopt->add_option("-O,--origin", mstr,"Set memory origin (e.g: 7c00)");

    CLI11_PARSE(app, argc, argv);
    if (!mstr.empty())
        memoryorigin = stoi(mstr, 0, 16);

    if (argc < 2) {
        cerr << app.help() << endl;
        return 1;
    }

    if (filename == "stdin") {
        cerr << "stdin not support yet!" << endl;
        return 1;
    }
    ifstream ifs(filename);
    if (ifs.fail()) {
        perror("ifstream");
        return -1;
    }

    string s; {
        stringstream str;
        str << ifs.rdbuf();
        s = str.str();
    }

    Token t(filename, s);
    t.ParseLexer();
    Parser pars(filename, t.ReturnLexer());
    auto x = pars.ReturnExpr();

    if (!Compile) 
        RunAST(VerboseEnabled,filename,x).Run();
    else {
        auto code = AST2MC(filename, x, memoryorigin).ReturnProg();
        cout << "Opcode:" << endl;
        for (auto c : code) {
            if (c.opcode.has_value())
                cout << hex << c.opcode.value() << endl;
        }
    }

    return 0;
}