#include <poincare/rational.h>
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
}
#include <poincare/arithmetic.h>
#include <poincare/complex.h>
#include <poincare/opposite.h>
#include "layout/string_layout.h"
#include "layout/fraction_layout.h"

namespace Poincare {

// Constructors

Rational::Rational(const Integer numerator, const Integer denominator) {
  assert(!denominator.isZero());
  if (numerator.isOne() || denominator.isOne()) {
    // Avoid computing GCD if possible
    m_numerator = numerator;
    m_denominator = denominator;
  } else {
    Integer gcd = Arithmetic::GCD(&numerator, &denominator);
    m_numerator = Integer::Division(numerator, gcd).quotient;
    m_denominator = Integer::Division(denominator, gcd).quotient;
  }
  if (m_numerator.isNegative() && m_denominator.isNegative()) {
    m_numerator.setNegative(false);
    m_denominator.setNegative(false);
  } else if (m_denominator.isNegative()) {
    m_numerator.setNegative(true);
    m_denominator.setNegative(false);
  }
}

Rational::Rational(const Integer numerator) {
  m_numerator = numerator;
  m_denominator = Integer(1);
}

Rational::Rational(const Rational & other) {
  m_numerator = other.m_numerator;
  m_denominator = other.m_denominator;
}

// Getter
const Integer Rational::numerator() const {
  return m_numerator;
}

const Integer Rational::denominator() const {
  return m_denominator;
}
// Expression subclassing

Expression::Type Rational::type() const {
  return Type::Rational;
}

Expression * Rational::clone() const {
  return new Rational(m_numerator, m_denominator);
}

Expression::Sign Rational::sign() const {
  if (m_numerator.isNegative()) {
    return Sign::Negative;
  }
  return Sign::Positive;
}

Expression * Rational::setSign(Sign s) {
  assert(s != Sign::Unknown);
  bool negative = s == Sign::Negative ? true : false;
  m_numerator.setNegative(negative);
  return this;
}

Expression * Rational::shallowBeautify(Context & context, AngleUnit angleUnit) {
  if (m_numerator.isNegative()) {
    m_numerator.setNegative(false);
    const Expression * opOperand[1] = {clone()};
    Opposite * o = new Opposite(opOperand, true);
    return replaceWith(o, true);
  }
  return this;
}

// Basic operations

Rational Rational::Addition(const Rational & i, const Rational & j) {
  Integer newNumerator = Integer::Addition(Integer::Multiplication(i.numerator(), j.denominator()), Integer::Multiplication(j.numerator(), i.denominator()));
  Integer newDenominator = Integer::Multiplication(i.denominator(), j.denominator());
  return Rational(newNumerator, newDenominator);
}

Rational Rational::Multiplication(const Rational & i, const Rational & j) {
  Integer newNumerator = Integer::Multiplication(i.numerator(), j.numerator());
  Integer newDenominator = Integer::Multiplication(i.denominator(), j.denominator());
  return Rational(newNumerator, newDenominator);
}

Rational Rational::Power(const Rational & i, const Integer & j) {
  Integer absJ = j;
  absJ.setNegative(false);
  Integer newNumerator = Integer::Power(i.numerator(), absJ);
  Integer newDenominator = Integer::Power(i.denominator(), absJ);
  if (j.isNegative()) {
    return Rational(newDenominator, newNumerator);
  }
  return Rational(newNumerator, newDenominator);
}

// Comparison

int Rational::simplificationOrderSameType(const Expression * e) const {
  assert(e->type() == Expression::Type::Rational);
  const Rational * other = static_cast<const Rational *>(e);
  Integer i1 = Integer::Multiplication(m_numerator, other->denominator());
  Integer i2 = Integer::Multiplication(m_denominator, other->numerator());
  return Integer::NaturalOrder(i1, i2);
}

template<typename T> Evaluation<T> * Rational::templatedEvaluate(Context& context, Expression::AngleUnit angleUnit) const {
  T n = m_numerator.approximate<T>();
  T d = m_denominator.approximate<T>();
  return new Complex<T>(Complex<T>::Float(n/d));
}

ExpressionLayout * Rational::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  ExpressionLayout * numeratorLayout = m_numerator.createLayout();
  if (m_denominator.isOne()) {
    return numeratorLayout;
  }
  ExpressionLayout * denominatorLayout = m_denominator.createLayout();
  return new FractionLayout(numeratorLayout, denominatorLayout);
}

int Rational::writeTextInBuffer(char * buffer, int bufferSize) const {
  buffer[bufferSize-1] = 0;
  int numberOfChar = m_numerator.writeTextInBuffer(buffer, bufferSize);
  if (m_denominator.isOne()) {
    return numberOfChar;
  }
  if (numberOfChar >= bufferSize-1) {
    return numberOfChar;
  }
  buffer[numberOfChar++] = '/';
  numberOfChar += m_denominator.writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  return numberOfChar;
}

}

