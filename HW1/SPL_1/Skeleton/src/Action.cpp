#include "Action.h"
#include "WareHouse.h"
#include <iostream>
#include "Volunteer.h"

extern WareHouse *backup;

BaseAction::BaseAction() : errorMsg(), status() {}

ActionStatus BaseAction::getStatus() const
{
    return ActionStatus();
}

string BaseAction::statusString() const
{
    if (status == ActionStatus::COMPLETED)
        return "COMPLETED";
    else
        return "ERROR";
}

void BaseAction::complete()
{
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(string errorMsg)
{
    status = ActionStatus::ERROR;
    std::cout << errorMsg << std::endl;
}

string BaseAction::getErrorMsg() const
{
    return errorMsg;
}

SimulateStep::SimulateStep(int numOfSteps) : numOfSteps(numOfSteps)
{
}

void SimulateStep::act(WareHouse &wareHouse)
{
    vector<Order *> &penOrders = wareHouse.getPendingOrders();
    vector<Volunteer *> &volunteersV = wareHouse.getVolunteers();
    vector<Order *> &inProOrd = wareHouse.getInProcessOrders();
    
    for (int n = 0; n < numOfSteps; n++)
    {

        for (auto it = penOrders.begin(); it != penOrders.end();)
        {
            
            bool added = false;
            if (penOrders.size() == 0)
            {
                break;
            }
            if ((*it)->getStatus() == OrderStatus::PENDING)
            {
                for (unsigned int j = 0; j < volunteersV.size(); j++)
                {
                    if (volunteersV[j]->canTakeOrder(*(*it)))
                    {
                        volunteersV[j]->acceptOrder(*(*it));

                        (*it)->setCollectorId(volunteersV[j]->getId());
                        (*it)->setStatus(OrderStatus::COLLECTING);

                        wareHouse.addInProcessOrder((*it));
                        it = penOrders.erase(it);
                        added = true;
                        break;
                    }
                }
            }
            else if ((*it)->getStatus() == OrderStatus::COLLECTING)
            
            {
                for (const auto &vol : volunteersV)
                {
                    if (vol->canTakeOrder(*(*it)))
                    {
                        vol->acceptOrder(*(*it));

                        (*it)->setDriverId(vol->getId());

                        (*it)->setStatus(OrderStatus::DELIVERING);
                        wareHouse.addInProcessOrder((*it));

                        it = penOrders.erase(it);
                        added = true;
                        break;
                    }
                }
            }
            if (!added)
                it++;
        }

        
        for (Volunteer *vol : volunteersV)
        {
            vol->step();
        }

        for (auto it = inProOrd.begin(); it != inProOrd.end();)
        {

            bool added = false;
            if (inProOrd.size() == 0)
            {
                break;
            }
            if ((*it)->getStatus() == OrderStatus::COLLECTING && wareHouse.getVolunteer((*it)->getCollectorId()).getCompletedOrderId() == (*it)->getId() )
            {

                wareHouse.addPendingOrder((*it));
                it = inProOrd.erase(it);
                added = true;
                break;
            }
            else if((*it)->getStatus() == OrderStatus::DELIVERING && wareHouse.getVolunteer((*it)->getDriverId()).getCompletedOrderId() == (*it)->getId())
            {
                (*it)->setStatus(OrderStatus::COMPLETED);
                wareHouse.addcompletedOrder((*it));
                it = inProOrd.erase(it);
                added = true;
                break;
            }
            if (!added)
                it++;
        }

        for (unsigned int k = 0; k < volunteersV.size(); k++)
        {
            if (!(volunteersV[k]->hasOrdersLeft()) && !(volunteersV[k]->isBusy()))
            {
                wareHouse.eraseFromVol(k);
            }
        }
    }
    complete();
}

std::string SimulateStep::toString() const
{
    string str;
    str = "step " + numOfSteps;
    str += " " + statusString();
    return str;
}

SimulateStep *SimulateStep::clone() const
{
    return new SimulateStep(*this);
}
AddOrder::AddOrder(int id) : BaseAction(), customerId(id) {}

void AddOrder::act(WareHouse &wareHouse)
{
    if (wareHouse.customerExists(customerId))
    {
        if (wareHouse.getCustomer(customerId).canMakeOrder())
        {
            wareHouse.getCustomer(customerId).addOrder(wareHouse.getOC());
            wareHouse.addOrder(new Order(wareHouse.getOC(), customerId, wareHouse.getCustomer(customerId).getCustomerDistance()));
            wareHouse.incOC();
            complete();
        }
        else
        {
            error("Cannot place this order");
        }
    }
    else
    {
        error("customer doesn't exist");
    }
}

AddOrder *AddOrder::clone() const
{
    return new AddOrder(*this);
}

string AddOrder::toString() const
{
    string str;
    str = "order " + customerId;
    str += " " + statusString();
    return str;
}

CustomerType AddCustomer::convertStrToEnum(const string &customerType)
{
    if (customerType == "Soldier")
    {
        return CustomerType::Soldier;
    }
    else
    {
        return CustomerType::Civilian;
    }
}

AddCustomer::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders) : customerName(customerName),
                                                                                                                customerType(convertStrToEnum(customerType)), distance(distance), maxOrders(maxOrders)
{
}

void AddCustomer::act(WareHouse &wareHouse)
{
    if (customerType == CustomerType::Soldier)
    {
        wareHouse.addToCustomer(new SoldierCustomer(wareHouse.getCC(), customerName, distance, maxOrders));
        wareHouse.incCC();

    }
    else
    {
        wareHouse.addToCustomer(new CivilianCustomer(wareHouse.getCC(), customerName, distance, maxOrders));
        wareHouse.incCC();

    }
}

AddCustomer *AddCustomer::clone() const
{
    return new AddCustomer(*this);
}

string AddCustomer::toString() const
{
    string cT = "civilian";
    if (customerType == CustomerType::Soldier)
    {
        cT = "soldier";
    }

    string str;
    str = "customer " + customerName + " " + cT + " " + to_string(distance) + " " + to_string(maxOrders) + " " + statusString();
    return str;
}

PrintOrderStatus::PrintOrderStatus(int id) : BaseAction(), orderId(id) {}

void PrintOrderStatus::act(WareHouse &wareHouse)
{
    if (wareHouse.orderExists(orderId))
    {
        
        Order currOrder = wareHouse.getOrder(orderId);
        cout << "OrderID: " << to_string(currOrder.getId()) << endl;
        cout << "OrderStatus: " << currOrder.StatusToString() << endl;
        cout << "CustomerID: " << currOrder.getCustomerId() << endl;

        if (currOrder.getCollectorId() == NO_VOLUNTEER)
        {
            cout << "Collector: None" << endl;
        }
        else
        {
            cout << "collector: " << currOrder.getCollectorId() << endl;
        }

        if (currOrder.getDriverId() == NO_VOLUNTEER)
        {
            cout << "Driver: None" << endl;
        }
        else
        {
            cout << "driver: " << currOrder.getDriverId() << endl;
        }

        complete();
    }
    else
    {
        error("Order doesn't exist");
    }
}

PrintOrderStatus *PrintOrderStatus::clone() const
{
    return new PrintOrderStatus(*this);
}

string PrintOrderStatus::toString() const
{
    string str;
    str = "orderStatus " + orderId;
    str += " " + statusString();
    return str;
}

PrintCustomerStatus::PrintCustomerStatus(int customerId) : BaseAction(), customerId(customerId) {}

void PrintCustomerStatus::act(WareHouse &wareHouse)
{
    if (wareHouse.customerExists(customerId))
    {
        cout << wareHouse.getCustomer(customerId).toString() ;
        for (const auto &order : wareHouse.getCustomer(customerId).getOrdersIds())
        {
            if (wareHouse.orderExists(order))
            {
                cout << "OrderID: " << order << endl;
                cout << "OrderStatus: " + wareHouse.getOrder(order).StatusToString() << endl;
            }
        }
        cout << "numOrdersLeft: " << (wareHouse.getCustomer(customerId).getMaxOrders() - wareHouse.getCustomer(customerId).getNumOrders()) << endl;
        complete();
    }
    else
    {
        error("customer doesn't exist");
    }
}

PrintCustomerStatus *PrintCustomerStatus::clone() const
{
    return new PrintCustomerStatus(*this);
}

string PrintCustomerStatus::toString() const
{
    string str;
    str = "customerStatus " + to_string(customerId);
    str += " " + statusString();
    return str;
}

PrintVolunteerStatus::PrintVolunteerStatus(int id) : BaseAction(), volunteerId(id) {}

void PrintVolunteerStatus::act(WareHouse &wareHouse)
{
    if (wareHouse.volunteerExists(volunteerId))
    {
        cout << wareHouse.getVolunteer(volunteerId).toString() << endl;
        complete();
    }
    else
    {
        error("Volunteer doesn't exist");
    }
}

PrintVolunteerStatus *PrintVolunteerStatus::clone() const
{
    return new PrintVolunteerStatus(*this);
}

string PrintVolunteerStatus::toString() const
{
    string str;
    str = "volunteerStatus " + volunteerId;
    str += " " + statusString();
    return str;
}

PrintActionsLog::PrintActionsLog() : BaseAction() {}

void PrintActionsLog::act(WareHouse &wareHouse)
{
    for (BaseAction *action : wareHouse.getActions())
    {
        cout << action->toString() << endl;
    }
    complete();
}

PrintActionsLog *PrintActionsLog::clone() const
{
    return new PrintActionsLog(*this);
}

string PrintActionsLog::toString() const
{
    string str;
    str = "log " + statusString();
    return str;
}

Close::Close() : BaseAction() {}

void Close::act(WareHouse &wareHouse)
{
    for (Order *c : wareHouse.getPendingOrders())
    {
        cout << "OrderID:" + to_string(c->getId()) << ", CustomerID: " + to_string(c->getCustomerId()) << ", OrderStatus: " + c->StatusToString() << endl;
    }
    for (Order *c : wareHouse.getInProcessOrders())
    {
        cout << "OrderID:" + to_string(c->getId()) << ", CustomerID: " + to_string(c->getCustomerId()) << ", OrderStatus: " + c->StatusToString() << endl;
    }
    for (Order *c : wareHouse.getCompletedOrders())
    {
        cout << "OrderID:" + to_string(c->getId()) << ", CustomerID: " + to_string(c->getCustomerId()) << ", OrderStatus: " + c->StatusToString() << endl;
    }
    wareHouse.close();
    complete();
}

Close *Close::clone() const
{
    return new Close(*this);
}

string Close::toString() const
{
    string str;
    str = "close " + statusString();
    return str;
}

BackupWareHouse::BackupWareHouse() : BaseAction() {}

void BackupWareHouse::act(WareHouse &wareHouse)
{
    if (backup != nullptr)
    {
        delete backup;
    }
    backup = new WareHouse(wareHouse);
    complete();
}

BackupWareHouse *BackupWareHouse::clone() const
{
    return new BackupWareHouse(*this);
}

string BackupWareHouse::toString() const
{
    string str;
    str = "backup " + statusString();
    return str;
}

RestoreWareHouse::RestoreWareHouse() : BaseAction() {}

void RestoreWareHouse::act(WareHouse &wareHouse)
{
    if (backup == nullptr)
    {
        error("No backup available\n");
    }
    else
    {
        wareHouse = *backup;
        complete();
    }
}

RestoreWareHouse *RestoreWareHouse::clone() const
{
    return new RestoreWareHouse(*this);
}

string RestoreWareHouse::toString() const
{
    string str;
    str = "restore " + statusString();
    return str;
}
