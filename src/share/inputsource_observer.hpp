#pragma once

#include "inputsource_observer_objc.h"
#include "logger.hpp"

namespace krbn {
class inputsource_observer final {
public:
  typedef std::function<void(const std::string& inputsource_id)>
      callback;

  inputsource_observer(const callback& callback) : callback_(callback),
                                                   observer_(nullptr) {
    logger::get_logger().info("inputsource_observer initialize");
    krbn_inputsource_observer_initialize(&observer_,
                                         static_cpp_callback,
                                         this);
  }

  ~inputsource_observer(void) {
    logger::get_logger().info("inputsource_observer terminate");
    krbn_inputsource_observer_terminate(&observer_);
  }

private:
  static void static_cpp_callback(const char* inputsource_id,
                                  void* context) {
    auto observer = reinterpret_cast<inputsource_observer*>(context);
    if (observer) {
      observer->cpp_callback(inputsource_id);
    }
  }

  void cpp_callback(const std::string& inputsource_id) {
    if (callback_) {
      callback_(inputsource_id);
    }
  }

  callback callback_;
  krbn_inputsource_observer_objc* observer_;
};
} // namespace krbn
