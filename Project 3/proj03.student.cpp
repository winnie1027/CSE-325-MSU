/*
    Winnie Yang
    CSE 325 Project 3
                        */

#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


using namespace std;


/* 
*  This function copies contents from one file to another.
*  param: Takes 2 arguments.
*  Return: 0 for Error or 1 otherwise.
*/
int main(int argc, char* argv[]) {

    // Represent -b, -a, -t. True if found in argument, False otherwise.
    bool bSize = false;
    bool aAppend = false;
    bool tTruncate = false;

    // Default buffer size is 64 bytes
    long bufferSize = 64;

    // To read the number of bytes
    ssize_t read_bytes;

    // True if found in argument, false otherwise.
    bool sourceFile = false;
    bool destFile = false;

    // To store source file and destination file.
    char *storeSource, *storeDest;

    // Initialized to open source and destination files
    int fdSource, fdDest;

    // Needs at least 3 arguments to work
    if (argc < 3)
    {
        cout << "Need at least 3 arguments." << endl;
        return 1;
    }

    // Loops through all arguments
    for (int i = 0; i < argc; i++)
    {   
        // Check for flag symbol '-'
        if (argv[i][0] == '-')
        {   
            // Option -b
            if (argv[i][1] == 'b')
            {
                if (bSize == false)
                {
                    bSize = true;
                    bufferSize = atoi(argv[i+1]);
                    cout << "Byte size has been changed to: " << bufferSize << endl;
                }
                else if (bSize == true)
                {
                    cout << "Error: Option -b is being repeated." << endl;
                    return 0;
                }
            }
            // Option -a
            else if (argv[i][1] == 'a')
            {
                if (aAppend == false)
                {
                    aAppend = true;
                }
                else if (aAppend == true)
                {
                    cout << "Error: Option -a is being repeated." << endl;
                    return 0;
                }
            }
            // Option -t
            else if (argv[i][1] == 't')
            {
                if (tTruncate == false)
                {
                    tTruncate = true;
                }
                else if (tTruncate == true)
                {
                    cout << "Error: Option -t is being repeated." << endl;
                    return 0;
                }
            }
            // No available options were entered
            else
            {
                cout << "Wrong flag is used, please input -a, -b, or -t." << endl;
                return 0;
            }
        }
        // No flag symbol detected, checks for 2 files names
        else {
            string buffStr = to_string(bufferSize);
            if (argv[i] != buffStr)
            {
                // If source file does not exist yet
                if (sourceFile == false && strcmp(argv[i], "proj03") != 0)
                {
                    sourceFile = true;
                    storeSource = argv[i];
                    cout << "Source file is stored!" << endl;
                }
                // If destination file does not exist yet
                else if (destFile == false && strcmp(argv[i], "proj03") != 0)
                {
                    destFile = true;
                    storeDest = argv[i];
                    cout << "Destination file is stored!" << endl;
                }
            }
        }
    }

    // If -b is inputted
    if (bSize == true)
    {
        if (bufferSize < 1)
        {
            cout << "Error: Buffer size is too small." << endl;
            return 0;
        }
    }
    // Reset buffer size to default 64 bytes
    else if (bSize == false)
    {
        bufferSize = 64;
    }

    // -a and -t co-exist
    if (aAppend == true && tTruncate == true)
    {
        cout << "Error: The append and Truncate options cannot coexist." << endl;
        return 0;
    }

    // If either one of the files don't exist
    if (sourceFile == false || destFile == false)
    {
        cout << "Error: There needs to be at least 2 file inputs." << endl;
        return 0;
    }
    // If both files are the same
    if (strcmp(storeSource, storeDest) == 0)
    {
        cout << "Error: Source and Destination files cannot be the same." << endl;
        return 0;
    }

    // Sets buffer
    char *buffer[bufferSize];

    // Opens source file
    fdSource = open(storeSource, O_RDWR, S_IRUSR|S_IWUSR);

    // Source file cannot be found
    if (fdSource < 0)
    {
        cout << "Error: Unable to open source file." << endl;
        return 0;
    }
    // -a does not exist but -t does
    if (aAppend == false && tTruncate == true)
    {
        fdDest = open(storeDest, O_APPEND|O_RDWR, S_IRUSR|S_IWUSR);
    }
    // -a exists but -t does not
    else if (aAppend == true && tTruncate == false)
    {
        fdDest = open(storeDest, O_APPEND|O_RDWR, S_IRUSR|S_IWUSR);
    }
    else
    {
        fdDest = open(storeDest, O_RDWR, S_IRUSR|S_IWUSR);
    }

    // Destination file cannot be found
    if (fdDest < 0)
    {
        fdDest = open(storeDest, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    }

    // Read bytes
    read_bytes = read(fdSource, buffer, bufferSize);

    // Write file
    write(fdDest, buffer, read_bytes);

    // Close files
    close(fdSource);
    close(fdDest);

    // End of program
    return 1;
}