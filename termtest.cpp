//
// Created by sobral on 27/07/19.
//
#include <limits.h>
#include <time.h>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <functional>
#include "Terminal.h"
#include "gtest/gtest.h"

using namespace std;

#define PROG "./Prog"

struct Result {
    string w;
    int n;

    Result(const string & s) {
        istringstream inp(s);

        inp >> w >> n;
        if (inp.fail()) throw s;
    }
    Result() {}
    bool operator==(const Result & o) const {
        return w == o.w;
    }
    bool operator<(const Result & o) const {
        if (n == o.n) return w < o.w;
        return n < o.n;
    }

    class R_Hash {
    public:
        long operator()(const Result & r) const {
            hash<string> func;

            long h = func(r.w) ^r.n;
            return h;
        }
    };

    operator string() const {
        string r = w + ' ' + to_string(n);
        return r;
    }

};

vector<string> le_nomes(const string & arqname, int n) {
    vector<string> l;
    if (n == 0) return l;

    ifstream arq(arqname);

    string nome;
    while (getline(arq, nome)) {
        l.push_back(nome);
    }

    random_shuffle(l.begin(), l.end());
    l.erase(l.begin()+n, l.end());

    return l;
}

void roda(const string & arq, int n, bool existe) {
    string cmd = PROG;
    Terminal term(cmd);
    auto v = le_nomes(arq, n);

    for (auto & nome: v) {
        auto prompt = term.leAlgo();
        if (prompt != "Consultar>") {
            FAIL() << "Deve ser apresentado o prompt \"Consultar>\"antes de ler um nome." << endl
                   << " Seu programa apresentou isto: " << prompt << endl;
        }
        term.escreva(nome+'\n');
        auto res = term.leLinha();
        strip(res);
        string name;
        if (existe) name = nome + ": EXISTE";
        else name = nome + ": INEXISTENTE";
        if (res != name) {
            FAIL() << "Consulta a \"" << nome +"\" deveria mostrar " << name << endl
                   << "Seu programa informou: " << res << endl;
        }
    }
    auto prompt = term.leAlgo();
    term.escreva("\n");
    auto fim = term.leAlgo();
    if (not fim.empty()) {
        FAIL() << "Seu programa deve terminar ao ler uma linha vazia !" << endl;
    }
}

//bool operator!=(const vector<Result> & v1, const vector<Result> & v2) {
//    unordered_set<Result,Result::R_Hash> set;
//
//    for (auto & r: v2) set.insert(r);
//    for (auto & r: v1) {
//        if (! set.count(r)) return true;
//    }
//
//    return false;
//}

TEST(TermTest, Existente) {
    for (int j=0; j < 10; j++) roda("../nomes.txt", 1+(rand() % 7), true);
}

TEST(TermTest, Inexistente) {
    roda("../inexistentes.txt", 2+(rand() % 7), false);
}

TEST(TermTest, Nada) {
    roda("../nomes.txt", 0, true);
}

