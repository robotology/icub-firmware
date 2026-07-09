//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: half_type.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 10.113
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Mon Oct 13 17:38:33 2025
//
#ifndef HALF_TYPE_H
#define HALF_TYPE_H
#include "rtwtypes.h"

// Utility function
uint32_T getBitfieldFromFloat(real32_T a);
real32_T getFloatFromBitfield(uint32_T a);

// C++ type definition
#ifdef __cplusplus
#ifdef _MSC_VER

#pragma warning( push )

// This class has multiple assignment operators.
// These assignment operators are callable in the program.
// This warning is informational.
#pragma warning( disable : 4522 )

#endif                                 // _MSC_VER

#if __cplusplus >= 201103L || _MSC_VER >= 1900

class real16_T final {

#else

  class real16_T {

#endif

   public:

#if __cplusplus >= 201103L || _MSC_VER >= 1900

    real16_T() = default;
    ~real16_T() = default;
    real16_T(const real16_T&) = default;
    real16_T(real16_T &&) = default;
    real16_T& operator = (real16_T &&) = default;

#else

    inline real16_T()
    {
    }

#endif

    uint16_T getBitPattern() const
    {
      return bitPattern;
    }

    void setBitPattern(uint16_T a)
    {
      bitPattern = a;
    }

    // Casting operations
    explicit real16_T(real32_T a);
    explicit real16_T(real_T a);

#if __cplusplus >= 201103L || _MSC_VER >= 1900

    explicit operator real32_T() const;
    explicit operator real_T() const;

#else

    operator real32_T() const;
    operator real_T() const;

#endif

    // Basic arithmetic operators
    real16_T operator+(real16_T a) const;
    real16_T operator-(real16_T a) const;
    real16_T operator*(real16_T a) const;
    real16_T operator/(real16_T a) const;
    real16_T operator+() const;
    real16_T operator-() const;
    real16_T& operator++();
    real16_T operator++(int);
    real16_T& operator--();
    real16_T operator--(int);

    // Relational operators
    boolean_T operator==(real16_T a) const;
    boolean_T operator!=(real16_T a) const;
    boolean_T operator>=(real16_T a) const;
    boolean_T operator>(real16_T a) const;
    boolean_T operator<=(real16_T a) const;
    boolean_T operator<(real16_T a) const;

    // Assignment operator overloading
    real16_T& operator= (const real16_T& a);
    void operator= (const volatile real16_T& a) volatile;
    void operator= (const real16_T& a) volatile;

    // Assignments
    real16_T& operator+=(real16_T a);
    real16_T& operator-=(real16_T a);
    real16_T& operator*=(real16_T a);
    real16_T& operator/=(real16_T a);
   private:
    uint16_T bitPattern;               // Internal Storage
  };

#ifdef _MSC_VER

#pragma warning( pop )

#endif                                 // _MSC_VER

  // Complex type definition for half_t
  typedef struct {
    real16_T re;
    real16_T im;
  } creal16_T;

  // Utility function for half and bit pattern conversion
  uint16_T getBitfieldFromHalf(real16_T a);
  real16_T getHalfFromBitfield(uint16_T a);

  // Data Type Conversion
  real32_T halfToFloat(real16_T a);
  real_T halfToDouble(real16_T a);
  real16_T floatToHalf(real32_T a);
  real16_T doubleToHalf(real_T a);

#endif                                 // __cplusplus
#endif                                 // HALF_TYPE_H

  //
  // File trailer for generated code.
  //
  // [EOF]
  //
