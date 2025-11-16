#pragma once
#include <string>
#include <vector>
using namespace std;
#include "Volunteer.h"
#include "Order.h"
#include "Customer.h"
#include "Action.h"
// Warehouse responsible for Volunteers, Customers and Actions.
class BaseAction;

class WareHouse {

    public:
        WareHouse(const string &configFilePath);
        ~WareHouse();
        WareHouse& operator=( WareHouse &other);
        WareHouse( WareHouse &other);
         WareHouse& operator=( WareHouse &&other);
        WareHouse( WareHouse &&other);
        void start();
        const vector<BaseAction*> &getActions() const;
        void addOrder(Order* order);
        void addAction(BaseAction* action);
        void eraseFromPen(int i);
        void eraseOrderFIN(Order* order);
        void eraseFromPenTop();
        void eraseOrderFINTop();
        void erasePenOrder(Order* order);
        void eraseFromVol(int i);
        int getCC() const;
        int getVC() const;
        int getOC() const;
        void cleanWareHouse();
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        Order &getOrder(int orderId) const;
        vector<Order*> &getPendingOrders() ;
        vector<Order*> &getInProcessOrders();
        vector<Order*> &getCompletedOrders();
        vector<Customer*> &getCustomers();
        vector<Volunteer*> &getVolunteers();
        void addInProcessOrder(Order *order);
        void addPendingOrder(Order *order);
        void addcompletedOrder(Order *order);
        void addToCustomer(Customer *customer);
        bool customerExists(int customerId);
        bool volunteerExists(int volunteerId);
        bool orderExists(int orderId);
        void incOC();
        void incCC();

        
        void close();
        void open();

    private:
        bool isOpen;
        vector<BaseAction*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        int orderCounter; //For assigning unique Order IDs
};