#include <iostream>
#include <random>
#include "TrafficLight.h"

#include <thread>
#include <chrono>
#include <future>

/* Implementation of class "MessageQueue" */
 
template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lck(_mtx);
    _condition.wait(lck, [this]{ return !_queue.empty(); });

    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _queue.emplace_back(msg);
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto curPhase = _queue.receive();
        if (curPhase == green) {
            return;
        }
    }
}

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
