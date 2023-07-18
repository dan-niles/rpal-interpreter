#ifndef TREE_H_
#define TREE_H_
#include <iostream>
#include <stack>

using namespace std;

class tree
{
private:
	string val;
	string type;

public:
	tree *left;
	tree *right;
	void print_my_tree(int dots);
	void setVal(string value);
	void setType(string typ);
	string getVal();
	string getType();
};

void tree::setVal(string value)
{
	val = value;
}

void tree::setType(string typ)
{
	type = typ;
}

string tree::getVal()
{
	return val;
}

string tree::getType()
{
	return type;
}

void tree::print_my_tree(int dots)
{
	int n = 0;
	while (n < dots)
	{
		cout << ".";
		n++;
	}
	// cout << val << " and " << type << endl;
	if (type == "ID" || type == "STR" || type == "INT")
	{
		// val = "<" + type + ":"+ val+ ">";
		// cout << "<" << type << ":" << val << ">";
		cout << "<";
		cout << type;
		cout << ":";
	}
	if (type == "BOOL" || type == "NIL" || type == "DUMMY")
	{
		cout << "<";
	}

	cout << val;

	if (type == "ID" || type == "STR" || type == "INT")
	{
		cout << ">";
	}

	if (type == "BOOL" || type == "NIL" || type == "DUMMY")
	{
		cout << ">";
	}

	cout << endl;

	if (left != NULL)
	{
		left->print_my_tree(dots + 1);
	}
	if (right != NULL)
	{
		right->print_my_tree(dots);
	}
}

#endif
