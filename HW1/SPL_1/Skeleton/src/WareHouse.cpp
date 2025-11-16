#include "WareHouse.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;
// Constructor
WareHouse::WareHouse(const string &configFilePath) : isOpen(false), actionsLog(), volunteers(), pendingOrders(),
                                                     inProcessOrders(), completedOrders(), customers(), customerCounter(0), volunteerCounter(0), orderCounter(0)
{
    ifstream file(configFilePath);
    if (!file.is_open())
    {
        cerr << "error opening config file" << endl;
        exit(-1);
    }
    string line;
    while (getline(file, line))
    {
        istringstream sstream(line);
        string data;
        sstream >> data;
        if (data == "customer")
        {
            string name, type;
            int distance, maxOrders;
            sstream >> name >> type >> distance >> maxOrders;
            if (type == "soldier")
            {
                customers.push_back(new SoldierCustomer(customerCounter, name, distance, maxOrders));
            }
            else
            {
                customers.push_back(new CivilianCustomer(customerCounter, name, distance, maxOrders));
            }
            customerCounter++;
        }
        else if (data == "volunteer")
        {
            string name, role;
            int val1, val2, val3;
            sstream >> name >> role >> val1 >> val2 >> val3;
            if (role == "collector")
            {
                volunteers.push_back(new CollectorVolunteer(volunteerCounter, name, val1));
            }
            else if (role == "limited_collector")
            {
                volunteers.push_back(new LimitedCollectorVolunteer(volunteerCounter, name, val1, val2));
            }
            else if (role == "driver")
            {
                volunteers.push_back(new DriverVolunteer(volunteerCounter, name, val1, val2));
            }
            else if (role == "limited_driver")
            {
                volunteers.push_back(new LimitedDriverVolunteer(volunteerCounter, name, val1, val2, val3));
            }
            volunteerCounter++;
        }
    }
}
// Destructor
WareHouse::~WareHouse()
{
    cleanWareHouse();
}
// Assignment Operator
WareHouse &WareHouse::operator=(WareHouse &other)
{
    if (&other != this)
    {
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        cleanWareHouse();
        for (unsigned int i = 0; i < other.actionsLog.size(); i++)
        {
            actionsLog.push_back(other.actionsLog[i]->clone());
        }
        for (unsigned int i = 0; i < other.volunteers.size(); i++)
        {
            volunteers.push_back(other.volunteers[i]->clone());
        }
        for (unsigned int i = 0; i < other.customers.size(); i++)
        {
            customers.push_back(other.customers[i]->clone());
        }
        for (unsigned int i = 0; i < other.pendingOrders.size(); i++)
        {
            pendingOrders.push_back(other.pendingOrders[i]->clone());
        }
        for (unsigned int i = 0; i < other.inProcessOrders.size(); i++)
        {
            inProcessOrders.push_back(other.inProcessOrders[i]->clone());
        }
        for (unsigned int i = 0; i < other.completedOrders.size(); i++)
        {
            completedOrders.push_back(other.completedOrders[i]->clone());
        }
    }
    return *this;
}
// Copy Constructor
WareHouse::WareHouse(WareHouse &other) : isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter)
{
    for (const auto &action : other.actionsLog)
    {
        actionsLog.push_back(action->clone());
    }
    for (const auto &vol : other.volunteers)
    {
        volunteers.push_back(vol->clone());
    }
    for (const auto &customer : other.customers)
    {
        customers.push_back(customer->clone());
    }
    for (const auto &penO : other.pendingOrders)
    {
        pendingOrders.push_back(penO->clone());
    }
    for (const auto &inPO : other.inProcessOrders)
    {
        inProcessOrders.push_back(inPO->clone());
    }
    for (const auto &cO : other.completedOrders)
    {
        completedOrders.push_back(cO->clone());
    }
}
// Move Assignment Operator
WareHouse &WareHouse::operator=(WareHouse &&other)
{
    if (&other != this)
    {
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        cleanWareHouse();
        for (unsigned int i = 0; i < other.actionsLog.size(); i++)
        {
            actionsLog.push_back(other.actionsLog[i]->clone());
        }
        for (unsigned int i = 0; i < other.volunteers.size(); i++)
        {
            volunteers.push_back(other.volunteers[i]->clone());
        }
        for (unsigned int i = 0; i < other.customers.size(); i++)
        {
            customers.push_back(other.customers[i]->clone());
        }
        for (unsigned int i = 0; i < other.pendingOrders.size(); i++)
        {
            pendingOrders.push_back(other.pendingOrders[i]->clone());
        }
        for (unsigned int i = 0; i < other.inProcessOrders.size(); i++)
        {
            inProcessOrders.push_back(other.inProcessOrders[i]->clone());
        }
        for (unsigned int i = 0; i < other.completedOrders.size(); i++)
        {
            completedOrders.push_back(other.completedOrders[i]->clone());
        }
        other.isOpen = false;
        other.customerCounter = 0;
        other.volunteerCounter = 0;
        other.orderCounter = 0;
        other.cleanWareHouse();
    }
    return *this;
}
// Move Constructor
WareHouse::WareHouse(WareHouse &&other) : isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter)
{
    for (unsigned int i = 0; i < other.actionsLog.size(); i++)
    {
        actionsLog.push_back(other.actionsLog[i]->clone());
    }
    for (unsigned int i = 0; i < other.volunteers.size(); i++)
    {
        volunteers.push_back(other.volunteers[i]->clone());
    }
    for (unsigned int i = 0; i < other.customers.size(); i++)
    {
        customers.push_back(other.customers[i]->clone());
    }
    for (unsigned int i = 0; i < other.pendingOrders.size(); i++)
    {
        pendingOrders.push_back(other.pendingOrders[i]->clone());
    }
    for (unsigned int i = 0; i < other.inProcessOrders.size(); i++)
    {
        inProcessOrders.push_back(other.inProcessOrders[i]->clone());
    }
    for (unsigned int i = 0; i < other.completedOrders.size(); i++)
    {
        completedOrders.push_back(other.completedOrders[i]->clone());
    }
    other.cleanWareHouse();
    other.isOpen = false;
    other.volunteerCounter = 0;
    other.customerCounter = 0;
    other.orderCounter = 0;
}

void WareHouse::start()
{
    open();
    cout << "The WareHouse is Open!" << endl;
    while (isOpen == true)
    {
        // get a line from user
        string line;
        getline(cin, line);
        // parse -> create action object
        BaseAction *action;
        istringstream sstream(line);
        string type;
        sstream >> type;
        if (type == "order")
        {
            int val1;
            sstream >> val1;
            action = new AddOrder(val1);
        }
        else if (type == "step")
        {
            int val1;
            sstream >> val1;
            action = new SimulateStep(val1);
        }
        else if (type == "customer")
        {
            string str1, str2;
            int val1, val2;
            sstream >> str1 >> str2 >> val1 >> val2;
            action = new AddCustomer(str1, str2, val1, val2);
        }
        else if (type == "orderStatus")
        {
            int val1;
            sstream >> val1;
            action = new PrintOrderStatus(val1);
        }
        else if (type == "customerStatus")
        {
            int val1;
            sstream >> val1;
            action = new PrintCustomerStatus(val1);
        }
        else if (type == "volunteerStatus")
        {
            int val1;
            sstream >> val1;
            action = new PrintVolunteerStatus(val1);
        }
        else if (type == "log")
        {
            action = new PrintActionsLog();
        }
        else if (type == "close")
        {
            action = new Close();
        }
        else if (type == "backup")
        {
            action = new BackupWareHouse();
        }
        else if (type == "restore")
        {
            action = new RestoreWareHouse();
        }
        else
        {
            cerr << "Unknown command " << type << endl;
        }
        // act the action object
        action->act(*this);
        // add to actions log
        addAction(action);
    }
   
}

const vector<BaseAction *> &WareHouse::getActions() const
{
    return actionsLog;
}

void WareHouse::addOrder(Order *order)
{
    pendingOrders.push_back(order);
}

void WareHouse::addAction(BaseAction *action)
{
    actionsLog.push_back(action);
}
void WareHouse::erasePenOrder(Order* order){
    for (size_t i = 0; i < pendingOrders.size(); i++)
    {
        if(order == pendingOrders[i]){
            pendingOrders.erase(pendingOrders.begin()+ i);
                    }
    }
    
    }
void WareHouse::eraseFromPen(int i)
{
    
    pendingOrders.erase(pendingOrders.begin() + i);
}

void WareHouse::eraseOrderFIN(Order* order )
{   for (size_t i = 0; i < inProcessOrders.size(); i++)
{
    if(order == inProcessOrders[i]){
    inProcessOrders.erase(inProcessOrders.begin() + i);
}
}

    
}

void WareHouse::eraseFromPenTop()
{
    pendingOrders.pop_back();
}

void WareHouse::eraseOrderFINTop()
{
    inProcessOrders.pop_back();
}

void WareHouse::eraseFromVol(int i)
{
    
    delete volunteers[i];
    volunteers[i] = nullptr;
    volunteers.erase(volunteers.begin() + i);
}

int WareHouse::getCC() const
{
    return customerCounter;
}

Customer &WareHouse::getCustomer(int customerId) const
{
    for (unsigned int i = 0; i < customers.size(); i++)
    {
        if (customers[i]->getId() == customerId)
        {
            return *customers[i];
        }
    }
    static Customer *c = new CivilianCustomer(-1, "", -1, -1);
    return *c;
}

Volunteer &WareHouse::getVolunteer(int volunteerId) const
{
    for (const auto &volunteer : volunteers)
    {
        if (volunteer->getId() == volunteerId)
        {
            return *volunteer;
        }
    }
    static Volunteer *c = new CollectorVolunteer(-1, "", -1);
    return *c;
}

Order &WareHouse::getOrder(int orderId) const
{
    for (unsigned int i = 0; i < pendingOrders.size(); i++)
    {
        if (pendingOrders[i]->getId() == orderId)
        {
            return *pendingOrders[i];
        }
    }
    for (unsigned int i = 0; i < inProcessOrders.size(); i++)
    {
        if (inProcessOrders[i]->getId() == orderId)
        {
            return *inProcessOrders[i];
        }
    }
    for (unsigned int i = 0; i < completedOrders.size(); i++)
    {
        if (completedOrders[i]->getId() == orderId)
        {
            return *completedOrders[i];
        }
    }
    static Order *c = new Order(-1, -1, -1);
    return *c;
}

vector<Order *> &WareHouse::getPendingOrders() 
{
    return pendingOrders;
}

vector<Order *> &WareHouse::getInProcessOrders() 
{
    return inProcessOrders;
}

vector<Order *> &WareHouse::getCompletedOrders() 
{
    return completedOrders;
}

vector<Customer *> &WareHouse::getCustomers() 
{
    return customers;
}

void WareHouse::addInProcessOrder(Order *order)
{
    
    inProcessOrders.push_back(order);
}

int WareHouse::getVC() const
{
    return volunteerCounter;
}

vector<Volunteer *> &WareHouse::getVolunteers()
{
    return volunteers;
}

void WareHouse::addPendingOrder(Order *order)
{
    pendingOrders.push_back(order);
}

void WareHouse::addcompletedOrder(Order *order)
{
    completedOrders.push_back(order);
}

void WareHouse::addToCustomer(Customer *customer)
{
    customers.push_back(customer);
}

void WareHouse::close()
{
    isOpen = false;
    cout << "The WareHouse is Closed!" << endl;
}

void WareHouse::open()
{
    isOpen = true;
}

int WareHouse::getOC() const
{
    return orderCounter;
}
void WareHouse::incOC()
{
    orderCounter++;
}
void WareHouse::incCC()
{
    customerCounter++;
}
void WareHouse::cleanWareHouse()
{
    for (unsigned int i = 0; i < actionsLog.size(); i++)
    {
        delete actionsLog[i];
        actionsLog[i] = nullptr;
    }
    for (unsigned int i = 0; i < volunteers.size(); i++)
    {
        delete volunteers[i];
        volunteers[i] = nullptr;
    }
    for (unsigned int i = 0; i < customers.size(); i++)
    {
        delete customers[i];
        customers[i] = nullptr;
    }
    for (unsigned int i = 0; i < pendingOrders.size(); i++)
    {
        
        delete pendingOrders[i];
        pendingOrders[i] = nullptr;
    }
    for (unsigned int i = 0; i < inProcessOrders.size(); i++)
    {
        
        delete inProcessOrders[i];
        inProcessOrders[i] = nullptr;
    }
    for (unsigned int i = 0; i < completedOrders.size(); i++)
    {
        
        delete completedOrders[i];
        completedOrders[i] = nullptr;
    }
    actionsLog.clear();
    volunteers.clear();
    customers.clear();
    pendingOrders.clear();
    inProcessOrders.clear();
    completedOrders.clear();
}

bool WareHouse::customerExists(int customerId)
{
    for (Customer *c : customers)
    {
        if (c->getId() == customerId)
            return true;
    }
    return false;
}

bool WareHouse::volunteerExists(int volunteerId)
{
    for (Volunteer *c : volunteers)
    {
        if (c->getId() == volunteerId)
            return true;
    }
    return false;
}

bool WareHouse::orderExists(int orderId)
{
    for (Order *c : pendingOrders)
    {
        if (c->getId() == orderId)
            return true;
    }
    for (Order *c : inProcessOrders)
    {
        if (c->getId() == orderId)
            return true;
    }
    for (Order *c : completedOrders)
    {
        if (c->getId() == orderId)
            return true;
    }
    return false;
}
