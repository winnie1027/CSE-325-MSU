/*********************************************************************
    Winnie Yang
    Computer Project #5
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

using namespace std;

// Struct for orders
struct Order
{
    int customerID = 0;
    int productID = 0;
    int amount = 0;
};

vector<Order> OrderVect;    // empty vector for orders

// Struct for inventory
struct Inventory {
    int productID = 0;
    double price = 0.00;
    int quantity = 0;
    string type = "";
};

vector<Inventory> InventoryVect;    // empty vector for inventory

  /*---------------------------------------------------------------------------
    Name:  producerThread
    Purpose:  Reads the inventory and adds it to the Order Struct and Vector
    Receive:  Path to the inventory file
    Return:   None
  ---------------------------------------------------------------------------*/
void *producerThread(void *ptr)
{
    ifstream file;
    file.open("orders");

    if (!file.is_open())
    {
        pthread_exit( NULL);
    }

    string line;      // line in file

    // Loops through file
    while (std::getline(file, line))
    {
        string word;

        string customerID = line.substr(0,7);   // customer ID
        string productID = line.substr(8, 6);    // product ID
        string quantity = line.substr(15, 5);    // quantity ordered

        Order order;
        order.customerID = std::stoi(customerID);
        order.productID = std::stoi(productID);
        order.amount = std::stoi(quantity);

        OrderVect.push_back(order);
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
    Receive:  
    Return:   None
  ---------------------------------------------------------------------------*/
void *consumerThread(void *ptr)
{
    // Create/open new file named orders.log
    ofstream newFile("log");

    newFile << "Customer ID " << "Product ID " << "Description " << "Quantity "
    << "Total Price " << "Result" << endl;
    for (int i = 0; i < static_cast<int>(InventoryVect.size()); i++)
    {
        for (int j = 0; j < static_cast<int>(OrderVect.size()); j++)
        {
            if (InventoryVect[i].productID == OrderVect[j].productID)
            {
                if (InventoryVect[i].quantity >= OrderVect[j].amount)
                {
                    InventoryVect[i].quantity -= OrderVect[j].amount;
                    double total = OrderVect[j].amount * InventoryVect[i].price;
                    newFile << OrderVect[j].customerID << " " <<  OrderVect[j].productID
                    << " " << InventoryVect[i].type << " " << InventoryVect[i].quantity
                   << " " << total << " " << "Filled" << endl;
                }
                else
                {

                    newFile << OrderVect[j].customerID << " " << OrderVect[j].productID 
                    << " " << InventoryVect[i].type << " " << InventoryVect[i].quantity
                    << " " << 0 << " " << "Rejected" << endl;
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
    Receive:  None
    Return:   None
  ---------------------------------------------------------------------------*/
int main() 
{
    ifstream file;
    file.open("inventory.old");

    string line;    // every line in file

    while (std::getline(file, line))
    {
        string word;

        string productID = line.substr(0, 6);
        string price = line.substr(7, 5);
        string amount = line.substr(12, 5);
        string type = line.substr(18, 30);

        Inventory product;
        product.productID = std::stoi(productID);
        product.price = std::stod(price);
        product.quantity = std::stoi(amount);
        product.type = type;

        InventoryVect.push_back(product);
    }

    pthread_t consumerThreadInitial;
    pthread_t producerThreadInitial;
    
    // Create the producer thread
    if (pthread_create( &producerThreadInitial, NULL, producerThread, NULL))
    {
    	  cout << "Error: Creating producer thread." << endl;
      	exit( -1 );
    }
    if (pthread_join( producerThreadInitial, NULL))
    {
        cout << "Error: Joining producer thread failed." << endl;
        exit(-1);
    }
    
    // Create the consumer thread
	  if (pthread_create( &consumerThreadInitial, NULL, consumerThread, NULL))
    {
    	  cout << "Error: Creating consumer thread." << endl;
      	exit( -1 );
    }
    
    if (pthread_join( consumerThreadInitial, NULL))
    {
        cout << "Error: Joining consumer thread failed." << endl;
        exit(-1);
    }
    
    // Create new file for the updated inventory
		ofstream newFile("inventory.new");
    
    for (auto &x : InventoryVect)
    {
        newFile << x.productID << " ";
        newFile << right << setw(5) << std::fixed << std::setprecision(2) << x.price;
        newFile << right << setw(5) << x.quantity;
        newFile << left << setw(30) << x.type << endl;
    }
    newFile.close();
}
