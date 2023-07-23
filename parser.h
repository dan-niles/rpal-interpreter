#include <iostream>
#include <stack>
#include <iterator>
#include <string.h>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include <stdio.h>
#include "token.h"
#include "tree.h"

using namespace std;

stack<tree *> st;

string keys[] = {"let", "fn", "in", "where", "aug", "or", "not", "true", "false", "nil", "dummy", "within",
                 "and", "rec", "gr", "ge", "ls", "le", "eq", "ne"};

char operators[] = {'+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|', '$', '!', '#', '%',
                    '^', '_', '[', ']', '{', '}', '"', '`', '?'};

class parser
{
public:
    token nt;            // Non-terminal
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

    // Checks if the given string is a keyword
    bool isReservedKey(string str)
    {
        // int size = keys.size();
        int i = 0;
        for (i = 0; i < 20; i++)
        {
            if (str == keys[i])
                return true;
        }
        return false;
    }

    // Checks if the given character is an operator
    bool isOperator(char ch)
    {
        for (int i = 0; i < 25; i++)
        {
            if (ch == operators[i])
            {
                return true;
            }
        }
        return false;
    }

    // Checks if the given character is an alphabet letter
    bool isAlpha(char ch)
    {
        if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
        {
            return true;
        }
        return false;
    }

    // Checks if the given character is a digit
    bool isDigit(char ch)
    {
        if (ch >= 48 && ch <= 57)
        {
            return true;
        }
        return false;
    }

    void read(string val, string type)
    {
        if (val != nt.getVal() || type != nt.getType())
        {
            cout << "Expected : " << val << " - " << type << " but found : " << nt.getVal() << " - " << nt.getType() << endl;
            exit(0);
        }

        if (type == "ID" || type == "INT" || type == "STR")
        {
            build_tree(val, type, 0);
        }

        nt = getToken(readnew);

        while (nt.getType() == "DELETE")
        {
            nt = getToken(readnew);
        }
    }

    void build_tree(string val, string type, int child) {}
};