/*
 * Copyright (C) 2015-2016  Intel Corporation. All rights reserved.
 *
 * This file is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <inttypes.h>
#include <vector>

#include <AP_HAL/HAL.h>
#include <AP_HAL/I2CDevice.h>

#include "Semaphores.h"

namespace Linux {

class I2CBus;

class I2CDevice : public AP_HAL::I2CDevice {
public:
    static I2CDevice *from(AP_HAL::I2CDevice *dev)
    {
        return static_cast<I2CDevice*>(dev);
    }

    /* AP_HAL::I2CDevice implementation */

    I2CDevice(I2CBus &bus, uint8_t address);

    ~I2CDevice();

    /* See AP_HAL::I2CDevice::set_address() */
    void set_address(uint8_t address) override { _address = address; }

    /* See AP_HAL::I2CDevice::set_retries() */
    void set_retries(uint8_t retries) override { _retries = retries; }

    /* AP_HAL::Device implementation */

    /* See AP_HAL::Device::set_speed(): Empty implementation, not supported. */
    bool set_speed(enum Device::Speed speed) override { return true; }

    /* See AP_HAL::Device::transfer() */
    bool transfer(const uint8_t *send, uint32_t send_len,
                  uint8_t *recv, uint32_t recv_len) override;

    bool read_registers_multiple(uint8_t first_reg, uint8_t *recv,
                                 uint32_t recv_len, uint8_t times) override;

    /* See AP_HAL::Device::get_semaphore() */
    AP_HAL::Semaphore *get_semaphore() override;

    /* See AP_HAL::Device::register_periodic_callback() */
    AP_HAL::Device::PeriodicHandle register_periodic_callback(
        uint32_t period_usec, AP_HAL::Device::PeriodicCb) override;

    /* See AP_HAL::Device::adjust_periodic_callback() */
    bool adjust_periodic_callback(
        AP_HAL::Device::PeriodicHandle h, uint32_t period_usec) override;

    /* set split transfers flag */
    void set_split_transfers(bool set) override {
        _split_transfers = set;
    }
    
protected:
    I2CBus &_bus;
    uint8_t _address;
    uint8_t _retries = 0;
    bool _split_transfers = false;
};

class I2CDeviceManager : public AP_HAL::I2CDeviceManager {
public:
    friend class I2CDevice;

    static I2CDeviceManager *from(AP_HAL::I2CDeviceManager *i2c_mgr)
    {
        return static_cast<I2CDeviceManager*>(i2c_mgr);
    }

    I2CDeviceManager();

    /* AP_HAL::I2CDeviceManager implementation */
    AP_HAL::I2CDevice *get_device_ptr(uint8_t bus, uint8_t address,
                                     uint32_t bus_clock=400000,
                                     bool use_smbus = false,
                                     uint32_t timeout_ms=4) override;

    /*
     * Stop all I2C threads and block until they are finalized. This doesn't
     * free memory because they can still be used by devices, however device
     * drivers won't receive any new event
     */
    void teardown();

    /*
      get mask of bus numbers for all configured I2C buses
     */
    uint32_t get_bus_mask(void) const override;

    /*
      get mask of bus numbers for all configured external I2C buses
     */
    uint32_t get_bus_mask_external(void) const override;

    /*
      get mask of bus numbers for all configured internal I2C buses
     */
    uint32_t get_bus_mask_internal(void) const override;
    
protected:
    void _unregister(I2CBus &b);
    AP_HAL::I2CDevice *_create_device(I2CBus &b, uint8_t address) const;

    std::vector<I2CBus*> _buses;
};

}
