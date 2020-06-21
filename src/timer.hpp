#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

template <class Duration, class F> class Timer {
private:
  std::function<F(void)> m_callable;
  std::atomic_bool m_running = false, m_got_stopped = false;
  std::thread m_timer_thread;
  Duration m_cycle_time;
  std::condition_variable m_cond_cycle;
  std::mutex m_cycle_mutex;
  void timer_counter() {

    while (m_running) {
      std::unique_lock lock(m_cycle_mutex);
      m_cond_cycle.wait_for(lock, m_cycle_time);
      if (!m_got_stopped){
        m_callable();
      }
    }
  };

public:
  Timer(const int &time_, const std::function<F(void)> &func_)
      : m_cycle_time(Duration(time_)), m_callable(func_), m_cond_cycle(),
        m_cycle_mutex(){};
  ~Timer(){};

  void start() {
    m_running = true;
    m_timer_thread = std::thread(&Timer::timer_counter, this);
  };

  void stop() {
    m_running = false;
    m_got_stopped = true;
    m_cond_cycle.notify_all();
    if (m_timer_thread.joinable())
      m_timer_thread.join();
  }
};
