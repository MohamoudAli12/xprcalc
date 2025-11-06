#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <stdbool.h>
#include <stdio.h>

#define INPUT_BUFFER_SIZE 256
#define RESULT_OK 0

typedef enum
{
  ERROR_NONE = 0,
  ERROR_USER_INPUT,
  ERROR_NULL_POINTER,
  ERROR_PARSER,
  ERROR_EXPECTED_RPAREN,
  ERROR_EXPECTED_NUMBER_OR_LPAREN,
  ERROR_EMPTY_INPUT,
  ERROR_INVALID_TOKEN,
  ERROR_DIVISION_BY_ZERO,
  ERROR_MODULO_BY_ZERO,
  ERROR_UNKNOWN_OPERATION,
  ERROR_UNKNOWN_FUNCTION,
  ERROR_NOT_INITIALIZED
} error_t;

typedef enum
{
  TOKEN_EOF,
  TOKEN_LOGICAL_OR,
  TOKEN_LOGICAL_AND,
  TOKEN_EQUAL,
  TOKEN_NOT_EQUAL,
  TOKEN_LESS_THAN,
  TOKEN_GREATER_THAN,
  TOKEN_LESS_EQUAL,
  TOKEN_GREATER_EQUAL,
  TOKEN_NUMBER,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_MULTIPLY,
  TOKEN_DIVIDE,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_POWER,
  TOKEN_MODULO,
  TOKEN_FUNCTION,
  TOKEN_INVALID
} token_type_t;

typedef struct
{
  token_type_t type;
  double value; // only numbers will have a value
  char functions[10];
} token_t;

typedef struct
{
  const char *input;
  size_t pos;
  token_t current_token;
  error_t error_code;
  bool has_error;
  bool is_logical_operator;
} parser_t;

// function prototypes

void skip_whitespace(parser_t *p);
double parse_number(parser_t *p);
token_t parse_function(parser_t *p);
token_t parse_boolean_op_token(parser_t *p);
token_t parse_arithmetic_op_token(const char c);
token_t get_next_token(parser_t *p);
void advance(parser_t *p);
int get_precedence(token_type_t type);
double parse_primary(parser_t *p);
double evaluate_function(parser_t *p);
double evaluate_operation(parser_t *p, double left, token_type_t op, double right);
double parse_expression(parser_t *p, int min_precedence);
void evaluate(const char *input);
void error_handler(error_t error);
int get_user_input(void);

#endif
