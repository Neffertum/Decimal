#include "decimal_core.h"
#include "s21_decimal.h"

int s21_is_equal(s21_decimal decim1, s21_decimal decim2) {
  s21_balancing(&decim1, &decim2);
  return s21_is_equal_lite(decim1, decim2);
}

int s21_is_not_equal(s21_decimal decim1, s21_decimal decim2) {
  return !s21_is_equal(decim1, decim2);
}

int s21_is_less(s21_decimal decim1, s21_decimal decim2) {
  int is_less = 0;
  int sign_1 = s21_check_sign(decim1);
  int sign_2 = s21_check_sign(decim2);
  if (!sign_1 && !sign_2) {
    s21_balancing(&decim1, &decim2);
    is_less = s21_is_less_lite(decim1, decim2);
  } else if (sign_1 ^ sign_2) {
    is_less = (sign_1) ? 1 : 0;
  } else {
    s21_balancing(&decim1, &decim2);
    if (!s21_is_equal_lite(decim1, decim2))
      is_less = !s21_is_less_lite(decim1, decim2);
    else
      is_less = 0;
  }
  return is_less;
}

int s21_is_greater(s21_decimal decim1, s21_decimal decim2) {
  return !s21_is_less_or_equal(decim1, decim2);
}

int s21_is_greater_or_equal(s21_decimal decim1, s21_decimal decim2) {
  int res = 0;
  if (s21_is_greater(decim1, decim2) == 1)
    res = 1;
  else if (s21_is_equal(decim1, decim2) == 1)
    res = 1;
  return res;
}

int s21_is_less_or_equal(s21_decimal decim1, s21_decimal decim2) {
  int res = 0;
  if (s21_is_less(decim1, decim2) == 1)
    res = 1;
  else if (s21_is_equal(decim1, decim2) == 1)
    res = 1;
  return res;
}

int s21_is_less_lite(s21_decimal decim1, s21_decimal decim2) {
  int res_less = 0;
  for (int bit = ALL_BIT - 1; bit >= 0; bit--) {
    if (s21_get_global_bit(decim1, bit) ^ s21_get_global_bit(decim2, bit)) {
      res_less = s21_get_global_bit(decim2, bit);
      break;
    }
  }
  return res_less;
}

int s21_is_greater_lite(s21_decimal decim1, s21_decimal decim2) {
  return !s21_is_less_or_equal_lite(decim1, decim2);
}

int s21_is_less_or_equal_lite(s21_decimal decim1, s21_decimal decim2) {
  int res = 0;
  if (s21_is_less_lite(decim1, decim2) == 1)
    res = 1;
  else if (s21_is_equal_lite(decim1, decim2) == 1)
    res = 1;
  return res;
}

int s21_is_equal_lite(s21_decimal decim1, s21_decimal decim2) {
  int res_equal = 1;
  for (int i = 0; i < (ALL_BIT + BITS_IN_INT); i++) {
    if (s21_get_global_bit(decim1, i) != s21_get_global_bit(decim2, i)) {
      res_equal = 0;
      break;
    }
  }
  return res_equal;
}