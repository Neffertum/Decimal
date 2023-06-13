#include "decimal_core.h"
#include "s21_decimal.h"

int s21_shifting(s21_decimal *a, int n) {
  /// Multiply the number by 10 and increase the exponent by 1 thereby shifting
  /// the number to the left
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal tmp_result;
  s21_nullify_all_decimal(&tmp_result);

  int exp = s21_get_exp(*a);
  int status = exp == 28;

  while (!status && n--) {
    status = s21_mul_lite(*a, ten, &tmp_result);
    if (!status) {
      *a = tmp_result;
      s21_nullify_all_decimal(&tmp_result);
      status = (++exp == 28);
    }
  }

  s21_set_exp(a, exp);
  return status;
}

int s21_balancing(s21_decimal *a, s21_decimal *b) {
  int diff = s21_get_exp(*a) - s21_get_exp(*b);

  if (diff) {
    if (diff > 0)
      s21_shifting(b, diff);
    else
      s21_shifting(a, -diff);

    diff = s21_get_exp(*a) - s21_get_exp(*b);
    if (diff) {
      if (diff > 0)
        s21_bank_round(a, diff);
      else
        s21_bank_round(b, -diff);
    }
  }

  return OK;
}

int s21_bank_rounding(int a) {
  /// returns 1 if you need to apply bank rounding or 0 if not
  return (a % 10 >= 5 || (a % 10 == 5 && (a / 10) % 2));
}

int s21_bank_round(s21_decimal *number, int n) {
  s21_decimal one, ten, hund;
  s21_from_int_to_decimal(1, &one);
  s21_from_int_to_decimal(10, &ten);
  s21_from_int_to_decimal(100, &hund);

  while (n--) {
    s21_decimal mod_res;
    s21_nullify_all_decimal(&mod_res);
    s21_mod_sign(*number, hund, &mod_res);

    int exp = s21_get_exp(*number) - 1;
    s21_int_div(*number, ten, number);
    s21_set_exp(number, exp);

    if (s21_bank_rounding(mod_res.bits[0])) {
      int buf = number->bits[INFO_BIT];
      s21_add_lite(*number, one, number);
      number->bits[INFO_BIT] = buf;
    }
  }

  return OK;
}
