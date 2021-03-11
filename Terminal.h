/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Terminal.h
 * Author: msobral
 *
 * Created on 11 de Fevereiro de 2019, 16:07
 */

#ifndef TERMINAL_H
#define TERMINAL_H
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

void strip(string & s);
vector<string> separa(const string & s, const string & delim=" \r\n\t");

string to_string(const string & s);

template <typename T> string v2str(vector<T> & v, const string & delim=" ") {
    string r;

    for (auto & x: v) r += to_string(x) + delim;
    if (! r.empty()) r.pop_back();

    return r;
}

class Terminal {
 public:
  Terminal(const string & cmd);

  ~Terminal();

  int escreva(const string & data);

  // lê até N bytes de cada vez (default: 1024 bytes)
  string leia(int N=1024);
  
  // lê todo o conteúdo
  string leiaTudo();

  // lê todas as linhas
  vector<string> leLinhas();

  // lê todas as palavras
  vector<string> lePalavras();

  string leLinha();
  string leAlgo(bool _wait=true, char c='\n');
  string leAte(char c);
  string lePalavra();
  
private:
  int master;
  pid_t pid;
  
  char ** gera_argv(const string & cmd);
};


#endif /* TERMINAL_H */

