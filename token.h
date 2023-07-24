#ifndef TOKEN_H_
#define TOKEN_H_

#include <iostream>

using namespace std;

// Class for token
class token
{
private:
    string type; // Type of token
    string val;  // Value of token

public:
    void setType(const string &sts); // Set type of token
    void setVal(const string &str);  // Set value of token
    string getType();                // Get type of token
    string getVal();                 // Get value of token
    bool operator!=(token t);
};

// Set type of token
void token::setType(const string &str)
{
    type = str;
}

// Set value of token
void token::setVal(const string &str)
{
    val = str;
}

// Get type of token
string token::getType()
{
    return type;
}

// Get value of token
string token::getVal()
{
    return val;
}

#endif
