#include <poincare/addition.h>
#include "layout/horizontal_layout.h"

Expression::Type Addition::type() {
  return Expression::Type::Addition;
}

float Addition::operateApproximatevelyOn(float a, float b) {
  return a + b;
}

ExpressionLayout * Addition::createLayout(ExpressionLayout * parent) {
  //FIXME: There can be more than two operands now! :-)
  return new HorizontalLayout(parent, operand(0), '+', operand(1));
}
