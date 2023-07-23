#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include "parser.h"

using namespace std;

int main(int argc, const char **argv)
{
    if (argc > 1)
    {
        int argv_idx = 1; // Index of file name in argv
        int ast_flag = 0; // Flag to check if AST or ST is to be printed

        if (argc == 3) // Check if AST or ST flag is present
        {
            argv_idx = 2;
            if (strcmp(argv[1], "-ast") == 0)
                ast_flag = 1;
            else if (strcmp(argv[2], "-st") == 0)
                ast_flag = 2;
            else
            {
                cout << " Error : Incorrect input value " << endl;
                return 1;
            }
        }

        string filepath = argv[argv_idx];    // Read file name from command line
        const char *file = filepath.c_str(); // Convert string to char array

        // Create input stream object and read file into string
        ifstream input(filepath);
        if (!input)
        {
            std::cout << "Failed to open file: " << filepath << "\n";
            return 1;
        }
        // Open file
        string file_str((istreambuf_iterator<char>(input)), (istreambuf_iterator<char>())); // Read file into string
        input.close();

        // Convert string to char array
        char file_array[file_str.size()];
        for (int i = 0; i < file_str.size(); i++)
            file_array[i] = file_str[i];

        // Create parser object and start parsing
        parser rpal_parser(file_array, 0, file_str.size(), ast_flag);
        rpal_parser.start_parsing();
    }
    else
        cout << " Error : Incorrect no. of inputs " << endl;

    return 0;
}