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
	void print_tree(int dots);					// Print tree
	void setVal(string value);					// Set value of node
	void setType(string typ);					// Set type of node
	string getVal();							// Get value of node
	string getType();							// Get type of node
	tree *createNode(string value, string typ); // Create node
	tree *createNode(tree *x);					// Create node
};

// Set value of node
void tree::setVal(string value)
{
	val = value;
}

// Set type of node
void tree::setType(string typ)
{
	type = typ;
}

// Get value of node
string tree::getVal()
{
	return val;
}

// Get type of node
string tree::getType()
{
	return type;
}

// Create node with value and type
tree *createNode(string value, string typ)
{
	tree *temp = new tree();
	temp->setVal(value);
	temp->setType(typ);
	temp->left = NULL;
	temp->right = NULL;
	return temp;
}

// Create node with tree object
tree *createNode(tree *x)
{
	tree *temp = new tree();
	temp->setVal(x->getVal());
	temp->setType(x->getType());
	temp->left = x->left;
	temp->right = NULL;
	return temp;
}

// Print abstact syntax tree
void tree::print_tree(int dots)
{
	int n = 0;
	while (n < dots)
	{
		cout << ".";
		n++;
	}

	if (type == "ID" || type == "STR" || type == "INT")
	{
		cout << "<";
		cout << type;
		cout << ":";
	}

	if (type == "BOOL" || type == "NIL" || type == "DUMMY")
		cout << "<";

	cout << val;

	if (type == "ID" || type == "STR" || type == "INT")
		cout << ">";

	if (type == "BOOL" || type == "NIL" || type == "DUMMY")
		cout << ">";

	cout << endl;

	if (left != NULL)
		left->print_tree(dots + 1);

	if (right != NULL)
		right->print_tree(dots);
}

#endif
