#include <iostream>
#include <random>
#include "TrafficLight.h"

#include <thread>
#include <chrono>
#include <future>

/* Implementation of class "MessageQueue" */

/* 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}
*/

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _queue.emplace_back(msg);
    _conditionVar.notify_one();
}

/* Implementation of class "TrafficLight" */

/* 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}
*/

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // random value generation between 4 and 6 seconds
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(4000, 6000);

    int cycleTime = dist(gen); // simulating random cycle duration

    auto lastUpdate = std::chrono::system_clock::now();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        auto secs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate);
        int secondsSinceSwitched = secs.count();
        
        if(secondsSinceSwitched >= cycleTime){
            _currentPhase = _currentPhase == red ? green : red;

            auto future = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, &_queue, std::move(_currentPhase));
            future.wait();

            lastUpdate = std::chrono::system_clock::now();
            cycleTime = dist(gen);
        }
        
    }
     
}
