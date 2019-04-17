#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esp_one_wire.h"

namespace esphome {
namespace dallas {

class DallasTemperatureSensor;

class DallasComponent : public PollingComponent {
 public:
  explicit DallasComponent(ESPOneWire *one_wire, uint32_t update_interval);

  DallasTemperatureSensor *get_sensor_by_address(const std::string &name, uint64_t address, uint8_t resolution);
  DallasTemperatureSensor *get_sensor_by_index(const std::string &name, uint8_t index, uint8_t resolution);

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void update() override;

 protected:
  friend DallasTemperatureSensor;

  ESPOneWire *one_wire_;
  std::vector<DallasTemperatureSensor *> sensors_;
  std::vector<uint64_t> found_sensors_;
};

/// Internal class that helps us create multiple sensors for one Dallas hub.
class DallasTemperatureSensor : public sensor::Sensor {
 public:
  DallasTemperatureSensor(const std::string &name, uint64_t address, uint8_t resolution, DallasComponent *parent);

  /// Helper to get a pointer to the address as uint8_t.
  uint8_t *get_address8();
  /// Helper to create (and cache) the name for this sensor. For example "0xfe0000031f1eaf29".
  const std::string &get_address_name();

  /// Set the 64-bit unsigned address for this sensor.
  void set_address(uint64_t address);
  /// Get the index of this sensor. (0 if using address.)
  optional<uint8_t> get_index() const;
  /// Set the index of this sensor. If using index, address will be set after setup.
  void set_index(uint8_t index);
  /// Get the set resolution for this sensor.
  uint8_t get_resolution() const;
  /// Set the resolution for this sensor.
  void set_resolution(uint8_t resolution);
  /// Get the number of milliseconds we have to wait for the conversion phase.
  uint16_t millis_to_wait_for_conversion() const;

  bool setup_sensor();
  bool read_scratch_pad();

  bool check_scratch_pad();

  float get_temp_c();

  std::string unique_id() override;

 protected:
  DallasComponent *parent_;
  uint64_t address_;
  optional<uint8_t> index_;

  uint8_t resolution_;
  std::string address_name_;
  uint8_t scratch_pad_[9] = {
      0,
  };
};

}  // namespace dallas
}  // namespace esphome