#include "decimal_core.h"
#include "s21_decimal.h"

//  включает бит под номером bit в bits
int s21_bit_on(int bits, int bit) { return bits | _2(bit % BITS_IN_INT); }
//  выключает бит под номером bit в bits
int s21_bit_off(int bits, int bit) { return bits & ~(_2(bit % BITS_IN_INT)); }

// включен ли бит под номером bit в bits
int s21_bit_is(int bits, int bit) {
  return (bits & (_2(bit % BITS_IN_INT))) != 0;
}
// получить бит
int s21_get_bit(int bits, int bit) {
  return s21_bit_is(bits, bit % BITS_IN_INT);
}
// получить int из decimal по номеру бита
int s21_get_bits(s21_decimal decim, int gbit) {
  return decim.bits[(gbit % (ALL_BIT + BITS_IN_INT)) / BITS_IN_INT];
}

int s21_get_global_bit(s21_decimal decim, int gbit) {
  return s21_get_bit(s21_get_bits(decim, gbit), gbit);
}
int s21_set_global_bit(s21_decimal *decim, int gbit, int new_bit) {
  return decim->bits[(gbit % (ALL_BIT + BITS_IN_INT)) / BITS_IN_INT] =
             s21_set_bit(s21_get_bits(*decim, gbit), gbit, new_bit);
}
//  возвращает знак 1 '-'; 0 '+'
int s21_check_sign(s21_decimal decim) {
  return s21_get_bit(decim.bits[INFO_BIT], SIGN_BIT);
}
//  возвращает новый измененный знак decimal
int s21_chang_sign(s21_decimal *decim) {
  if (s21_check_sign(*decim) == 1)
    decim->bits[INFO_BIT] = s21_bit_off(decim->bits[INFO_BIT], SIGN_BIT);
  else
    decim->bits[INFO_BIT] = s21_bit_on(decim->bits[INFO_BIT], SIGN_BIT);
  return s21_check_sign(*decim);
}

int s21_set_sign(s21_decimal *decim, int sign) {
  decim->bits[INFO_BIT] = s21_set_bit(decim->bits[INFO_BIT], SIGN_BIT, sign);
  return s21_check_sign(*decim);
}
//  Зануление всего decimal
s21_decimal s21_nullify_all_decimal(s21_decimal *decim) {
  for (int i = 0; i < BITS; i++) {
    decim->bits[i] = 0;
  }
  return *decim;
}
//  Меняет в bits бит под номером bit на new_bit
int s21_set_bit(int bits, int bit, int new_bit) {
  if (new_bit) {
    bits = s21_bit_on(bits, bit);
  } else {
    bits = s21_bit_off(bits, bit);
  }
  return bits;
}

// получение степени из decimal
int s21_get_exp(s21_decimal decim) {
  int exp = 0;
  for (int i = START_EXP_BIT; i <= END_EXP_BIT; i++) {
    if (s21_bit_is(decim.bits[INFO_BIT], i)) {
      exp += (1 << (i - START_EXP_BIT));
    }
  }
  return exp;
}

int s21_set_exp(s21_decimal *decim, int new_exp) {
  int sign = s21_check_sign(*decim);
  decim->bits[INFO_BIT] = new_exp << START_EXP_BIT;
  if (s21_check_sign(*decim) != sign) {
    s21_chang_sign(decim);
  }
  return decim->bits[INFO_BIT];
}

int s21_is_null_decimal(s21_decimal decim) {
  s21_decimal null_decimal = {{0, 0, 0, 0}};
  return s21_is_equal_lite(decim, null_decimal);
}

int s21_is_null_decimal_bin(s21_decimal decim) {
  s21_decimal null_decimal = {{0, 0, 0, 0}};
  int res_equal = 1;
  for (int i = 0; i < (ALL_BIT); i++) {
    if (s21_get_global_bit(decim, i) != s21_get_global_bit(null_decimal, i)) {
      res_equal = 0;
      break;
    }
  }
  return res_equal;
}

int s21_swift_bits_right(int bits, int number) {
  if (bits < 0) {
    bits = s21_bit_off(bits, SIGN_BIT);
    bits = bits >> number;
    bits = s21_bit_on(bits, SIGN_BIT - number);
  } else {
    bits = bits >> number;
  }
  return bits;
}

int s21_bit_swift_left(s21_decimal decim, int number, s21_decimal *result) {
  int error_mark = 0;
  s21_nullify_all_decimal(result);
  int buffer_bits = 0;
  result->bits[INFO_BIT] = decim.bits[INFO_BIT];

  for (int i = 0; i < BITS - 1; i++) {
    if (i != 0)
      buffer_bits = s21_swift_bits_right(
          decim.bits[i - 1], (BITS_IN_INT - (number % BITS_IN_INT)));
    if (i == 2)
      if (s21_swift_bits_right(decim.bits[i],
                               (BITS_IN_INT - (number % BITS_IN_INT))) != 0)
        error_mark = 1;
    result->bits[i] = decim.bits[i] << number;
    result->bits[i] = result->bits[i] | buffer_bits;
  }

  return error_mark;
}

s21_decimal s21_bit_swift_right(s21_decimal decim, int number) {
  s21_decimal result;
  s21_nullify_all_decimal(&result);
  int buffer_bits = 0;
  result.bits[INFO_BIT] = decim.bits[INFO_BIT];

  for (int i = BITS - 2; i >= 0; i--) {
    if (i != BITS - 2)
      buffer_bits = decim.bits[i + 1] << (BITS_IN_INT - (number % BITS_IN_INT));
    result.bits[i] = s21_swift_bits_right(decim.bits[i], number);
    result.bits[i] = result.bits[i] | buffer_bits;
  }

  return result;
}
// ^
s21_decimal s21_bit_exclusive_or(s21_decimal decim1, s21_decimal decim2) {
  s21_decimal result;
  s21_nullify_all_decimal(&result);
  for (int i = 0; i < BITS - 1; i++) {
    result.bits[i] = decim1.bits[i] ^ decim2.bits[i];
  }
  return result;
}
// &
s21_decimal s21_bit_and(s21_decimal decim1, s21_decimal decim2) {
  s21_decimal result;
  s21_nullify_all_decimal(&result);
  for (int i = 0; i < BITS - 1; i++) {
    result.bits[i] = decim1.bits[i] & decim2.bits[i];
  }
  return result;
}

int s21_truncate_to_exp(s21_decimal decim, int new_exp,
                        s21_decimal *result_decimal) {
  int exp = s21_get_exp(decim);
  if (exp > 0) {
    s21_set_exp(&decim, 0);
  }
  int sign = s21_check_sign(decim);
  if (sign == 1) {
    s21_chang_sign(&decim);
  }
  int error_mark = 0;
  s21_decimal ten = {{10, 0, 0, 0}};
  for (int i = exp; i > new_exp && error_mark == 0; i--) {
    error_mark = s21_div_lite(decim, ten, &decim);
  }
  *result_decimal = decim;
  if (sign == 1) {
    s21_chang_sign(result_decimal);
  }
  s21_set_exp(&decim, exp);
  return error_mark;
}