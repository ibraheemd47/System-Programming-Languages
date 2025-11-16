#include "Order.h"
#include <string>
#include <iostream>
using std::string;
using namespace std;
Order::Order(int id, int customerId, int distance) : id(id), customerId(customerId),
                                                     distance(distance), status(OrderStatus::PENDING), collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER) {
    
}

int Order::getId() const
{
    return id;
}

int Order::getCustomerId() const
{
    return customerId;
}

void Order::setStatus(OrderStatus _status)
{
    status = _status;
}

void Order::setCollectorId(int _collectorId)
{
    collectorId = _collectorId;
}

void Order::setDriverId(int _driverId)
{
    driverId = _driverId;
}

int Order::getCollectorId() const
{
    return collectorId;
}

int Order::getDriverId() const
{
    return driverId;
}
int Order::getDistance() const
{
    return distance;
}
OrderStatus Order::getStatus() const
{
    return status;
}

const string Order::toString() const
{
    string val;
    val = "OrderID: " ;
    val += getId();
    val += "\n";
    val += "OrderStatus: ";
    OrderStatus status = getStatus();
    if (status == OrderStatus::PENDING)
        val += "Pending";
    
    else if(status== OrderStatus::COLLECTING)
        val += "Collecting";
        
    else if(status== OrderStatus::DELIVERING)
        val += "Delivering";
        
    else 
        val += "Completed";
       
    
    val += "\n";
    val += "CustomerID: " ;
    val += getCustomerId();
    val += "\n";
    if (getCollectorId() == NO_VOLUNTEER)
    {
        val += "Collector: None";
    }
    else
    {
        val += "Collector: " ;
        val += getCollectorId();
    }
    val += "\n";

    if (getDriverId() == NO_VOLUNTEER)
    {
        val += "Driver: None";
    }
    else
    {
        val += "Driver: " ;
        val += getDriverId();
    }
    val += "\n";
    return val;
}

const string Order::StatusToString() const
{
    string val;
    OrderStatus status = getStatus();
    if (status == OrderStatus::PENDING)
        val += "Pending";
    
    else if(status== OrderStatus::COLLECTING)
        val += "Collecting";
        
    else if(status== OrderStatus::DELIVERING)
        val += "Delivering";
        
    else 
        val += "Completed";
       

    return val;
}

Order *Order::clone()
{
    return new Order(*this);
}
