#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_
#include <map>
#include <iostream>

using namespace std;

class environment
{
public:
	environment *prev;
	string name;
	map<tree *, vector<tree *> > boundVar;

	environment()
	{
		prev = NULL;
		name = "env0";
	}

	// Copy constructor
	environment(const environment &);

	// Assignment operator
	environment &operator=(const environment &env);
};

#endif
