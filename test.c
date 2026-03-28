#include "xprcalc.h"
#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <criterion/redirect.h>

// Redirect stdout to capture printed output
static void redirect_all_std(void)
{
  cr_redirect_stdout();
  cr_redirect_stderr();
}

// --- BASIC OPERATIONS ---

Test(calculator_basic, addition, .init = redirect_all_std)
{
  evaluate("2+3");
  cr_assert_stdout_eq_str("Result: 5.000\n");
}

Test(calculator_basic, subtraction, .init = redirect_all_std)
{
  evaluate("10 - 4");
  cr_assert_stdout_eq_str("Result: 6.000\n");
}

Test(calculator_basic, multiplication, .init = redirect_all_std)
{
  evaluate("3*4");
  cr_assert_stdout_eq_str("Result: 12.000\n");
}

Test(calculator_basic, division, .init = redirect_all_std)
{
  evaluate("12 / 3");
  cr_assert_stdout_eq_str("Result: 4.000\n");
}

Test(calculator_basic, power, .init = redirect_all_std)
{
  evaluate("2^3");
  cr_assert_stdout_eq_str("Result: 8.000\n");
}

Test(calculator_basic, modulo, .init = redirect_all_std)
{
  evaluate("10 % 3");
  cr_assert_stdout_eq_str("Result: 1.000\n");
}

// --- ADVANCED EXPRESSIONS ---

Test(calculator_advanced, parentheses, .init = redirect_all_std)
{
  evaluate("(2 + 3) * 4");
  cr_assert_stdout_eq_str("Result: 20.000\n");
}

Test(calculator_advanced, nested_parentheses, .init = redirect_all_std)
{
  evaluate("((2+3)*2)^2");
  cr_assert_stdout_eq_str("Result: 100.000\n");
}
Test(calculator_basic, long_expression, .init = redirect_all_std)
{
  evaluate("(1+2)*(3+4)-(5-6)+(7*8)/(9+10)-(11%3)+(12^2)-(13+14)*(15-16)+(17*18)+(19/"
           "2)-(20%7)+(21+22)-(23-24)+(25*26)/(27+28)-(29-30)+(31^2)");
  cr_assert_stdout_eq_str("Result: 1521.266\n");
}
Test(calculator_basic, long_expression2, .init = redirect_all_std)
{
  evaluate(
      "(1+2)*(3+4)-(5-6)+(7*8)/(9+10)-(11%3)+(12^2)-(13+14)*(15-16)+(17*18)+(19/"
      "2)-(20%7)+(21+22)-(23-24)+(25*26)/(27+28)-(29-30)+(31^2)+(32/"
      "4)+(33*2)-(34^2)+(35%6)+(36+37)-(38-39)+(40/"
      "5)+(41*2)-(42^2)+(43%7)+(44+45)+(46*2)+(47^2)-(48/6)+(49*3)-(50/5)+(51^2)+(52+1+1+1)+1");
  cr_assert_stdout_eq_str("Result: 4021.266\n");
}
Test(calculator_funtions, function, .init = redirect_all_std)
{
  evaluate("sin(1.5)*cos(2.4)+log(10)+sqrt(8)");
  cr_assert_stdout_eq_str("Result: 3.093\n");
}

// --- LOGICAL EXPRESSIONS ---

Test(calculator_logical, less_than, .init = redirect_all_std)
{
  evaluate("3 < 5");
  cr_assert_stdout_eq_str("Result: true\n");
}

Test(calculator_logical, greater_than_false, .init = redirect_all_std)
{
  evaluate("3 > 10");
  cr_assert_stdout_eq_str("Result: false\n");
}

Test(calculator_logical, equal_true, .init = redirect_all_std)
{
  evaluate("7 == 7");
  cr_assert_stdout_eq_str("Result: true\n");
}

Test(calculator_logical, not_equal_true, .init = redirect_all_std)
{
  evaluate("8 != 9");
  cr_assert_stdout_eq_str("Result: true\n");
}

// --- VARIABLES ---

Test(calculator_variables, assign, .init = redirect_all_std)
{
  evaluate("a=1");
  cr_assert_stdout_eq_str("Result: 1.000\n");
}

// --- ERROR CASES ---

Test(calculator_errors, division_by_zero, .init = redirect_all_std)
{
  evaluate("5 / 0");
  cr_assert_stderr_eq_str("Error: division by zero\n");
}

Test(calculator_errors, modulo_by_zero, .init = redirect_all_std)
{
  evaluate("10 % 0");
  cr_assert_stderr_eq_str("Error: modulo by zero\n");
}

Test(calculator_errors, invalid_expression, .init = redirect_all_std)
{
  evaluate("3 + * 2");
  cr_assert_stderr_eq_str("Error: expected number or '('\n");
}

Test(calculator_errors, null_input, .init = redirect_all_std)
{
  evaluate(NULL);
  cr_assert_stderr_eq_str("Error: null input provided\n");
}
