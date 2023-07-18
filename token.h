#ifndef TOKEN_H_
#define TOKEN_H_

#include <iostream>

using namespace std;

class token
{
private:
    string val;
    string type;

public:
    void setVal(const string &str);
    void setType(const string &sts);
    string getVal();
    string getType();
    bool operator!=(token t);
};

void token::setVal(const string &str)
{
    val = str;
}

void token::setType(const string &str)
{
    type = str;
}

string token::getVal()
{
    return val;
}

string token::getType()
{
    return type;
}

#endif
