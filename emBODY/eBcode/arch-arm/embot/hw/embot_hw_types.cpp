
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_types.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp.h"
#include <cstring>
#include <vector>
#include <array>

#include "embot_core_binary.h"
#include "embot_core.h"


using namespace std;
using namespace embot::core::binary;


// --------------------------------------------------------------------------------------------------------------------
// - shared static checks
// --------------------------------------------------------------------------------------------------------------------

// - check of values for embot::hw::led          
static_assert(embot::core::tointegral(embot::hw::LED::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "LED::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::LED::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "LED::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::LED::maxnumberof) < embot::core::tointegral(embot::hw::LED::none), "LED::maxnumberof must be higher that LED::none, so that we can optimise code");




// - check of values for embot::hw::button           
static_assert(embot::core::tointegral(embot::hw::BTN::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "BTN::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::BTN::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "BTN::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::BTN::maxnumberof) < embot::core::tointegral(embot::hw::BTN::none), "BTN::maxnumberof must be higher that BTN::none, so that we can optimise code");



// - check of values for embot::hw::can
static_assert(embot::core::tointegral(embot::hw::CAN::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "CAN::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::CAN::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "CAN::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::CAN::maxnumberof) < embot::core::tointegral(embot::hw::CAN::none), "CAN::maxnumberof must be higher that CAN::none, so that we can optimise code");




// - check of values for embot::hw::flash
static_assert(embot::core::tointegral(embot::hw::FLASH::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "FLASH::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::FLASH::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "FLASH::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::FLASH::maxnumberof) < embot::core::tointegral(embot::hw::FLASH::none), "FLASH::maxnumberof must be higher that FLASH::none, so that we can optimise code");




// - check of values for embot::hw::pga308

static_assert(embot::core::tointegral(embot::hw::PGA308::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "PGA308::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::PGA308::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "PGA308::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::PGA308::maxnumberof) < embot::core::tointegral(embot::hw::PGA308::none), "PGA308::maxnumberof must be higher that PGA308::none, so that we can optimise code");




// - check of values for embot::hw::si7051
static_assert(embot::core::tointegral(embot::hw::SI7051::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "SI7051::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::SI7051::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "SI7051::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::SI7051::maxnumberof) < embot::core::tointegral(embot::hw::SI7051::none), "SI7051::maxnumberof must be higher that SI7051::none, so that we can optimise code");



// - check of values for embot::hw::onewire

static_assert(embot::core::tointegral(embot::hw::ONEWIRE::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "ONEWIRE::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::ONEWIRE::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "ONEWIRE::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::ONEWIRE::maxnumberof) < embot::core::tointegral(embot::hw::ONEWIRE::none), "ONEWIRE::maxnumberof must be higher that ONEWIRE::none, so that we can optimise code");



// - check of values for embot::hw::adc           
static_assert(embot::core::tointegral(embot::hw::ADC::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "ADC::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::ADC::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "ADC::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::ADC::maxnumberof) < embot::core::tointegral(embot::hw::ADC::none), "ADC::maxnumberof must be higher that ADC::none, so that we can optimise code");




// - check of values for embot::hw::timer
static_assert(embot::core::tointegral(embot::hw::TIMER::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "TIMER::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::TIMER::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "TIMER::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::TIMER::maxnumberof) < embot::core::tointegral(embot::hw::TIMER::none), "TIMER::maxnumberof must be higher that CAN::none, so that we can optimise code");



// - check of values for embot::hw::i2c
static_assert(embot::core::tointegral(embot::hw::I2C::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "I2C::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::I2C::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "I2C::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::I2C::maxnumberof) < embot::core::tointegral(embot::hw::I2C::none), "I2C::maxnumberof must be higher that I2C::none, so that we can optimise code");





// - check of values for embot::hw::bno055
static_assert(embot::core::tointegral(embot::hw::BNO055::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "BNO055::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::BNO055::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "BNO055::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::BNO055::maxnumberof) < embot::core::tointegral(embot::hw::BNO055::none), "BNO055::maxnumberof must be higher that BNO055::none, so that we can optimise code");



// - check of values for embot::hw::tlv493d

static_assert(embot::core::tointegral(embot::hw::TLV493D::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "TLV493D::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::TLV493D::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "TLV493D::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::TLV493D::maxnumberof) < embot::core::tointegral(embot::hw::TLV493D::none), "TLV493D::maxnumberof must be higher that TLV493D::none, so that we can optimise code");

// - check of values for embot::hw::ads122c04
 
static_assert(embot::core::tointegral(embot::hw::ADS122C04::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "ADS122C04::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::ADS122C04::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "ADS122C04::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::ADS122C04::maxnumberof) < embot::core::tointegral(embot::hw::ADS122C04::none), "ADS122C04::maxnumberof must be higher that ADS122C04::none, so that we can optimise code");



// - check of values for embot::hw::ad7147

static_assert(embot::core::tointegral(embot::hw::AD7147::none) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "AD7147::none must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::AD7147::maxnumberof) < 8*sizeof(embot::hw::bsp::SUPP::supportedmask), "AD7147::maxnumberof must be less than 32 to be able to address a std::uint32_t mask");
static_assert(embot::core::tointegral(embot::hw::AD7147::maxnumberof) < embot::core::tointegral(embot::hw::AD7147::none), "AD7147::maxnumberof must be higher that AD7147::none, so that we can optimise code");



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

