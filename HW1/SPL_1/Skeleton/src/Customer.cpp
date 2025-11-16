#include "Customer.h"
#include "WareHouse.h"

#include <iostream>
using std::vector;
using namespace std;

Customer::Customer(int id, const string &name, int locationDistance, int maxOrders) : id(id), name(name), locationDistance(locationDistance), maxOrders(maxOrders), ordersId() {}

const string &Customer::getName() const
{
    return name;
}

int Customer::getId() const
{
    return id;
}

int Customer::getCustomerDistance() const
{
    return locationDistance;
}

int Customer::getMaxOrders() const
{
    return maxOrders;
}

int Customer::getNumOrders() const
{
    return ordersId.size();
}

bool Customer::canMakeOrder() const
{
    return getNumOrders() < getMaxOrders();
}

const vector<int> &Customer::getOrdersIds() const
{
    return ordersId;
}

int Customer::addOrder(int orderId)
{
    
    for (unsigned int i = 0; i < ordersId.size(); i++)
    {
        if (ordersId[i] == orderId)
        {
            return -1;
        }
    }
    ordersId.push_back(orderId);
    return orderId;
}

SoldierCustomer::SoldierCustomer(int id, const string &name, int locationDistance, int maxOrders) : Customer(id, name, locationDistance, maxOrders) {}

SoldierCustomer *SoldierCustomer::clone() const
{
    return new SoldierCustomer(*this);
}
string SoldierCustomer::toString()
{
    string str = "CustomerID: " + to_string(getId());
    str += "\n";
    return str;
}

CivilianCustomer::CivilianCustomer(int id, const string &name, int locationDistance, int maxOrders) : Customer(id, name, locationDistance, maxOrders) {}

CivilianCustomer *CivilianCustomer::clone() const
{
    return new CivilianCustomer(*this);
}

string CivilianCustomer::toString()
{
    string str = "CustomerID: " + to_string(getId());
    str += "\n";
    return str;
}