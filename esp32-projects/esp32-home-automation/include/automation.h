#ifndef AUTOMATION_H
#define AUTOMATION_H
#include <Arduino.h>
#include <RBDdimmer.h>

class AutomationProto
{
private:
    bool m_tube;
    bool m_socket;
    bool m_bulb;
    int m_dimValue;
    dimmerLamp m_dimmer;

public:
    AutomationProto();
    void init();
    void setTubeStatus(bool status);
    bool isTubeOn();
    void setSocketStatus(bool status);
    bool isSocketOn();
    void setBulbStatus(bool status);
    bool isBulbOn();
    void setDimmerValue(int value);
    int getDimmerValue();

    // Static Constant Pin
    static int const TUBE_PIN;
    static int const BULB_PIN;
    static int const SOCKET_PIN;

    // Dimmer Constant Pin
    static int const DIMMER_ZERO_CROSS_PIN;
    static int const DIMMER_OUTPUT_PIN;
};

#endif