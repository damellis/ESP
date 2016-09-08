#pragma once
#include <atomic>

class Stream {
  public:
    Stream() : has_started_(false) {}

    /**
     Start the stream.
     */
    virtual bool start() { has_started_ = true; return true; }
    virtual void stop() { has_started_ = false; }

    void toggle() {
        if (has_started_) { stop(); }
        else { start(); }
    }
    
    bool hasStarted() { return has_started_; }

  protected:
    std::atomic_bool has_started_;
};
