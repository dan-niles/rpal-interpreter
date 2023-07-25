#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_
#include <map>
#include <iostream>

using namespace std;

class environment
{
public:
	environment *prev; // Pointer to the previous environment
	string name; // Name of the environment
	map<tree *, vector<tree *> > boundVar; // Map of bound variables to their values

	environment()
	{
		prev = NULL; // Default previous environment
		name = "env0"; // Default name
	}

	// Copy constructor
	environment(const environment &);

	// Assignment operator
	environment &operator=(const environment &env);
};

#endif
