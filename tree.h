#ifndef TREE_H_
#define TREE_H_
#include <iostream>
#include <stack>

using namespace std;

// Class for syntax tree
class tree
{
private:
	string val;	 // Value of node
	string type; // Type of node

public:
	tree *left;									// Left child
	tree *right;								// Right child
	void setType(string typ);					// Set type of node
	void setVal(string value);					// Set value of node
	string getType();							// Get type of node
	string getVal();							// Get value of node
	tree *createNode(string value, string typ); // Create node
	tree *createNode(tree *x);					// Create node
	void print_tree(int no_of_dots);					// Print tree
};

// Set type of node
void tree::setType(string typ)
{
	type = typ;
}

// Set value of node
void tree::setVal(string value)
{
	val = value;
}

// Get type of node
string tree::getType()
{
	return type;
}

// Get value of node
string tree::getVal()
{
	return val;
}

// Create node with value and type
tree *createNode(string value, string typ)
{
	tree *t = new tree();
	t->setVal(value);
	t->setType(typ);
	t->left = NULL;
	t->right = NULL;
	return t;
}

// Create node with tree object
tree *createNode(tree *x)
{
	tree *t = new tree();
	t->setVal(x->getVal());
	t->setType(x->getType());
	t->left = x->left;
	t->right = NULL;
	return t;
}

// Print syntax tree
void tree::print_tree(int no_of_dots)
{
	int n = 0;
	while (n < no_of_dots) 
	{
		cout << ".";
		n++;
	}

	// If node type is ID, STR or INT, print <type:val>
	if (type == "ID" || type == "STR" || type == "INT")
	{
		cout << "<";
		cout << type;
		cout << ":";
	}

	// If node type is BOOL, NIL or DUMMY, print <val>
	if (type == "BOOL" || type == "NIL" || type == "DUMMY")
		cout << "<";

	cout << val;

	// If node type is ID, STR or INT, print >
	if (type == "ID" || type == "STR" || type == "INT")
		cout << ">";

	// If node type is BOOL, NIL or DUMMY, print >
	if (type == "BOOL" || type == "NIL" || type == "DUMMY")
		cout << ">";

	cout << endl;

	// Print left and right subtrees
	if (left != NULL)	
		left->print_tree(no_of_dots + 1);

	if (right != NULL)
		right->print_tree(no_of_dots);
}

#endif
