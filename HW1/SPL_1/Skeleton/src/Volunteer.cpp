#include "Volunteer.h"
#include "WareHouse.h"
#include <iostream>
Volunteer::Volunteer(int id, const string &name) : completedOrderId(NO_ORDER), activeOrderId(NO_ORDER), id(id), name(name) {}

int Volunteer::getId() const
{
    return id;
}

const string &Volunteer::getName() const
{
    return name;
}


int Volunteer::getActiveOrderId() const
{
    return activeOrderId;
}

int Volunteer::getCompletedOrderId() const
{
    return completedOrderId;
}

bool Volunteer::isBusy() const
{
    return getActiveOrderId() != NO_ORDER;
}

CollectorVolunteer::CollectorVolunteer(int id, const string &name, int coolDown) : Volunteer(id, name), coolDown(coolDown), timeLeft(-1) {}

CollectorVolunteer *CollectorVolunteer::clone() const
{
    return new CollectorVolunteer(*this);
}

void CollectorVolunteer::step()
{
    timeLeft--;
    if (timeLeft == 0)
    {
        completedOrderId = getActiveOrderId();
        activeOrderId = NO_ORDER;
    }
         
}

int CollectorVolunteer::getCoolDown() const
{
    return coolDown;
}

int CollectorVolunteer::getTimeLeft() const
{
    return timeLeft;
}

bool CollectorVolunteer::decreaseCoolDown()
{
    timeLeft--;
    if (timeLeft == 0)
    {
        return true;
    }
    return false;
}

bool CollectorVolunteer::hasOrdersLeft() const
{

    return true;
}

bool CollectorVolunteer::canTakeOrder(const Order &order) const
{
    return !isBusy() && order.getStatus() == OrderStatus::PENDING;
}

void CollectorVolunteer::acceptOrder(const Order &order)
{
    activeOrderId = order.getId();
    timeLeft = coolDown;
}

string CollectorVolunteer::toString() const
{
    string val;
    val += "VolunteerID: " + to_string(getId());
    val += "\n";
    
    if (isBusy())
    {
        val += "isBusy: true" ;
        val += "\n";
        val += "OrderId: " + to_string(getActiveOrderId());
        val += "\n";
        val += "TimeLeft: " + to_string(getTimeLeft());
    }
    else
    {
        val += "isBusy: false" ;
        val += "\n";
        val += "OrderId: None";
        val += "\n";
        val += "TimeLeft: None";
    }
    val+="\n";
    val += "OrdersLeft: No Limit";

    return val;
}

LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, const string &name, int coolDown, int maxOrders) : CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const
{
    return new LimitedCollectorVolunteer(*this);
}

bool LimitedCollectorVolunteer::hasOrdersLeft() const
{
    
    return ordersLeft != 0;
}

bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const
{
    return hasOrdersLeft() && CollectorVolunteer::canTakeOrder(order);
}

void LimitedCollectorVolunteer::acceptOrder(const Order &order)
{
    CollectorVolunteer::acceptOrder(order);
    ordersLeft--;
}

int LimitedCollectorVolunteer::getMaxOrders() const
{
    return maxOrders;
}

int LimitedCollectorVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}

string LimitedCollectorVolunteer::toString() const
{
    string val;
    val += "VolunteerID: " + to_string(getId());
    val += "\n";
    
    if (isBusy())
    {
        val += "isBusy: true" ;
        val += "\n";
        val += "OrderId: " + to_string(getActiveOrderId());
        val += "\n";
        val += "TimeLeft: " + to_string(getTimeLeft());
    }
    else
    {
        val += "isBusy: false" ;
        val += "\n";
        val += "OrderId: None";
        val += "\n";
        val += "TimeLeft: None";
    }
    val+="\n";

    val += "OrdersLeft: " + to_string(getNumOrdersLeft());

    return val;
}

DriverVolunteer::DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep) : Volunteer(id, name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(-1)
{
}

DriverVolunteer *DriverVolunteer::clone() const
{
    return new DriverVolunteer(*this);
}

int DriverVolunteer::getDistanceLeft() const
{
    return distanceLeft;
}

int DriverVolunteer::getMaxDistance() const
{
    return maxDistance;
}

int DriverVolunteer::getDistancePerStep() const
{
    return distancePerStep;
}

bool DriverVolunteer::decreaseDistanceLeft()
{
    distanceLeft = distanceLeft - distancePerStep;
    if (distanceLeft <= 0)
    {
        distanceLeft = 0;
        return true;
    }
    return false;
}

bool DriverVolunteer::hasOrdersLeft() const
{
    return true;
}

bool DriverVolunteer::canTakeOrder(const Order &order) const
{
    return !isBusy() && order.getDistance() <= getMaxDistance() && order.getStatus() == OrderStatus::COLLECTING;
}

void DriverVolunteer::acceptOrder(const Order &order)
{
    activeOrderId = order.getId();
    distanceLeft = order.getDistance();
}

void DriverVolunteer::step()
{
    decreaseDistanceLeft();
    if (distanceLeft == 0)
    {
        completedOrderId = getActiveOrderId();
        activeOrderId = NO_ORDER;
    }
    
        
    
}

string DriverVolunteer::toString() const
{
    string val;
    val += "  VolunteerID: " + to_string(getId());
    val += "\n";
    
    if (isBusy())
    {
        val += "isBusy: true" ;
        val += "\n";
        val += "OrderId: " + to_string(getActiveOrderId());
        val += "\n";
        val += "DistanceLeft: " + to_string(getDistanceLeft());
    }
    else
    {
        val += "isBusy: false" ;
        val += "\n";
        val += "OrderId: None";
        val += "\n";
        val += "DistanceLeft: None";
    }
    val+="\n";
    val += "OrdersLeft: No Limit";

    return val;
}

LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep, int maxOrders) : DriverVolunteer(id, name, maxDistance, distancePerStep), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const
{
    return new LimitedDriverVolunteer(*this);
}

int LimitedDriverVolunteer::getMaxOrders() const
{
    return maxOrders;
}

int LimitedDriverVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}

bool LimitedDriverVolunteer::hasOrdersLeft() const
{
    
    return ordersLeft > 0;
}

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const
{
    return DriverVolunteer::canTakeOrder(order) && hasOrdersLeft();
}

void LimitedDriverVolunteer::acceptOrder(const Order &order)
{
    DriverVolunteer::acceptOrder(order);
    ordersLeft--;
}

string LimitedDriverVolunteer::toString() const
{

    string val;
    val += "VolunteerID: " + to_string(getId());
    val += "\n";
    
    if (isBusy())
    {   
        val += "isBusy: true" ;
        val += "\n";
        val += "OrderId: " + to_string(getActiveOrderId());
        val += "\n";
        val += "DistanceLeft: " + to_string(getDistanceLeft());
        
    }
    else
    {
        val += "isBusy: false" ;
        val += "\n";
        val += "OrderId: None";
        val += "\n";
        val += "DistanceLeft: None";
        
    }
    val+= "\n";
    val += "OrdersLeft: " + to_string(getNumOrdersLeft());

    return val;
}
