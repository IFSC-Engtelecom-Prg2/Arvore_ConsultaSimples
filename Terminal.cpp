/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Terminal.cpp
 * Author: msobral
 * 
 * Created on 11 de Fevereiro de 2019, 16:07
 */

#include "Terminal.h"
#include <pty.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <ctype.h>
#include <poll.h>

using namespace std;

string to_string(const string & s) { return s;}

// transforma a linha de comando contida em "cmd" em um vetor de argumentos no formato
// apropriado a ser usado em execvp.
// Os argumentos estão separados por espaços ou tab. Sequências delimitadas por aspas são
// respeitadas, sendo consideradas um único argumento.
// O vetor de argumentos resultante deve ser posteriormente destruído com delete[]
char** Terminal::gera_argv(const string& cmd) {
    vector<string> v;
    int state = 0;
    int pos = 0, startpos;
    
    while (pos < cmd.size()) {
        char c = cmd[pos];
        
        switch (state) {
            case 0: // espaços
                if (! isblank(c)) {                    
                    if (c == '"') {
                        state = 2;
                        startpos = pos+1;
                    } else {
                        state = 1;
                        startpos = pos;
                    }
                }
                break;
            case 1:
                if (isblank(c)) {
                    v.push_back(cmd.substr(startpos, pos-startpos));
                    state = 0;
                }
                break;
            case 2:
                if (c == '"') {
                    v.push_back(cmd.substr(startpos, pos-startpos));
                    state = 0;                    
                }
                break;
        }
        pos++;
    }
    
    if (state != 0) v.push_back(cmd.substr(startpos, pos-startpos));
    
    char ** args = new char*[v.size()+1];
    int i=0;
    for (auto & arg: v) {
        args[i] = new char[arg.size()+1];
        strcpy(args[i], arg.c_str());
        i++;
    }
    args[i] = nullptr;
    
    return args;
}

Terminal::Terminal(const string & cmd) {
    termios termp;
      
    pid = forkpty(&master, NULL, NULL, NULL);
    tcgetattr(master, &termp);
    termp.c_lflag ^= ECHO;
    tcsetattr(master, 0, &termp);    
    if (pid < 0) throw -1;
    if (! pid) {
      auto argv = gera_argv(cmd);
      execvp(argv[0], argv);
      throw -2;
    }
  }

Terminal::~Terminal() {
    kill(pid, SIGTERM);
    waitpid(pid, NULL, 0);
    close(master);
  }

int Terminal::escreva(const string & data) {
    int n = write(master, data.c_str(), data.size());
    return n;
  }

string Terminal::leia(int N) {
    char data[N];

    sched_yield();
    int n = read(master, data, N);
    if (n > 0) return string(data, n);
    else return "";
  }

string Terminal::leiaTudo() {
    string r;
    
    while (true) {
        string data = leia();
        if (data.size() == 0) break;
        r += data;
    }
    return r;
}

vector<string> Terminal::lePalavras() {
    vector<string> l;
    while (true) {
        string w = lePalavra();
        if (w.size() == 0) break;
        l.push_back(w);
    }
    return l;    
    
}

vector<string> Terminal::leLinhas() {
    vector<string> l;
    
    while (true) {
        string w = leLinha();
        if (w.size() == 0) break;
        l.push_back(w);
    }
    return l;    
}

string Terminal::leAlgo(bool _wait, char c) {
    char data[10240];
    int pos = 0;
    int timeout = 0;

    if (_wait) timeout = -1;

    while (pos < 10240) {
        pollfd fds[2];

        fds[0].fd = master;
        fds[0].events = POLLIN;
        int n = poll(fds, 1, timeout);
        if (n== 0) break;
        timeout = 0; // após ler primeiro caractere ... não espera os demais
        n = read(master, data+pos, 1);
        if (n <= 0) break;
        if (data[pos] == c) {
            break;
        }
        pos++;
    }
    if (pos == 0) return "";
    return string(data, pos);
}

string Terminal::leAte(char c) {
    char data[10240];
    int pos = 0;
    
    while (pos < 10240) {
        sched_yield();
        int n = read(master, data+pos, 1);
        if (n <= 0) break;
        if (data[pos] == c) {
            break;
        }
        pos++;
    }
    if (pos == 0) return "";
    return string(data, pos);
}

string Terminal::leLinha() {
    return leAte('\n');
}

string Terminal::lePalavra() {
    char data[10240];
    int pos = 0;
    bool started = false;
    
    while (true) {
        sched_yield();
        int n = read(master, data+pos, 1);
        if (n <= 0) break;
        if (isspace(data[pos])) {
            if (started) break;
        } else {
            started = true;
            pos++;
        }
    }
    if (pos == 0) return "";
    return string(data, pos);
}

void strip(string &s) {
    int pos = 0;
    while (pos < s.size()) {
        if (!isspace(s[pos])) {
            if (pos > 0) s.erase(0, pos);
            break;
        }
        pos++;
    }
    if (pos == s.size()) {
        s.clear();
        return;
    }

    pos = s.size()-1;
    while (pos >= 0) {
        if (!isspace(s[pos])) {
            if (pos < s.size()-1) s.erase(pos+1);
            break;
        }
        pos--;
    }
}

vector<string> separa(const string & s, const string & delim) {
    vector<string> v;
    if (s.empty()) return v;

    int pos = 0;
    while (pos != string::npos) {
        int pos1 = s.find_first_not_of(delim, pos);
        if (pos1 == string::npos) break;
        pos = s.find_first_of(delim, pos1);
        if (pos == string::npos) {
            v.push_back(s.substr(pos1));
            break;
        } else {
            v.push_back(s.substr(pos1, pos-pos1));
        }
    }
    return v;
}
