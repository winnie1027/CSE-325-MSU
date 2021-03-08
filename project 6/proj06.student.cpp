/*********************************************************************
    Winnie Yang
    Computer Project #6
**********************************************************************/

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>
#include <pthread.h> 
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <semaphore.h>

using namespace std;

// Struct for orders
struct Order
{
    string customerID;
    int productID = 0;
    int amount = 0;
    int queue;
};

// Struct for inventory
struct Inventory {
    int productID = 0;
    double price = 0.00;
    int quantity = 0;
    string type = "";
};

vector<Inventory> InventoryVect;    // initializing vector for inventory

int bufferSize = 10;    // Default size of buffer
int producerNum = 1;    // Default producer number

sem_t empty;    // number of empty slots, to load producer and consumer
sem_t items;    // number of items, to load producer and consumer
sem_t mutual;   // mutual exclusion, to access buffer

int in = 0;    // put item in circular buffer
int out = 0;    // get item in circular buffer

array<Order, 10> circularBuffer;    // initializing circular buffer with R records

  /*---------------------------------------------------------------------------
    Name:  producerThread
    Purpose:  Reads the inventory and adds it to the Order Struct and circular
              buffer.
    Receive:  Pointer to the order file
    Return:   None
  ---------------------------------------------------------------------------*/
void *producerThread(void *ptr)
{
    // Use passed in ptr as an orders# file name
    char *self = (char*)(ptr);

    ifstream file;
    file.open(self);
    
    if (!file.is_open())
    {
        pthread_exit( NULL);
    }

    string line;      // line in file

    // Loops through file
    while (std::getline(file, line))
    {
        string word;
        
        // extracting data from orders file
        string customerID = line.substr(0,7);   // customer ID
        string productID = line.substr(8, 6);    // product ID
        string quantity = line.substr(15, 5);    // quantity ordered

        // putting data inside Orders struct
        Order order;
        order.customerID = customerID;
        order.productID = std::stoi(productID);
        order.amount = std::stoi(quantity);
                
        sem_wait(&empty); 
        sem_wait(&mutual);
        
        circularBuffer[in] = order;        // adding current order to circular buffer
        in = (in + 1) % bufferSize;       // incrementing to next placement

        sem_post(&mutual);
        sem_post(&items);
    }

    pthread_exit(NULL);
}

  /*---------------------------------------------------------------------------
    Name:  consumerThread
    Purpose:  Create the output file named "log" then process the sequence of 
              orders in the data structure. Validate each purchase request, if 
              there's enough seedlings on hand to fill the order, the current
              inventory will be updated. If not, it'll be rejected. Track the 
              results of processing each order by sending lines to the "log" 
              output file.
    Receive:  Pointer
    Return:   None
  ---------------------------------------------------------------------------*/
void *consumerThread(void *ptr)
{
    // Create/open new file named orders.log
    ofstream newFile("log");

    while (!circularBuffer.empty())
    {
        sem_wait(&items);
        sem_wait(&mutual);

        Order order = circularBuffer[out];
        out = (out + 1) % bufferSize;

        sem_post(&mutual);
        sem_post(&empty);
        
        for (int i = 0; i < static_cast<int>(InventoryVect.size()); i++)
        {
            // matching product IDs
            if (InventoryVect[i].productID == order.productID)
            {   
                // if inventory has enough for the order
                if (InventoryVect[i].quantity >= order.amount)
                {   
                    // updating inventory quantity after order
                    InventoryVect[i].quantity -= order.amount;
                
                    // transaction amount
                    double total = order.amount * InventoryVect[i].price;

                    newFile << order.customerID << " ";
                    newFile << order.productID << " ";
                    newFile << left << setw(30) << InventoryVect[i].type << " ";
                    newFile << right << setw(8) << InventoryVect[i].quantity << " ";
                    newFile << '$';
                    newFile << left << setw(10) << std::fixed << 
                            std::setprecision(2) << total << " ";
                    newFile << "Filled" << endl;
                }
                // if the order exedes amount in inventory
                else
                {
                    newFile << order.customerID << " ";
                    newFile << order.productID << " ";
                    newFile << left << setw(30) << InventoryVect[i].type << " ";
                    newFile << right << InventoryVect[i].quantity << " ";
                    // transaction amount would be $0 since it'll be rejected
                    newFile << '$';
                    newFile << left << setw(5) << std::fixed << 
                            std::setprecision(2) << 0 << " ";
                    newFile << "Rejected" << endl;
                }
            }
        }
    }
    newFile.close();
    pthread_exit(NULL);
}

  /*---------------------------------------------------------------------------
    Name:  main
    Purpose:  Opens old inventory file, create threads, and create new inventory
              file.
    Receive:  Number of arguments, array of arguments
    Return:   None
  ---------------------------------------------------------------------------*/
int main(int argc, char* argv[]) 
{
    ifstream file;
    file.open("inventory.old");

    string line;    // every line in file
    
    // going through each line in inventory file
    while (std::getline(file, line))
    {
        string word;
        
        // extracting data from each line
        string productID = line.substr(0, 6);
        string price = line.substr(7, 5);
        string amount = line.substr(12, 6);
        string type = line.substr(18, 30);

        // putting data inside Inventory struct
        Inventory product;
        product.productID = std::stoi(productID);
        product.price = std::stod(price);
        product.quantity = std::stoi(amount);
        product.type = type;
        
        // adding product struct to the inventory vector
        InventoryVect.push_back(product);
    }

    // Loop through all arguments from command line
    for (int i = 1; i < argc; i++)
    {   
        // if option is -p
        if (argv[i][0] == '-' && argv[i][1] == 'p')
        {
            int pnumber = atoi(argv[i+1]);   // converts command to int

            // if producer number is acceptable
            if (pnumber >= 1 && pnumber <= 9)
            {
                producerNum = pnumber;
                i++;
            }
            else
            {
                producerNum = 1;    // back to default number
                cout << "Error: Enter a producer thread between 1 to 9." << endl;
            }
        }
        // if option is -b
        else if (argv[i][0] == '-' && argv[i][1] == 'b')
        {
            int records = atoi(argv[i+1]);    // converts command to int

            // if buffer number is acceptable
            if (records >= 10 && records <= 30)
            {
                bufferSize = records;
                i++;
            }
            else
            {
                bufferSize = 10;    // back to default buffer size
                cout << "Error: Enter a buffer size between 10 to 30." << endl;
            }
        }
        else
        {
            cout << "Error: Invalid command." << endl;
            return 0;
        }
    }
    
    // initializing circular buffer size
    circularBuffer[bufferSize];

    // initializing semaphores
    sem_init(&empty, 0, bufferSize);
    sem_init(&items, 0, 0);
    sem_init(&mutual, 0, 1);

    // initializing producer and consumer threads
    pthread_t consumerThreadInitial;
    pthread_t producerThreadInitial[producerNum];
    
    // Create the consumer thread
	if (pthread_create(&consumerThreadInitial, NULL, consumerThread, NULL))
    {
    	cout << "Error: Creating consumer thread." << endl;
      	exit(-1);
    }

    // Creates the producer thread for each orders# file
    for (int i = 1; i < producerNum; i++)     // to loop through every order file N
    {
        string order = "orders" + std::to_string(i);
        if (pthread_create(&producerThreadInitial[i], NULL, producerThread, (void *) order.c_str()))
        {
            cout << "Error: Creating producer thread." << endl;
            exit(-1);
        }
        if (pthread_join( producerThreadInitial[i], NULL))
        {
            cout << "Error: Joining producer thread failed." << endl;
            exit(-2);
        }
    }
    
    // Create new file for the updated inventory
	ofstream newFile("inventory.new");
    
    for (auto &x : InventoryVect)
    {
        newFile << x.productID << " ";
        newFile << right << setw(5) << std::fixed << std::setprecision(2) << x.price;
        newFile << right << setw(7) << x.quantity;
        newFile << left << setw(30) << x.type << endl;
    }
    newFile.close();
}

