/**********************************************************************
	Winnie Yang
    Computer Project #10
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
#include <bitset>

using namespace std;

bool debug = false;
vector<int> freeFrames;     // vector used to store free frames

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
    int wrCount = 0;        // number of write operations
    int pageFaultsCount = 0;        // number of page faults
    int writeBackCount = 0;         // number of write backs

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

        string pageFault = " ";
        string writeBack = " ";

        if (pageTableVector[int(pageNum)].Vbit == 0)
        {
            cout << "Fatal error: Page is not valid." << endl;
            return;
        }
        else if (pageTableVector[int(pageNum)].Vbit == 1 && pageTableVector[int(pageNum)].Pbit == 0)
        {
            cout << "Page fault processing: Page must be fetched." << endl;

            pageFault = "F";
            pageFaultsCount++;

            if (freeFrames.size() == 0)
            {
                for (int i = 0; i < pageTableVector.size(); i++)
                {
                    if (pageTableVector[i].Pbit == 1)
                    {
                        if (pageTableVector[i].Mbit == 1)
                        {
                            writeBack = "W";
                            writeBackCount++;
                        }
                        pageTableVector[i].Pbit = 0;
                        freeFrames.push_back(stoi(pageTableVector[i].Fn));
                        pageTableVector[i].Fn = "00";
                        break;
                    }
                }
            }
            else
            {
                int frame = freeFrames[0];      // first free frame in vector
                pageTableVector[int(pageNum)].Pbit = 1;
                pageTableVector[int(pageNum)].Rbit = 0;
                pageTableVector[int(pageNum)].Mbit = 0;
                pageTableVector[int(pageNum)].Fn = to_string(frame);
                freeFrames.erase(freeFrames.begin());     // erase used frame
                freeFrames.push_back(frame);        // add it to the back of vector
            }
        }

        // Changing R and M bit
        if (operation == "WR" && pageTableVector[int(pageNum)].Wbit == 1)
        {
            pageTableVector[int(pageNum)].Rbit = 1;
            pageTableVector[int(pageNum)].Mbit = 1;
        }
        else if (operation == "WR" && pageTableVector[int(pageNum)].Wbit == 0)
        {
            cout << "Access error: This page does not have write access." << endl;
        }

        // Changing R bit
        if (operation == "RD")
        {
            pageTableVector[int(pageNum)].Rbit  = 1;
        }

        unsigned int frameNum;
        stringstream ssFN;
        ssFN << hex << pageTableVector[int(pageNum)].Fn;
        ssFN >> frameNum;

        string physicalAddress;
        bitset<6> fn(frameNum);
        bitset<10> pageOffset(offset);
        string physicalBinary = fn.to_string() + pageOffset.to_string();

        for (int i = 0; i < physicalBinary.size(); i = i + 4)
        {
            string fourBits = physicalBinary.substr(i, 4);
            string hexStr;
            if (fourBits == "0000") { hexStr = "0";}
            else if (fourBits == "0001") { hexStr = "1"; }
            else if (fourBits == "0010") { hexStr = "2"; }
            else if (fourBits == "0011") { hexStr = "3"; }
            else if (fourBits == "0100") { hexStr = "4"; }
            else if (fourBits == "0101") { hexStr = "5"; }
            else if (fourBits == "0110") { hexStr = "6"; }
            else if (fourBits == "0111") { hexStr = "7"; }
            else if (fourBits == "1000") { hexStr = "8"; }
            else if (fourBits == "1001") { hexStr = "9"; }
            else if (fourBits == "1010") { hexStr = "a"; }
            else if (fourBits == "1011") { hexStr = "b"; }
            else if (fourBits == "1100") { hexStr = "c"; }
            else if (fourBits == "1101") { hexStr = "d"; }
            else if (fourBits == "1110") { hexStr = "e"; }
            else if (fourBits == "1111") { hexStr = "f"; }

            physicalAddress += hexStr;
        }

		cout << operation << ' ';       // operation
        printf("%04x ", address);       // virtual address
        printf("%01x ", pageNum);       // page number
        printf("%03x ", offset);         // page offset
        cout << pageFault << ' ';       // page fault flag
        cout << writeBack << ' ';       // write back flag
        cout << physicalAddress << endl;       // physical address
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
    cout << "Total number of page faults: " << pageFaultsCount << endl;
    cout << "Total number of write backs: " << writeBackCount << endl;

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
        cout << "Error: Proc file cannot be opened." << endl;
        return;
    }
    
    string line;
    bool firstLine = false;

    while (getline(file, line))
    {
        // read page frame count
        if (!firstLine)
        {
            firstLine = true;
            int pFrame = 20;
            for (int i = 0; i < stoi(line.substr(0, 1)); i++)
            {
                freeFrames.push_back(pFrame+i);
            }
            continue;
        }

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
    // If proc file exists
    if (proc)
    {
        ProcessProc(procFile);
    }
    // If refs file exists
    if (refs)
    {
        ProcessRefs(refsFile);
    }
    // Display everything at the end
    DisplayPageTable();

    return 0;
}
