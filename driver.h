#ifndef __driver_H
#define __driver_H

class Driver {
    public:
        Driver();
        ~Driver();

        virtual void Activate();
        virtual int Reset();
        virtual void Deactivate();
};

class DriverManager {
    private:
        Driver* drivers[256];
        int numDrivers;

    public:
        DriverManager();
        void AddDriver(Driver* drv);
        void ActivateAll();
};

#endif