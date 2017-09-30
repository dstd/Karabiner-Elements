#pragma once

#include "manipulator/details/conditions/base.hpp"
#include <string>
#include <vector>

namespace krbn {
namespace manipulator {
namespace details {
namespace conditions {
class inputsource final : public base {
public:
  enum class type {
    inputsource_if,
    inputsource_unless,
  };

  inputsource(const nlohmann::json& json) : base(),
                                            type_(type::inputsource_if) {
    if (json.is_object()) {
      for (auto it = std::begin(json); it != std::end(json); std::advance(it, 1)) {
        // it.key() is always std::string.
        const auto& key = it.key();
        const auto& value = it.value();

        if (key == "type") {
          if (value.is_string()) {
            if (value == "inputsource_if") {
              type_ = type::inputsource_if;
            }
            if (value == "inputsource_unless") {
              type_ = type::inputsource_unless;
            }
          }
        } else if (key == "id") {
          if (value.is_string()) {
            id_ = value;
          } else {
            logger::get_logger().error("complex_modifications json error: Invalid form of {0} in {1}", key, json.dump());
          }
        } else {
          logger::get_logger().error("complex_modifications json error: Unknown key: {0} in {1}", key, json.dump());
        }
      }
      if (id_.empty()) {
        logger::get_logger().error("complex_modifications json error: missing inputsource id in {0}", json.dump());
      }
    }
  }

  virtual ~inputsource(void) {
  }

  virtual bool is_fulfilled(const event_queue::queued_event& queued_event,
                            const manipulator_environment& manipulator_environment) const {
    auto current_id = manipulator_environment.get_inputsource();
    
    switch (type_) {
      case type::inputsource_if:
        return current_id == id_;
      case type::inputsource_unless:
        return current_id != id_;
    }
  }

private:
  type type_;
  std::string id_;
};
} // namespace conditions
} // namespace details
} // namespace manipulator
} // namespace krbn
