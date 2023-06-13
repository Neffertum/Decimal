#include "decimal_core.h"
#include "s21_decimal.h"

// https://russianblogs.com/article/568550142/

// add without degree and sign
int s21_add_lite(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal) {
  s21_nullify_all_decimal(result_decimal);
  int error_mark = 0;
  for (int i = 0; i < 96; i++) {
    int bit_decim1 = s21_get_global_bit(decim1, i);
    int bit_decim2 = s21_get_global_bit(decim2, i);
    if (bit_decim1 ^ bit_decim2 ^ error_mark) {
      s21_set_global_bit(result_decimal, i, 1);
    } else {
      s21_set_global_bit(result_decimal, i, 0);
    }
    error_mark = (bit_decim1 && bit_decim2) || (bit_decim1 && error_mark) ||
                 (bit_decim2 && error_mark);
  }
  return error_mark;
}

int s21_add(s21_decimal decim1, s21_decimal decim2,
            s21_decimal *result_decimal) {
  int error_mark = 0;
  int decim1_sign = s21_check_sign(decim1);
  int decim2_sign = s21_check_sign(decim2);
  s21_balancing(&decim1, &decim2);
  int scale = s21_get_exp(decim1);
  if (!(decim1_sign ^ decim2_sign)) {
    error_mark = s21_add_lite(decim1, decim2, result_decimal);
    s21_set_sign(result_decimal, decim1_sign);
  } else if (s21_is_less_or_equal_lite(decim1, decim2)) {
    s21_sub_lite(decim2, decim1, result_decimal);
    s21_set_sign(result_decimal, decim2_sign);
  } else {
    s21_sub_lite(decim1, decim2, result_decimal);
    s21_set_sign(result_decimal, decim1_sign);
  }
  if (error_mark && scale) {
    s21_bank_round(&decim1, 1);
    s21_bank_round(&decim2, 1);
    error_mark = s21_add(decim1, decim2, result_decimal);
  } else {
    s21_set_exp(result_decimal, scale);
  }
  return error_mark;
}

int s21_sub_lite(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal) {
  int error_mark = 0;
  s21_nullify_all_decimal(result_decimal);
  for (int i = 0; i < 96; i++) {
    int bit_decim1 = s21_get_global_bit(decim1, i);
    int bit_decim2 = s21_get_global_bit(decim2, i);
    if (bit_decim1 ^ bit_decim2) {
      s21_set_global_bit(result_decimal, i, 1);
    }
    if (!bit_decim1 && bit_decim2) {
      int k = i + 1;
      while ((bit_decim1 = !s21_get_global_bit(decim1, k)) && k < 96) {
        s21_set_global_bit(&decim1, k, 1);
        k++;
      }
      if (k == 96) {
        error_mark = 1;
        s21_nullify_all_decimal(result_decimal);
      }
      s21_set_global_bit(&decim1, k, 0);
    }
  }
  return error_mark;
}

int s21_sub_sign(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal) {
  int error_mark = 0;
  int sign_decim1 = s21_check_sign(decim1);
  int sign_decim2 = s21_check_sign(decim2);
  int priority = s21_is_greater_or_equal(decim1, decim2);

  if (sign_decim1 == 1 && sign_decim2 == 1) {
    s21_chang_sign(&decim1);
    s21_chang_sign(&decim2);
    if (priority == 0) {
      error_mark = s21_sub_lite(decim1, decim2, result_decimal);
      s21_chang_sign(result_decimal);
    } else {
      error_mark = s21_sub_lite(decim2, decim1, result_decimal);
    }
  } else if (sign_decim1 == 1) {
    error_mark = s21_add_lite(decim1, decim2, result_decimal);
    s21_chang_sign(result_decimal);
  } else if (sign_decim2 == 1) {
    error_mark = s21_add_lite(decim1, decim2, result_decimal);
  } else {
    if (priority == 1) {
      error_mark = s21_sub_lite(decim1, decim2, result_decimal);
    } else {
      error_mark = s21_sub_lite(decim2, decim1, result_decimal);
      s21_chang_sign(result_decimal);
    }
  }
  return error_mark;
}

int s21_sub(s21_decimal decim1, s21_decimal decim2,
            s21_decimal *result_decimal) {
  int error_mark = 0;
  s21_nullify_all_decimal(result_decimal);
  s21_balancing(&decim1, &decim2);
  int exp = s21_get_exp(decim1);
  error_mark = s21_sub_sign(decim1, decim2, result_decimal);
  s21_set_exp(result_decimal, exp);
  return error_mark;
}

int s21_mul_lite(s21_decimal decim1, s21_decimal decim2,
                 s21_decimal *result_decimal) {
  // nullify_all_decimal(result_decimal);

  int status = 0;
  s21_decimal tmp = *result_decimal;
  int sign = s21_check_sign(decim1) ^ s21_check_sign(decim2);
  s21_set_sign(&tmp, s21_check_sign(decim1) ^ s21_check_sign(decim2));
  s21_set_sign(&decim2, 0);
  s21_set_sign(&decim1, 0);

  while (!s21_is_null_decimal_bin(decim2) && !status) {
    if (s21_get_global_bit(decim2, 0)) {
      status = s21_add_lite(tmp, decim1, &tmp);
    }
    decim2 = s21_bit_swift_right(decim2, 1);
    if (!status) status = s21_bit_swift_left(decim1, 1, &decim1);
  }

  if (!status)
    *result_decimal = tmp;
  else if (s21_check_sign(decim1) ^ s21_check_sign(decim2))
    ++status;
  s21_set_sign(result_decimal, sign);
  return status;
}

int s21_mul(s21_decimal decim1, s21_decimal decim2,
            s21_decimal *result_decimal) {
  s21_nullify_all_decimal(result_decimal);
  int error_mark = 0;  // mul_lite(decim1, decim2, result_decimal);
  if (s21_is_null_decimal(decim1) == 1 || s21_is_null_decimal(decim2) == 1) {
    s21_nullify_all_decimal(result_decimal);
  } else if (s21_get_exp(decim1) == 0 && s21_get_exp(decim2) == 0) {
    error_mark = s21_mul_lite(decim1, decim2, result_decimal);
  } else {
    int result_exp = s21_get_exp(decim1) + s21_get_exp(decim2);
    error_mark = s21_mul_lite(decim1, decim2, result_decimal);
    if (result_exp <= MAX_EXP)
      s21_set_exp(result_decimal, result_exp);
    else
      s21_nullify_all_decimal(result_decimal);
  }
  return error_mark;
}

int s21_div_lite(s21_decimal decim1, s21_decimal decim2, s21_decimal *result) {
  int status = 0;
  s21_decimal one;
  s21_from_int_to_decimal(1, &one);
  s21_decimal temp;
  s21_nullify_all_decimal(&temp);
  *result = one;
  if (s21_is_equal_lite(decim1, decim2)) {
    *result = one;
  } else if (s21_is_less_lite(decim1, decim2)) {
    s21_nullify_all_decimal(result);
  } else {
    s21_decimal divcopy = decim2;
    s21_decimal tmp;
    while (s21_is_greater_lite(decim1, decim2) == 1 && status == 0) {
      tmp = decim2;
      status = s21_bit_swift_left(decim2, 1, &decim2);
      s21_bit_swift_left(*result, 1, result);
    }
    if (status == 1) {
      decim2 = tmp;
      *result = s21_bit_swift_right(*result, 1);
      status--;
    }
    if (s21_is_less_lite(decim1, decim2) == 1) {
      decim2 = s21_bit_swift_right(decim2, 1);
      *result = s21_bit_swift_right(*result, 1);
    }
    s21_sub_lite(decim1, decim2, &temp);
    status = s21_div_lite(temp, divcopy, &one);
    status = s21_add_lite(*result, one, result);
  }
  return status;
}

int s21_int_div(s21_decimal decim1, s21_decimal decim2, s21_decimal *result) {
  int error_mark = 0;

  if (s21_is_null_decimal(decim2)) {
    error_mark = 3;
  } else {
    s21_from_int_to_decimal(1, result);

    int sign = s21_check_sign(decim1) ^ s21_check_sign(decim2);
    s21_set_sign(&decim1, 0);
    s21_set_sign(&decim2, 0);

    int res_exp = s21_get_exp(decim1) - s21_get_exp(decim2);
    s21_set_exp(&decim1, 0);
    s21_set_exp(&decim2, 0);

    error_mark = s21_div_lite(decim1, decim2, result);
    if (res_exp < 0) {
      error_mark = s21_shifting(result, -res_exp);
      error_mark += -res_exp << 2;

      res_exp = 0;
    }
    s21_set_exp(result, res_exp);
    s21_set_sign(result, sign);
  }

  return error_mark;
}

s21_decimal s21_mod_lite(s21_decimal decim1, s21_decimal decim2) {
  int status = 0;
  s21_decimal divcopy = decim2;
  s21_decimal res, tmp;
  s21_decimal zero = {{0, 0, 0, 0}};
  if (s21_is_equal_lite(decim1, decim2))
    return zero;
  else if (s21_is_less_lite(decim1, decim2))
    return decim1;

  while (s21_is_less_or_equal_lite(decim2, decim1) && !s21_check_sign(decim2) &&
         !s21_is_null_decimal(decim2)) {
    tmp = decim2;
    status = s21_bit_swift_left(decim2, 1, &decim2);
  }
  if (status == 1) {
    decim2 = tmp;
    status--;
  }
  if (s21_is_less_lite(decim1, decim2)) {
    decim2 = s21_bit_swift_right(decim2, 1);
  }
  s21_sub_lite(decim1, decim2, &zero);
  res = s21_mod_lite(zero, divcopy);
  return res;
}

int s21_mod_sign(s21_decimal decim1, s21_decimal decim2, s21_decimal *result) {
  if (s21_is_null_decimal(decim2)) return 3;
  s21_nullify_all_decimal(result);
  int sign = s21_check_sign(decim1);
  s21_set_sign(&decim1, 0);
  s21_set_sign(&decim2, 0);
  *result = s21_mod_lite(decim1, decim2);
  s21_set_sign(result, sign);
  return 0;
}

int s21_mod(s21_decimal decim1, s21_decimal decim2, s21_decimal *result) {
  s21_balancing(&decim1, &decim2);
  int exp = s21_get_exp(decim1);
  int res = s21_mod_sign(decim1, decim2, result);
  s21_set_exp(result, exp);
  return res;
}

int s21_additional_int_prec(s21_decimal decim1, s21_decimal decim2,
                            s21_decimal *result, int status) {
  s21_set_exp(&decim2, s21_get_exp(decim2) - status);
  int decim1_sign = s21_check_sign(decim1);
  int decim2_sign = s21_check_sign(decim2);
  s21_decimal mod_res, div_res;
  s21_nullify_all_decimal(&div_res);
  s21_nullify_all_decimal(&mod_res);

  s21_mod(decim1, decim2, &mod_res);
  s21_shifting(&mod_res, -1);

  s21_set_exp(&decim2, s21_get_exp(decim2) + status);

  while (s21_is_less_lite(mod_res, decim2)) s21_bank_round(&decim2, 1);
  s21_int_div(mod_res, decim2, &div_res);
  status = s21_add(div_res, *result, result);
  s21_set_sign(result, decim1_sign ^ decim2_sign);
  return status;
}

int s21_div(s21_decimal decim1, s21_decimal decim2, s21_decimal *result) {
  int status = s21_int_div(decim1, decim2, result);
  if (!(status & 3)) {
    status >>= 2;
    s21_decimal mod_res;
    s21_nullify_all_decimal(&mod_res);
    if (status)
      status = s21_additional_int_prec(decim1, decim2, result, status);
    s21_truncate(*result, result);
    s21_mod(decim1, decim2, &mod_res);
    if (!s21_is_null_decimal_bin(mod_res)) {
      s21_decimal div_res, tmp;
      s21_decimal ten = {{10, 0, 0, 0}};
      s21_nullify_all_decimal(&div_res);
      s21_nullify_all_decimal(&tmp);
      int exp = s21_get_exp(mod_res);
      for (int sub_stat = 0; !sub_stat; s21_nullify_all_decimal(&tmp)) {
        sub_stat = s21_mul_lite(mod_res, ten, &tmp);
        if (!sub_stat) {
          mod_res = tmp;
          if (++exp < 29) s21_set_exp(&mod_res, exp);
        }
      }
      if (!s21_get_exp(mod_res)) s21_set_exp(&mod_res, 28);
      while (s21_is_less_lite(mod_res, decim2)) s21_bank_round(&decim2, 1);
      s21_int_div(mod_res, decim2, &div_res);
      if (exp > 28) s21_set_exp(&div_res, exp - s21_get_exp(decim2));
      status = s21_add(div_res, *result, result);
    }
  } else {
    status &= 3;
  }
  if (status == 1 && s21_check_sign(decim1) ^ s21_check_sign(decim2)) ++status;
  return status;
}
