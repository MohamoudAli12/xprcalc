#include "xprcalc.h"
#include <ctype.h>
#include <math.h>
#include <string.h>

variable_t var_table[MAX_VARIABLES] = { 0 };

void skip_whitespace(parser_t *p)
{
  while (p->input[p->pos] && isspace(p->input[p->pos]))
  {
    p->pos++;
  }
}

int create_variable(const char *name)
{
  for (int i = 0; i < MAX_VARIABLES; i++)
  {
    if (!var_table[i].in_use)
    {
      var_table[i].in_use = true;
      strncpy(var_table[i].name, name, MAX_IDENTIFIER_NAME);
      return i;
    }
  }
  return -1;
}

int find_variable(const char *name)
{
  for (int i = 0; i < MAX_VARIABLES; i++)
  {
    if (var_table[i].in_use && strcmp(var_table[i].name, name) == 0)
      return i;
  }
  return -1;
}

int get_user_input(void)
{
  char input[INPUT_BUFFER_SIZE] = { 0 };
  printf("Enter 'q' to quit\n");
  while (1)
  {
    printf(">>>");
    if (!fgets(input, sizeof(input), stdin))
    {
      return -1;
      break;
    }
    if (input[0] == '\0' || input[0] == '\n')
    {
      continue;
    }
    if (input[0] == 'q' && input[1] == '\n')
    {
      break;
    }

    evaluate(input);
  }
  return 0;
}

void evaluate(const char *input)
{
  if (!input)
  {
    error_handler(ERROR_NULL_POINTER);
    fflush(stdout);
    return;
  }

  parser_t parser = { 0 };
  parser.input = input;
  parser.pos = 0;
  parser.has_error = false;
  parser.is_logical_operator = false;
  parser.error_code = ERROR_NONE;

  advance(&parser);

  double result = parse_expression(&parser, 0);

  if (parser.has_error)
  {
    error_handler(parser.error_code);
    fflush(stdout);
    return;
  }

  if (parser.current_token.type != TOKEN_EOF)
  {
    error_handler(ERROR_INVALID_TOKEN);
    fflush(stdout);
    return;
  }
  if (parser.is_logical_operator == true)
  {
    printf("Result: %s\n", result == 1.0 ? "true" : "false");
    fflush(stdout);
    return;
  }
  else
  {
    printf("Result: %.3f\n", result);
    fflush(stdout);
  }
}

void advance(parser_t *p)
{
  p->current_token = get_next_token(p);
}

token_t get_next_token(parser_t *p)
{
  skip_whitespace(p);

  char c = p->input[p->pos];

  if (c == '\0')
  {
    return (token_t) { .type = TOKEN_EOF, .value = 0, .identifier = { 0 } };
  }

  if (isalpha(p->input[p->pos]))
  {
    return parse_identifier(p);
  }

  // Numeric tokens
  if (isdigit(c) || c == '.')
  {
    double value = parse_number(p);
    if (p->has_error)
    {
      p->error_code = ERROR_PARSER;
      return (token_t) { .type = TOKEN_INVALID, .value = 0, .identifier = { 0 } };
    }
    return (token_t) { .type = TOKEN_NUMBER, .value = value, .identifier = { 0 } };
  }

  token_t boolean_operator = parse_boolean_op_token(p);
  if (boolean_operator.type != TOKEN_INVALID)
  {
    return boolean_operator;
  }

  p->pos++;
  return parse_arithmetic_op_token(c);
}
double parse_expression(parser_t *p, int min_precedence)
{
  if (p->has_error)
    return -1;

  if (p->current_token.type == TOKEN_VARIABLE)
  {
    token_t ident = p->current_token;
    size_t saved_pos = p->pos;

    advance(p);

    if (p->current_token.type == TOKEN_ASSIGN)
    {
      advance(p);

      double value = parse_expression(p, 0);
      if (p->has_error)
        return -1;

      int idx = find_variable(ident.identifier);
      if (idx == -1)
        idx = create_variable(ident.identifier);

      if (idx == -1)
      {
        p->has_error = true;
        return -1;
      }

      var_table[idx].value = value;
      return value;
    }

    p->pos = saved_pos;
    p->current_token = ident;
  }

  double left = parse_primary(p);
  if (p->has_error)
    return -1;

  while (!p->has_error && get_precedence(p->current_token.type) > min_precedence)
  {
    token_type_t op = p->current_token.type;
    int precedence = get_precedence(op);

    advance(p);

    double right = parse_expression(p, (op == TOKEN_POWER) ? precedence - 1 : precedence);

    if (p->has_error)
      return -1;

    left = evaluate_operation(p, left, op, right);
  }

  return left;
}

int get_precedence(token_type_t type)
{
  switch (type)
  {
  case TOKEN_ASSIGN:
    return 0;
  case TOKEN_LOGICAL_OR:
    return 1;
  case TOKEN_LOGICAL_AND:
    return 2;
  case TOKEN_EQUAL:
  case TOKEN_NOT_EQUAL:
  case TOKEN_LESS_THAN:
  case TOKEN_GREATER_THAN:
  case TOKEN_LESS_EQUAL:
  case TOKEN_GREATER_EQUAL:
    return 3;
  case TOKEN_PLUS:
  case TOKEN_MINUS:
    return 4;
  case TOKEN_MULTIPLY:
  case TOKEN_DIVIDE:
  case TOKEN_MODULO:
    return 5;
  case TOKEN_POWER:
    return 6;
  default:
    return 0;
  }
}

double parse_primary(parser_t *p)
{
  if (p->has_error)
  {
    return -1;
  }

  if (p->current_token.type == TOKEN_FUNCTION)
  {
    return evaluate_function(p);
  }

  if (p->current_token.type == TOKEN_VARIABLE)
  {
    char name[MAX_IDENTIFIER_NAME];
    strncpy(name, p->current_token.identifier, sizeof(name));

    advance(p);

    int idx = find_variable(name);
    if (idx == -1)
    {
      p->has_error = true;
      p->error_code = ERROR_UNKNOWN_VARIABLE;
      return 0;
    }

    return var_table[idx].value;
  }

  if (p->current_token.type == TOKEN_NUMBER)
  {
    double value = p->current_token.value;
    advance(p);
    return value;
  }

  if (p->current_token.type == TOKEN_MINUS)
  {
    advance(p);
    double val = parse_primary(p);
    return -val;
  }
  /* the condition (&& ((p->pos - 1) == 0))(p->current_token.type == TOKEN_PLUS) && ((p->pos - 1) ==
  0)) is to allow +1+2 and not allow +1++2. BUG using the condition +1++2 doesn't work but also
  +1+(+1) doesn't work
  */
  if ((p->current_token.type == TOKEN_PLUS) && ((p->pos - 1) == 0))
  {
    advance(p);
    double val = parse_primary(p);
    return val;
  }

  if (p->current_token.type == TOKEN_LPAREN)
  {
    advance(p);
    double val = parse_expression(p, 0);
    if (p->current_token.type != TOKEN_RPAREN)
    {

      p->has_error = true;
      p->error_code = ERROR_EXPECTED_RPAREN;
      return -1;
    }
    advance(p);
    return val;
  }

  if (p->current_token.type == TOKEN_INVALID)
  {
    p->error_code = ERROR_INVALID_TOKEN;
  }
  else
  {
    p->error_code = ERROR_EXPECTED_NUMBER_OR_LPAREN;
  }

  p->has_error = true;
  return -1;
}

double parse_number(parser_t *p)
{
  double result = 0;
  bool has_dot = false;
  double frac = 0.1;
  size_t start_pos = p->pos;

  while (isdigit(p->input[p->pos]))
  {
    result = result * 10 + (p->input[p->pos] - '0');
    p->pos++;
  }

  if (p->input[p->pos] == '.')
  {
    has_dot = true;
    p->pos++;
    while (isdigit(p->input[p->pos]))
    {
      result += (p->input[p->pos] - '0') * frac;
      frac *= 0.1;
      p->pos++;
    }
  }

  if (p->pos == start_pos || (p->pos == start_pos + 1 && has_dot))
  {
    p->has_error = true;
    p->error_code = ERROR_PARSER;
    return -1;
  }

  return result;
}

token_t parse_identifier(parser_t *p)
{
  char name[MAX_IDENTIFIER_NAME] = { 0 };
  int i = 0;

  while (isalnum(p->input[p->pos]) && i < MAX_IDENTIFIER_NAME - 1)
  {
    name[i++] = p->input[p->pos++];
  }

  token_t token = { 0 };

  // Peek: function if next char is '('
  if (p->input[p->pos] == '(')
  {
    token.type = TOKEN_FUNCTION;
    strncpy(token.identifier, name, sizeof(token.identifier));
  }
  else
  {
    token.type = TOKEN_VARIABLE;
    strncpy(token.identifier, name, sizeof(token.identifier));
  }

  return token;
}
double evaluate_function(parser_t *p)
{
  char function_name[10];
  strncpy(function_name, p->current_token.identifier, sizeof(function_name));
  advance(p);
  if (p->current_token.type != TOKEN_LPAREN)
  {
    p->has_error = true;
    p->error_code = ERROR_EXPECTED_NUMBER_OR_LPAREN;
    return 0;
  }
  advance(p);
  double arg = parse_expression(p, 0);
  if (p->current_token.type != TOKEN_RPAREN)
  {
    p->has_error = true;
    p->error_code = ERROR_EXPECTED_RPAREN;
    return 0;
  }
  advance(p);
  if (strncmp(function_name, "sin", sizeof(function_name)) == 0)
  {
    return sin(arg);
  }
  else if (strncmp(function_name, "cos", sizeof(function_name)) == 0)
  {
    return cos(arg);
  }
  else if (strncmp(function_name, "tan", sizeof(function_name)) == 0)
  {
    return tan(arg);
  }
  else if (strncmp(function_name, "log", sizeof(function_name)) == 0)
  {
    return log10(arg);
  }
  else if (strncmp(function_name, "ln", sizeof(function_name)) == 0)
  {
    return log(arg);
  }
  else if (strncmp(function_name, "sqrt", sizeof(function_name)) == 0)
  {
    return sqrt(arg);
  }

  else
  {
    p->has_error = true;
    p->error_code = ERROR_UNKNOWN_FUNCTION;
    return 0;
  }
}
token_t parse_arithmetic_op_token(const char c)
{
  switch (c)
  {
  case '+':
    return (token_t) { .type = TOKEN_PLUS, .value = 0, .identifier = { 0 } };
  case '-':
    return (token_t) { .type = TOKEN_MINUS, .value = 0, .identifier = { 0 } };
  case '*':
    return (token_t) { .type = TOKEN_MULTIPLY, .value = 0, .identifier = { 0 } };
  case '/':
    return (token_t) { .type = TOKEN_DIVIDE, .value = 0, .identifier = { 0 } };
  case '(':
    return (token_t) { .type = TOKEN_LPAREN, .value = 0, .identifier = { 0 } };
  case ')':
    return (token_t) { .type = TOKEN_RPAREN, .value = 0, .identifier = { 0 } };
  case '^':
    return (token_t) { .type = TOKEN_POWER, .value = 0, .identifier = { 0 } };
  case '%':
    return (token_t) { .type = TOKEN_MODULO, .value = 0, .identifier = { 0 } };
  default:
    return (token_t) { .type = TOKEN_INVALID, .value = 0, .identifier = { 0 } };
  }
}
token_t parse_boolean_op_token(parser_t *p)
{
  char c = p->input[p->pos];
  char next_c = p->input[p->pos + 1];
  if (c == '<')
  {
    p->pos += 1;
    return (token_t) { .type = TOKEN_LESS_THAN, .value = 0, .identifier = { 0 } };
  }
  if (c == '>')
  {
    p->pos += 1;
    return (token_t) { .type = TOKEN_GREATER_THAN, .value = 0, .identifier = { 0 } };
  }

  if (c == '=' && next_c == '=')
  {
    p->pos += 2;
    return (token_t) { .type = TOKEN_EQUAL, .value = 0, .identifier = { 0 } };
  }
  if (c == '=' && next_c != '=')
  {
    p->pos += 1;
    return (token_t) { .type = TOKEN_ASSIGN, .value = 0, .identifier = { 0 } };
  }
  if (c == '!' && next_c == '=')
  {
    p->pos += 2;
    return (token_t) { .type = TOKEN_NOT_EQUAL, .value = 0, .identifier = { 0 } };
  }
  if (c == '<' && next_c == '=')
  {
    p->pos += 2;
    return (token_t) { .type = TOKEN_LESS_EQUAL, .value = 0, .identifier = { 0 } };
  }
  if (c == '>' && next_c == '=')
  {
    p->pos += 2;
    return (token_t) { .type = TOKEN_GREATER_EQUAL, .value = 0, .identifier = { 0 } };
  }

  return (token_t) { .type = TOKEN_INVALID, .value = 0, .identifier = { 0 } }; // If not matched
}

double evaluate_operation(parser_t *p, double left, token_type_t op, double right)
{
  double result = 0;
  switch (op)
  {
  case TOKEN_PLUS:
    result = left + right;
    break;
  case TOKEN_MINUS:
    result = left - right;
    break;
  case TOKEN_MULTIPLY:
    result = left * right;
    break;
  case TOKEN_POWER:
    result = pow(left, right);
    break;
  case TOKEN_DIVIDE:
    if (right == 0)
    {
      p->has_error = true;
      p->error_code = ERROR_DIVISION_BY_ZERO;
      break;
    }
    result = left / right;
    break;
  case TOKEN_MODULO:
    if (right == 0)
    {
      p->has_error = true;
      p->error_code = ERROR_MODULO_BY_ZERO;
      break;
    }
    result = (int)left % (int)right;
    break;
  case TOKEN_EQUAL:
    p->is_logical_operator = true;
    return (left == right) ? 1 : 0;

    break;
  case TOKEN_NOT_EQUAL:
    p->is_logical_operator = true;
    return (left != right) ? 1 : 0;
    break;
  case TOKEN_LESS_THAN:
    p->is_logical_operator = true;
    return (left < right) ? 1 : 0;
    break;
  case TOKEN_GREATER_THAN:
    p->is_logical_operator = true;
    return (left > right) ? 1 : 0;
    break;
  case TOKEN_LESS_EQUAL:
    p->is_logical_operator = true;
    return (left <= right) ? 1 : 0;
    break;
  case TOKEN_GREATER_EQUAL:
    p->is_logical_operator = true;
    return (left >= right) ? 1 : 0;
    break;
  default:
    p->has_error = true;
    p->error_code = ERROR_UNKNOWN_OPERATION;
    break;
  }
  return result;
}
void error_handler(error_t error)
{
  switch (error)
  {
  case ERROR_NONE:
    break;
  case ERROR_NULL_POINTER:
    fprintf(stderr, "Error: null input provided\n");
    break;
  case ERROR_INVALID_TOKEN:
    fprintf(stderr, "Error: unexpected token in expression\n");
    break;
  case ERROR_PARSER:
    fprintf(stderr, "Error: syntax error in expression\n");
    break;
  case ERROR_DIVISION_BY_ZERO:
    fprintf(stderr, "Error: division by zero\n");
    break;
  case ERROR_MODULO_BY_ZERO:
    fprintf(stderr, "Error: modulo by zero\n");
    break;
  case ERROR_EXPECTED_RPAREN:
    fprintf(stderr, "Error: expected ')'\n");
    break;
  case ERROR_EXPECTED_NUMBER_OR_LPAREN:
    fprintf(stderr, "Error: expected number or '('\n");
    break;
  case ERROR_UNKNOWN_OPERATION:
    fprintf(stderr, "Error: unknown operation\n");
    break;
  case ERROR_UNKNOWN_FUNCTION:
    fprintf(stderr, "Error: unknown function\n");
    break;
  default:
    fprintf(stderr, "Error: unknown error occurred\n");
    break;
  }
}
