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
#include "environment.h"

using namespace std;

stack<tree *> st;

string keys[] = {"let", "fn", "in", "where", "aug", "or", "not", "true", "false", "nil", "dummy", "within",
                 "and", "rec", "gr", "ge", "ls", "le", "eq", "ne"};

char operators[] = {'+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|', '$', '!', '#', '%',
                    '^', '_', '[', ']', '{', '}', '"', '`', '?'};

class parser
{
public:
    token nextToken;            // Next token
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

    // Checks if the given string is a binary operator
    bool isBiOper(string op)
    {
        if (op == "+" || op == "-" || op == "*" || op == "/" || op == "**" || op == "gr" || op == "ge" || op == "<" || op == "<=" || op == ">" || op == ">=" || op == "ls" || op == "le" || op == "eq" || op == "ne" || op == "&" || op == "or" || op == "><")
        {
            return true;
        }

        return false;
    }

    // Checks if the given string is a number
    bool is_number(const std::string &s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it))
            ++it;
        return !s.empty() && it == s.end();
    }

    // Read the next token
    void read(string val, string type)
    {
        if (val != nextToken.getVal() || type != nextToken.getType())
        {
            cout << "Expected : " << val << " - " << type << " but found : " << nextToken.getVal() << " - " << nextToken.getType() << endl;
            exit(0);
        }

        if (type == "IDENTIFIER" || type == "INTEGER" || type == "STRING")
        {
            build_tree(val, type, 0);
        }

        nextToken = getToken(readnew);

        while (nextToken.getType() == "DELETE")
        {
            nextToken = getToken(readnew);
        }
    }

    // Build tree for the given string, type and number of children
    void build_tree(string val, string type, int child)
    {
        if (child == 0) // Leaf node
        {
            tree *temp = createNode(val, type);
            st.push(temp);
        }
        else if (child > 0) // Non-leaf node
        {
            stack<tree *> temp;
            int no_of_pops = child;
            while (!st.empty() && no_of_pops > 0)
            {
                temp.push(st.top());
                st.pop();
                no_of_pops--;
            }
            tree *tempLeft = temp.top();
            temp.pop();
            child--;
            if (!temp.empty() && child > 0)
            {
                tree *rightNode = temp.top();
                tempLeft->right = rightNode;
                temp.pop();
                child--;
                while (!temp.empty() && child > 0)
                {
                    tree *addRight = temp.top();
                    temp.pop();
                    rightNode->right = addRight;
                    rightNode = rightNode->right;
                    child--;
                }
            }
            tree *toPush = createNode(val, type);
            toPush->left = tempLeft;
            st.push(toPush);
        }
    }

    // Get the next token
    token getToken(char read[])
    {
        token t;
        int i = index;         // Index of character
        string id = "";        // Identifier
        string num = "";       // Number
        string isop = "";      // Operator
        string isString = "";  // String
        string isPun = "";     // Punctuation
        string isComment = ""; // Comment
        string isSpace = "";   // Space

        // Check if end of file is reached
        if (read[i] == '\0' || i == sizeOfFile)
        {
            t.setType("EOF");
            t.setVal("EOF");
            return t;
        }

        // Read the next token
        while (i < sizeOfFile || i < 10000 || read[i] != '\0')
        {
            // Check if character is a digit
            if (isDigit(read[i]))
            {
                while (isDigit(read[i]))
                {
                    num = num + read[i];
                    i++;
                }
                index = i;
                t.setVal(num);
                t.setType("INTEGER");
                return t;
            }
            // Check if character is an alphabet letter
            else if (isAlpha(read[i]))
            {
                while (isAlpha(read[i]) || isDigit(read[i]) || read[i] == '_')
                {
                    id = id + read[i];
                    i++;
                }

                if (isReservedKey(id))
                {
                    index = i;
                    t.setVal(id);
                    t.setType("KEYWORD");
                    return t;
                }
                else
                {
                    index = i;
                    t.setVal(id);
                    t.setType("IDENTIFIER");
                    return t;
                }
            }
            // Check if character is a comment
            else if (read[i] == '/' && read[i + 1] == '/')
            {
                while (read[i] == '\'' || read[i] == '\\' || read[i] == '(' || read[i] == ')' || read[i] == ';' || read[i] == ',' || read[i] == ' ' || read[i] == '\t' || isAlpha(read[i]) || isDigit(read[i]) || isOperator(read[i]))
                {

                    if (read[i] == '\n')
                    {
                        i++;
                        break;
                    }
                    else
                    {
                        isComment = isComment + read[i];
                        i++;
                    }
                }

                index = i;
                t.setVal(isComment);
                t.setType("DELETE");
                return t;
            }
            // Check if character is an operator
            else if (isOperator(read[i]))
            {
                while (isOperator(read[i]))
                {
                    isop = isop + read[i];
                    i++;
                }

                index = i;
                t.setVal(isop);
                t.setType("OPERATOR");
                return t;
            }

            else if (read[i] == '\'')
            {
                isString = isString + read[i];
                i++;
                while (read[i] == '\'' || read[i] == '\\' || read[i] == '(' || read[i] == ')' || read[i] == ';' || read[i] == ',' || read[i] == ' ' || isAlpha(read[i]) || isDigit(read[i]) || read[i] == '_' || isOperator(read[i]))
                {
                    if (read[i] == '\'')
                    {
                        isString = isString + read[i];
                        i++;
                        break;
                    }
                    else if (read[i] == '\\')
                    {
                        isString = isString + read[i];
                        i++;
                        if (read[i] == 't' || read[i] == 'n' || read[i] == '\\' || read[i] == '"')
                        {
                            isString = isString + read[i];
                            i++;
                        }
                        else
                        {
                            i++;
                        }
                    }
                    else
                    {
                        isString = isString + read[i];
                        i++;
                    }
                }
                index = i;
                t.setVal(isString);
                t.setType("STRING");
                return t;
            }
            else if (read[i] == ')' || read[i] == '(' || read[i] == ';' || read[i] == ',')
            {
                isPun = isPun + read[i];
                i++;

                index = i;
                t.setVal(isPun);
                t.setType("PUNCTION");
                return t;
            }
            else if (isspace(read[i]))
            {
                while (isspace(read[i]))
                {
                    isSpace = isSpace + read[i];
                    i++;
                }
                index = i;
                t.setVal(isSpace);
                t.setType("DELETE");
                return t;
            }
            else
            {
                string temp = "";
                temp = temp + read[i];
                t.setVal(temp);
                t.setType("UNKNOWN");
                i++;
                index = i;
                return t;
            }
        }

        return t;
    }

    // Start parsing
    void parse()
    {
        nextToken = getToken(readnew);          // Get the first token
        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
        {
            nextToken = getToken(readnew);
        }

        procedure_E(); // Start parsing from E

        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
        {
            nextToken = getToken(readnew);
        }

        if (index >= sizeOfFile - 1) // Check if end of file is reached
        {
            tree *t = st.top(); // Get the root of the tree

            // Print the tree
            if (astFlag == 1)
            {
                t->print_tree(0);
            }

            // Make the ST
            makeST(t);

            vector<tree *> delta;
            tree *setOfDeltaArray[200][200];
            myDelta(t, setOfDeltaArray);

            int size = 0;
            int inner = 0;
            while (setOfDeltaArray[size][0] != NULL)
                size++;

            vector<vector<tree *>> setOfDelta;
            for (int i = 0; i < size; i++)
            {
                vector<tree *> temp;
                for (int j = 0; j < 200; j++)
                {
                    if (setOfDeltaArray[i][j] != NULL)
                        temp.push_back(setOfDeltaArray[i][j]);
                }
                setOfDelta.push_back(temp);
            }

            cse_machine(setOfDelta);
        }
    }

    // Control Stack Environment Machine
    void cse_machine(vector<vector<tree *>> &controlStructure)
    {
        stack<tree *> control;
        stack<tree *> machine;
        stack<environment *> stackOfEnvironment; // Stack of environments
        stack<environment *> getCurrEnvironment;

        int currEnvIndex = 0;                     // Initial environment
        environment *currEnv = new environment(); // env0;

        currEnvIndex++;
        machine.push(createNode(currEnv->name, "ENV"));
        control.push(createNode(currEnv->name, "ENV"));
        stackOfEnvironment.push(currEnv);
        getCurrEnvironment.push(currEnv);

        vector<tree *> tempDelta;
        tempDelta = controlStructure.at(0);
        for (int i = 0; i < tempDelta.size(); i++)
        {
            control.push(tempDelta.at(i));
        }

        while (!control.empty())
        {
            tree *nextToken;
            nextToken = control.top();
            control.pop();

            if (nextToken->getVal() == "nil")
            {
                nextToken->setType("tau");
            }

            if (nextToken->getType() == "INTEGER" || nextToken->getType() == "STRING" || nextToken->getVal() == "lambda" || nextToken->getVal() == "YSTAR" || nextToken->getVal() == "Print" || nextToken->getVal() == "Isinteger" || nextToken->getVal() == "Istruthvalue" || nextToken->getVal() == "Isstring" || nextToken->getVal() == "Istuple" || nextToken->getVal() == "Isfunction" || nextToken->getVal() == "Isdummy" || nextToken->getVal() == "Stem" || nextToken->getVal() == "Stern" || (nextToken->getVal() == "Conc" /*&& behind->getVal()=="gamma" && behind2->getVal()=="gamma"*/) || nextToken->getType() == "BOOL" || nextToken->getType() == "NIL" || nextToken->getType() == "DUMMY" || nextToken->getVal() == "Order" || nextToken->getVal() == "nil" || nextToken->getVal() == "ItoS" /*|| isBiOper(nextToken->getVal()) || nextToken->getVal() == "neg" || nextToken->getVal() == "not"*/)
            {
                if (nextToken->getVal() == "lambda")
                {
                    tree *boundVar = control.top();
                    control.pop();
                    tree *nextDeltaIndex = control.top();
                    control.pop();
                    tree *env = createNode(currEnv->name, "ENV");
                    machine.push(nextDeltaIndex); // index of next delta structure to open
                    machine.push(boundVar);       // lambda is bound to this variable
                    machine.push(env);            // then environment it was created in
                    machine.push(nextToken);      // 1 : lambda
                }
                else
                {
                    machine.push(nextToken);
                }
            }
            else if (nextToken->getVal() == "gamma")
            {
                tree *machineTop = machine.top();
                if (machineTop->getVal() == "lambda")
                {
                    machine.pop(); // popped lambda

                    tree *prevEnv = machine.top();
                    machine.pop(); // popped the evn in which it was created

                    tree *boundVar = machine.top();
                    machine.pop(); // bound variable of lambda

                    tree *nextDeltaIndex = machine.top();
                    machine.pop(); // next delta to be opened

                    environment *newEnv = new environment(); // env0;

                    std::stringstream ss;
                    ss << currEnvIndex;
                    string str = ss.str(); // creating the string of current evn number

                    newEnv->name = "env" + str;

                    if (currEnvIndex > 2000)
                        return;

                    stack<environment *> tempEnv = stackOfEnvironment;

                    while (tempEnv.top()->name != prevEnv->getVal())
                    {
                        tempEnv.pop();
                    }
                    newEnv->prev = tempEnv.top();

                    if (boundVar->getVal() == "," && machine.top()->getVal() == "tau")
                    {
                        // machine.pop()
                        vector<tree *> boundVariables;
                        tree *leftOfComa = boundVar->left;
                        while (leftOfComa != NULL)
                        {
                            boundVariables.push_back(createNode(leftOfComa));
                            leftOfComa = leftOfComa->right;
                        }

                        vector<tree *> boundValues;
                        tree *tau = machine.top(); // popping the tau;
                        machine.pop();

                        tree *tauLeft = tau->left;
                        while (tauLeft != NULL)
                        {
                            boundValues.push_back(tauLeft);
                            tauLeft = tauLeft->right;
                        }

                        for (int i = 0; i < boundValues.size(); i++)
                        {
                            if (boundValues.at(i)->getVal() == "tau")
                            {
                                stack<tree *> res;
                                printTuple(boundValues.at(i), res);
                            }

                            vector<tree *> listOfNodeVal;
                            listOfNodeVal.push_back(boundValues.at(i));
                            newEnv->boundVar.insert(std::pair<tree *, vector<tree *>>(boundVariables.at(i), listOfNodeVal));
                        }
                    }
                    else if (machine.top()->getVal() == "eta")
                    {
                        vector<tree *> listOfNodeVal;
                        stack<tree *> temp;
                        int j = 0;
                        while (j < 4)
                        {
                            temp.push(machine.top());
                            machine.pop();
                            j++;
                        }

                        while (!temp.empty())
                        {
                            tree *fromStack;
                            fromStack = temp.top();
                            temp.pop();
                            listOfNodeVal.push_back(fromStack);
                        }

                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *>>(boundVar, listOfNodeVal));
                    }
                    else if (machine.top()->getVal() == "lambda")
                    {
                        vector<tree *> listOfNodeVal;
                        stack<tree *> temp;
                        int j = 0;
                        while (j < 4)
                        {
                            temp.push(machine.top());
                            machine.pop();
                            j++;
                        }

                        while (!temp.empty())
                        {
                            tree *fromStack;
                            fromStack = temp.top();
                            temp.pop();
                            listOfNodeVal.push_back(fromStack);
                        }
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *>>(boundVar, listOfNodeVal));
                    }
                    else if (machine.top()->getVal() == "Conc")
                    {
                        vector<tree *> listOfNodeVal;
                        stack<tree *> temp;
                        int j = 0;
                        while (j < 2)
                        {
                            temp.push(machine.top());
                            machine.pop();
                            j++;
                        }

                        while (!temp.empty())
                        {
                            tree *fromStack;
                            fromStack = temp.top();
                            temp.pop();
                            listOfNodeVal.push_back(fromStack);
                        }
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *>>(boundVar, listOfNodeVal));
                    }
                    else
                    {
                        tree *bindVarVal = machine.top();

                        machine.pop();
                        vector<tree *> listOfNodeVal;
                        listOfNodeVal.push_back(bindVarVal);
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *>>(boundVar, listOfNodeVal));
                    }

                    currEnv = newEnv;
                    control.push(createNode(currEnv->name, "ENV"));
                    machine.push(createNode(currEnv->name, "ENV"));
                    stackOfEnvironment.push(currEnv);
                    getCurrEnvironment.push(currEnv);

                    istringstream is3(nextDeltaIndex->getVal());
                    int deltaIndex;
                    is3 >> deltaIndex;

                    vector<tree *> nextDelta = controlStructure.at(deltaIndex);
                    for (int i = 0; i < nextDelta.size(); i++)
                    {
                        control.push(nextDelta.at(i));
                    }
                    currEnvIndex++;
                }
                else if (machineTop->getVal() == "tau")
                {

                    tree *tau = machine.top(); // saved the tau with kids on left to right;
                    machine.pop();
                    tree *selectChildNum = machine.top();
                    machine.pop();

                    istringstream is4(selectChildNum->getVal());
                    int childIndex;
                    is4 >> childIndex;
                    tree *tauLeft = tau->left;
                    tree *selectedChild;
                    while (childIndex > 1)
                    {
                        childIndex--;
                        tauLeft = tauLeft->right;
                    }
                    selectedChild = createNode(tauLeft);
                    if (selectedChild->getVal() == "lamdaTuple")
                    {
                        tree *getNode = selectedChild->left;
                        while (getNode != NULL)
                        {
                            machine.push(createNode(getNode));
                            getNode = getNode->right;
                        }
                    }
                    else
                    {
                        machine.push(selectedChild);
                    }
                }
                else if (machineTop->getVal() == "YSTAR")
                {
                    machine.pop(); // pop the YSTAR
                    if (machine.top()->getVal() == "lambda")
                    {
                        tree *etaNode = createNode(machine.top()->getVal(), machine.top()->getType()); // getting eta
                        etaNode->setVal("eta");
                        machine.pop();
                        tree *boundEvn1 = machine.top(); // getting bound evn
                        machine.pop();
                        tree *boundVar1 = machine.top(); // getting bound var
                        machine.pop();
                        tree *deltaIndex1 = machine.top(); // getting delta index
                        machine.pop();

                        machine.push(deltaIndex1); // pushing eta node 1
                        machine.push(boundVar1);   // 2
                        machine.push(boundEvn1);   // 3
                        machine.push(etaNode);     // eta completed 4
                    }
                    else
                    {
                        cout << " PROBLEM WITH RECURSSION ******** " << machine.top()->getVal() << endl;
                        return;
                    }
                }
                else if (machineTop->getVal() == "eta")
                {
                    tree *eta = machine.top(); // getting eta
                    machine.pop();
                    tree *boundEvn1 = machine.top(); // getting bound evn
                    machine.pop();
                    tree *boundVar1 = machine.top(); // getting bound var
                    machine.pop();
                    tree *deltaIndex1 = machine.top(); // getting delta index
                    machine.pop();

                    machine.push(deltaIndex1); // pushing eta node 1
                    machine.push(boundVar1);   // 2
                    machine.push(boundEvn1);   // 3
                    machine.push(eta);         // eta completed 4

                    machine.push(deltaIndex1);                     // 1
                    machine.push(boundVar1);                       // 2
                    machine.push(boundEvn1);                       // 3
                    machine.push(createNode("lambda", "KEYWORD")); // 4
                    control.push(createNode("gamma", "KEYWORD"));
                    control.push(createNode("gamma", "KEYWORD"));
                }
                else if (machineTop->getVal() == "Print")
                {
                    machine.pop();
                    tree *nextToPrint = machine.top();
                    if (nextToPrint->getVal() == "lambda")
                    {
                        machine.pop();
                        tree *env = machine.top();
                        machine.pop();
                        tree *boundVar = machine.top();
                        machine.pop();
                        tree *num = machine.top();
                        machine.pop();
                        cout << "[lambda closure: " << boundVar->getVal() << ": " << num->getVal() << "]";
                        return;
                    }
                    else if (nextToPrint->getVal() == "tau")
                    {
                        tree *getTau = machine.top();
                        stack<tree *> res;
                        printTuple(getTau, res);
                        stack<tree *> getRev;
                        while (!res.empty())
                        {
                            getRev.push(res.top());
                            res.pop();
                        }
                        cout << "(";
                        while (getRev.size() > 1)
                        {
                            if (getRev.top()->getType() == "STRING")
                                cout << addSpaces(getRev.top()->getVal()) << ", ";
                            else
                                cout << getRev.top()->getVal() << ", ";
                            getRev.pop();
                        }
                        if (getRev.top()->getType() == "STRING")
                            cout << addSpaces(getRev.top()->getVal()) << ")";
                        else
                            cout << getRev.top()->getVal() << ")";
                        getRev.pop();
                    }
                    else
                    {

                        if (machine.top()->getType() == "STRING")
                            cout << addSpaces(machine.top()->getVal());
                        else
                            cout << machine.top()->getVal();
                    }
                }
                else if (machineTop->getVal() == "Isinteger")
                {
                    machine.pop();
                    tree *isNextInt = machine.top();
                    machine.pop();
                    if (isNextInt->getType() == "INTEGER")
                    {
                        tree *resNode = createNode("true", "boolean");
                        machine.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "boolean");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Istruthvalue")
                {
                    machine.pop();
                    tree *isNextBool = machine.top();
                    machine.pop();
                    if (isNextBool->getVal() == "true" || isNextBool->getVal() == "false")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        machine.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Isstring")
                {
                    machine.pop();
                    tree *isNextString = machine.top();
                    machine.pop();
                    if (isNextString->getType() == "STRING")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        machine.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Istuple")
                {
                    machine.pop();
                    tree *isNextTau = machine.top();
                    machine.pop();
                    if (isNextTau->getType() == "tau")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        machine.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Isfunction")
                {
                    machine.pop();
                    tree *isNextFn = machine.top();

                    if (isNextFn->getVal() == "lambda")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        machine.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Isdummy")
                {
                    machine.pop();
                    tree *isNextDmy = machine.top();
                    if (isNextDmy->getVal() == "dummy")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        machine.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Stern")
                {
                    machine.pop();
                    tree *isNextString = machine.top();

                    if (isNextString->getVal() == "")
                    {
                        cout << " empty string" << endl;
                        return;
                    }

                    if (isNextString->getType() == "STRING")
                    {
                        string strRes = "'" + isNextString->getVal().substr(2, isNextString->getVal().length() - 3) + "'";
                        machine.pop();
                        machine.push(createNode(strRes, "STRING"));
                    }
                }
                else if (machineTop->getVal() == "Stem")
                {
                    machine.pop();
                    tree *isNextString = machine.top();

                    if (isNextString->getVal() == "")
                    {
                        cout << " empty string" << endl;
                        return;
                    }

                    if (isNextString->getType() == "STRING")
                    {
                        string strRes = "'" + isNextString->getVal().substr(1, 1) + "'";
                        machine.pop();
                        machine.push(createNode(strRes, "STRING"));
                    }
                }
                else if (machineTop->getVal() == "Order")
                {

                    machine.pop();

                    tree *getTau = machine.top();
                    tree *saveTau = getTau;

                    int numOfKids = 0;

                    if (getTau->left != NULL)
                        getTau = getTau->left;

                    while (getTau != NULL)
                    {
                        numOfKids++;
                        getTau = getTau->right;
                    }
                    getTau = machine.top();
                    machine.pop();

                    if ((getTau->getVal() == "nil"))
                    {
                        numOfKids = 0;
                    }

                    stringstream ss11;
                    ss11 << numOfKids;
                    string str34 = ss11.str();
                    tree *orderNode = createNode(str34, "INTEGER");
                    machine.push(orderNode);
                }
                else if (machineTop->getVal() == "Conc")
                {
                    tree *concNode = machine.top();
                    machine.pop();
                    tree *firstString = machine.top();
                    machine.pop();
                    tree *secondString = machine.top();
                    if (secondString->getType() == "STRING" || (secondString->getType() == "STRING" && secondString->left != NULL && secondString->left->getVal() == "true"))
                    {
                        machine.pop();
                        string res = "'" + firstString->getVal().substr(1, firstString->getVal().length() - 2) + secondString->getVal().substr(1, secondString->getVal().length() - 2) + "'";
                        tree *resNode = createNode(res, "STRING");
                        machine.push(resNode);
                        control.pop();
                    }
                    else
                    {
                        concNode->left = firstString;
                        machine.push(concNode);
                        firstString->left = createNode("true", "flag");
                    }
                }
                else if (machineTop->getVal() == "ItoS")
                {
                    machine.pop(); // popping ItoS
                    tree *convertToString = machine.top();
                    machine.pop();
                    machine.push(createNode("'" + convertToString->getVal() + ",", "STRING"));
                }
            }
            else if (nextToken->getVal().substr(0, 3) == "env")
            {
                stack<tree *> removeFromMachineToPutBack;
                if (machine.top()->getVal() == "lambda")
                {
                    removeFromMachineToPutBack.push(machine.top());
                    machine.pop();
                    removeFromMachineToPutBack.push(machine.top());
                    machine.pop();
                    removeFromMachineToPutBack.push(machine.top());
                    machine.pop();
                    removeFromMachineToPutBack.push(machine.top());
                    machine.pop();
                }
                else
                {
                    removeFromMachineToPutBack.push(machine.top());
                    machine.pop();
                }
                tree *remEnv = machine.top();

                if (nextToken->getVal() == remEnv->getVal())
                {
                    machine.pop();

                    stack<tree *> printMachine = machine;

                    getCurrEnvironment.pop();
                    if (!getCurrEnvironment.empty())
                    {
                        currEnv = getCurrEnvironment.top();
                    }
                    else
                    {
                        currEnv = NULL;
                    }
                }
                else
                {
                    cout << "error evn do not match";
                    return;
                }

                while (!removeFromMachineToPutBack.empty())
                {
                    machine.push(removeFromMachineToPutBack.top());
                    removeFromMachineToPutBack.pop();
                }
            }
            else if (nextToken->getType() == "IDENTIFIER" && nextToken->getVal() != "Print" && nextToken->getVal() != "Isinteger" && nextToken->getVal() != "Istruthvalue" && nextToken->getVal() != "Isstring" && nextToken->getVal() != "Istuple" && nextToken->getVal() != "Isfunction" && nextToken->getVal() != "Isdummy" && nextToken->getVal() != "Stem" && nextToken->getVal() != "Stern" && nextToken->getVal() != "Conc")
            {
                environment *temp = currEnv;
                int flag = 0;
                while (temp != NULL)
                {

                    map<tree *, vector<tree *>>::iterator itr = temp->boundVar.begin();
                    while (itr != temp->boundVar.end())
                    {
                        if (nextToken->getVal() == itr->first->getVal())
                        {
                            vector<tree *> temp = itr->second;
                            if (temp.size() == 1 && temp.at(0)->getVal() == "Conc" && temp.at(0)->left != NULL)
                            {
                                control.push(createNode("gamma", "KEYWORD"));
                                machine.push(temp.at(0)->left);
                                machine.push(temp.at(0));
                            }
                            else
                            {
                                int i = 0;
                                while (i < temp.size())
                                {
                                    if (temp.at(i)->getVal() == "lamdaTuple")
                                    {
                                        tree *myLambda = temp.at(i)->left;
                                        while (myLambda != NULL)
                                        {
                                            machine.push(createNode(myLambda));
                                            myLambda = myLambda->right;
                                        }
                                    }
                                    else
                                    {
                                        if (temp.at(i)->getVal() == "tau")
                                        {
                                            stack<tree *> res;
                                            printTuple(temp.at(i), res);
                                        }
                                        machine.push(temp.at(i));
                                    }
                                    i++;
                                }
                            }
                            flag = 1;
                            break;
                        }
                        itr++;
                    }
                    if (flag == 1)
                        break;
                    temp = temp->prev;
                }
                if (flag == 0)
                    return;
            }
            else if (isBiOper(nextToken->getVal()) || nextToken->getVal() == "neg" || nextToken->getVal() == "not")
            {
                string op = nextToken->getVal();
                if (isBiOper(nextToken->getVal()))
                {
                    tree *node1 = machine.top();
                    machine.pop();
                    tree *node2 = machine.top();
                    machine.pop();
                    if (node1->getType() == "INTEGER" && node2->getType() == "INTEGER")
                    {
                        int num1;
                        int num2;

                        istringstream is1(node1->getVal());
                        is1 >> num1;
                        istringstream is2(node2->getVal());
                        is2 >> num2;

                        int res = 0;
                        double resPow;
                        if (op == "+")
                        {
                            res = num1 + num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INTEGER");
                            machine.push(res);
                        }
                        else if (op == "-")
                        {
                            res = num1 - num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INTEGER");
                            machine.push(res);
                        }
                        else if (op == "*")
                        {
                            res = num1 * num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INTEGER");
                            machine.push(res);
                        }
                        else if (op == "/")
                        {
                            if (num2 == 0)
                                cout << " divide by zero " << endl;
                            res = num1 / num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INTEGER");
                            machine.push(res);
                        }
                        else if (op == "**")
                        {
                            resPow = pow((double)num1, (double)num2);
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INTEGER");
                            machine.push(res);
                        }
                        else if (op == "gr" || op == ">")
                        {
                            string resStr = num1 > num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            machine.push(res);
                        }
                        else if (op == "ge" || op == ">=")
                        {
                            string resStr = num1 >= num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            machine.push(res);
                        }
                        else if (op == "ls" || op == "<")
                        {
                            string resStr = num1 < num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            machine.push(res);
                        }
                        else if (op == "le" || op == "<=")
                        {
                            string resStr = num1 <= num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            machine.push(res);
                        }
                        else if (op == "eq" || op == "=")
                        {
                            string resStr = num1 == num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            machine.push(res);
                        }
                        else if (op == "ne" || op == "><")
                        {
                            string resStr = num1 != num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            machine.push(res);
                        }
                    }
                    else if (node1->getType() == "STRING" && node2->getType() == "STRING")
                    {
                        if (op == "ne" || op == "<>")
                        {
                            string resStr = node1->getVal() != node2->getVal() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            machine.push(res);
                        }
                        else if (op == "eq" || op == "==")
                        {
                            string resStr = node1->getVal() == node2->getVal() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            machine.push(res);
                        }
                    }
                    else if ((node1->getVal() == "true" || node1->getVal() == "false") && (node2->getVal() == "false" || node2->getVal() == "true"))
                    {
                        if (op == "ne" || op == "<>")
                        {
                            string resStr = node1->getVal() != node2->getVal() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            machine.push(res);
                        }
                        else if (op == "eq" || op == "==")
                        {
                            string resStr = node1->getVal() == node2->getVal() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            machine.push(res);
                        }
                        else if (op == "or")
                        {
                            string resStr;
                            if (node1->getVal() == "true" || node2->getVal() == "true")
                            {
                                resStr = "true";
                                tree *res = createNode(resStr, "BOOL");
                                machine.push(res);
                            }
                            else
                            {
                                resStr = "false";
                                tree *res = createNode(resStr, "BOOL");
                                machine.push(res);
                            }
                        }
                        else if (op == "&")
                        {
                            string resStr;
                            if (node1->getVal() == "true" && node2->getVal() == "true")
                            {
                                resStr = "true";
                                tree *res = createNode(resStr, "BOOL");
                                machine.push(res);
                            }
                            else
                            {
                                resStr = "false";
                                tree *res = createNode(resStr, "BOOL");
                                machine.push(res);
                            }
                        }
                    }
                }
                else if (op == "neg" || op == "not")
                {
                    if (op == "neg")
                    {
                        tree *node1 = machine.top();
                        machine.pop();
                        istringstream is1(node1->getVal());
                        int num1;
                        is1 >> num1;
                        int res = -num1;
                        stringstream ss;
                        ss << res;
                        string str = ss.str();
                        tree *resStr = createNode(str, "INTEGER");
                        machine.push(resStr);
                    }
                    else if (op == "not" && (machine.top()->getVal() == "true" || machine.top()->getVal() == "false"))
                    {
                        if (machine.top()->getVal() == "true")
                        {
                            machine.pop();
                            machine.push(createNode("false", "BOOL"));
                        }
                        else
                        {
                            machine.pop();
                            machine.push(createNode("true", "BOOL"));
                        }
                    }
                }
                else
                {
                    cout << " WTF op " << nextToken->getVal() << endl;
                }
            }
            else if (nextToken->getVal() == "beta")
            {
                tree *truthVal = machine.top();
                machine.pop();
                tree *elseIndex = control.top();
                control.pop();
                tree *thenIndex = control.top();
                control.pop();

                int index;
                if (truthVal->getVal() == "true")
                {
                    istringstream is1(thenIndex->getVal());
                    is1 >> index;
                }
                else
                {
                    istringstream is1(elseIndex->getVal());
                    is1 >> index;
                }
                vector<tree *> nextDelta = controlStructure.at(index);
                for (int i = 0; i < nextDelta.size(); i++)
                {
                    control.push(nextDelta.at(i));
                }
            }
            else if (nextToken->getVal() == "tau")
            {
                tree *myTuple = createNode("tau", "tau");

                tree *noOfChild = control.top(); // behind tua is the no of kids of tau
                control.pop();

                int numOfKids;

                istringstream is1(noOfChild->getVal());
                is1 >> numOfKids;

                if (machine.top()->getVal() == "lambda")
                {
                    tree *lamda = createNode(machine.top()->getVal(), machine.top()->getType()); // popped lambda
                    machine.pop();

                    tree *prevEnv = createNode(machine.top()->getVal(), machine.top()->getType());
                    machine.pop(); // popped the evn in which it was created

                    tree *boundVar = createNode(machine.top()->getVal(), machine.top()->getType());
                    machine.pop(); // bound variable of lambda

                    tree *nextDeltaIndex = createNode(machine.top()->getVal(), machine.top()->getType());
                    machine.pop(); // next delta to be opened

                    tree *myLambda = createNode("lamdaTuple", "lamdaTuple");
                    myLambda->left = nextDeltaIndex;
                    nextDeltaIndex->right = boundVar;
                    boundVar->right = prevEnv;
                    prevEnv->right = lamda;
                    myTuple->left = myLambda;
                }
                else
                {
                    myTuple->left = createNode(machine.top()); // removing right link
                    machine.pop();
                }

                tree *sibling = myTuple->left;

                for (int i = 1; i < numOfKids; i++)
                {
                    tree *temp = machine.top();
                    if (temp->getVal() == "lambda")
                    {
                        tree *lamda = createNode(machine.top()->getVal(), machine.top()->getType()); // popped lambda
                        machine.pop();

                        tree *prevEnv = createNode(machine.top()->getVal(), machine.top()->getType());
                        machine.pop(); // popped the evn in which it was created

                        tree *boundVar = createNode(machine.top()->getVal(), machine.top()->getType());
                        machine.pop(); // bound variable of lambda

                        tree *nextDeltaIndex = createNode(machine.top()->getVal(), machine.top()->getType());
                        machine.pop(); // next delta to be opened

                        tree *myLambda = createNode("lamdaTuple", "lamdaTuple");

                        myLambda->left = nextDeltaIndex;
                        nextDeltaIndex->right = boundVar;
                        boundVar->right = prevEnv;
                        prevEnv->right = lamda;
                        sibling->right = myLambda;
                        sibling = sibling->right;
                    }
                    else
                    {
                        machine.pop();
                        sibling->right = temp;
                        sibling = sibling->right;
                    }
                }
                machine.push(myTuple);
            }
            else if (nextToken->getVal() == "aug")
            {
                tree *tokenOne = createNode(machine.top());
                machine.pop();
                tree *tokenTwo = createNode(machine.top());
                machine.pop();
                if (tokenOne->getVal() == "nil" && tokenTwo->getVal() == "nil")
                {
                    tree *myNilTuple = createNode("tau", "tau");
                    myNilTuple->left = tokenOne;
                    machine.push(myNilTuple);
                }
                else if (tokenOne->getVal() == "nil")
                {
                    tree *myNilTuple = createNode("tau", "tau");
                    myNilTuple->left = tokenTwo;
                    machine.push(myNilTuple);
                }
                else if (tokenTwo->getVal() == "nil")
                {
                    tree *myNilTuple = createNode("tau", "tau");
                    myNilTuple->left = tokenOne;
                    machine.push(myNilTuple);
                }
                else if (tokenOne->getType() != "tau")
                {
                    tree *myTuple = tokenTwo->left;
                    while (myTuple->right != NULL)
                    {
                        myTuple = myTuple->right;
                    }
                    myTuple->right = createNode(tokenOne);
                    machine.push(tokenTwo);
                }
                else if (tokenTwo->getType() != "tau")
                {
                    tree *myTuple = tokenOne->left;
                    while (myTuple->right != NULL)
                    {
                        myTuple = myTuple->right;
                    }
                    myTuple->right = createNode(tokenTwo);
                    machine.push(tokenOne);
                }
                else
                {
                    tree *myNilTuple = createNode("tau", "tau");
                    myNilTuple->left = tokenOne;
                    myNilTuple->left->right = tokenTwo;
                    machine.push(myNilTuple);
                }
            }
        }

        cout << endl;
    }

    void printTuple(tree *tauNode, stack<tree *> &res)
    {
        if (tauNode == NULL)
            return;
        if (tauNode->getVal() == "lamdaTuple")
            return;
        if (tauNode->getVal() != "tau" && tauNode->getVal() != "nil")
        {
            res.push(tauNode);
        }
        printTuple(tauNode->left, res);
        printTuple(tauNode->right, res);
    }

    string addSpaces(string temp)
    {
        for (int i = 1; i < temp.length(); i++)
        {
            if (temp[i - 1] == 92 && (temp[i] == 'n'))
            {
                temp.replace(i - 1, 2, "\\\n");
            }
            if (temp[i - 1] == 92 && (temp[i] == 't'))
            {
                temp.replace(i - 1, 2, "\\\t");
            }
        }

        for (int i = 0; i < temp.length(); i++)
        {
            if (temp[i] == 92 || temp[i] == 39)
            {
                temp.replace(i, 1, "");
            }
        }
        return temp;
    }

    void myDelta(tree *x, tree *(*setOfDelta)[200])
    {
        static int index = 1;
        static int j = 0;
        static int i = 0;
        static int betaCount = 1;
        if (x == NULL)
            return;

        if (x->getVal() == "lambda")
        {
            std::stringstream ss;

            int t1 = i;
            int k = 0;
            setOfDelta[i][j] = createNode("", "");
            i = 0;

            while (setOfDelta[i][0] != NULL)
            {
                i++;
                k++;
            }
            i = t1;
            ss << k;
            index++;

            string str = ss.str();
            tree *temp = createNode(str, "deltaNumber");

            setOfDelta[i][j++] = temp;

            setOfDelta[i][j++] = x->left;

            setOfDelta[i][j++] = x;

            int myStoredIndex = i;
            int tempj = j + 3;

            while (setOfDelta[i][0] != NULL)
                i++;
            j = 0;

            myDelta(x->left->right, setOfDelta);

            i = myStoredIndex;
            j = tempj;
        }
        else if (x->getVal() == "->")
        {
            int myStoredIndex = i;
            int tempj = j;
            int nextDelta = index;
            int k = i;

            std::stringstream ss2;
            ss2 << nextDelta;
            string str2 = ss2.str();
            tree *temp1 = createNode(str2, "deltaNumber");

            setOfDelta[i][j++] = temp1;

            int nextToNextDelta = index;
            std::stringstream ss3;
            ss3 << nextToNextDelta;
            string str3 = ss3.str();
            tree *temp2 = createNode(str3, "deltaNumber");
            setOfDelta[i][j++] = temp2;

            tree *beta = createNode("beta", "beta");

            setOfDelta[i][j++] = beta;

            while (setOfDelta[k][0] != NULL)
            {
                k++;
            }
            int firstIndex = k;
            int lamdaCount = index;

            myDelta(x->left, setOfDelta);
            int diffLc = index - lamdaCount;

            while (setOfDelta[i][0] != NULL)
                i++;
            j = 0;

            myDelta(x->left->right, setOfDelta);

            while (setOfDelta[i][0] != NULL)
                i++;
            j = 0;

            myDelta(x->left->right->right, setOfDelta);

            stringstream ss23;
            if (diffLc == 0 || i < lamdaCount)
            {
                ss23 << firstIndex;
            }
            else
            {
                ss23 << i - 1;
            }

            string str5 = ss23.str();

            setOfDelta[myStoredIndex][tempj]->setVal(str5);
            stringstream ss24;
            ss24 << i;
            string str6 = ss24.str();

            setOfDelta[myStoredIndex][tempj + 1]->setVal(str6);

            i = myStoredIndex;
            j = 0;

            while (setOfDelta[i][j] != NULL)
            {
                j++;
            }
            // j=0;
            betaCount += 2;
        }
        else if (x->getVal() == "tau")
        {
            tree *tauLeft = x->left;
            int numOfChildren = 0;
            while (tauLeft != NULL)
            {
                numOfChildren++;
                tauLeft = tauLeft->right;
            }
            std::stringstream ss;
            ss << numOfChildren;
            string str = ss.str();
            tree *countNode = createNode(str, "CHILDCOUNT");

            setOfDelta[i][j++] = countNode; // putting the number of kids of tua
            tree *tauNode = createNode("tau", "tau");

            setOfDelta[i][j++] = tauNode; // putting the tau node and not pushing x

            myDelta(x->left, setOfDelta);
            x = x->left;
            while (x != NULL)
            {
                myDelta(x->right, setOfDelta);
                x = x->right;
            }
        }
        else
        {
            setOfDelta[i][j++] = createNode(x->getVal(), x->getType());
            myDelta(x->left, setOfDelta);
            if (x->left != NULL)
                myDelta(x->left->right, setOfDelta);
        }
    }

    void makeST(tree *t)
    {
        // standardizeAnd(t);
        makeStandard(t);
    }

    tree *standardizeAnd(tree *t)
    {
        if (t == NULL)
            return NULL;

        if (t->getVal() == "and")
        {
            tree *equal = t->left;
            t->setVal("=");
            t->setType("KEYWORD");
            t->left = createNode(",", "PUNCTION");
            tree *comma = t->left;
            comma->left = createNode(equal->left);
            t->left->right = createNode("tau", "KEYWORD");
            tree *tau = t->left->right;

            tau->left = createNode(equal->left->right);
            tau = tau->left;
            comma = comma->left;
            equal = equal->right;

            while (equal != NULL)
            {
                comma->right = createNode(equal->left);
                comma = comma->right;
                tau->right = createNode(equal->left->right);
                tau = tau->right;

                equal = equal->right;
            }
        }

        standardizeAnd(t->left);
        standardizeAnd(t->right);

        return t;
    }

    tree *makeStandard(tree *t)
    {
        if (t == NULL)
            return NULL;
        makeStandard(t->left);
        makeStandard(t->right);

        if (t->getVal() == "let")
        {
            if (t->left->getVal() == "=")
            {
                t->setVal("gamma");
                t->setType("KEYWORD");
                tree *P = createNode(t->left->right);
                tree *X = createNode(t->left->left);
                tree *E = createNode(t->left->left->right);
                t->left = createNode("lambda", "KEYWORD");
                t->left->right = E; // do i set the right pointer to null
                tree *lambda = t->left;
                lambda->left = X;
                lambda->left->right = P;
            }
        }
        else if (t->getVal() == "and" && t->left->getVal() == "=")
        {
            tree *equal = t->left;
            t->setVal("=");
            t->setType("KEYWORD");
            t->left = createNode(",", "PUNCTION");
            tree *comma = t->left;
            comma->left = createNode(equal->left);
            t->left->right = createNode("tau", "KEYWORD");
            tree *tau = t->left->right;

            tau->left = createNode(equal->left->right);
            tau = tau->left;
            comma = comma->left;
            equal = equal->right;

            while (equal != NULL)
            {
                comma->right = createNode(equal->left);
                comma = comma->right;
                tau->right = createNode(equal->left->right);
                tau = tau->right;

                equal = equal->right;
            }
        }
        else if (t->getVal() == "where")
        {
            t->setVal("gamma");
            t->setType("KEYWORD");
            if (t->left->right->getVal() == "=")
            {
                tree *P = createNode(t->left);
                tree *X = createNode(t->left->right->left);
                tree *E = createNode(t->left->right->left->right);
                t->left = createNode("lambda", "KEYWORD");
                t->left->right = E;
                t->left->left = X;
                t->left->left->right = P;
            }
        }
        else if (t->getVal() == "within")
        {
            if (t->left->getVal() == "=" && t->left->right->getVal() == "=")
            {
                tree *X1 = createNode(t->left->left);
                tree *E1 = createNode(t->left->left->right);
                tree *X2 = createNode(t->left->right->left);
                tree *E2 = createNode(t->left->right->left->right);
                t->setVal("=");
                t->setType("KEYWORD");
                t->left = X2;
                t->left->right = createNode("gamma", "KEYWORD");
                tree *temp = t->left->right;
                temp->left = createNode("lambda", "KEYWORD");
                temp->left->right = E1;
                temp = temp->left;
                temp->left = X1;
                temp->left->right = E2;
            }
        }
        else if (t->getVal() == "rec" && t->left->getVal() == "=")
        {
            tree *X = createNode(t->left->left);
            tree *E = createNode(t->left->left->right);

            t->setVal("=");
            t->setType("KEYWORD");
            t->left = X;
            t->left->right = createNode("gamma", "KEYWORD");
            t->left->right->left = createNode("YSTAR", "KEYWORD");
            tree *ystar = t->left->right->left;

            ystar->right = createNode("lambda", "KEYWORD");

            ystar->right->left = createNode(X);
            ystar->right->left->right = createNode(E);
        }
        else if (t->getVal() == "function_form")
        {
            tree *P = createNode(t->left);
            tree *V = t->left->right;

            t->setVal("=");
            t->setType("KEYWORD");
            t->left = P;

            tree *temp = t;
            while (V->right->right != NULL)
            {
                temp->left->right = createNode("lambda", "KEYWORD");
                temp = temp->left->right;
                temp->left = createNode(V);
                V = V->right;
            }

            temp->left->right = createNode("lambda", "KEYWORD");
            temp = temp->left->right;

            temp->left = createNode(V);
            temp->left->right = V->right;
        }
        else if (t->getVal() == "lambda")
        {
            if (t->left != NULL)
            {
                tree *V = t->left;
                tree *temp = t;
                if (V->right != NULL && V->right->right != NULL)
                {
                    while (V->right->right != NULL)
                    {
                        temp->left->right = createNode("lambda", "KEYWORD");
                        temp = temp->left->right;
                        temp->left = createNode(V);
                        V = V->right;
                    }

                    temp->left->right = createNode("lambda", "KEYWORD");
                    temp = temp->left->right;
                    temp->left = createNode(V);
                    temp->left->right = V->right;
                }
            }
        }
        else if (t->getVal() == "@")
        {
            tree *E1 = createNode(t->left);
            tree *N = createNode(t->left->right);
            tree *E2 = createNode(t->left->right->right);
            t->setVal("gamma");
            t->setType("KEYWORD");
            t->left = createNode("gamma", "KEYWORD");
            t->left->right = E2;
            t->left->left = N;
            t->left->left->right = E1;
        }

        return NULL;
    }

    /* -------------------------------- Grammar Rules -------------------------------- */

    // E -> ’let’ D ’in’ E | ’fn’ Vb+ ’.’ E | Ew
    void procedure_E()
    {
        // E -> ’let’ D ’in’ E
        if (nextToken.getVal() == "let")
        {
            read("let", "KEYWORD");
            procedure_D();

            read("in", "KEYWORD"); // read in
            procedure_E();

            build_tree("let", "KEYWORD", 2);
        }
        // E -> ’fn’ Vb+ ’.’ E
        else if (nextToken.getVal() == "fn")
        {
            int n = 0;
            read("fn", "KEYWORD");
            do
            {
                procedure_Vb();
                n++;
            } while (nextToken.getType() == "IDENTIFIER" || nextToken.getVal() == "(");
            read(".", "OPERATOR");
            procedure_E();

            build_tree("lambda", "KEYWORD", n + 1);
        }
        // E -> Ew
        else
        {
            procedure_Ew();
        }
    }

    // Ew ->  T ’where’ Dr | T
    void procedure_Ew()
    {
        procedure_T();

        if (nextToken.getVal() == "where") // If next token is 'where', then rule is, Ew ->  T ’where’ Dr | T
        {
            read("where", "KEYWORD");
            procedure_Dr();
            build_tree("where", "KEYWORD", 2);
        }
    }

    // T -> Ta ( ’,’ Ta )+ | Ta
    void procedure_T()
    {
        procedure_Ta();

        int n = 1;
        while (nextToken.getVal() == ",") // Iterate and read ","; Rule is T -> Ta ( ’,’ Ta )+
        {
            n++;
            read(",", "PUNCTION");
            procedure_Ta();
        }
        if (n > 1)
        {
            build_tree("tau", "KEYWORD", n);
        }
    }

    // Ta -> Ta ’aug’ Tc | Tc
    void procedure_Ta()
    {
        procedure_Tc();

        while (nextToken.getVal() == "aug") // If next token is 'aug', then rule is, Ta -> Ta ’aug’ Tc
        {
            read("aug", "KEYWORD");
            procedure_Tc();
            build_tree("aug", "KEYWORD", 2);
        }
    }

    // Tc -> B ’->’ Tc ’|’ Tc | B
    void procedure_Tc()
    {
        procedure_B();

        if (nextToken.getVal() == "->") // If next token is '->', then rule is, Tc -> B ’->’ Tc ’|’ Tc
        {
            read("->", "OPERATOR");
            procedure_Tc();
            read("|", "OPERATOR");
            procedure_Tc();
            build_tree("->", "KEYWORD", 3);
        }
    }

    // B -> B ’or’ Bt | Bt
    void procedure_B()
    {
        procedure_Bt();
        while (nextToken.getVal() == "or") // If next token is 'or', then rule is, B -> B ’or’ Bt
        {
            read("or", "KEYWORD");
            procedure_Bt();
            build_tree("or", "KEYWORD", 2);
        }
    }

    // Bt -> Bt ’&’ Bs | Bs
    void procedure_Bt()
    {
        procedure_Bs();
        while (nextToken.getVal() == "&") // If next token is '&', then rule is, Bt -> Bt ’&’ Bs
        {
            read("&", "OPERATOR");
            procedure_Bs();
            build_tree("&", "KEYWORD", 2);
        }
    }

    // Bs -> ’not’ Bp | Bp
    void procedure_Bs()
    {
        if (nextToken.getVal() == "not") // If next token is 'not', then rule is, Bs -> ’not’ Bp
        {
            read("not", "KEYWORD");
            procedure_Bp();
            build_tree("not", "KEYWORD", 1);
        }
        else // Else the rule is, Bs -> Bp
        {
            procedure_Bp();
        }
    }

    /*
    Bp  -> A (’gr’ | ’>’ )
        -> A (’ge’ | ’>=’)
        -> A (’ls’ | ’<’ )
        -> A (’le’ | ’<=’)
        -> A ’eq’ A
        -> A ’ne’ A
        -> A ;
    */
    void procedure_Bp()
    {
        procedure_A();
        string temp = nextToken.getVal();
        string temp2 = nextToken.getType();

        if (nextToken.getVal() == "gr" || nextToken.getVal() == ">") // Bp -> A (’gr’ | ’>’ )
        {
            read(temp, temp2);
            procedure_A();
            build_tree("gr", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "ge" || nextToken.getVal() == ">=") // Bp -> A (’ge’ | ’>=’)
        {
            read(temp, temp2);
            procedure_A();
            build_tree("ge", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "ls" || nextToken.getVal() == "<") // Bp -> A (’ls’ | ’<’ )
        {
            read(temp, temp2);
            procedure_A();
            build_tree("ls", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "le" || nextToken.getVal() == "<=") // Bp -> A (’le’ | ’<=’)
        {
            read(temp, temp2);
            procedure_A();
            build_tree("le", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "eq") // Bp -> A ’eq’ A
        {
            read(temp, temp2);
            procedure_A();
            build_tree("eq", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "ne") // Bp -> A ’ne’ A
        {
            read(temp, temp2);
            procedure_A();
            build_tree("ne", "KEYWORD", 2);
        }
    }

    /*
    A   -> A ’+’ At
        -> A ’-’ At
        -> ’+’ At
        -> ’-’ At
        -> At ;
    */
    void procedure_A()
    {
        if (nextToken.getVal() == "+") // A -> ’+’ At
        {
            read("+", "OPERATOR");
            procedure_At();
        }
        else if (nextToken.getVal() == "-") // A -> ’-’ At
        {
            read("-", "OPERATOR");
            procedure_At();
            build_tree("neg", "KEYWORD", 1);
        }
        else
        {
            procedure_At();
        }

        // A   -> A ’+’ At | A ’-’ At
        while (nextToken.getVal() == "+" || nextToken.getVal() == "-")
        {
            string temp = nextToken.getVal();
            read(temp, "OPERATOR");
            procedure_At();
            build_tree(temp, "OPERATOR", 2);
        }
    }

    /*
    At  -> At ’*’ Af
        -> At ’/’  Af
        -> Af ;
    */
    void procedure_At()
    {
        procedure_Af(); // At -> Af

        while (nextToken.getVal() == "*" || nextToken.getVal() == "/") // At -> At ’*’ Af | At ’/’  Af
        {
            string temp = nextToken.getVal();
            read(temp, "OPERATOR");
            procedure_Af();
            build_tree(temp, "OPERATOR", 2);
        }
    }

    /*
    Af -> Ap ’**’ Af
       -> Ap ;
    */
    void procedure_Af()
    {
        procedure_Ap();

        if (nextToken.getVal() == "**") // Af -> Ap ’**’ Af
        {
            read("**", "OPERATOR");
            procedure_Af();
            build_tree("**", "KEYWORD", 2);
        }
    }

    /*
    Ap -> Ap ’@’ ’<IDENTIFIERENTIFIER>’ R
       -> R ;
    */
    void procedure_Ap()
    {
        procedure_R();
        while (nextToken.getVal() == "@")
        {
            read("@", "OPERATOR"); // read a type IDENTIFIER
            if (nextToken.getType() != "IDENTIFIER")
            {
                cout << " unexpected token ";
            }
            else
            {
                read(nextToken.getVal(), "IDENTIFIER");
                procedure_R();
                build_tree("@", "KEYWORD", 3);
            }
        }
    }

    /*
    R -> R Rn
      -> Rn ;
    */
    void procedure_R()
    {
        procedure_Rn();
        while (nextToken.getType() == "IDENTIFIER" || nextToken.getType() == "INTEGER" || nextToken.getType() == "STRING" || nextToken.getVal() == "true" || nextToken.getVal() == "false" || nextToken.getVal() == "nil" || nextToken.getVal() == "(" || nextToken.getVal() == "dummy")
        {
            procedure_Rn();
            build_tree("gamma", "KEYWORD", 2);
        }
    }

    /*
    Rn -> ’<IDENTIFIER>’
        -> ’<INTEGER>’
        -> ’<STRING>’
        -> ’true’
        -> ’false’
        -> ’nil’
        -> ’(’ E ’)’
        -> ’dummy’
    */
    void procedure_Rn()
    {
        if (nextToken.getType() == "IDENTIFIER" || nextToken.getType() == "INTEGER" || nextToken.getType() == "STRING") // Rn -> ’<IDENTIFIER>’ | ’<INTEGER>’ | ’<STRING>’
        {
            read(nextToken.getVal(), nextToken.getType());
        }
        else if (nextToken.getVal() == "true") // Rn -> ’true’
        {
            read("true", "KEYWORD");
            build_tree("true", "BOOL", 0);
        }
        else if (nextToken.getVal() == "false") // Rn -> ’false’
        {
            read("false", "KEYWORD");
            build_tree("false", "BOOL", 0);
        }
        else if (nextToken.getVal() == "nil") // Rn -> ’nil’
        {
            read("nil", "KEYWORD");
            build_tree("nil", "NIL", 0);
        }
        else if (nextToken.getVal() == "(") // Rn -> ’(’ E ’)’
        {
            read("(", "PUNCTION");
            procedure_E();
            read(")", "PUNCTION");
        }
        else if (nextToken.getVal() == "dummy") // Rn -> ’dummy’
        {
            read("dummy", "KEYWORD");
            build_tree("dummy", "DUMMY", 0);
        }
    }

    /*
    D -> Da ’within’ D => ’within’
      -> Da ;
    */
    void procedure_D()
    {
        procedure_Da();
        if (nextToken.getVal() == "within")
        {
            read("within", "KEYWORD");
            procedure_Da();
            build_tree("within", "KEYWORD", 2);
        }
    }

    /*
    Da -> Dr ( ’and’ Dr )+ 
       -> Dr ;
    */
    void procedure_Da()
    {
        procedure_Dr();

        int n = 1;
        while (nextToken.getVal() == "and")
        {
            n++;
            read("and", "KEYWORD");
            procedure_Dr();
        }
        if (n > 1)
        {
            build_tree("and", "KEYWORD", n);
        }
    }
    
    /*
    Dr -> ’rec’ Db 
        -> Db ;
    */
    void procedure_Dr()
    {
        if (nextToken.getVal() == "rec")
        {
            read("rec", "KEYWORD");
            procedure_Db();
            build_tree("rec", "KEYWORD", 1);
        }
        else
        {
            procedure_Db();
        }
    }

    /*
   Db -> Vl ’=’ E => ’=’
      -> ’<IDENTIFIER>’ Vb+ ’=’ E => ’fcn_form’
      -> ’(’ D ’)’ ;
    */
    void procedure_Db()
    {
        if (nextToken.getVal() == "(")
        {
            read("(", "PUNCTION");
            procedure_D();
            read(")", "PUNCTION");
        }
        else if (nextToken.getType() == "IDENTIFIER")
        {
            read(nextToken.getVal(), "IDENTIFIER");
            int n = 1;
            if (nextToken.getVal() == "=" || nextToken.getVal() == ",")
            {
                while (nextToken.getVal() == ",")
                {
                    read(",", "PUNCTION");
                    read(nextToken.getVal(), "IDENTIFIER");
                    n++;
                }
                if (n > 1)
                {
                    build_tree(",", "KEYWORD", n);
                }
                read("=", "OPERATOR");
                procedure_E();
                build_tree("=", "KEYWORD", 2);
            }
            else
            {
                do
                {
                    procedure_Vb();
                    n++;
                } while (nextToken.getType() == "IDENTIFIER" || nextToken.getVal() == "(");
                read("=", "OPERATOR");
                procedure_E();
                build_tree("function_form", "KEYWORD", n + 1);
            }
        }
    }

    /*
    Vb -> ’<IDENTIFIER>’
        -> ’(’ Vl ’)’
        -> ’(’ ’)’
    */
    void procedure_Vb()
    {
        if (nextToken.getType() == "IDENTIFIER")
        {
            read(nextToken.getVal(), "IDENTIFIER");
        }
        else if (nextToken.getVal() == "(")
        {
            read("(", "PUNCTION");
            if (nextToken.getVal() == ")")
            {
                read(")", "PUNCTION");
                build_tree("()", "KEYWORD", 0);
            }
            else
            {
                procedure_Vl();
                read(")", "PUNCTION");
            }
        }
    }

    /*
    Vl -> ’<IDENTIFIER>’ list ’,’
    */
    void procedure_Vl()
    {
        int n = 1;
        read(nextToken.getVal(), "IDENTIFIER");

        while (nextToken.getVal() == ",")
        {
            read(",", "PUNCTION");
            read(nextToken.getVal(), "IDENTIFIER");
            n++;
        }
        if (n > 1)
        {
            build_tree(",", "KEYWORD", n);
        }
    }
};
