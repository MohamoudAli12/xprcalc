# xprcalc

A lightweight expression calculator written in C99. It parses and evaluates arithmetic, power, modulo, logical, and comparison expressions, supports user-defined variables, and provides common math functions — all from an interactive REPL or programmatic API.

---

## Features

- Arithmetic: `+`, `-`, `*`, `/`, `%`, `^` (power)
- Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Logical operators: `&&`, `||`
- Variable assignment and lookup (e.g. `x = 5`, then `x + 3`)
- Built-in math functions: `sin`, `cos`, `tan`, `log`, `ln`, `sqrt`
- Parenthesised sub-expressions

---

## Building

```sh
gcc -o xprcalc main.c xprcalc.c -lm
```

---

## Usage

### Interactive REPL

```
$ ./xprcalc
Enter 'q' to quit
>>> 2 + 3 * 4
Result: 14.000
>>> x = 10
Result: 10.000
>>> x ^ 2
Result: 100.000
>>> sqrt(144)
Result: 12.000
>>> 5 == 5
Result: true
>>> q
```
## Supported Functions

| Function | Description               |
|----------|---------------------------|
| `sin(x)` | Sine (radians)            |
| `cos(x)` | Cosine (radians)          |
| `tan(x)` | Tangent (radians)         |
| `log(x)` | Base-10 logarithm         |
| `ln(x)`  | Natural logarithm         |
| `sqrt(x)`| Square root               |

---

## Operator Precedence

From lowest to highest:

| Precedence | Operators                          |
|------------|------------------------------------|
| 1          | `\|\|`                             |
| 2          | `&&`                               |
| 3          | `==` `!=` `<` `>` `<=` `>=`        |
| 4          | `+` `-`                            |
| 5          | `*` `/` `%`                        |
| 6          | `^` (right-associative)            |

---


## Known Limitations
- Expression calculator is stack based.
- Expression length is limited to `INPUT_BUFFER_SIZE` which is set to `256`
- Variables are stored in a fixed-size global table (`MAX_VARIABLES`) which is `256` variables.
