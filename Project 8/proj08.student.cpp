/**********************************************************************
		Winnie Yang
    Computer Project #8
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

bool debug = false;     // checks if debug is on or off

// Structure to store registers
struct Registers
{
    string reg;
    string regBits = "0000";
};

vector <Registers> registerVector;      // Vector to store registers

// Structure to store information
struct DataCache
{
    int vBit = 0;       // V bit
    int mBit = 0;       // M bit
    string tagBits = "000";       // Tag bits
    // data block representing the 16 bits, 8 bytes
    string dataBlock[8] = {"00", "00", "00", "00", "00", "00", "00", "00"};     
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
    Name:  DisplayRegisters
    Purpose:  To display contents of registers
    Receive:  none
    Return:   None, just prints
  ---------------------------------------------------------------------------*/
void DisplayRegisters()
{
    for (int i = 0; i < 4; i++)
    {   
        // prints in horisontal order, every 4 per row
        for (int j = i; j < 16; j = j + 4)
        {
            cout << registerVector[j].reg << ": ";
            cout << registerVector[j].regBits;
            cout << "   ";
        }
        cout << endl;
    }
    cout << endl;
}

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
        cout << '[' << i << "]: ";      // index
        cout << dataCacheVector[i].vBit << ' ' << dataCacheVector[i].mBit << ' ';       // V and M bit
        cout << setfill('0') << setw(3) << dataCacheVector[i].tagBits << ' ';         // tag bits
        cout << dataCacheVector[i].dataBlock[0] << ' ';      // byte 0
        cout << dataCacheVector[i].dataBlock[1] << ' ';      // byte 1
        cout << dataCacheVector[i].dataBlock[2] << ' ';      // byte 2
        cout << dataCacheVector[i].dataBlock[3] << ' ';      // byte 3
        cout << dataCacheVector[i].dataBlock[4] << ' ';      // byte 4
        cout << dataCacheVector[i].dataBlock[5] << ' ';      // byte 5
        cout << dataCacheVector[i].dataBlock[6] << ' ';      // byte 6
        cout << dataCacheVector[i].dataBlock[7] << ' ';      // byte 7
        cout << "\n";
    }
    cout << "\n";
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
    Name:  Process
    Purpose:  Store info into the Data structures by processing command line
    Receive:  file name
    Return:   None
  ---------------------------------------------------------------------------*/
void Process (char* filename)
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

        int regIndex = 0;
        if (regNum == "0") {regIndex = 0; }
        if (regNum == "1") {regIndex = 1; }
        if (regNum == "2") {regIndex = 2; }
        if (regNum == "3") {regIndex = 3; }
        if (regNum == "4") {regIndex = 4; }
        if (regNum == "5") {regIndex = 5; }
        if (regNum == "6") {regIndex = 6; }
        if (regNum == "7") {regIndex = 7; }
        if (regNum == "8") {regIndex = 8; }
        if (regNum == "9") {regIndex = 9; }
        if (regNum == "a") {regIndex = 10; }
        if (regNum == "b") {regIndex = 11; }
        if (regNum == "c") {regIndex = 12; }
        if (regNum == "d") {regIndex = 13; }
        if (regNum == "e") {regIndex = 14; }
        if (regNum == "f") {regIndex = 15; }

        unsigned int address;
        string address_str = line.substr(6, 4);
        stringstream ss;
        ss << hex << address_str;
        ss >> address;
        
        unsigned int tag = (address >> 6) & 0x0000FFFF;     // Tag bits
        unsigned int cacheLine = (address >> 3) & 0x00000003;        // cache line accessed, index
        unsigned short offset = (address >> 0) & 0x3;       // byte offset

        char tagChar[10];
        int tagBuffer = sprintf(tagChar, "%03x", tag);
        
        char cacheChar[10];
        int cacheBuffer = sprintf(cacheChar, "%01x", cacheLine);
        int index = stoi(string(cacheChar));

        if (cacheLine > 7)
        {
            cout << "Error: Index goes over the data cache range." << endl;
            return;
        }

        char result = 'M';
        // hit means v bit is 1 and tag in cache matches tag in address
        if (dataCacheVector[index].vBit == 1 && dataCacheVector[index].tagBits == tagChar)
        {
            result = 'H';
        }

        dataCacheVector[index].vBit = 1;
        string dataValue = "0000";

        if (instruction == "LDR")
        {
            // load 2 bytes from RAM at address into register
            int ramLine = int(address / 16);    // line of ram to access
            int ramStart = address % 16;      // ram data block to start
            string ram1 = ramVector[ramLine].dataBlock[ramStart];   // first byte
            string ram2 = ramVector[ramLine].dataBlock[ramStart+1];    // second byte
            registerVector[regIndex].regBits = ram1 + ram2;
            dataCacheVector[index].tagBits = tagChar;   // changing tag bit
            dataCacheVector[index].dataBlock[offset] = ram1;   // first byte
            dataCacheVector[index].dataBlock[offset+1] = ram2;   // second byte
            dataValue = ram1 + ram2;    // data value
        }
        else if (instruction == "STR")
        {
            dataCacheVector[index].mBit = 1;     // only time when m bit would be set to 1
            // 2 bytes in register are stored into address of RAM
            string byte1 = registerVector[regIndex].regBits.substr(0,2);    // first 2 digits
            string byte2 = registerVector[regIndex].regBits.substr(2,2);    // last 2 digits
            int ramLine = int(address / 16);    // line of ram to access
            int ramStart = address % 16;      // ram data block to start
            ramVector[ramLine].dataBlock[ramStart] = byte1;     // changing first byte in ram
            ramVector[ramLine].dataBlock[ramStart+1] = byte2;   // changing second byte in ram

            // data value is copied from register to data cache
            dataCacheVector[index].tagBits = tagChar;
            dataCacheVector[index].dataBlock[offset] = byte1;   // first byte
            dataCacheVector[index].dataBlock[offset+1] = byte2;     // second byte
            dataValue = byte1 + byte2;      // data value
        }

        cout << instruction << ' ';
        cout << regNum << ' ';
        printf("%04x ", address);
        printf("%03x ", tag);
        printf("%01x ", cacheLine);
        cout << offset << ' ';
        cout << result << ' ';
        cout << dataValue << endl;
        cout << endl;
        
        if (debug)
        {
            DisplayCache();
        }
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

    // initializing data cache vector
    for (int i = 0; i < 8; i++)
    {
        DataCache data;
        dataCacheVector.push_back(data);
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
        // Display registers, data cache, and RAM
        DisplayRegisters();
        DisplayCache();
        DisplayRam();
    }

    if (ramExist)
    {
        OpenRam(ramFile);
    }

    Process(file);

    // display everything at the end of the program
    DisplayRegisters();
    DisplayCache();
    DisplayRam();
}

