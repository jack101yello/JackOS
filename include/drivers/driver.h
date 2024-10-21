#ifndef __jackos_DRIVERS_driver_H
#define __jackos_DRIVERS_driver_H

namespace jackos {
    namespace drivers {
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
    }
}

#endif