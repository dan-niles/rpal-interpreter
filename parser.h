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

stack<tree *> st; // Stack for syntax tree

// Array of operators
char operators[] = {'+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|', '$', '!', '#', '%',
                    '^', '_', '[', ']', '{', '}', '"', '`', '?'};

// Array of binary operators
string binary_operators[] = {"+", "-", "*", "/", "**", "gr", "ge", "<", "<=", ">", ">=", "ls", "le", "eq", "ne", "&", "or", "><"};

// Array of keywords
string keys[] = {"let", "fn", "in", "where", "aug", "or", "not", "true", "false", "nil", "dummy", "within",
                 "and", "rec", "gr", "ge", "ls", "le", "eq", "ne"};

class parser
{
public:
    token nextToken;     // Next token to be read
    char readnew[10000]; // Stores program for parsing
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
    bool isBinaryOperator(string op)
    {
        for (int i = 0; i < 18; i++)
        {
            if (op == binary_operators[i])
            {
                return true;
            }
        }
        return false;
    }

    // Checks if the given string is a number
    bool isNumber(const std::string &s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it))
            ++it;
        return !s.empty() && it == s.end();
    }

    // Read the next token
    void read(string val, string type)
    {
        if (val != nextToken.getVal() || type != nextToken.getType()) // Check if the next token is the expected token
        {
            cout << "Parse error: Expected " << "\"" << val << "\"" << ", but " << "\"" << nextToken.getVal() << "\"" << " was there" << endl;
            exit(0);
        }

        if (type == "ID" || type == "INT" || type == "STR") // If the token is an identifier, integer or string
            buildTree(val, type, 0);

        nextToken = getToken(readnew); // Get the next token

        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = getToken(readnew);
    }

    // Build tree for the given string, type and number of children
    void buildTree(string val, string type, int child)
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
        nextToken = getToken(readnew);          // Get the first token
        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = getToken(readnew);

        procedure_E(); // Start parsing from E

        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = getToken(readnew);

        if (index >= sizeOfFile - 1) // Check if end of file is reached
        {
            tree *t = st.top(); // Get the root of the tree

            // Print the abstact syntax tree if the flag is set
            if (astFlag == 1)
                t->print_tree(0);

            // Make the ST
            makeST(t);

            // Print the standardized tree if the flag is set
            if (astFlag == 2)
                t->print_tree(0);

            // Create
            tree *controlStructureArray[200][200];
            createControlStructures(t, controlStructureArray);

            int size = 0;
            int inner = 0;
            while (controlStructureArray[size][0] != NULL)
                size++;

            vector<vector<tree *> > setOfControlStruct;
            for (int i = 0; i < size; i++)
            {
                vector<tree *> temp;
                for (int j = 0; j < 200; j++)
                {
                    if (controlStructureArray[i][j] != NULL)
                        temp.push_back(controlStructureArray[i][j]);
                }
                setOfControlStruct.push_back(temp);
            }

            cse_machine(setOfControlStruct);
        }
    }

   // Helper function for makeStandardTree
    void makeST(tree *t)
    {
        makeStandardTree(t);
    }

    // Makes the tree standard
    tree *makeStandardTree(tree *t)
    {
        if (t == NULL)
            return NULL;
        makeStandardTree(t->left);
        makeStandardTree(t->right);

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
                t->left->right = E; 
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

    /* -------------------------------- CSE Machine -------------------------------- */

    // Creates the necessary control structures for CSE machine
    void createControlStructures(tree *x, tree *(*setOfControlStruct)[200])
    {
        static int index = 1;
        static int j = 0;
        static int i = 0;
        static int betaCount = 1;
        if (x == NULL)
            return;

        if (x->getVal() == "lambda") // If node is lambda
        {
            std::stringstream ss;

            int t1 = i;
            int k = 0;
            setOfControlStruct[i][j] = createNode("", "");
            i = 0;

            while (setOfControlStruct[i][0] != NULL)
            {
                i++;
                k++;
            }
            i = t1;
            ss << k;
            index++;

            string str = ss.str();
            tree *temp = createNode(str, "deltaNumber");

            setOfControlStruct[i][j++] = temp;

            setOfControlStruct[i][j++] = x->left;

            setOfControlStruct[i][j++] = x;

            int myStoredIndex = i;
            int tempj = j + 3;

            while (setOfControlStruct[i][0] != NULL)
                i++;
            j = 0;

            createControlStructures(x->left->right, setOfControlStruct);

            i = myStoredIndex;
            j = tempj;
        }
        else if (x->getVal() == "->") // If node is conditional node
        {
            int myStoredIndex = i;
            int tempj = j;
            int nextDelta = index;
            int k = i;

            std::stringstream ss2;
            ss2 << nextDelta;
            string str2 = ss2.str();
            tree *temp1 = createNode(str2, "deltaNumber");

            setOfControlStruct[i][j++] = temp1;

            int nextToNextDelta = index;
            std::stringstream ss3;
            ss3 << nextToNextDelta;
            string str3 = ss3.str();
            tree *temp2 = createNode(str3, "deltaNumber");
            setOfControlStruct[i][j++] = temp2;

            tree *beta = createNode("beta", "beta");

            setOfControlStruct[i][j++] = beta;

            while (setOfControlStruct[k][0] != NULL)
            {
                k++;
            }
            int firstIndex = k;
            int lamdaCount = index;

            createControlStructures(x->left, setOfControlStruct);
            int diffLc = index - lamdaCount;

            while (setOfControlStruct[i][0] != NULL)
                i++;
            j = 0;

            createControlStructures(x->left->right, setOfControlStruct);

            while (setOfControlStruct[i][0] != NULL)
                i++;
            j = 0;

            createControlStructures(x->left->right->right, setOfControlStruct);

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

            setOfControlStruct[myStoredIndex][tempj]->setVal(str5);
            stringstream ss24;
            ss24 << i;
            string str6 = ss24.str();

            setOfControlStruct[myStoredIndex][tempj + 1]->setVal(str6);

            i = myStoredIndex;
            j = 0;

            while (setOfControlStruct[i][j] != NULL)
            {
                j++;
            }
            betaCount += 2;
        }
        else if (x->getVal() == "tau") // If node is tau node
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

            setOfControlStruct[i][j++] = countNode; // putting the number of kids of tua
            tree *tauNode = createNode("tau", "tau");

            setOfControlStruct[i][j++] = tauNode; // putting the tau node and not pushing x

            createControlStructures(x->left, setOfControlStruct);
            x = x->left;
            while (x != NULL)
            {
                createControlStructures(x->right, setOfControlStruct);
                x = x->right;
            }
        }
        else
        {
            setOfControlStruct[i][j++] = createNode(x->getVal(), x->getType());
            createControlStructures(x->left, setOfControlStruct);
            if (x->left != NULL)
                createControlStructures(x->left->right, setOfControlStruct);
        }
    }

    // Control Stack Environment Machine
    void cse_machine(vector<vector<tree *> > &controlStructure)
    {
        stack<tree *> control;                   // Stack for control structure
        stack<tree *> m_stack;                   // Stack for operands
        stack<environment *> stackOfEnvironment; // Stack of environments
        stack<environment *> getCurrEnvironment;

        int currEnvIndex = 0;                     // Initial environment
        environment *currEnv = new environment(); // e0
        currEnv->name = "env0";

        currEnvIndex++;
        m_stack.push(createNode(currEnv->name, "ENV"));
        control.push(createNode(currEnv->name, "ENV"));
        stackOfEnvironment.push(currEnv);
        getCurrEnvironment.push(currEnv);

        vector<tree *> tempDelta;
        tempDelta = controlStructure.at(0); // Get the first control structure
        for (int i = 0; i < tempDelta.size(); i++)
        {
            control.push(tempDelta.at(i)); // Push each element of the control structure to the control stack
        }

        while (!control.empty())
        {
            tree *nextToken;
            nextToken = control.top(); // Get the top of the control stack
            control.pop();             // Pop the top of the control stack

            if (nextToken->getVal() == "nil")
            {
                nextToken->setType("tau");
            }

            if (nextToken->getType() == "INT" || nextToken->getType() == "STR" || nextToken->getVal() == "lambda" || nextToken->getVal() == "YSTAR" || nextToken->getVal() == "Print" || nextToken->getVal() == "Isinteger" || nextToken->getVal() == "Istruthvalue" || nextToken->getVal() == "Isstring" || nextToken->getVal() == "Istuple" || nextToken->getVal() == "Isfunction" || nextToken->getVal() == "Isdummy" || nextToken->getVal() == "Stem" || nextToken->getVal() == "Stern" || nextToken->getVal() == "Conc" || nextToken->getType() == "BOOL" || nextToken->getType() == "NIL" || nextToken->getType() == "DUMMY" || nextToken->getVal() == "Order" || nextToken->getVal() == "nil")
            {
                if (nextToken->getVal() == "lambda")
                {
                    tree *boundVar = control.top(); // Variable bouded to lambda
                    control.pop();

                    tree *nextDeltaIndex = control.top(); // Index of next control structure to access
                    control.pop();

                    tree *env = createNode(currEnv->name, "ENV");

                    m_stack.push(nextDeltaIndex); // Index of next control structure to access
                    m_stack.push(boundVar);       // Variable bouded to lambda
                    m_stack.push(env);            // Environment it was created in
                    m_stack.push(nextToken);      // Lambda Token
                }
                else
                {
                    m_stack.push(nextToken); // Push token to the stack
                }
            }
            else if (nextToken->getVal() == "gamma") // If gamma is on top of control stack
            {
                tree *machineTop = m_stack.top();
                if (machineTop->getVal() == "lambda") // CSE Rule 4 (Apply lambda)
                {
                    m_stack.pop(); // Pop lambda token

                    tree *prevEnv = m_stack.top();
                    m_stack.pop(); // Pop the environment in which it was created

                    tree *boundVar = m_stack.top();
                    m_stack.pop(); // Pop variable bounded to lambda

                    tree *nextDeltaIndex = m_stack.top();
                    m_stack.pop(); // Pop index of next control structure to access

                    environment *newEnv = new environment(); // Create new environment

                    std::stringstream ss;
                    ss << currEnvIndex;
                    string str = ss.str(); // Create string of new environment name

                    newEnv->name = "env" + str;

                    // if (currEnvIndex > 10000) // Avoid stack overflow
                    //     return;

                    stack<environment *> tempEnv = stackOfEnvironment;

                    while (tempEnv.top()->name != prevEnv->getVal()) // Get the previous environment node
                        tempEnv.pop();

                    newEnv->prev = tempEnv.top(); // Set the previous environment node

                    // Bounding variables to the environment
                    if (boundVar->getVal() == "," && m_stack.top()->getVal() == "tau") // If Rand is tau
                    {
                        vector<tree *> boundVariables;     // Vector of bound variables
                        tree *leftOfComa = boundVar->left; // Get the left of the comma
                        while (leftOfComa != NULL)
                        {
                            boundVariables.push_back(createNode(leftOfComa));
                            leftOfComa = leftOfComa->right;
                        }

                        vector<tree *> boundValues; // Vector of bound values
                        tree *tau = m_stack.top();  // Pop the tau token
                        m_stack.pop();

                        tree *tauLeft = tau->left; // Get the left of the tau
                        while (tauLeft != NULL)
                        {
                            boundValues.push_back(tauLeft);
                            tauLeft = tauLeft->right; // Get the right of the tau
                        }

                        for (int i = 0; i < boundValues.size(); i++)
                        {
                            if (boundValues.at(i)->getVal() == "tau")
                            {
                                stack<tree *> res;
                                arrangeTuple(boundValues.at(i), res);
                            }

                            vector<tree *> nodeValVector;
                            nodeValVector.push_back(boundValues.at(i));

                            // Insert the bound variable and its value to the environment
                            newEnv->boundVar.insert(std::pair<tree *, vector<tree *> >(boundVariables.at(i), nodeValVector));
                        }
                    }
                    else if (m_stack.top()->getVal() == "lambda") // If Rand is lambda
                    {
                        vector<tree *> nodeValVector;
                        stack<tree *> temp;
                        int j = 0;
                        while (j < 4)
                        {
                            temp.push(m_stack.top());
                            m_stack.pop();
                            j++;
                        }

                        while (!temp.empty())
                        {
                            tree *fromStack;
                            fromStack = temp.top();
                            temp.pop();
                            nodeValVector.push_back(fromStack);
                        }

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *> >(boundVar, nodeValVector));
                    }
                    else if (m_stack.top()->getVal() == "Conc") // If Rand is Conc
                    {
                        vector<tree *> nodeValVector;
                        stack<tree *> temp;
                        int j = 0;
                        while (j < 2)
                        {
                            temp.push(m_stack.top());
                            m_stack.pop();
                            j++;
                        }

                        while (!temp.empty())
                        {
                            tree *fromStack;
                            fromStack = temp.top();
                            temp.pop();
                            nodeValVector.push_back(fromStack);
                        }

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *> >(boundVar, nodeValVector));
                    }
                    else if (m_stack.top()->getVal() == "eta") // If Rand is eta
                    {
                        vector<tree *> nodeValVector;
                        stack<tree *> temp;
                        int j = 0;
                        while (j < 4)
                        {
                            temp.push(m_stack.top());
                            m_stack.pop();
                            j++;
                        }

                        while (!temp.empty())
                        {
                            tree *fromStack;
                            fromStack = temp.top();
                            temp.pop();
                            nodeValVector.push_back(fromStack);
                        }

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *> >(boundVar, nodeValVector));
                    }
                    else // If Rand is an Int
                    {
                        tree *bindVarVal = m_stack.top();
                        m_stack.pop();

                        vector<tree *> nodeValVector;
                        nodeValVector.push_back(bindVarVal);

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *> >(boundVar, nodeValVector));
                    }

                    currEnv = newEnv;
                    control.push(createNode(currEnv->name, "ENV"));
                    m_stack.push(createNode(currEnv->name, "ENV"));
                    stackOfEnvironment.push(currEnv);
                    getCurrEnvironment.push(currEnv);

                    istringstream is3(nextDeltaIndex->getVal());
                    int deltaIndex;
                    is3 >> deltaIndex;

                    vector<tree *> nextDelta = controlStructure.at(deltaIndex); // Get the next control structure
                    for (int i = 0; i < nextDelta.size(); i++)
                    {
                        control.push(nextDelta.at(i)); // Push each element of the next control structure to the control stack
                    }
                    currEnvIndex++;
                }
                else if (machineTop->getVal() == "tau") // CSE Rule 10 (Tuple Selection)
                {

                    tree *tau = m_stack.top(); // Get tau node from top of stack
                    m_stack.pop();

                    tree *selectTupleIndex = m_stack.top(); // Get the index of the child to be selected
                    m_stack.pop();

                    istringstream is4(selectTupleIndex->getVal());
                    int tupleIndex;
                    is4 >> tupleIndex;

                    tree *tauLeft = tau->left;
                    tree *selectedChild;
                    while (tupleIndex > 1) // Get the child to be selected
                    {
                        tupleIndex--;
                        tauLeft = tauLeft->right;
                    }

                    selectedChild = createNode(tauLeft);
                    if (selectedChild->getVal() == "lamdaTuple")
                    {
                        tree *getNode = selectedChild->left;
                        while (getNode != NULL)
                        {
                            m_stack.push(createNode(getNode));
                            getNode = getNode->right;
                        }
                    }
                    else
                    {
                        m_stack.push(selectedChild);
                    }
                }
                else if (machineTop->getVal() == "YSTAR") // CSE Rule 12 (Applying YStar)
                {
                    m_stack.pop(); // Pop YSTAR token
                    if (m_stack.top()->getVal() == "lambda")
                    {
                        tree *etaNode = createNode(m_stack.top()->getVal(), m_stack.top()->getType()); // Create eta node
                        etaNode->setVal("eta");
                        m_stack.pop();

                        tree *boundEnv1 = m_stack.top(); // Pop bounded environment
                        m_stack.pop();

                        tree *boundVar1 = m_stack.top(); // Pop bounded variable
                        m_stack.pop();

                        tree *deltaIndex1 = m_stack.top(); // Pop index of next control structure
                        m_stack.pop();

                        // Push the required nodes to the stack
                        m_stack.push(deltaIndex1);
                        m_stack.push(boundVar1);
                        m_stack.push(boundEnv1);
                        m_stack.push(etaNode);
                    }
                    else
                        return; // Error
                }
                else if (machineTop->getVal() == "eta") // CSE Rule 13 (Applying f.p)
                {
                    tree *eta = m_stack.top(); // Pop eta node
                    m_stack.pop();

                    tree *boundEnv1 = m_stack.top(); // Pop bounded environment
                    m_stack.pop();

                    tree *boundVar1 = m_stack.top(); // Pop bounded variable
                    m_stack.pop();

                    tree *deltaIndex1 = m_stack.top(); // Pop index of next control structure
                    m_stack.pop();

                    // Push the eta node back into the stack
                    m_stack.push(deltaIndex1);
                    m_stack.push(boundVar1);
                    m_stack.push(boundEnv1);
                    m_stack.push(eta);

                    // Push a lambda node with same parameters as the eta node
                    m_stack.push(deltaIndex1);
                    m_stack.push(boundVar1);
                    m_stack.push(boundEnv1);
                    m_stack.push(createNode("lambda", "KEYWORD"));

                    // Push two gamma nodes onto control stack
                    control.push(createNode("gamma", "KEYWORD"));
                    control.push(createNode("gamma", "KEYWORD"));
                }
                else if (machineTop->getVal() == "Print") // Print next item on stack
                {
                    m_stack.pop();
                    tree *nextToPrint = m_stack.top(); // Get item to print

                    if (nextToPrint->getVal() == "tau") // If the next item is a tuple
                    {
                        tree *getTau = m_stack.top();

                        stack<tree *> res;
                        arrangeTuple(getTau, res); // Arrange the tuple into a stack

                        stack<tree *> getRev; // Reverse the stack
                        while (!res.empty())
                        {
                            getRev.push(res.top());
                            res.pop();
                        }

                        cout << "("; // Print the tuple
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
                    else if (nextToPrint->getVal() == "lambda") // If the next item is a lambda token
                    {
                        m_stack.pop(); // Pop lambda token

                        tree *env = m_stack.top(); // Get the environment in which it was created
                        m_stack.pop();

                        tree *boundVar = m_stack.top(); // Get the variable bounded to lambda
                        m_stack.pop();

                        tree *num = m_stack.top(); // Get the index of next control structure to access
                        m_stack.pop();

                        cout << "[lambda closure: " << boundVar->getVal() << ": " << num->getVal() << "]";
                        return;
                    }
                    else // If the next item is a string or integer
                    {
                        if (m_stack.top()->getType() == "STR")
                            cout << addSpaces(m_stack.top()->getVal());
                        else
                            cout << m_stack.top()->getVal();
                    }
                }
                else if (machineTop->getVal() == "Isinteger") // Check if next item in stack is an integer
                {
                    m_stack.pop(); // Pop Isinteger token

                    tree *isNextInt = m_stack.top(); // Get next item in stack
                    m_stack.pop();

                    if (isNextInt->getType() == "INT")
                    {
                        tree *resNode = createNode("true", "boolean");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "boolean");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Istruthvalue") // Check if next item in stack is a boolean value
                {
                    m_stack.pop(); // Pop Istruthvalue token

                    tree *isNextBool = m_stack.top(); // Get next item in stack
                    m_stack.pop();

                    if (isNextBool->getVal() == "true" || isNextBool->getVal() == "false")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Isstring") // Check if next item in stack is a string
                {
                    m_stack.pop(); // Pop Isstring token

                    tree *isNextString = m_stack.top(); // Get next item in stack
                    m_stack.pop();

                    if (isNextString->getType() == "STR")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Istuple") // Check if next item in stack is a tuple
                {
                    m_stack.pop(); // Pop Istuple token

                    tree *isNextTau = m_stack.top(); // Get next item in stack
                    m_stack.pop();

                    if (isNextTau->getType() == "tau")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Isfunction") // Check if next item in stack is a function
                {
                    m_stack.pop(); // Pop Isfunction token

                    tree *isNextFn = m_stack.top(); // Get next item in stack

                    if (isNextFn->getVal() == "lambda")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Isdummy") // Check if next item in stack is dummy
                {
                    m_stack.pop(); // Pop Isdummy token

                    tree *isNextDmy = m_stack.top(); // Get next item in stack

                    if (isNextDmy->getVal() == "dummy")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getVal() == "Stem") // Get first character of string
                {
                    m_stack.pop();                      // Pop Stem token
                    tree *isNextString = m_stack.top(); // Get next item in stack

                    if (isNextString->getVal() == "")
                        return;

                    if (isNextString->getType() == "STR")
                    {
                        string strRes = "'" + isNextString->getVal().substr(1, 1) + "'"; // Get first character
                        m_stack.pop();
                        m_stack.push(createNode(strRes, "STR"));
                    }
                }
                else if (machineTop->getVal() == "Stern") // Get remaining characters other the first character
                {
                    m_stack.pop();                      // Pop Stern token
                    tree *isNextString = m_stack.top(); // Get next item in stack

                    if (isNextString->getVal() == "")
                        return;

                    if (isNextString->getType() == "STR")
                    {
                        string strRes = "'" + isNextString->getVal().substr(2, isNextString->getVal().length() - 3) + "'"; // Get remaining characters
                        m_stack.pop();
                        m_stack.push(createNode(strRes, "STR"));
                    }
                }
                else if (machineTop->getVal() == "Order") // Get number of items in tuple
                {
                    m_stack.pop(); // Pop Order token

                    int numOfItems = 0;
                    tree *getTau = m_stack.top(); // Get next item in stack

                    if (getTau->left != NULL)
                        getTau = getTau->left;

                    while (getTau != NULL)
                    {
                        numOfItems++; // Count number of items
                        getTau = getTau->right;
                    }

                    getTau = m_stack.top();
                    m_stack.pop();

                    if ((getTau->getVal() == "nil"))
                        numOfItems = 0;

                    stringstream ss11;
                    ss11 << numOfItems;
                    string str34 = ss11.str();
                    tree *orderNode = createNode(str34, "INT");
                    m_stack.push(orderNode);
                }
                else if (machineTop->getVal() == "Conc") // Concatenate two strings
                {
                    tree *concNode = m_stack.top(); // Pop Conc token
                    m_stack.pop();

                    tree *firstString = m_stack.top(); // Get first string
                    m_stack.pop();

                    tree *secondString = m_stack.top(); // Get second string

                    if (secondString->getType() == "STR" || (secondString->getType() == "STR" && secondString->left != NULL && secondString->left->getVal() == "true"))
                    {
                        m_stack.pop();
                        string res = "'" + firstString->getVal().substr(1, firstString->getVal().length() - 2) + secondString->getVal().substr(1, secondString->getVal().length() - 2) + "'";
                        tree *resNode = createNode(res, "STR");
                        m_stack.push(resNode);
                        control.pop();
                    }
                    else
                    {
                        concNode->left = firstString;
                        m_stack.push(concNode);
                        firstString->left = createNode("true", "flag");
                    }
                }
            }
            else if (nextToken->getVal().substr(0, 3) == "env") // If env token is on top of control stack (CSE Rule 5)
            {
                stack<tree *> removeFromMachineToPutBack;
                if (m_stack.top()->getVal() == "lambda") // Pop lambda token and its parameters
                {
                    removeFromMachineToPutBack.push(m_stack.top());
                    m_stack.pop();
                    removeFromMachineToPutBack.push(m_stack.top());
                    m_stack.pop();
                    removeFromMachineToPutBack.push(m_stack.top());
                    m_stack.pop();
                    removeFromMachineToPutBack.push(m_stack.top());
                    m_stack.pop();
                }
                else
                {
                    removeFromMachineToPutBack.push(m_stack.top()); // Pop value from stack
                    m_stack.pop();
                }

                tree *remEnv = m_stack.top(); // Get the environment to remove

                if (nextToken->getVal() == remEnv->getVal()) // If the environment to remove is the same as the one on top of the control stack
                {
                    m_stack.pop();

                    stack<tree *> printMachine = m_stack;

                    getCurrEnvironment.pop();
                    if (!getCurrEnvironment.empty())
                    {
                        currEnv = getCurrEnvironment.top();
                    }
                    else
                        currEnv = NULL;
                }
                else
                    return;

                while (!removeFromMachineToPutBack.empty()) // Push the popped values back to the stack
                {
                    m_stack.push(removeFromMachineToPutBack.top());
                    removeFromMachineToPutBack.pop();
                }
            }
            // If any variables are on top of the control stack
            else if (nextToken->getType() == "ID" && nextToken->getVal() != "Print" && nextToken->getVal() != "Isinteger" && nextToken->getVal() != "Istruthvalue" && nextToken->getVal() != "Isstring" && nextToken->getVal() != "Istuple" && nextToken->getVal() != "Isfunction" && nextToken->getVal() != "Isdummy" && nextToken->getVal() != "Stem" && nextToken->getVal() != "Stern" && nextToken->getVal() != "Conc")
            {
                environment *temp = currEnv;
                int flag = 0;
                while (temp != NULL)
                {
                    map<tree *, vector<tree *> >::iterator itr = temp->boundVar.begin();
                    while (itr != temp->boundVar.end())
                    {
                        if (nextToken->getVal() == itr->first->getVal())
                        {
                            vector<tree *> temp = itr->second;
                            if (temp.size() == 1 && temp.at(0)->getVal() == "Conc" && temp.at(0)->left != NULL)
                            {
                                control.push(createNode("gamma", "KEYWORD"));
                                m_stack.push(temp.at(0)->left);
                                m_stack.push(temp.at(0));
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
                                            m_stack.push(createNode(myLambda));
                                            myLambda = myLambda->right;
                                        }
                                    }
                                    else
                                    {
                                        if (temp.at(i)->getVal() == "tau")
                                        {
                                            stack<tree *> res;
                                            arrangeTuple(temp.at(i), res);
                                        }
                                        m_stack.push(temp.at(i));
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
            // If a binary or unary operator is on top of the control stack (CSE Rule 6 and CSE Rule 7)
            else if (isBinaryOperator(nextToken->getVal()) || nextToken->getVal() == "neg" || nextToken->getVal() == "not")
            {
                string op = nextToken->getVal();           // Get the operator
                if (isBinaryOperator(nextToken->getVal())) // If token is a binary operator
                {
                    tree *node1 = m_stack.top(); // Get the first operand
                    m_stack.pop();

                    tree *node2 = m_stack.top(); // Get the second operand
                    m_stack.pop();

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

                        // Perform the operation and create a node with the result
                        if (op == "+") // Addition
                        {
                            res = num1 + num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
                            m_stack.push(res);
                        }
                        else if (op == "-") // Subtraction
                        {
                            res = num1 - num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
                            m_stack.push(res);
                        }
                        else if (op == "*") // Multiplication
                        {
                            res = num1 * num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
                            m_stack.push(res);
                        }
                        else if (op == "/") // Division
                        {
                            if (num2 == 0) // If division by zero
                                cout << "Exception: STATUS_INTEGER_DIVIDE_BY_ZERO" << endl; 
                            res = num1 / num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
                            m_stack.push(res);
                        }
                        else if (op == "**") // Power
                        {
                            resPow = pow((double)num1, (double)num2);
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
                            m_stack.push(res);
                        }
                        else if (op == "gr" || op == ">") // Greater than
                        {
                            string resStr = num1 > num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                        else if (op == "ge" || op == ">=") // Greater than or equal to
                        {
                            string resStr = num1 >= num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                        else if (op == "ls" || op == "<") // Less than
                        {
                            string resStr = num1 < num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                        else if (op == "le" || op == "<=") // Less than or equal to
                        {
                            string resStr = num1 <= num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                        else if (op == "eq" || op == "=") // Equal
                        {
                            string resStr = num1 == num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                        else if (op == "ne" || op == "><") // Not equal
                        {
                            string resStr = num1 != num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                    }
                    else if (node1->getType() == "STR" && node2->getType() == "STR") // If both operands are strings
                    {
                        if (op == "ne" || op == "<>") 
                        {
                            string resStr = node1->getVal() != node2->getVal() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            m_stack.push(res);
                        }
                        else if (op == "eq" || op == "==")
                        {
                            string resStr = node1->getVal() == node2->getVal() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            m_stack.push(res);
                        }
                    }
                    // If both operands are boolean values
                    else if ((node1->getVal() == "true" || node1->getVal() == "false") && (node2->getVal() == "false" || node2->getVal() == "true"))
                    {
                        if (op == "ne" || op == "<>")
                        {
                            string resStr = node1->getVal() != node2->getVal() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            m_stack.push(res);
                        }
                        else if (op == "eq" || op == "==")
                        {
                            string resStr = node1->getVal() == node2->getVal() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            m_stack.push(res);
                        }
                        else if (op == "or")
                        {
                            string resStr;
                            if (node1->getVal() == "true" || node2->getVal() == "true")
                            {
                                resStr = "true";
                                tree *res = createNode(resStr, "BOOL");
                                m_stack.push(res);
                            }
                            else
                            {
                                resStr = "false";
                                tree *res = createNode(resStr, "BOOL");
                                m_stack.push(res);
                            }
                        }
                        else if (op == "&")
                        {
                            string resStr;
                            if (node1->getVal() == "true" && node2->getVal() == "true")
                            {
                                resStr = "true";
                                tree *res = createNode(resStr, "BOOL");
                                m_stack.push(res);
                            }
                            else
                            {
                                resStr = "false";
                                tree *res = createNode(resStr, "BOOL");
                                m_stack.push(res);
                            }
                        }
                    }
                }
                // If unary operator
                else if (op == "neg" || op == "not")
                {
                    if (op == "neg") // If negation
                    {
                        tree *node1 = m_stack.top();
                        m_stack.pop();
                        istringstream is1(node1->getVal());
                        int num1;
                        is1 >> num1;
                        int res = -num1;
                        stringstream ss;
                        ss << res;
                        string str = ss.str();
                        tree *resStr = createNode(str, "INT");
                        m_stack.push(resStr);
                    }
                    else if (op == "not" && (m_stack.top()->getVal() == "true" || m_stack.top()->getVal() == "false")) // If not
                    {
                        if (m_stack.top()->getVal() == "true")
                        {
                            m_stack.pop();
                            m_stack.push(createNode("false", "BOOL"));
                        }
                        else
                        {
                            m_stack.pop();
                            m_stack.push(createNode("true", "BOOL"));
                        }
                    }
                }
            }
            // If beta is on top of control stack (CSE Rule 8)
            else if (nextToken->getVal() == "beta")
            {
                tree *boolVal = m_stack.top(); // Get the boolean value from stack
                m_stack.pop();

                tree *elseIndex = control.top(); // Get the index of the else statement
                control.pop();

                tree *thenIndex = control.top(); // Get the index of the then statement
                control.pop();

                int index;
                if (boolVal->getVal() == "true") // If the boolean value is true, then go to the then statement
                {
                    istringstream is1(thenIndex->getVal());
                    is1 >> index;
                }
                else // If the boolean value is false, then go to the else statement
                {
                    istringstream is1(elseIndex->getVal());
                    is1 >> index;
                }

                vector<tree *> nextDelta = controlStructure.at(index); // Get the next control structure from index
                for (int i = 0; i < nextDelta.size(); i++)             // Push each element of the next control structure to the control stack
                    control.push(nextDelta.at(i));
            }
            // If tau is on top of control stack (CSE Rule 9)
            else if (nextToken->getVal() == "tau")
            {
                tree *tupleNode = createNode("tau", "tau");

                tree *noOfItems = control.top(); // Get the number of items in the tuple
                control.pop();

                int numOfItems;

                istringstream is1(noOfItems->getVal());
                is1 >> numOfItems; // Convert the number of items to an integer

                if (m_stack.top()->getVal() == "lambda") // If the first item is a lambda token
                {
                    tree *lamda = createNode(m_stack.top()->getVal(), m_stack.top()->getType()); // Pop lambda
                    m_stack.pop();

                    tree *prevEnv = createNode(m_stack.top()->getVal(), m_stack.top()->getType()); // Pop the environment in which it was created
                    m_stack.pop();                                                                 // popped the evn in which it was created

                    tree *boundVar = createNode(m_stack.top()->getVal(), m_stack.top()->getType()); // Pop the variable bounded to lambda
                    m_stack.pop();                                                                  // bound variable of lambda

                    tree *nextDeltaIndex = createNode(m_stack.top()->getVal(), m_stack.top()->getType()); // Pop the index of next control structure
                    m_stack.pop();                                                                        // next delta to be opened

                    // Create a lambda tuple and set parameters
                    tree *myLambda = createNode("lamdaTuple", "lamdaTuple");
                    myLambda->left = nextDeltaIndex;
                    nextDeltaIndex->right = boundVar;
                    boundVar->right = prevEnv;
                    prevEnv->right = lamda;
                    tupleNode->left = myLambda;
                }
                else
                {
                    tupleNode->left = createNode(m_stack.top());
                    m_stack.pop();
                }

                tree *sibling = tupleNode->left; // Get the first item in the tuple

                for (int i = 1; i < numOfItems; i++) // For each item in the tuple
                {
                    tree *temp = m_stack.top();     // Get the next item in the stack
                    if (temp->getVal() == "lambda") // If the next item is a lambda token
                    {
                        tree *lamda = createNode(m_stack.top()->getVal(), m_stack.top()->getType()); // Pop lambda
                        m_stack.pop();

                        tree *prevEnv = createNode(m_stack.top()->getVal(), m_stack.top()->getType()); // Pop the environment in which it was created
                        m_stack.pop();

                        tree *boundVar = createNode(m_stack.top()->getVal(), m_stack.top()->getType()); // Pop the variable bounded to lambda
                        m_stack.pop();

                        tree *nextDeltaIndex = createNode(m_stack.top()->getVal(), m_stack.top()->getType()); // Pop the index of next control structure
                        m_stack.pop();

                        tree *myLambda = createNode("lamdaTuple", "lamdaTuple");

                        myLambda->left = nextDeltaIndex;
                        nextDeltaIndex->right = boundVar;
                        boundVar->right = prevEnv;
                        prevEnv->right = lamda;
                        sibling->right = myLambda;
                        sibling = sibling->right;
                    }
                    else // If the next item is not a lambda token
                    {
                        m_stack.pop();
                        sibling->right = temp;
                        sibling = sibling->right;
                    }
                }
                m_stack.push(tupleNode); // Push the tuple to the stack
            }
            // If aug is on top of control stack
            else if (nextToken->getVal() == "aug")
            {
                tree *token1 = createNode(m_stack.top()); // Get the first item in the stack
                m_stack.pop();

                tree *token2 = createNode(m_stack.top()); // Get the second item in the stack
                m_stack.pop();

                if (token1->getVal() == "nil" && token2->getVal() == "nil") // If both tokens are nil
                {
                    tree *tupleNode = createNode("tau", "tau");
                    tupleNode->left = token1;
                    m_stack.push(tupleNode);
                }
                else if (token1->getVal() == "nil") // If the first token is nil
                {
                    tree *tupleNode = createNode("tau", "tau");
                    tupleNode->left = token2;
                    m_stack.push(tupleNode);
                }
                else if (token2->getVal() == "nil") // If the second token is nil
                {
                    tree *tupleNode = createNode("tau", "tau");
                    tupleNode->left = token1;
                    m_stack.push(tupleNode);
                }
                else if (token1->getType() != "tau") // If the first token is not a tuple
                {
                    tree *tupleNode = token2->left;
                    while (tupleNode->right != NULL)
                    {
                        tupleNode = tupleNode->right;
                    }
                    tupleNode->right = createNode(token1);
                    m_stack.push(token2);
                }
                else if (token2->getType() != "tau") // If the second token is not a tuple
                {
                    tree *tupleNode = token1->left;
                    while (tupleNode->right != NULL)
                    {
                        tupleNode = tupleNode->right;
                    }
                    tupleNode->right = createNode(token2);
                    m_stack.push(token1);
                }
                else // If both tokens are tuples
                {
                    tree *tupleNode = createNode("tau", "tau");
                    tupleNode->left = token1;
                    tupleNode->left->right = token2;
                    m_stack.push(tupleNode);
                }
            }
        }

        cout << endl;
    }

    /* -------------------------------- Helper Functions -------------------------------- */

    // Arranges nodes in tauNode into a stack
    void arrangeTuple(tree *tauNode, stack<tree *> &res)
    {
        if (tauNode == NULL)
            return;
        if (tauNode->getVal() == "lamdaTuple")
            return;
        if (tauNode->getVal() != "tau" && tauNode->getVal() != "nil")
        {
            res.push(tauNode);
        }
        arrangeTuple(tauNode->left, res);
        arrangeTuple(tauNode->right, res);
    }

    // Adds spaces to the string
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

            buildTree("let", "KEYWORD", 2);
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
            } while (nextToken.getType() == "ID" || nextToken.getVal() == "(");
            read(".", "OPERATOR");
            procedure_E();

            buildTree("lambda", "KEYWORD", n + 1);
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
            buildTree("where", "KEYWORD", 2);
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
            buildTree("tau", "KEYWORD", n);
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
            buildTree("aug", "KEYWORD", 2);
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
            buildTree("->", "KEYWORD", 3);
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
            buildTree("or", "KEYWORD", 2);
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
            buildTree("&", "KEYWORD", 2);
        }
    }

    // Bs -> ’not’ Bp | Bp
    void procedure_Bs()
    {
        if (nextToken.getVal() == "not") // If next token is 'not', then rule is, Bs -> ’not’ Bp
        {
            read("not", "KEYWORD");
            procedure_Bp();
            buildTree("not", "KEYWORD", 1);
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
            buildTree("gr", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "ge" || nextToken.getVal() == ">=") // Bp -> A (’ge’ | ’>=’)
        {
            read(temp, temp2);
            procedure_A();
            buildTree("ge", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "ls" || nextToken.getVal() == "<") // Bp -> A (’ls’ | ’<’ )
        {
            read(temp, temp2);
            procedure_A();
            buildTree("ls", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "le" || nextToken.getVal() == "<=") // Bp -> A (’le’ | ’<=’)
        {
            read(temp, temp2);
            procedure_A();
            buildTree("le", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "eq") // Bp -> A ’eq’ A
        {
            read(temp, temp2);
            procedure_A();
            buildTree("eq", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "ne") // Bp -> A ’ne’ A
        {
            read(temp, temp2);
            procedure_A();
            buildTree("ne", "KEYWORD", 2);
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
            buildTree("neg", "KEYWORD", 1);
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
            buildTree(temp, "OPERATOR", 2);
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
            buildTree(temp, "OPERATOR", 2);
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
            buildTree("**", "KEYWORD", 2);
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
            read("@", "OPERATOR"); // read a type ID
            if (nextToken.getType() != "ID")
            {
                cout << "Exception: UNEXPECTED_TOKEN";
            }
            else
            {
                read(nextToken.getVal(), "ID");
                procedure_R();
                buildTree("@", "KEYWORD", 3);
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
        while (nextToken.getType() == "ID" || nextToken.getType() == "INT" || nextToken.getType() == "STR" || nextToken.getVal() == "true" || nextToken.getVal() == "false" || nextToken.getVal() == "nil" || nextToken.getVal() == "(" || nextToken.getVal() == "dummy")
        {
            procedure_Rn();
            buildTree("gamma", "KEYWORD", 2);
        }
    }

    /*
    Rn -> ’<ID>’
        -> ’<INT>’
        -> ’<STR>’
        -> ’true’
        -> ’false’
        -> ’nil’
        -> ’(’ E ’)’
        -> ’dummy’
    */
    void procedure_Rn()
    {
        if (nextToken.getType() == "ID" || nextToken.getType() == "INT" || nextToken.getType() == "STR") // Rn -> ’<ID>’ | ’<INT>’ | ’<STR>’
        {
            read(nextToken.getVal(), nextToken.getType());
        }
        else if (nextToken.getVal() == "true") // Rn -> ’true’
        {
            read("true", "KEYWORD");
            buildTree("true", "BOOL", 0);
        }
        else if (nextToken.getVal() == "false") // Rn -> ’false’
        {
            read("false", "KEYWORD");
            buildTree("false", "BOOL", 0);
        }
        else if (nextToken.getVal() == "nil") // Rn -> ’nil’
        {
            read("nil", "KEYWORD");
            buildTree("nil", "NIL", 0);
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
            buildTree("dummy", "DUMMY", 0);
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
            buildTree("within", "KEYWORD", 2);
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
            buildTree("and", "KEYWORD", n);
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
            buildTree("rec", "KEYWORD", 1);
        }
        else
        {
            procedure_Db();
        }
    }

    /*
   Db -> Vl ’=’ E => ’=’
      -> ’<ID>’ Vb+ ’=’ E => ’fcn_form’
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
        else if (nextToken.getType() == "ID")
        {
            read(nextToken.getVal(), "ID");
            int n = 1;
            if (nextToken.getVal() == "=" || nextToken.getVal() == ",")
            {
                while (nextToken.getVal() == ",")
                {
                    read(",", "PUNCTION");
                    read(nextToken.getVal(), "ID");
                    n++;
                }
                if (n > 1)
                {
                    buildTree(",", "KEYWORD", n);
                }
                read("=", "OPERATOR");
                procedure_E();
                buildTree("=", "KEYWORD", 2);
            }
            else
            {
                do
                {
                    procedure_Vb();
                    n++;
                } while (nextToken.getType() == "ID" || nextToken.getVal() == "(");
                read("=", "OPERATOR");
                procedure_E();
                buildTree("function_form", "KEYWORD", n + 1);
            }
        }
    }

    /*
    Vb -> ’<ID>’
        -> ’(’ Vl ’)’
        -> ’(’ ’)’
    */
    void procedure_Vb()
    {
        if (nextToken.getType() == "ID")
        {
            read(nextToken.getVal(), "ID");
        }
        else if (nextToken.getVal() == "(")
        {
            read("(", "PUNCTION");
            if (nextToken.getVal() == ")")
            {
                read(")", "PUNCTION");
                buildTree("()", "KEYWORD", 0);
            }
            else
            {
                procedure_Vl();
                read(")", "PUNCTION");
            }
        }
    }

    /*
    Vl -> ’<ID>’ list ’,’
    */
    void procedure_Vl()
    {
        int n = 1;
        read(nextToken.getVal(), "ID");

        while (nextToken.getVal() == ",")
        {
            read(",", "PUNCTION");
            read(nextToken.getVal(), "ID");
            n++;
        }
        if (n > 1)
        {
            buildTree(",", "KEYWORD", n);
        }
    }
};
