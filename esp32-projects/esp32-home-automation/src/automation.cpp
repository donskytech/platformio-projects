#include "automation.h"

AutomationProto::AutomationProto() : m_tube{false}, m_socket{false}, m_bulb{false},
                                     m_dimValue{0}, m_dimmer(DIMMER_OUTPUT_PIN, DIMMER_ZERO_CROSS_PIN)
{
};

// Pin configuration for tube/bulb/socket
int const AutomationProto::TUBE_PIN = 16;
int const AutomationProto::BULB_PIN = 17;
int const AutomationProto::SOCKET_PIN = 18;

// Dimmer Pin implementation
int const AutomationProto::DIMMER_OUTPUT_PIN = 32;
int const AutomationProto::DIMMER_ZERO_CROSS_PIN = 36;

void AutomationProto::init()
{
    pinMode(TUBE_PIN, OUTPUT);
    pinMode(BULB_PIN, OUTPUT);
    pinMode(SOCKET_PIN, OUTPUT);

    // Relays are active low so we set it to HIGH 
    digitalWrite(TUBE_PIN, HIGH);
    digitalWrite(BULB_PIN, HIGH);
    digitalWrite(SOCKET_PIN, HIGH);

    // Begin dimmer setup
    m_dimmer.begin(NORMAL_MODE, ON); //dimmer initialisation: name.begin(MODE, STATE)
}

// Getters and Setters
void AutomationProto::setTubeStatus(bool status)
{
    m_tube = status;
    digitalWrite(TUBE_PIN, !status);
}

bool AutomationProto::isTubeOn()
{
    return m_tube;
}
void AutomationProto::setSocketStatus(bool status)
{
    m_socket = status;
    digitalWrite(SOCKET_PIN, !status);
}
bool AutomationProto::isSocketOn()
{
    return m_socket;
}
void AutomationProto::setBulbStatus(bool status)
{
    m_bulb = status;
    digitalWrite(BULB_PIN, !status);
}
bool AutomationProto::isBulbOn()
{
    return m_bulb;
}

void AutomationProto::setDimmerValue(int value)
{
    m_dimValue = value;
    m_dimmer.setPower(value);
}
int AutomationProto::getDimmerValue()
{
    return m_dimValue;
}