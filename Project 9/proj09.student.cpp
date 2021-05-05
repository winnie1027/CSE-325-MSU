/**********************************************************************
		Winnie Yang
    Computer Project #9
**********************************************************************/

#include <iostream>
#include <iomanip>
#include <ratio>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <typeinfo>

using namespace std;

bool debug = false;

// Structure to store page table
struct PageTable
{
    string index;
    int Vbit = 0;
    int Wbit = 0;
    int Pbit = 0;
    int Rbit = 0;
    int Mbit = 0;
    string Fn = "00";
};

vector<PageTable> pageTableVector;      // Vector used to store page table structures

/*---------------------------------------------------------------------------
    Name:  DisplayPageTable
    Purpose:  Displays the page table structure
    Receive:  argc(length of argv), argv
    Return:   None
---------------------------------------------------------------------------*/
void DisplayPageTable()
{
    cout << "     V W P R M FN" << endl;
    cout << "-----------------" << endl;
    for (int i = 0; i < 16; i++)
    {
        cout << '[' << pageTableVector[i].index << "]: ";       // index
        cout << pageTableVector[i].Vbit << ' ';     // V bit, valid
        cout << pageTableVector[i].Wbit << ' ';     // W bit, writable
        cout << pageTableVector[i].Pbit << ' ';     // P bit, present
        cout << pageTableVector[i].Rbit << ' ';     // R bit, referenced
        cout << pageTableVector[i].Mbit << ' ';     // M bit, modified
        cout << pageTableVector[i].Fn << "\n";      // Frame number
    }
    cout << "\n";
}

/*---------------------------------------------------------------------------
    Name:  ProcessRefs
    Purpose:  Process refs file
    Receive:  refs filename
    Return:   None
---------------------------------------------------------------------------*/
void ProcessRefs(char* filename)
{
    ifstream file;
    file.open(filename);

    if (!file.is_open())
    {
        cout << "Error: Refs file cannot be opened." << endl;
        return;
    }

    string line;    // each line in refs file

    int rdCount = 0;        // number of read operations
    int wrCount = 0;        // Number of write operations

    while (getline(file, line))
    {
        string operation = line.substr(0, 2);
        string address_str = line.substr(3, 4);
					
        if (operation == "RD")
        {
            rdCount++;
        }
        else if (operation == "WR")
        {
            wrCount++;
        }
 
				unsigned int address;
        stringstream ss;
        ss << hex << address_str;
        ss >> address;
        
        unsigned short pageNum = (address & 0xFC00) >> 10;        // page number
        unsigned int offset = address & 0x000003FF;         // page offset

        cout << operation << ' ';
        printf("%04x ", address);
        printf("%01x ", pageNum);
        printf("%03x", offset);
        cout << endl;

        if (debug)
        {
            cout << endl;
            DisplayPageTable();
        }
    }
    cout << endl;
    cout << "Total number of read operations: " << rdCount << endl;
    cout << "Total number of write operations: " << wrCount << endl;
    cout << endl;
}

/*---------------------------------------------------------------------------
    Name:  ProcessProc
    Purpose:  Process proc file
    Receive:  proc filename
    Return:   None
---------------------------------------------------------------------------*/
void ProcessProc(char* filename)
{
    ifstream file;
    file.open(filename);

    if (!file.is_open())
    {
        cout << "Error: Proc file canoot be opened." << endl;
        return;
    }
    
    string line;

    while (getline(file, line))
    {
        string pageNum = line.substr(0, 1);
        int writePermission = stoi(line.substr(2, 1));

        int pageIndex = 0;
        if (pageNum == "0") { pageIndex = 0; }
        if (pageNum == "1") { pageIndex = 1; }
        if (pageNum == "2") { pageIndex = 2; }
        if (pageNum == "3") { pageIndex = 3; }
        if (pageNum == "4") { pageIndex = 4; }
        if (pageNum == "5") { pageIndex = 5; }
        if (pageNum == "6") { pageIndex = 6; }
        if (pageNum == "7") { pageIndex = 7; }
        if (pageNum == "8") { pageIndex = 8; }
        if (pageNum == "9") { pageIndex = 9; }
        if (pageNum == "a") { pageIndex = 10; }
        if (pageNum == "b") { pageIndex = 11; }
        if (pageNum == "c") { pageIndex = 12; }
        if (pageNum == "d") { pageIndex = 13; }
        if (pageNum == "e") { pageIndex = 14; }
        if (pageNum == "f") { pageIndex = 15; }

        // change Wbit of page table for write permission
        pageTableVector[pageIndex].Wbit = writePermission;
        // change Vbit of page table for validity
        pageTableVector[pageIndex].Vbit = 1;
    }
}

/*---------------------------------------------------------------------------
    Name:  main
    Purpose:  Main function
    Receive:  argc(length of argv), argv
    Return:   None
---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    char *procFile;
    char *refsFile;
    bool proc = false;
    bool refs = false;

    for (int i = 1; i < argc; i++)
    {
        // proc command
        if (strcmp(argv[i], "-proc") == 0)
        {
            // if the command after is not a proc filename
            if (argv[i+1][0] == '-')
            {
                cout << "Error: This is not a proc file name." << endl;
                return 0;
            }
            procFile = argv[i+1];
            proc = true;
            i++;
        }
        // debug command
        else if (strcmp(argv[i], "-debug") == 0)
        {
            debug = true;
        }
        // refs command
        else if (strcmp(argv[i], "-refs") == 0)
        {
            // if the command after is not a filename
            if (argv[i+1][0] == '-')
            {
                cout << "Error: This is not a refs file name." << endl;
                return 0;
            }
            refsFile = argv[i+1];
            refs = true;
            i++;
        }
        else
        {
            cout << "Error: Invalid command." << endl;
            return 0;
        }
    }

    // Initializing page table vector
    for (int i = 0; i < 16; i++)
    {
        PageTable table;
        string indexName = table.index;
        // Index that are not alphabets
        if (i < 10)
        {
            indexName = to_string(i);
        }
        // Change index to letters
        else
        {
            if (i == 10)
            {
                indexName = "a";
            }
            else if (i == 11)
            {
                indexName = "b";
            }
            else if (i == 12)
            {
                indexName = "c";
            }
            else if (i == 13)
            {
                indexName = "d";
            }
            else if (i == 14)
            {
                indexName = "e";
            }
            else if (i == 15)
            {
                indexName = "f";
            }
        }
        table.index = indexName;
        pageTableVector.push_back(table);
    }
    // If debug command is entered
    if (debug)
    {
        DisplayPageTable();
    }
    // If refs file exists
    if (refs)
    {
        ProcessRefs(refsFile);
    }
    // If proc file exists
    if (proc)
    {
        ProcessProc(procFile);
    }
    // Display everything at the end
    DisplayPageTable();

    return 0;
}
