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
        int i = index; // Index of character
        string id = ""; // Identifier
        string num = ""; // Number
        string isop = ""; // Operator
        string isString = ""; // String
        string isPun = ""; // Punctuation
        string isComment = ""; // Comment
        string isSpace = ""; // Space

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
                t.setType("INT");
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
                    t.setType("ID");
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
                t.setType("STR");
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
        nt = getToken(readnew); // Get the first token
        while (nt.getType() == "DELETE") // Ignore all DELETE tokens
        {
            nt = getToken(readnew);
        }

        proc_E(); // Start parsing from E

        while (nt.getType() == "DELETE")
        {
            nt = getToken(readnew);
        }
        
        if (index >= sizeOfFile - 1)
        {
            tree *t = st.top();

            if (astFlag == 1)
            {
                t->print_tree(0);
            }

            makeST(t);

            vector<tree *> delta;
            tree *setOfDeltaArray[200][200];
            myDelta(t, setOfDeltaArray);

            int size = 0;
            int inner = 0;
            while (setOfDeltaArray[size][0] != NULL)
            {
                size++;
            }

            vector<vector<tree *> > setOfDelta;
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

            csem(setOfDelta);
        }
    }

    void csem(vector<vector<tree *> > &controlStructure)
    {
        stack<tree *> control;
        stack<tree *> machine;
        stack<environment *> stackOfEnvironment;
        stack<environment *> getCurrEnvironment;

        int currEnvIndex = 0;                     // initial invironment
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

            if (nextToken->getType() == "INT" || nextToken->getType() == "STR" || nextToken->getVal() == "lambda" || nextToken->getVal() == "YSTAR" || nextToken->getVal() == "Print" || nextToken->getVal() == "Isinteger" || nextToken->getVal() == "Istruthvalue" || nextToken->getVal() == "Isstring" || nextToken->getVal() == "Istuple" || nextToken->getVal() == "Isfunction" || nextToken->getVal() == "Isdummy" || nextToken->getVal() == "Stem" || nextToken->getVal() == "Stern" || (nextToken->getVal() == "Conc" /*&& behind->getVal()=="gamma" && behind2->getVal()=="gamma"*/) || nextToken->getType() == "BOOL" || nextToken->getType() == "NIL" || nextToken->getType() == "DUMMY" || nextToken->getVal() == "Order" || nextToken->getVal() == "nil" || nextToken->getVal() == "ItoS" /*|| isBiOper(nextToken->getVal()) || nextToken->getVal() == "neg" || nextToken->getVal() == "not"*/)
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
                            newEnv->boundVar.insert(std::pair<tree *, vector<tree *> >(boundVariables.at(i), listOfNodeVal));
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

                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *> >(boundVar, listOfNodeVal));
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
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *> >(boundVar, listOfNodeVal));
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
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *> >(boundVar, listOfNodeVal));
                    }
                    else
                    {
                        tree *bindVarVal = machine.top();

                        machine.pop();
                        vector<tree *> listOfNodeVal;
                        listOfNodeVal.push_back(bindVarVal);
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *> >(boundVar, listOfNodeVal));
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
                            if (getRev.top()->getType() == "STR")
                                cout << addSpaces(getRev.top()->getVal()) << ", ";
                            else
                                cout << getRev.top()->getVal() << ", ";
                            getRev.pop();
                        }
                        if (getRev.top()->getType() == "STR")
                            cout << addSpaces(getRev.top()->getVal()) << ")";
                        else
                            cout << getRev.top()->getVal() << ")";
                        getRev.pop();
                    }
                    else
                    {

                        if (machine.top()->getType() == "STR")
                            cout << addSpaces(machine.top()->getVal());
                        else
                            cout << machine.top()->getVal();
                    }
                }
                else if (machineTop->getVal() == "Isinteger")
                {
                    // cout << " inside " << nextToken->getVal() << " with " << machineTop->getVal() << endl;
                    machine.pop();
                    tree *isNextInt = machine.top();
                    machine.pop();
                    // cout << " 1.  next is " << isNextInt->getVal() << " and type " << isNextInt->getType() << endl;
                    if (isNextInt->getType() == "INT")
                    {
                        // cout << " pushing true" << endl;
                        tree *resNode = createNode("true", "boolean");
                        machine.push(resNode);
                    }
                    else
                    {
                        // cout << "pushing false" << endl;
                        tree *resNode = createNode("false", "boolean");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Istruthvalue")
                {
                    // cout << " inside " << nextToken->getVal() << " with " << machineTop->getVal() << endl;
                    machine.pop();
                    tree *isNextBool = machine.top();
                    machine.pop(); // poppoedxfg
                    // cout << " 2. next is " << isNextBool->getVal() << " and type " << isNextBool->getType() << endl;
                    if (isNextBool->getVal() == "true" || isNextBool->getVal() == "false")
                    {
                        // cout << " pushing true" << endl;
                        tree *resNode = createNode("true", "BOOL");
                        machine.push(resNode);
                    }
                    else
                    {
                        // cout << "pushing false" << endl;
                        tree *resNode = createNode("false", "BOOL");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Isstring")
                {
                    // cout << " inside " << nextToken->getVal() << " with " << machineTop->getVal() << endl;
                    machine.pop();
                    tree *isNextString = machine.top();
                    // cout << " 3. next is " << isNextString->getVal() << " and type " << isNextString->getType() << endl;
                    machine.pop();
                    if (isNextString->getType() == "STR")
                    {
                        // cout << " pushing true" << endl;
                        tree *resNode = createNode("true", "BOOL");
                        machine.push(resNode);
                    }
                    else
                    {
                        // cout << "pushing false" << endl;
                        tree *resNode = createNode("false", "BOOL");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Istuple")
                {
                    // cout << " inside " << nextToken->getVal() << " with " << machineTop->getVal() << endl;
                    machine.pop();
                    tree *isNextTau = machine.top();
                    machine.pop();
                    // cout << " 4. next is " << isNextTau->getVal() << " and type " << isNextTau->getType() << endl;
                    if (isNextTau->getType() == "tau")
                    {
                        // cout << " pushing true" << endl;
                        tree *resNode = createNode("true", "BOOL");
                        machine.push(resNode);
                    }
                    else
                    {
                        // cout << "pushing false" << endl;
                        tree *resNode = createNode("false", "BOOL");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Isfunction")
                {
                    // cout << " inside " << nextToken->getVal() << " with " << machineTop->getVal() << endl;
                    machine.pop();
                    tree *isNextFn = machine.top();
                    // cout << " 5. next is " << isNextFn->getVal() << " and type " << isNextFn->getType() << endl;
                    if (isNextFn->getVal() == "lambda")
                    {
                        // cout << " pushing true" << endl;
                        tree *resNode = createNode("true", "BOOL");
                        machine.push(resNode);
                    }
                    else
                    {
                        // cout << "pushing false" << endl;
                        tree *resNode = createNode("false", "BOOL");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Isdummy")
                {
                    // cout << " inside " << nextToken->getVal() << " with " << machineTop->getVal() << endl;
                    machine.pop();
                    tree *isNextDmy = machine.top();
                    // cout << " 6. next is " << isNextDmy->getVal() << " and type " << isNextDmy->getType() << endl;
                    if (isNextDmy->getVal() == "dummy")
                    {
                        // cout << " pushing true" << endl;
                        tree *resNode = createNode("true", "BOOL");
                        machine.push(resNode);
                    }
                    else
                    {
                        // cout << "pushing false" << endl;
                        tree *resNode = createNode("false", "BOOL");
                        machine.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Stern")
                {
                    // cout << " inside " << nextToken->getVal() << " with " << machineTop->getVal() << endl;
                    machine.pop();
                    tree *isNextString = machine.top();

                    // cout << " 7. next is " << isNextString->getVal() << " and type " << isNextString->getType() << endl;

                    if (isNextString->getVal() == "")
                    {
                        // isNextString->setVal("''");
                        cout << " empty string" << endl;
                        return;
                    }

                    if (isNextString->getType() == "STR")
                    {
                        // int ind = isNextString->getVal().length()-3;
                        string strRes = "'" + isNextString->getVal().substr(2, isNextString->getVal().length() - 3) + "'";
                        // cout << " after Stern " << strRes << endl;
                        machine.pop();
                        machine.push(createNode(strRes, "STR"));
                    }
                    else
                    {
                        // cout << " i don't know what to do !!!!! with " <<  isNextString->getType() << " val " << isNextString->getVal() << endl;
                    }
                }
                else if (machineTop->getVal() == "Stem")
                {
                    // cout << " inside " << nextToken->getVal() << " with " << machineTop->getVal() << endl;
                    // cout << machine.top()->getVal() << endl;
                    machine.pop();
                    tree *isNextString = machine.top();

                    // cout << " 8. next is " << isNextString->getVal() << " and type " << isNextString->getType() << endl;

                    if (isNextString->getVal() == "")
                    {
                        // isNextString->setVal("''");
                        cout << " empty string" << endl;
                        return;
                    }

                    if (isNextString->getType() == "STR")
                    {
                        string strRes = "'" + isNextString->getVal().substr(1, 1) + "'";
                        // cout << " after Stem " << strRes << endl;
                        machine.pop();
                        machine.push(createNode(strRes, "STR"));
                    }
                    else
                    {
                        // cout << " i don't know what to do !!!!! with " <<  isNextString->getType() << " val " << isNextString->getVal() << endl;
                    }
                }
                else if (machineTop->getVal() == "Order")
                {
                    // cout << " in gamma with Order" << endl;
                    machine.pop();

                    tree *getTau = machine.top();
                    tree *saveTau = getTau;
                    // cout << " 10. next is tua " << getTau->getVal() << endl;
                    int numOfKids = 0;
                    // cout <<getTau->left->right->getVal() << endl;
                    if (getTau->left != NULL)
                        getTau = getTau->left;
                    // cout << " hello " << endl;
                    while (getTau != NULL)
                    {
                        numOfKids++;
                        // cout <<  getTau->getVal() << ", ";
                        getTau = getTau->right;
                    }
                    getTau = machine.top();
                    machine.pop();

                    if ((getTau->getVal() == "nil"))
                    {
                        // cout << " ONLY 1 KID which is nil" << endl;
                        numOfKids = 0;
                    }

                    // cout << " oder is " << numOfKids;
                    stringstream ss11;
                    ss11 << numOfKids;
                    string str34 = ss11.str();
                    tree *orderNode = createNode(str34, "INT");
                    machine.push(orderNode);
                    // cout << " helllo poddsfs" << endl;
                }
                else if (machineTop->getVal() == "Conc")
                {
                    // cout << " inside " << nextToken->getVal() << " with " << machineTop->getVal() << endl;
                    tree *concNode = machine.top();
                    machine.pop();
                    tree *firstString = machine.top();
                    // cout << " string one " << firstString->getVal().substr(1,firstString->getVal().length()-2)<< "with type " << firstString->getType() << endl;
                    machine.pop();
                    tree *secondString = machine.top();
                    // cout << " second string " << secondString->getVal().substr(1,secondString->getVal().length()-2) << " with type " << secondString->getType() << endl;
                    if (secondString->getType() == "STR" || (secondString->getType() == "STR" && secondString->left != NULL && secondString->left->getVal() == "true"))
                    {
                        machine.pop();
                        string res = "'" + firstString->getVal().substr(1, firstString->getVal().length() - 2) + secondString->getVal().substr(1, secondString->getVal().length() - 2) + "'";
                        // cout << " result " << res << endl;
                        tree *resNode = createNode(res, "STR");
                        machine.push(resNode);
                        // pop gamma from control
                        control.pop();
                    }
                    else
                    {
                        // cout << " **************** ONLY ONE STRING ************************" << endl;
                        concNode->left = firstString;
                        // machine.push(firstString);
                        machine.push(concNode);
                        // cout <<  machine.top()->getVal() << " ON TOP OF MACHINE" << endl;
                        // cout << " applying Conc to only 1 string added a left kid flag and keeping conc " << endl;
                        firstString->left = createNode("true", "flag");
                    }
                }
                else if (machineTop->getVal() == "ItoS")
                {
                    // cout << " inside " << nextToken->getVal() << " with " << machineTop->getVal() << endl;
                    machine.pop(); // popping ItoS
                    tree *convertToString = machine.top();
                    // cout << " convert " << convertToString->getVal() << " with " << convertToString->getType() << " to str " << endl;
                    machine.pop();
                    machine.push(createNode("'" + convertToString->getVal() + ",", "STR"));
                }
            }
            else if (nextToken->getVal().substr(0, 3) == "env")
            {
                // cout << " killing env " << nextToken->getVal() << endl;
                stack<tree *> removeFromMachineToPutBack;
                /*while(machine.top()->getType() != "ENV"){
                    //tree *temp = machine.top();
                    cout << " next token in mchine " << machine.top()->getVal() << " with type " << machine.top()->getType() << endl;
                    removeFromMachineToPutBack.push(machine.top());
                    machine.pop();
                }*/
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
                    // cout << " in else putting " << endl;
                    removeFromMachineToPutBack.push(machine.top());
                    machine.pop();
                    // cout << removeFromMachineToPutBack.top()->getVal() << " to the temp stack " << endl;
                }
                // cout << " KILL IT" << endl;
                // tree *topToken = machine.top();
                // cout << " on machine top " << machine.top()->getVal() << endl;
                // machine.pop();
                tree *remEnv = machine.top();
                // cout << " expecting env " << machine.top()->getVal() << endl;
                // machine.pop();
                if (nextToken->getVal() == remEnv->getVal())
                {
                    // control.pop();
                    // cout << "killing off " << nextToken->getVal() << " and " << remEnv->getVal() << endl;
                    machine.pop();
                    // cout << "killed off " << nextToken->getVal() << " and " << remEnv->getVal() << endl;

                    // cout << " killing from stack of env " << stackOfEnvironment.top()->name << endl;
                    // stackOfEnvironment.pop();

                    stack<tree *> printMachine = machine;
                    /*cout << " left on machine $$$$$$$$$$" << endl;
                    while( !printMachine.empty()){
                        cout << printMachine.top()->getVal() << endl;;
                        printMachine.pop();
                    }*/
                    // cout << " all this is left on machine $$$$$" << endl;

                    getCurrEnvironment.pop();
                    if (!getCurrEnvironment.empty())
                    {
                        currEnv = getCurrEnvironment.top();
                        // cout << getCurrEnvironment.top()->name << endl;
                        // cout << " new curr env is " << currEnv->name << endl;
                    }
                    else
                    {
                        currEnv = NULL;
                        // cout << " current env is NULL ************" << endl;
                    }
                }
                else
                {
                    cout << "error evn do not match";
                    return;
                }

                // cout << " to put back " << removeFromMachineToPutBack.size() << endl;

                while (!removeFromMachineToPutBack.empty())
                {
                    // cout << " putting back " <<  removeFromMachineToPutBack.top()->getVal() <<endl;
                    machine.push(removeFromMachineToPutBack.top());
                    removeFromMachineToPutBack.pop();
                }
                // pushing the token back on machine
            }
            else if (nextToken->getType() == "ID" && nextToken->getVal() != "Print" && nextToken->getVal() != "Isinteger" && nextToken->getVal() != "Istruthvalue" && nextToken->getVal() != "Isstring" && nextToken->getVal() != "Istuple" && nextToken->getVal() != "Isfunction" && nextToken->getVal() != "Isdummy" && nextToken->getVal() != "Stem" && nextToken->getVal() != "Stern" && nextToken->getVal() != "Conc")
            {
                // cout << " in else with next " << nextToken << endl;
                environment *temp = currEnv;
                int flag = 0;
                while (temp != NULL)
                {

                    // cout <<" key is " << nextToken->getVal() << endl;

                    map<tree *, vector<tree *> >::iterator itr = temp->boundVar.begin();
                    while (itr != temp->boundVar.end())
                    {
                        // cout << " inside evn name " << temp->name << " prev " << temp->prev->name << endl;
                        if (nextToken->getVal() == itr->first->getVal())
                        {
                            // cout << " value of " << itr->first->getVal() << " is of size "<< itr->second.size() << " FOUND%%%%%%%%" << endl;
                            vector<tree *> temp = itr->second;
                            if (temp.size() == 1 && temp.at(0)->getVal() == "Conc" && temp.at(0)->left != NULL)
                            {
                                // cout << " BOUND VAL IS CONC ************* %%%%%%%%% ************"<< endl;
                                control.push(createNode("gamma", "KEYWORD"));
                                machine.push(temp.at(0)->left);
                                machine.push(temp.at(0));
                            }
                            else
                            {
                                int i = 0;
                                while (i < temp.size())
                                {
                                    // cout << " bound val is " << temp.at(i)->getVal() << endl;
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
                    if (node1->getType() == "INT" && node2->getType() == "INT")
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
                            tree *res = createNode(str, "INT");
                            machine.push(res);
                        }
                        else if (op == "-")
                        {
                            res = num1 - num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
                            machine.push(res);
                        }
                        else if (op == "*")
                        {
                            res = num1 * num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
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
                            tree *res = createNode(str, "INT");
                            machine.push(res);
                        }
                        else if (op == "**")
                        {
                            resPow = pow((double)num1, (double)num2);
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
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
                    else if (node1->getType() == "STR" && node2->getType() == "STR")
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
                        tree *resStr = createNode(str, "INT");
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

        // cout <<  " aaja " << endl;
        if (tauNode == NULL)
            return;
        if (tauNode->getVal() == "lamdaTuple")
            return;
        // cout << "(";
        // while(tauNode != NULL){
        if (tauNode->getVal() != "tau" && tauNode->getVal() != "nil")
        {
            // cout << " hello " << endl;
            // if(tauNode->getType()=="STR") cout <<  tauNode->getVal().substr(1,tauNode->getVal().length()-2) << ", ";
            // else cout << tauNode->getVal() << ", ";
            // cout << " bye " << endl;
            res.push(tauNode);
        }
        // cout << tauNode->getVal() << endl;
        // }
        //	cout << ")";
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
        // cout << temp ;
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

        // cout << " next token " <<x->getVal() << endl;

        if (x->getVal() == "lambda")
        {
            std::stringstream ss;

            // if(i<=index)++index;
            // else index = i+1;
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
            // ss	<< ++index;
            string str = ss.str();
            tree *temp = createNode(str, "deltaNumber");

            // cout << " pushing lambda val " << i << " " << j << endl;
            setOfDelta[i][j++] = temp;

            // cout <<  " pushing left of lambda " << x->left->getVal() << i << " " << j << endl;
            setOfDelta[i][j++] = x->left;

            // cout <<  " pushing lambda " << x->getVal() << i << " " << j << endl;
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

            // cout << " next Delta " << nextDelta << endl;
            std::stringstream ss2;
            ss2 << nextDelta;
            string str2 = ss2.str();
            tree *temp1 = createNode(str2, "deltaNumber");
            // cout <<  " pushing number " << x->getVal() << i << " " << j << endl;
            setOfDelta[i][j++] = temp1;

            int nextToNextDelta = index;
            std::stringstream ss3;
            ss3 << nextToNextDelta;
            string str3 = ss3.str();
            // cout <<  " pushing number " << x->getVal() << i << " " << j << endl;
            tree *temp2 = createNode(str3, "deltaNumber");
            setOfDelta[i][j++] = temp2;

            // delta.push_back(x->left);
            tree *beta = createNode("beta", "beta");
            // cout <<  " pushing number " << x->getVal() << i << " " << j << endl;
            setOfDelta[i][j++] = beta;

            // cout << " print here " << setOfDelta[i][j-2]->getVal() <<  "  " <<  setOfDelta[i][j-1]->getVal() << endl;
            while (setOfDelta[k][0] != NULL)
            {
                k++;
            }
            int firstIndex = k;
            int lamdaCount = index;

            myDelta(x->left, setOfDelta);
            int diffLc = index - lamdaCount;

            // cout<<"pushing1"<<endl;
            while (setOfDelta[i][0] != NULL)
                i++;
            j = 0;

            myDelta(x->left->right, setOfDelta);

            // cout<<"pushing2"<<endl;

            while (setOfDelta[i][0] != NULL)
                i++;
            j = 0;

            // cout << " kid 1 "<<x->left->getVal() << endl;
            // cout << " kid 2 "<<x->left->right->getVal() << endl;
            // cout << " kid 3 "<<x->left->right->right->getVal() << endl;

            /*if(x->left->right->right != NULL) */
            myDelta(x->left->right->right, setOfDelta);

            // cout<<"pushing5"<<endl;
            // setOfDelta.push_back(delta);

            // cout << " no of lambda is " << myStoredIndex << endl;
            // cout << " j is " <<  j << endl;

            stringstream ss23;
            if (diffLc == 0 || i < lamdaCount)
            {
                // cout << " lambda count " +  lamdaCount << endl;
                // cout<< " index " << index << endl;
                ss23 << firstIndex;
            }
            else
            {
                // cout << " no lambda = index " << endl;
                // cout << " index is " << endl;
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
            // cout << " no of kids of tau here " << endl;
            std::stringstream ss;
            ss << numOfChildren;
            string str = ss.str();
            tree *countNode = createNode(str, "CHILDCOUNT");
            // cout <<  " pushing child count  " << countNode->getVal() << i << " " << j << endl;

            setOfDelta[i][j++] = countNode; // putting the number of kids of tua
            // x->setType("tau");
            tree *tauNode = createNode("tau", "tau");
            // cout <<  " pushing tau node " << tauNode->getVal() << i << " " << j << endl;

            setOfDelta[i][j++] = tauNode; // putting the tau node and not pushing x
            // x = x->left;
            // myDelta(x->left,delta,setOfDelta,index); //calling the delta on kid of tau
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
            // cout << " pushing to delta " << x->getVal() << " at pos " << i <<  " "<< j << endl;
            setOfDelta[i][j++] = createNode(x->getVal(), x->getType());
            myDelta(x->left, setOfDelta);
            if (x->left != NULL)
                myDelta(x->left->right, setOfDelta);
        }
    }

    void makeST(tree *t)
    {
        // tree *root;
        // standardizeAnd(t);
        makeStandard(t);
    }

    /*print_preOrder(int dots, tree *t){
        int n=0;
        while(n<dots){
            cout << ".";
            n++;
        }
        //cout << val << " and " << type << endl;
        if(t->getType() == "ID" || t->getType() == "STR" || t->getType() == "INT"){
            t->setVal("<" + t->getType() + ":"+ t->getVal()+ ">") ;
        }

        cout << t->getVal() << endl;

        if(t->left != NULL){
            print_preOrder(dots+1,t->left);
        }
        if(t->right != NULL){
            print_preOrder(dots,t->right);
        }
    } */

    tree *standardizeAnd(tree *t)
    {
        if (t == NULL)
            return NULL;
        // cout << " val " << t->getVal() << endl;

        if (t->getVal() == "and")
        {
            tree *equal = t->left;
            // cout << " my left " << equal->getVal() << endl;
            // tree *restEqual = t->left->right;
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
        // while()
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

        // cout << " my val " << t->getVal() << endl;

        if (t->getVal() == "let")
        {
            if (t->left->getVal() == "=")
            {
                t->setVal("gamma");
                t->setType("KEYWORD");
                tree *P = createNode(t->left->right);
                tree *X = createNode(t->left->left);
                tree *E = createNode(t->left->left->right);
                // t = t->left;
                t->left = createNode("lambda", "KEYWORD");
                t->left->right = E; // do i set the right pointer to null
                tree *lambda = t->left;
                lambda->left = X;
                lambda->left->right = P;
                // if(t->left->left != NULL)cout << " find comma" << t->left->left->getVal() << endl;
                /*if(t->left->left->getVal()==","){
                    tree *lamba = createNode(t->left);
                    //cout << " hello 1" << endl;
                    tree *X = lambda->left->left;
                    tree *E = createNode(lambda->left->right);
                    stack<tree*> num;
                    //cout << " hello 2" << endl;
                    while(X != NULL){
                        tree *temp = createNode(X);
                        num.push(temp);
                        X = X->right;
                    }
                    //cout << " hello 3" << endl;
                    lambda->left = createNode("T","KEYWORD");
                    //lambda->left->right = createNode("gamma","KEYWORD");
                    //lambda = lambda->left;
                    while(!num.empty()){
                        //cout << " hello 4" << endl;
                        lambda->left->right = createNode("gamma","KEYWORD");
                        lambda = lambda->left->right;
                        //cout << " hello 4 a" << endl;

                        lambda->left = createNode("lambda","KEYWORD");
                        lambda->left->right = createNode("gamma","KEYWORD");
                        lambda->left->left = num.top();
                        //cout << " hello 4 b" << endl;

                        //string  len = (string)num.size();
                        std::stringstream ss;
                        ss	<< num.size();
                        string len = ss.str();
                        num.pop();
                        lambda->left->right->left = createNode("T","KEYWORD");
                        lambda->left->right->left->right = createNode(len,"KEYWORD");
                        //cout << " lambda->left " << lambda->left->getVal() << endl;
                        lambda= lambda->left;
                        //	cout << " hello 4 c" << endl;

                    }
                    //cout << " hello 5" << endl;
                    if(num.empty()){
                        lambda->left->right = E;
                    }
                    //cout << " hello 6" << endl;
            } */
            }
        }
        else if (t->getVal() == "and" && t->left->getVal() == "=")
        {
            tree *equal = t->left;
            // cout << " my left " << equal->getVal() << endl;
            // tree *restEqual = t->left->right;
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
            // cout << " where left " << t->left->getVal() << endl;
            // cout << " where left right "<<t->left->right->getVal() << endl;
            if (t->left->right->getVal() == "=")
            {
                tree *P = createNode(t->left);
                tree *X = createNode(t->left->right->left);
                tree *E = createNode(t->left->right->left->right);
                t->left = createNode("lambda", "KEYWORD");
                t->left->right = E;
                // t = t->left;
                t->left->left = X;
                t->left->left->right = P;
            }
        } /*else if(t->getVal()=="tau"){
             //cout << t->getVal() << endl;
             t->setVal("gamma");
             t->setType("KEYWORD");
             //if(t->left != NULL){
                 tree *E = (t->left);
             //	tree *firstE = t->left ;
                 //firstE->right = NULL;

                 stack<tree*> num;
                 //num.push(firstE);
                 while(E != NULL){
                     tree *temp = createNode(E);
                     //temp->right = NULL;
                     num.push(temp);
                     E = E->right;
             //		cout<<"HELLO"<<endl;
                 }

             //	cout << " here " << endl;
                 tree *firstE = num.top();
                 num.pop();
             //	cout << " first E "<<firstE->getVal() << endl;

                 t->left = createNode("gamma","KEYWORD");

                 tree *gamma = t->left;
                 gamma->right = createNode(firstE);
                 gamma->left = createNode("aug", "KEYWORD");
                 gamma = gamma->left;
                 //cout << " now start adding rest " << gamma->getVal() << endl;

                 while(!num.empty()){
                     tree *temp = num.top();
                     num.pop();
                     gamma->right = createNode("gamma","KEYWORD");
                     gamma = gamma->right;
                     //cout << " on right of aug should be gamma " << gamma->getVal() << endl;
                     gamma->left = createNode("gamma", "KEYWORD");
                     //cout << " new gamma on left " << gamma->left->getVal() << endl;

                     gamma->left->right = createNode(temp);
                     //cout << " gamma->left->right " << gamma->left->right->getVal() << endl;
                     gamma->left->left = createNode("aug", "KEYWORD");
                     //cout << " gamma->left->left should be aug " << gamma->left->left->getVal() << endl;
                     gamma = gamma->left->left;
                     //cout << " for next stay at aug " << gamma->getVal() << endl;
                 }

                 if(num.empty()){
                     gamma->right = createNode("nil","KEYWORD");
                 }
                 //cout << " t "<<t->getVal() << endl;
                 //cout << " t-> left " << t->left->getVal() << endl;
                 //cout << " t left right " << t->left->right->getVal() << endl;

         } else if(t->getVal()=="->"){
             tree *B = createNode(t->left);
             tree *T = createNode(t->left->right);
             tree *E = createNode(t->left->right->right); //after this need to see what links to change
             //t = createNode("gamma","KEYWORD");
             t->setVal("gamma");
             t->setType("KEYWORD");

             t->left = createNode("gamma","KEYWORD");
             t->left->right = createNode("nil","KEYWORD");
             tree *tLeft = t->left;
             tLeft->left = createNode("gamma","KEYWORD");
             tLeft->left->right = createNode("lambda","KEYWORD");
             tLeft->left->right->left = createNode("()","KEYWORD");
             tLeft->left->right->left->right = E;
             tLeft = tLeft->left;
             tLeft->left = createNode("gamma","KEYWORD");
             tLeft->left->right = createNode("lambda","KEYWORD");
             tLeft->left->right->left = createNode("()","KEYWORD");
             tLeft->left->right->left->right = T;
             tLeft->left->left = createNode("Cond","KEYWORD");
             tLeft->left->left->right = B;

         }else if(t->getVal()=="not"){
             tree *E = createNode(t->left);
             //t = createNode("gamma","KEYWORD");
             t->setVal("gamma");
             t->setType("KEYWORD");

             t->left = createNode("not","KEYWORD");
             t->left->right = E;

         }else if(t->getVal()=="neg"){
             tree *E = createNode(t->left);
             //t = createNode("gamma","KEYWORD");
             t->setVal("gamma");
             t->setType("KEYWORD");
             t->left = createNode("neg","KEYWORD");
             t->left->right = E;

         }*/
        else if (t->getVal() == "within")
        {
            if (t->left->getVal() == "=" && t->left->right->getVal() == "=")
            {
                tree *X1 = createNode(t->left->left);
                tree *E1 = createNode(t->left->left->right);
                tree *X2 = createNode(t->left->right->left);
                tree *E2 = createNode(t->left->right->left->right);
                // t = createNode("=","KEYWORD");
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
            // cout << t->getVal() << " here in rec " << endl;
            tree *X = createNode(t->left->left);
            tree *E = createNode(t->left->left->right);
            // cout << " x left " << E->left->getVal() << endl;
            // cout << " x left right " << E->left->right->getVal() << endl;
            // cout << " x left right left" << E->left->right->left->getVal() << endl;
            // cout << " x left->left " << E->left->left->getVal() << endl;

            t->setVal("=");
            t->setType("KEYWORD");
            t->left = X;
            t->left->right = createNode("gamma", "KEYWORD");
            t->left->right->left = createNode("YSTAR", "KEYWORD");
            tree *ystar = t->left->right->left;
            // t->left->right->left->right = createNode("lambda","KEYWORD");
            ystar->right = createNode("lambda", "KEYWORD");
            // cout<<"CHECK+ "<<t->left->right->left->right->getVal();
            // cout << X->getVal() << endl;
            // cout << E->getVal() << endl << " hello 54535";
            // t->left->right->left->right->left = X;
            // t->left->right->left->right->left->right = E;
            ystar->right->left = createNode(X);
            ystar->right->left->right = createNode(E);

            //}
            // cout << " printing rec tree" << endl;
            // t->print_tree(0) ;
        }
        else if (t->getVal() == "function_form")
        {
            // cout << " function form " << t->getVal() << endl;
            tree *P = createNode(t->left);
            tree *V = t->left->right;
            // tree *E = t->left->right->right;
            // t->setVal("=");
            // t= createNode("=","KEYWORD");
            t->setVal("=");
            t->setType("KEYWORD");
            t->left = P;
            // t->left->right = createNode("lambda","KEYWORD");
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
                        // cout << " here 1 " << V->getVal() << endl;
                        temp->left->right = createNode("lambda", "KEYWORD");
                        temp = temp->left->right;
                        temp->left = createNode(V);
                        // temp = temp->left->right;
                        V = V->right;
                    }

                    temp->left->right = createNode("lambda", "KEYWORD");
                    temp = temp->left->right;
                    // cout << " lambda " << temp->getVal() << endl ;
                    // cout << " V " << V->getVal() << endl;
                    // cout << " V->right " << V->right->getVal() << endl;
                    // if(V->right == NULL){
                    temp->left = createNode(V);
                    temp->left->right = V->right;
                }
                //}
            }
        } /*else if(t->getVal() == "+" || t->getVal() == "-" || t->getVal() == "*" || t->getVal() == "/" || t->getVal() == "**"
         || t->getVal() == "&" || t->getVal() == "gr" || t->getVal() == "ge" || t->getVal() == "ls" || t->getVal() == "le"
         || t->getVal() == "eq" || t->getVal() == "ne"  ){
         string op = t->getVal();
         //cout << " operator " << t->getVal() << endl;
         tree *E1 = createNode(t->left);
         //cout << " E1 " << t->left->getVal() << endl;
         if(t->left->right != NULL){
             tree *E2 = createNode(t->left->right);
             //cout << " E2 " << E2->getVal() << endl;
             //t =  createNode("gamma","KEYWORD");
             t->setVal("gamma");
             t->setType("KEYWORD");
             t->left = createNode("gamma","KEYWORD");
             t->left->right = E2;
             tree *tLeft= t->left;
             tLeft->left = createNode(op,"KEYWORD");
             tLeft->left->right = E1;
         }else{
             //t = createNode("gamma","KEYWORD");
             t->setVal("gamma");
             t->setType("KEYWORD");
             t->left = createNode("gamma","KEYWORD");
             t->left->right = E1;
         }
     }*/
        else if (t->getVal() == "@")
        {
            tree *E1 = createNode(t->left);
            tree *N = createNode(t->left->right);
            tree *E2 = createNode(t->left->right->right);
            // t=  createNode("gamma","KEYWORD");
            t->setVal("gamma");
            t->setType("KEYWORD");
            t->left = createNode("gamma", "KEYWORD");
            t->left->right = E2;
            // t= t->left;
            t->left->left = N;
            t->left->left->right = E1;
        }

        return NULL;
    }

    void proc_E()
    {
        if (nt.getVal() == "let")
        {
            read("let", "KEYWORD");
            proc_D();

            read("in", "KEYWORD"); // read in
            proc_E();
        
            build_tree("let", "KEYWORD", 2);
        }
        else if (nt.getVal() == "fn")
        {
            int n = 0;
            read("fn", "KEYWORD");
            do
            {
                proc_Vb();
                n++;
            } while (nt.getType() == "ID" || nt.getVal() == "(");
            read(".", "OPERATOR"); // read "."
            proc_E();
           
            build_tree("lambda", "KEYWORD", n + 1);
        }
        else
        {
            proc_Ew();
        }
    }

    void proc_Ew()
    {
        proc_T();

        if (nt.getVal() == "where")
        {
            read("where", "KEYWORD");
            proc_Dr();
            build_tree("where", "KEYWORD", 2);
        }

    }

    void proc_T()
    {
        proc_Ta();

        int n = 1;
        while (nt.getVal() == ",")
        {
            n++;
            read(",", "PUNCTION");
            proc_Ta();
        }
        if (n > 1)
        {
            build_tree("tau", "KEYWORD", n);
        }
    }

    void proc_Ta()
    {
        proc_Tc();

        while (nt.getVal() == "aug")
        {
            read("aug", "KEYWORD");
            proc_Tc();
            build_tree("aug", "KEYWORD", 2);
        }
    }

    void proc_Tc()
    {
        proc_B();
        if (nt.getVal() == "->")
        {
            read("->", "OPERATOR");
            proc_Tc();
            read("|", "OPERATOR"); // read "|"
            proc_Tc();
            build_tree("->", "KEYWORD", 3);
        }
    }

    void proc_B()
    {
        // cout<< " Enter B with token "  << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() << endl;
        // cout << " inside B" << endl;
        proc_Bt();
        // cout << " back to b with token " << nt.getVal() << " and type " << nt.getType() << " stack size " << st.size() << endl;
        while (nt.getVal() == "or")
        {
            read("or", "KEYWORD");
            proc_Bt();
            build_tree("or", "KEYWORD", 2);
        }
        // cout<< " Exit B with token "  << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() << endl;
    }

    void proc_Bt()
    {
        // cout << " Enter Bt with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
        // cout << " inside Bt " << endl;
        proc_Bs();
        // cout << " back to bt with token " << nt.getVal() << " and type " << nt.getType() << " stack size " << st.size() << endl;
        while (nt.getVal() == "&")
        {
            read("&", "OPERATOR");
            proc_Bs();
            build_tree("&", "KEYWORD", 2);
        }
        //	cout << " Exit Bt with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
    }

    void proc_Bs()
    {
        // cout << " inside bs " << endl;
        // cout << " Enter Bs with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;

        if (nt.getVal() == "not")
        {
            read("not", "KEYWORD");
            proc_Bp();
            build_tree("not", "KEYWORD", 1);
        }
        else
        {
            proc_Bp();
            //	cout << " back to bs with token " << nt.getVal() << " and type " << nt.getType() << " stack size " << st.size() << endl;
        }
        // cout << " Exit Bs with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
    }

    void proc_Bp()
    {
        // cout << " inside Bp" << endl;
        // cout << " Enter Bp with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;

        proc_A();
        string temp = nt.getVal();
        string temp2 = nt.getType();
        // cout << " back to Bp with token " << nt.getVal() << " and type " << nt.getType() << "stack size " << st.size() << endl;
        if (nt.getVal() == "gr" || nt.getVal() == ">")
        {
            // read("gr","KEYWORD");
            read(temp, temp2);
            proc_A();
            build_tree("gr", "KEYWORD", 2);
        }
        else if (nt.getVal() == "ge" || nt.getVal() == ">=")
        {
            // read("ge","KEYWORD");
            read(temp, temp2);
            proc_A();
            build_tree("ge", "KEYWORD", 2);
        }
        else if (nt.getVal() == "ls" || nt.getVal() == "<")
        {
            // read("ls","KEYWORD");
            read(temp, temp2);
            proc_A();
            build_tree("ls", "KEYWORD", 2);
        }
        else if (nt.getVal() == "le" || nt.getVal() == "<=")
        {
            // read("le","KEYWORD");
            read(temp, temp2);
            proc_A();
            build_tree("le", "KEYWORD", 2);
        }
        else if (nt.getVal() == "eq")
        {
            // read("eq","KEYWORD");
            read(temp, temp2);
            proc_A();
            //	cout << " back to Bp with token " << nt.getVal() << " with stack size " << st.size() << endl;
            build_tree("eq", "KEYWORD", 2);
        }
        else if (nt.getVal() == "ne")
        {
            // read("ne","KEYWORD");
            read(temp, temp2);
            proc_A();
            build_tree("ne", "KEYWORD", 2);
        }
        // cout << " Exit Bp with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
    }

    void proc_A()
    {
        // cout << " inside A " << endl;
        //	cout << " Enter A with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
        if (nt.getVal() == "-")
        {
            read("-", "OPERATOR");
            proc_At();
            build_tree("neg", "KEYWORD", 1);
        }
        else if (nt.getVal() == "+")
        {
            read("+", "OPERATOR");
            proc_At();
        }
        else
        {
            proc_At();
            //	cout << " back to A with token " << nt.getVal() << " and type  " << nt.getType() << " and size "<< st.size() <<endl;
        }

        while (nt.getVal() == "+" || nt.getVal() == "-")
        {
            string temp = nt.getVal();
            read(temp, "OPERATOR");
            proc_At();
            build_tree(temp, "OPERATOR", 2);
        }
        //	cout << " Exit A with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
    }

    void proc_At()
    {
        // cout << " inside At " << endl;
        //	cout << " Enter At with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;

        proc_Af();
        //	cout << " back to At with token " << nt.getVal() << " and type " << nt.getType() << " with size "<< st.size() <<endl;
        while (nt.getVal() == "*" || nt.getVal() == "/")
        {
            string temp = nt.getVal();
            read(temp, "OPERATOR");
            proc_Af();
            build_tree(temp, "OPERATOR", 2);
        }
        //	cout << " Exit At with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
    }

    void proc_Af()
    {
        // cout << " inside Af " << endl;
        //	cout << " Enter Af with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
        proc_Ap();
        //	cout << " back to Af with token " << nt.getVal() << " and type " << nt.getType() << " and size "<< st.size() <<endl;
        if (nt.getVal() == "**")
        {
            read("**", "OPERATOR");
            proc_Af();
            build_tree("**", "KEYWORD", 2);
        }
        //	cout << " Exit Af with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
    }

    void proc_Ap()
    {
        // cout << " inside Ap " << endl;
        //	cout << " Enter Ap with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;

        proc_R();
        //	cout << " back to Ap with token " << nt.getVal() << " and size " << st.size() << endl;
        // token temp;
        while (nt.getVal() == "@")
        {
            read("@", "OPERATOR"); // read a type ID
            if (nt.getType() != "ID")
            {
                cout << " unexpected token ";
            }
            else
            {
                read(nt.getVal(), "ID");
                proc_R();
                //		    cout << " back to Ap token is " << nt.getVal() << " type " << nt.getType() << " and size "  << st.size() << endl;
                build_tree("@", "KEYWORD", 3);
            }
        }
        //	cout << " Exit Ap with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
    }

    void proc_R()
    {
        // cout << " inside R " << endl;
        //	cout << " Enter R with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
        proc_Rn();
        //	cout << " coming back to R with token " << nt.getVal() <<  " with size "<< st.size()  <<endl;
        //	cout << " in R " << nt.getType() << " with token " << nt.getVal() << endl;
        // cout << " again executing rn for token " << nt.getVal() << endl;
        while (nt.getType() == "ID" || nt.getType() == "INT" || nt.getType() == "STR" || nt.getVal() == "true" || nt.getVal() == "false" || nt.getVal() == "dummy" || nt.getVal() == "nil" || nt.getVal() == "(")
        {
            proc_Rn();
            build_tree("gamma", "KEYWORD", 2); // keep reading till valid node from rn comes
                                               //		cout << " token " << nt.getVal() << " if of type " << nt.getType() << endl;
        }
        //	cout << " Exit R with token " << nt.getVal() << " with type " << nt.getType() << "  with size " << st.size() << endl;
    }

    void proc_Rn()
    {
        //	cout << " Enter Rn  " <<" with token " << nt.getVal() << " and type " << nt.getType() << "in Rn with size " << st.size() <<endl;
        if (nt.getType() == "ID" || nt.getType() == "INT" || nt.getType() == "STR")
        {
            // cout << " this is " << nt.getType() << endl;
            read(nt.getVal(), nt.getType());
        }
        else if (nt.getVal() == "true")
        {
            read("true", "KEYWORD");
            build_tree("true", "BOOL", 0);
        }
        else if (nt.getVal() == "false")
        {
            read("false", "KEYWORD");
            build_tree("false", "BOOL", 0);
        }
        else if (nt.getVal() == "nil")
        {
            read("nil", "KEYWORD"); // i think i need to pass tokentype
            build_tree("nil", "NIL", 0);
        }
        else if (nt.getVal() == "(")
        {
            // cout << " i got here with " << nt.getVal() << endl;
            read("(", "PUNCTION");
            // cout << nt.getVal() << " with type before E " << nt.getType() << endl;
            proc_E();
            // cout << " came back to Rn with token " << nt.getVal() << endl;
            read(")", "PUNCTION"); // read ")" punction
        }
        else if (nt.getVal() == "dummy")
        {
            read("dummy", "KEYWORD");
            build_tree("dummy", "DUMMY", 0);
        }
        //	cout << " Exit Rn  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
    }

    void proc_D()
    {
        //	cout << " Enter D  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
        // cout << " inside D " << endl;
        proc_Da();
        if (nt.getVal() == "within")
        {
            read("within", "KEYWORD");
            proc_Da();
            build_tree("within", "KEYWORD", 2);
        }
        //	cout << " Exit D  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
    }

    void proc_Da()
    {
        //	cout << " Enter Da  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
        // cout << " inside Da" << endl;
        proc_Dr();
        int n = 1;
        while (nt.getVal() == "and")
        {
            n++;
            read("and", "KEYWORD");
            proc_Dr();
        }
        if (n > 1)
        {
            build_tree("and", "KEYWORD", n);
        }
        //	cout << " Exit Da  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
    }

    void proc_Dr()
    {
        //	cout << " Enter Dr  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
        // cout << " inside Dr " << endl;
        if (nt.getVal() == "rec")
        {
            read("rec", "KEYWORD");
            proc_Db();
            build_tree("rec", "KEYWORD", 1);
        }
        else
        {
            proc_Db();
        }
        //	cout << " Exit Dr  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
    }

    void proc_Db()
    { // verify this function doubtful, sort of read functuon and getTOken
        //	cout << " inside Db " << endl;
        // cout << " Enter Db  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
        // token temp;
        if (nt.getVal() == "(")
        {
            read("(", "PUNCTION");
            proc_D();
            read(")", "PUNCTION"); // read ")"
        }
        else if (nt.getType() == "ID")
        {
            read(nt.getVal(), "ID");
            int n = 1;
            if (nt.getVal() == "=" || nt.getVal() == ",")
            { // do i need to read a token here ????
                while (nt.getVal() == ",")
                {
                    read(",", "PUNCTION");
                    // temp.setVal(nt.getVal());
                    // temp.setType("ID");
                    read(nt.getVal(), "ID");
                    // read(nt);
                    n++;
                }
                if (n > 1)
                {
                    build_tree(",", "KEYWORD", n);
                }
                read("=", "OPERATOR"); // read "="
                proc_E();
                build_tree("=", "KEYWORD", 2);
            }
            else
            {
                do
                {
                    proc_Vb();
                    n++;
                } while (nt.getType() == "ID" || nt.getVal() == "(");
                read("=", "OPERATOR"); // read "="
                proc_E();
                build_tree("function_form", "KEYWORD", n + 1);
            }
        }
        //	cout << " Exit Db  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
    }

    void proc_Vb()
    {
        //	cout << " inside Vb" << endl;
        // cout << " Enter Vb  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
        if (nt.getType() == "ID")
        {
            read(nt.getVal(), "ID");
        }
        else if (nt.getVal() == "(")
        {
            read("(", "PUNCTION");
            if (nt.getVal() == ")")
            {
                read(")", "PUNCTION");
                build_tree("()", "KEYWORD", 0);
            }
            else
            {
                proc_Vl();
                //			cout << " came back from Vl " << endl;
                read(")", "PUNCTION"); // read ")"
            }
        }
        // cout << " Exit Vb  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
    }

    void proc_Vl()
    {
        //	cout << " inside Vl" << endl;
        // cout << " Enter Vl  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
        int n = 1;
        read(nt.getVal(), "ID"); // read type ID

        while (nt.getVal() == ",")
        {
            read(",", "PUNCTION");   // read "," and get next ID
            read(nt.getVal(), "ID"); // read and build tree for next ID and get next "," or anything else
            n++;
        }
        if (n > 1)
        {
            build_tree(",", "KEYWORD", n);
        }
        // cout << " Exit Vl  " <<" with token " << nt.getVal() << " and type " << nt.getType() << " with size " << st.size() <<endl;
    }
};
