  /****************************************************************************
    Winnie Yang
    Computer Project #4
  ****************************************************************************/

#include <unistd.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <iostream>

using namespace std;

  /*---------------------------------------------------------------------------
    Name:  optionA
    Purpose:  Option A: Displays the absolute pathname of the file entered
    Receive:  none
    Return:   none
  ---------------------------------------------------------------------------*/
void optionA(char *filename)
{   
    char *path = realpath(filename, NULL);
    cout << "Absolute path name: " << path << endl;
}

  /*---------------------------------------------------------------------------
    Name:  optionB
    Purpose:  Option B: Displays permissions for the file
    Receive:  none
    Return:   none
  ---------------------------------------------------------------------------*/
char* optionB(char *filename)
{
    struct stat sb;
    char *modeval = (char*)malloc(sizeof(char) * 9 + 1);
    if (stat(filename, &sb) == 0)
    {
        mode_t perm = sb.st_mode;
        modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
        modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
        modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
        modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
        modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
        modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
        modeval[6] = (perm & S_IROTH) ? 'r' : '-';
        modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
        modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
        modeval[9] = '\0';
        cout << "File permissions: " << modeval << endl;
        return modeval;
    }
    else
    {
        return modeval;
    }
}
  /*-------------------------------------------------------------------------
    Purpose:  Option C: Displays size of the file in bytes
    Receive:  filename
    Return:   none
  ---------------------------------------------------------------------------*/
void optionC(char *filename)
{
    struct stat sb;
    if (stat(filename, &sb) == 0)
    {
        cout << "File size in bytes: " << sb.st_size << endl;
    }
}

  /*---------------------------------------------------------------------------
    Name:  optionD
    Purpose:  Option D: Displays the numeric ID of the file's owner
    Receive:  filename
    Return:   none
  ---------------------------------------------------------------------------*/
void optionD(char *filename)
{
    struct stat sb;
    if (stat(filename, &sb) == 0)
    {
        cout << "Ownership ID: " << sb.st_uid << endl;
    }

}

  /*---------------------------------------------------------------------------
    Name:  optionE
    Purpose:  Option E: Displays the numeric ID of the file's group
    Receive:  filename
    Return:   none
  ---------------------------------------------------------------------------*/
void optionE(char *filename)
{
    struct stat sb;
    if (stat(filename, &sb) == 0)
    {
        cout << "Group ID: " << sb.st_gid << endl;
    }
}

  /*---------------------------------------------------------------------------
    Name:  optionF
    Purpose:  Option F: Displays the time of last read from the file
    Receive:  filename
    Return:   none
  ---------------------------------------------------------------------------*/
void optionF(char *filename)
{
    struct stat sb;
    if (stat(filename, &sb) == 0)
    {
        cout << "Last time file was read: " << ctime(&sb.st_atime) << endl;
    }
}

  /*---------------------------------------------------------------------------
    Name:  optionG
    Purpose:  Option G: Displays the last time of last write to the file
    Receive:  filename
    Return:   none
  ---------------------------------------------------------------------------*/
void optionG(char *filename)
{
    struct stat sb;
    if (stat(filename, &sb) == 0)
    {
        cout << "Last modification time: " << ctime(&sb.st_mtime) << endl;
    }
}

  /*---------------------------------------------------------------------------
    Name:  optionH
    Purpose:  Option H: Displays the time time of last change to the file's status
    Receive:  filename
    Return:   none
  ---------------------------------------------------------------------------*/
void optionH(char *filename)
{
    struct stat sb;
    if (stat(filename, &sb) == 0)
    {
        cout << "Last status change time: " << ctime(&sb.st_ctime) << endl;
    }
}

  /*---------------------------------------------------------------------------
    Name:  commandL
    Purpose:  Calls functions A to H
    Receive:  filename
    Return:   none
  ---------------------------------------------------------------------------*/
void commandL(char *filename)
{
    optionA(filename);
    optionB(filename);
    optionC(filename);
    optionD(filename);
    optionE(filename);
    optionF(filename);
    optionG(filename);
    optionH(filename);
}

  /*---------------------------------------------------------------------------
    Name:  commandS
    Purpose:  Calls functions A to C
    Receive:  filename
    Return:   none
  ---------------------------------------------------------------------------*/
void commandS(char *filename)
{
    optionA(filename);
    optionB(filename);
    optionC(filename);
}
  /*---------------------------------------------------------------------------
    Name:  main
    Purpose:  Allow the user to search for and display information about selected
              files. For each file name which is supplied by the user, the 
              program will examine each directory in a designated set of 
              directories for a matching file name and display information about 
              that file.
    Receive:  File name, with commands "-L" and "-S"
    Return:   Information about entered file
  ---------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
    bool command_L = false;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] == 'S')
        {
            command_L = false;
            cout << "Short display." << endl;
        }
        else if (argv[i][0] == '-' && argv[i][1] == 'L')
        {
            command_L = true;
            cout << "Long display." << endl;
        }
        else if(argv[i][0] == '-')
        {
            if (argv[i][1] != 'S' && argv[i][1] != 'L')
            {
               cout << "Error: Invalid command." << endl;
               return 0;
            }
        }
        else
        {
          if (command_L == true)
          {
            commandL(argv[i]);
            command_L = false;
          }
          else if (command_L == false)
          {
            commandS(argv[i]);
          }
        }
    }
    return 0;
}
