# quarks

# Quarks Programming Language

A compiled programming language with a focus on simplicity and expressiveness.

## Features

- **Variables & Assignment**: Declare and assign variables with intuitive syntax
- **Control Flow**: Support for `if`, `elif`, and `else` statements
- **Arithmetic Operations**: Full support for mathematical expressions with proper operator precedence
- **Scoped Blocks**: Lexical scoping with curly braces
- **Exit Statements**: Explicit program termination with exit codes

## Language Syntax

### Variable Declaration and Assignment
```qs
assign x = 42;
y = x + 10;
```

### Control Flow
```qs
if (x > 0) {
    assign result = x * 2;
}

if (condition) {
    // then block
} elif (other_condition) {
    // elif block
} else {
    // else block
}
```

### Expressions
```qs
// Arithmetic with operator precedence
assign result = (10 + 5) * 2 - 3;

// Division and multiplication (precedence = 1)
assign div = 20 / 4;
assign mul = 5 * 3;

// Addition and subtraction (precedence = 0)
assign sum = 10 + 20;
assign diff = 50 - 15;
```

### Program Termination
```qs
exit(0);
```

## Grammar

The formal grammar for Quarks is defined as follows:

```
[prog] → [Statement]*

[Statement] → exit([Expression]);
            | assign identifier = [Expression];
            | identifier = [Expression];
            | if ([Expression]) [Scope] [IfPredicate]
            | {[Scope]*}

[Scope] → {[Statement]*}

[IfPredicate] → elif([Expression]) [Scope] [IfPredicate]
              | [Else]
              | ε

[Else] → else [Scope]
       | ε

[Expression] → [Term]
             | [BinaryExpression]

[BinaryExpression] → [Expression] * [Expression]  // precedence = 1
                   | [Expression] / [Expression]  // precedence = 1
                   | [Expression] + [Expression]  // precedence = 0
                   | [Expression] - [Expression]  // precedence = 0

[Term] → integer_literal
       | identifier
       | ([Expression])
```

## Building from Source

### Prerequisites

- **CMake** 3.25 or higher
- **Clang++** (C++20 compatible compiler)
- **Bash** (for build script)

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/yourusername/quarks.git
cd quarks
```

2. Create build directory:
```bash
mkdir -p build
```

3. Configure and build:
```bash
cmake -S . -B build
cmake --build build
```

Or use the provided build script:
```bash
chmod +x build.sh
./build.sh
```

### Project Structure

```
quarks/
├── src/           # Source files (.cpp)
├── include/       # Header files (.h, .hpp)
├── vendor/        # Third-party dependencies
├── sample/        # Example .qs programs
│   └── test.qs
├── build/         # Build output (generated)
│   └── bin/       # Compiled executable
├── CMakeLists.txt
├── build.sh       # Build script
└── README.md
```

## Usage

After building, run the Quarks compiler on a `.qs` source file:

```bash
./build/bin/quarks sample/test.qs
```

Or use the build script which automatically runs the test file:
```bash
./build.sh
```

## Example Program

```qs
// sample/test.qs
assign x = 10;
assign y = 20;

if (x > 5) {
    assign result = x + y;
}

exit(0);
```

## Development

### Compiler Phases

1. **Lexical Analysis**: Tokenization of source code
2. **Parsing**: Building an Abstract Syntax Tree (AST) according to the grammar
3. **Code Generation**: Generating target code from the AST

### Adding New Features

The modular design makes it easy to extend Quarks with new features:

- **Lexer**: Add new token types in the tokenizer
- **Parser**: Extend the grammar rules in the parser
- **Code Generator**: Implement code emission for new constructs

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is open source and available under the [MIT License](LICENSE).

## Roadmap

- [ ] Function declarations and calls
- [ ] Loops (while, for)
- [ ] String literals and operations
- [ ] Array/list support
- [ ] Standard library
- [ ] Better error messages with line/column information
- [ ] Optimization passes
- [ ] REPL (Read-Eval-Print Loop)

## Contact

Project Link: [https://github.com/yourusername/quarks](https://github.com/yourusername/quarks)

---

**Note**: Quarks is currently in active development. The language specification and implementation may change as the project evolves.
