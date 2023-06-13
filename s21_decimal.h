#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include "decimal_core.h"

//  comparison help operations
int s21_is_less_lite(s21_decimal decim1, s21_decimal decim2);
int s21_is_less_or_equal_lite(s21_decimal decim1, s21_decimal decim2);
int s21_is_greater_lite(s21_decimal decim1, s21_decimal decim2);
int s21_is_equal_lite(s21_decimal decim1, s21_decimal decim2);
//  comparison operations
int s21_is_equal(s21_decimal decim1, s21_decimal decim2);
int s21_is_not_equal(s21_decimal decim1, s21_decimal decim2);
int s21_is_greater_or_equal(s21_decimal decim1, s21_decimal decim2);
int s21_is_greater(s21_decimal decim1, s21_decimal decim2);
int s21_is_less_or_equal(s21_decimal decim1, s21_decimal decim2);
int s21_is_less(s21_decimal decim1, s21_decimal decim2);

// convert function
int s21_from_int_to_decimal(int src_int, s21_decimal *dst_decimal);
int s21_from_float_to_decimal(float src_float, s21_decimal *dst_decimal);
int s21_from_decimal_to_int(s21_decimal src_decimal, int *dst_int);
int s21_from_decimal_to_float(s21_decimal src_decimal, float *dst_float);

// arifmetic help
int s21_add_lite(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal);
int s21_add_sign(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal);
int s21_sub_lite(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal);
int s21_sub_sign(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal);
s21_decimal s21_negative_decimal(s21_decimal decim);
int s21_mul_lite(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal);
int s21_mul_sign(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal);
int s21_div_lite(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal);
int s21_div_exp(s21_decimal decim1, s21_decimal decim2,
                s21_decimal *result_decimal);
int s21_div_sign(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal);
int s21_int_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_additional_int_prec(s21_decimal value_1, s21_decimal value_2,
                            s21_decimal *result, int status);
s21_decimal s21_mod_lite(s21_decimal value_1, s21_decimal value_2);
int s21_mod_sign(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal);

// arifmetic operation
int s21_add(s21_decimal decim1, s21_decimal decim2,
            s21_decimal *result_decimal);
int s21_sub(s21_decimal decim1, s21_decimal decim2,
            s21_decimal *result_decimal);
int s21_mul(s21_decimal decim1, s21_decimal decim2,
            s21_decimal *result_decimal);
int s21_div(s21_decimal decim1, s21_decimal decim2,
            s21_decimal *result_decimal);
int s21_mod(s21_decimal decim1, s21_decimal decim2,
            s21_decimal *result_decimal);

// another
int s21_negate(s21_decimal decim, s21_decimal *result_decimal);
int s21_truncate(s21_decimal decim, s21_decimal *result_decimal);
int s21_round(s21_decimal decim, s21_decimal *result_decimal);
int s21_floor(s21_decimal decim, s21_decimal *result_decimal);

#endif  //  SRC_S21_DECIMAL_H_
