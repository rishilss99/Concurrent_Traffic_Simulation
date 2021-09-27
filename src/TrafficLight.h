#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <chrono>
#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>
#include <thread>
#include <memory>

// forward declarations to avoid include cycle
class Vehicle;
enum TrafficLightPhase { red, green };

template <class T> class MessageQueue {
public:
  void send(T &&msg);
  T receive();

private:
  std::deque<T> _queue;
  std::condition_variable _condition;
  std::mutex _mut;
};

class TrafficLight : public TrafficObject {
public:
  // constructor / desctructor
  TrafficLight();

  // getters / setters
  TrafficLightPhase getCurrentPhase();

  // typical behaviour methods
  void waitForGreen();
  void simulate();

private:
  // typical behaviour methods
  void cycleThroughPhases();
  TrafficLightPhase _currentPhase;
  std::condition_variable _condition;
  std::mutex _mutex;
  MessageQueue<TrafficLightPhase> _msgQueue;
};

#endif