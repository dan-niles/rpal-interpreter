#include <iostream>
#include <stack>
#include <iterator>
#include <string.h>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include <stdio.h>

using namespace std;

// stack<tree *> st;

string keys[] = {"let", "fn", "in", "where", "aug", "or", "not", "true", "false", "nil", "dummy", "within",
                 "and", "rec", "gr", "ge", "ls", "le", "eq", "ne"};

string ops[] = {"+", "-", "*", "/", "**", "@", "o", "&", "aug", "->", "gr", "ge", "ls", "le", "eq", "ne", "not", "or", "and"};

class parser
{
public:
    // token nt;            // Non-terminal
    char readnew[10000]; // Read new character
    int index;           // Index of character
    int sizeOfFile;      // Size of file
    int astFlag;         // Flag to check if AST or ST is to be printed

    parser(char read_array[], int i, int size, int af)
    {
        copy(read_array, read_array + 10000, readnew);
        index = i;
        sizeOfFile = size;
        astFlag = af;
    }
};