# Lexer

A comprehensive Lexer written in C, designed to tokenize a variety of elements from C language source files, including keywords, operators, identifiers, and more. This tool efficiently processes source code to output tokens, aiding in the understanding and analysis of the code structure.

## Features

- Tokenization of keywords, operators, string literals, and more.
- Efficient parsing of C language syntax.
- Output of tokens to output.txt for easy analysis.

## Demo Output
![Lexer Output](https://github.com/danielw98/Lexer/blob/main/Lexer.png?raw=true)

## Getting Started

Clone this repository to get started with the Lexer:

```bash
git clone https://github.com/DanielW98/Lexer.git
```

To compile the Lexer, navigate to the project directory and run:

```bash
gcc lexer.c -o lexer
```

Then, to tokenize a source file:

```bash
./lexer source_file.c
```

The tokens will be output to output.txt.

## Dive Into the Code

`lexer.c` contains the core logic for tokenizing the source code. It implements a finite state machine to distinguish between different types of tokens and uses a switch-case structure to handle the transitions between states.

## Output and Analysis

The output in `output.txt` includes categorized tokens with detailed information, providing insights into the parsed source file's structure and elements.

## Contribute
Your contributions are welcome! If you have suggestions for improvements or want to add features, please fork the repository, make your changes, and submit a pull request.

## License
This project is licensed under the MIT License. For more details, see the LICENSE file.

