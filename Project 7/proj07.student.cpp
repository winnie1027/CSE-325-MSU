/**********************************************************************
    Winnie Yang
    Computer Project #7
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

// Structure to store registers
struct Registers
{
    string reg;
    unsigned int regBits[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
};

vector <Registers> registerVector;      // Vector to store registers

// Structure to store information
struct DataCache
{
    unsigned short vBit = 0;       // V bit
    unsigned short mBit = 0;       // M bit
    unsigned int tagBits = 0x000;       // Tag bits
    // data block representing the 16 bits, 8 bytes
    unsigned int dataBlock[8] = {0, 0, 0, 0, 0, 0, 0, 0};     
};

vector<DataCache> dataCacheVector;      // Vector to store each line of data cache

// Structure to store RAM
struct Ram
{
    unsigned int address;
    string dataBlock[16] = {"00", "00", "00", "00", "00", "00", "00", "00",
      "00", "00", "00", "00", "00", "00", "00", "00"};
};

vector<Ram> ramVector;      // Vector to store Ram lines

/*---------------------------------------------------------------------------
    Name:  DisplayCache
    Purpose:  To display contents of data cache
    Receive:  none
    Return:   None, just prints
  ---------------------------------------------------------------------------*/
void DisplayCache()
{
    cout << "     V M Tag  0  1  2  3  4  5  6  7" << endl;
    cout << "------------------------------------" << endl;

    for (int i = 0; i < 8; i++)
    {
        DataCache data;

        cout << '[' << i << "]: ";      // index
        cout << data.vBit << ' ' << data.mBit << ' ';       // V and M bit
        cout << setfill('0') << setw(3) << data.tagBits;         // tag bits
        cout << "  ";
        cout << data.dataBlock[0] << setw(2) << hex << "  ";      // byte 0
        cout << data.dataBlock[1] << setw(2) << hex << "  ";      // byte 1
        cout << data.dataBlock[2] << setw(2) << hex << "  ";      // byte 2
        cout << data.dataBlock[3] << setw(2) << hex << "  ";      // byte 3
        cout << data.dataBlock[4] << setw(2) << hex << "  ";      // byte 4
        cout << data.dataBlock[5] << setw(2) << hex << "  ";      // byte 5
        cout << data.dataBlock[6] << setw(2) << hex << "  ";      // byte 6
        cout << data.dataBlock[7] << setw(2) << hex << "  ";      // byte 7
        cout << "\n";
    }
    cout << "\n";
}

/*---------------------------------------------------------------------------
    Name:  DisplayRegisters
    Purpose:  To display contents of registers
    Receive:  none
    Return:   None, just prints
  ---------------------------------------------------------------------------*/
void DisplayRegisters()
{
    // Add each register to register vector
    for (int i = 0; i < 16; i++)
    {
        Registers reg;
        string registerName = reg.reg;
        // register names that end with a number
        if (i < 10)
        {
            registerName = 'R' + to_string(i);
        }
        // change register names to letters
        else
        {
            if (i == 10)
            {
                registerName = "Ra";
            }
            else if (i == 11)
            {
                registerName = "Rb";
            }
            else if (i == 12)
            {
                registerName = "Rc";
            }
            else if (i == 13)
            {
                registerName = "Rd";
            }
            else if (i == 14)
            {
                registerName = "Re";
            }
            else if (i == 15)
            {
                registerName = "Rf";
            }
        }
        reg.reg = registerName;
        registerVector.push_back(reg);
    }

    for (int i = 0; i < 4; i++)
    {   
        // prints in horisontal order, every 4 per row
        for (int j = i; j < 16; j = j + 4)
        {
            cout << registerVector[j].reg << ": ";
            cout << setfill('0') << setw(4) << registerVector[j].regBits[j];
            cout << "   ";
        }
        cout << endl;
    }
    cout << endl;
}

/*---------------------------------------------------------------------------
    Name:  DisplayRam
    Purpose:  To display initial contents of Ram
    Receive:  none
    Return:   None, just prints
  ---------------------------------------------------------------------------*/
void DisplayRam()
{
    // Printing ram content
    for (int i = 0; i < 8; i++)
    {
        cout << "00" << ramVector[i].address << '0' << ": ";
        for (int j = 0; j < 16; j++)
        {
            cout << ramVector[i].dataBlock[j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

/*---------------------------------------------------------------------------
    Name:  OpenRam
    Purpose:  To open ram file
    Receive:  File name
    Return:   None, just prints
  ---------------------------------------------------------------------------*/
void OpenRam(char* filename)
{
    ifstream file;
    file.open(filename);

    if (!file.is_open())
    {
        cout << "Error: Ram file cannot be opened." << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        Ram ram;
        int address;
        string address_str = line.substr(0, 4);
        stringstream ss;
        ss << hex << address_str;
        ss >> address;
        address = int(address) / 16;
      
        int byteIndex = 0;    // starting position of vector for Ram structure
        for (int i = 5; i < 51; i = i + 3)
        {
            string byte = line.substr(i, 2);
            ram.dataBlock[byteIndex] = byte;
            byteIndex++;
        }
        ram.address = address;
        ramVector[address] = ram;
    }
    file.close();
}

/*---------------------------------------------------------------------------
    Name:  Debug
    Purpose:  Store info into the Data structure with debugging
    Receive:  file name
    Return:   None
  ---------------------------------------------------------------------------*/
void Debug (char* filename)
{
    // Display registers, data cache, and RAM
    DisplayRegisters();
    DisplayCache();
    DisplayRam();
    
    ifstream file;
    file.open(filename);

    if (!file.is_open())
    {
        cout << "Error: Cannot be opened." << endl;
        return;
    }

    string line;        // every line in file

    while (getline(file, line))
    {    
        string instruction = line.substr(0, 3);
        string regNum = line.substr(4, 1);       // register number
        
        unsigned int address;       // physical address
        string address_str = line.substr(6, 4); 
        stringstream ss;
        ss << hex << address_str;
        ss >> address;

        unsigned short tag = (address >> 6) & 0x0000FFFF;     // Tag bits
        unsigned short cacheLine = (address >> 3) & 0x00000003;        // cache line accessed
        unsigned short offset = (address >> 0) & 0x3;       // byte offset

        if (instruction == "LDR")
        {
            // load 2 bytes from RAM at address into register

        }
        else if (instruction == "STR")
        {
            // 2 bytes in register are stored into address of RAM

        }
        cout << instruction << ' ';
        cout << regNum << ' ';
        printf("%04x ", address);
        printf("%03x ", tag);
        printf("%01x ", cacheLine);
        cout << offset << endl;
    }
    cout << endl;
}

/*---------------------------------------------------------------------------
    Name:  NonDebug
    Purpose:  Store info into the Data structure without debugging
    Receive:  file name
    Return:   None
  ---------------------------------------------------------------------------*/
void NonDebug (char* filename)
{
    ifstream file;
    file.open(filename);

    if (!file.is_open())
    {
        cout << "Error: Cannot be opened." << endl;
        return;
    }

    string line;        // every line in file

    while (getline(file, line))
    {    
        string instruction = line.substr(0, 3);
        string regNum = line.substr(4, 1);       // register number
    
        unsigned int address;
        string address_str = line.substr(6, 4);
        stringstream ss;
        ss << hex << address_str;
        ss >> address;
        
        unsigned int tag = (address >> 6) & 0x0000FFFF;     // Tag bits
        unsigned int cacheLine = (address >> 3) & 0x00000003;        // cache line accessed
        unsigned short offset = (address >> 0) & 0x3;       // byte offset

        if (instruction == "LDR")
        {
            // load 2 bytes from RAM at address into register

        }
        else if (instruction == "STR")
        {
            // 2 bytes in register are stored into address of RAM

        }
        cout << instruction << ' ';
        cout << regNum << ' ';
        printf("%04x ", address);
        printf("%03x ", tag);
        printf("%01x ", cacheLine);
        cout << offset << endl;
    }
    cout << endl;
}

/*---------------------------------------------------------------------------
    Name:  main
    Purpose:  Main function
    Receive:  argc(length of argv), argv
    Return:   None
  ---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    char *file;     // stores the input file
    char *ramFile;      // stores the ram file
    bool debug = false;     // checks if debug is on or off
    bool ramExist = false;      // checks if ram file is entered

    for (int i = 1; i < argc; i++)
    {
        // input command
        if (strcmp(argv[i], "-input") == 0)
        {
            // if the command after is not a filename
            if (argv[i+1][0] == '-')
            {
                cout << "Error: This is not a file name." << endl;
                return 0;
            }
            file = argv[i+1];
            i++;
        }
        // debug command
        else if (strcmp(argv[i], "-debug") == 0)
        {
            debug = true;
        }
        // ram command
        else if (strcmp(argv[i], "-ram") == 0)
        {
            // if the command after is not a file name
            if (argv[i+1][0] == '-')
            {
                cout << "Error: This is not a file name." << endl;
                return 0;
            }
            ramFile = argv[i+1];
            i++;
            ramExist = true;
        }
        else 
        {
            cout << "Invalid command." << endl;
            return 0;
        }
    }
    
    // Setting address names for ram
    for (unsigned int i = 0; i < 4096; i++)
    {
       Ram ram;
       ram.address = i;
       ramVector.push_back(ram);
    }

    if (debug == true)
    {
        Debug(file);
    }
    else if (debug == false)
    {
        NonDebug(file);
    }

    if (ramExist)
    {
        OpenRam(ramFile);
    }
    // display everything at the end of the program
    DisplayRegisters();
    DisplayCache();
    DisplayRam();
}

