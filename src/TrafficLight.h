#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


template <class T>
class MessageQueue
{
public:
    void send(T &&val);     // rvalue reference
    T receive();
private:
    std::deque<T> _queue;
    std::condition_variable _condition;
    std::mutex _mtx;
};


enum TrafficLightPhase {
    red,
    green,
};

class TrafficLight : TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    void waitForGreen();
    void simulate();

    // typical behaviour methods
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    std::condition_variable _condition;
    std::mutex _mtx;

    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _queue;
};

#endif