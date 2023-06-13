#include "s21_decimal.h"

int s21_from_int_to_decimal(int src_int, s21_decimal *dst_decimal) {
  int error_mark = 0;
  if (!dst_decimal)
    return 1;
  else {
    s21_nullify_all_decimal(dst_decimal);
    if (src_int < 0) {
      src_int = -src_int;
      s21_chang_sign(dst_decimal);
    }
    dst_decimal->bits[0] = src_int;
  }
  return error_mark;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int result = 0;
  int ten_pow = s21_get_exp(src);
  if (!dst) {
    result = 1;
  } else if (ten_pow > 28) {
    *dst = 0;
    result = 1;
  } else {
    s21_truncate(src, &src);

    for (int i = 1; i < 3; i++) {
      for (int j = 0; j < 16; j++) {
        if (s21_get_bit(src.bits[i], j) == 1) {
          result = 1;
          break;
        }
      }
      if (result == 1) {
        break;
      }
    }

    if (result == 0) {
      if (s21_check_sign(src) == 1)
        *dst = 0 - src.bits[0];
      else
        *dst = src.bits[0];
      if (*dst) *dst = s21_set_bit(*dst, 31, s21_check_sign(src));
    }
  }
  return result;
}

// if 0 - function can do convertors
// if 1 - mistake

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int error_mark = 0;
  float mark = src;
  if (!dst) {
    error_mark = 1;
  } else if (src == 0.0) {
    error_mark = 0;
    *dst = s21_nullify_all_decimal(dst);
    if (signbit(src) != 0) {  // look on sign
      s21_set_sign(dst, 0);   // add sign for -0.0
    }
  } else if (isinf(mark) || isnan(mark)) {
    error_mark = 1;
    *dst = s21_decimal_get_inf();
    if (signbit(src) != 0) {
      s21_set_sign(dst, 0);
    }
  } else if (fabsf(src) > MAX_FLOAT_TO_CONVERT) {
    error_mark = 1;
    *dst = s21_decimal_get_inf();
    if (signbit(src) != 0) {
      s21_set_sign(dst, 0);
    }
  } else if (fabsf(src) < MIN_FLOAT_TO_CONVERT) {
    error_mark = 1;
    *dst = s21_nullify_all_decimal(dst);
  } else {
    *dst = s21_nullify_all_decimal(dst);
    s21_decimal results;
    char flt[64];
    // add float to view 1 number and 6 number afte point
    // for example 1.234566
    sprintf(flt, "%.7E", fabsf(src));
    int exp = s21_get_float_exp_from_string(flt);

    if (exp <= -23) {
      // if scale too smole
      // correct accuracy
      int float_precision = exp + 28;
      sprintf(flt, "%.*E", float_precision, fabsf(src));
    }

    results = s21_float_string_to_decimal(flt);

    if (signbit(src) != 0) {
      s21_set_sign(&results, 0);
    }
    if (src < 0) {
      s21_set_sign(&results, 1);
    }

    *dst = results;
  }
  return error_mark;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int return_code = OK;

  if (!dst) {
    return_code = 1;
  } else {
    double reserve = 0;
    for (int i = 0; i < 96; i++)
      if (src.bits[i / 32] & (1u << (i % 32))) reserve += pow(2, i);
    int sign = (src.bits[3] < 0) ? 1 : 0;
    int ten_pow = (src.bits[3] << 1) >> 17;
    if (ten_pow >= 0) {
      if (ten_pow > 28) {
        *dst = 0;
        return_code = 1;
      } else {
        for (int i = ten_pow; i > 0; i--) {
          reserve /= 10;
        }
        *dst = (float)reserve;
        if (sign) *dst *= -1;
      }
    }
  }
  return return_code;
}
