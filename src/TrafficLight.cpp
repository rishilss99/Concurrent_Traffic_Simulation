#include "TrafficLight.h"
#include <iostream>
#include <random>

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
  std::unique_lock<std::mutex> uLock(_mut);
  _condition.wait(uLock, [this] { return !_queue.empty(); });
  T msg = std::move(_queue.back());
  _queue.pop_back();
  return msg;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  std::lock_guard<std::mutex> lock(_mut);
  _queue.emplace_back(std::move(msg));
  _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

void TrafficLight::waitForGreen() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if (_msgQueue.receive() == TrafficLightPhase::green)
      return;
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {

  std::random_device rd;
  std::mt19937 eng(rd());
  std::uniform_int_distribution<int> dist(4000, 6000);
  long durationCycle = dist(eng);

  auto updateTime = std::chrono::system_clock::now();

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto currentTime = std::chrono::system_clock::now();
    if ((std::chrono::duration_cast<std::chrono::milliseconds>(currentTime -
                                                               updateTime)
             .count()) >= durationCycle) {

      if (_currentPhase == TrafficLightPhase::green)
        _currentPhase = TrafficLightPhase::red;
      else
        _currentPhase = TrafficLightPhase::green;

      _msgQueue.send(std::move(_currentPhase));
      updateTime = std::chrono::system_clock::now();
      durationCycle = dist(eng);
    }
  }
}
