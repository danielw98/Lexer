#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_TKN_SIZE 256
#define NUM_KEYWORDS 32
#define NUM_OPERATORS 31
#define KEYWORD_MAX_SIZE 10
#define OPERATOR_MAX_SIZE 4

// all possible states
typedef enum State {
    ST_KEYWORD,
    ST_OPERATOR,
    ST_STRING_LITERAL,
    ST_CHAR_LITERAL,
    ST_INTEGER_CONSTANT,
    ST_FLOATING_POINT_CONSTANT,
    ST_COMMENT,
    ST_COMMENT_SINGLE_LINE,
    ST_COMMENT_MULTI_LINE,
    ST_IDENTIFIER,
    ST_PROCESS_DIGIT,
    ST_PROCESS_ALPHA,
    ST_PUNCTUATION,
    ST_PREPROCESSOR_DIRECTIVE,
    ST_INITIAL_STATE,
} State;

/*Test
Multi line*/

const char language_keywords[NUM_KEYWORDS][KEYWORD_MAX_SIZE] = {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while"
};

const char language_operators[NUM_OPERATORS][OPERATOR_MAX_SIZE] = {
  "!", 
  "|", 
  "^",  
  "&", 
  "~", 
  ">", 
  "<", 
  "=",
  "-",
  "+", 
  "*", 
  "/",
  "%%", 
  "==", 
  "+=", 
  "-=",
  "|=", 
  "&=", 
  "^=",
  "!=",
  "++",
  "<<",
  ">>",
  "--",
  "<=",
  ">=",
  "*=", 
  "/=",
  "%%=",
  "<<=", 
  ">>="
};

const char punctuation[] = "(){}[],.;:?";
const char operators_characters[] = "|&^~!<>=+-*/%%";

bool is_keyword(const char *str)
{
    for(int i = 0; i < 32; i++)
        if(strcmp(str, language_keywords[i]) == 0)
            return true;
    return false;
}

bool is_operator(const char *str)
{
    for(int i = 0; i < 32; i++)
        if(strcmp(str, language_operators[i]) == 0)
            return true;
    return false;
}

int lexer(FILE *fp)
{
    bool running = true;
    State currentState = ST_INITIAL_STATE;
    char currentChar;
    int numCharactersRead = 0;
    char currentToken[MAX_TKN_SIZE];
    while(running)
    {
        switch(currentState)
        {
            // initial state is reached at the beginning of the file,
            // or when the previous token was fully consumed
            case ST_INITIAL_STATE: {
                numCharactersRead = 0;
                memset(currentToken, 0, MAX_TKN_SIZE*sizeof(char));
                currentChar = (char) getc(fp);
                if(feof(fp))
                {
                    running = false;
                    break;
                }
                if(isspace(currentChar))
                    continue;

                if(currentChar == '\'') {
                    currentState = ST_CHAR_LITERAL;
                    continue;
                }
                if(currentChar == '\"') {
                    currentState = ST_STRING_LITERAL;
                    continue;
                }
                if(isdigit(currentChar))
                    currentState = ST_PROCESS_DIGIT;
                if(isalpha(currentChar))
                    currentState = ST_PROCESS_ALPHA;
                if(strchr(punctuation, currentChar))
                    currentState = ST_PUNCTUATION;
                if(strchr(operators_characters, currentChar))
                    currentState = ST_OPERATOR;
                if(currentChar == '#')
                    currentState = ST_PREPROCESSOR_DIRECTIVE;
                if(currentChar == '/')
                {
                    currentState = ST_COMMENT;
                    continue;
                }
                currentToken[numCharactersRead++] = currentChar;
                // printf("initial state %c\n", currentChar);
                break;
            }
            case ST_PREPROCESSOR_DIRECTIVE: {
                currentChar = (char) getc(fp);
                if(currentChar != '\n')
                    currentToken[numCharactersRead++] = currentChar;
                else
                {
                    printf("preproc directive: `%s`\n", currentToken);
                    currentState = ST_INITIAL_STATE;
                }
                break;
            }   
            case ST_KEYWORD: {
                printf("keyword: `%s`\n", currentToken);
                currentState = ST_INITIAL_STATE;
                break;
            }
            case ST_OPERATOR: {
                currentChar = (char) getc(fp);
                if(strchr(operators_characters, currentChar))
                    currentToken[numCharactersRead++] = currentChar;
                else {
                    if(!feof(fp))
                        fseek(fp, -1, SEEK_CUR);
                    printf("operator: `%s`\n", currentToken);
                    currentState = ST_INITIAL_STATE;
                }
                break;
            }
            case ST_PUNCTUATION: {
                printf("punctuation: `%s`\n", currentToken);
                currentState = ST_INITIAL_STATE;
                break;
            }
            case ST_STRING_LITERAL: {
                currentChar = (char) getc(fp);
                if(currentChar != '\"')
                    currentToken[numCharactersRead++] = currentChar;
                else {
                    currentState = ST_INITIAL_STATE;
                    currentToken[numCharactersRead++] = '\0';
                    printf("string constant: `%s`\n", currentToken);
                }
                break;
            }
            case ST_CHAR_LITERAL: {
                currentChar = (char) getc(fp);
                if(currentChar == '\\')
                {
                    currentToken[numCharactersRead++] = '\\';
                    currentChar = (char) getc(fp);
                    currentToken[numCharactersRead++] = currentChar;
                    printf("character constant: `%s`\n", currentToken);
                    currentState = ST_INITIAL_STATE;
                }
                else
                {
                    currentToken[numCharactersRead++] = currentChar;
                    currentState = ST_INITIAL_STATE;
                }
                break;
            }
            case ST_INTEGER_CONSTANT: {
                printf("integer constant: %s\n", currentToken);
                currentState = ST_INITIAL_STATE;
                break;
            }
            case ST_FLOATING_POINT_CONSTANT: {
                currentChar = getc(fp);
                if(isdigit(currentChar))
                    currentToken[numCharactersRead++] = currentChar;
                else
                {
                    // printf("Found %hhd\n", currentChar);
                    if(!feof(fp))
                        fseek(fp, -1, SEEK_CUR);
                    
                    currentToken[numCharactersRead] = '\0';
                    printf("floating point constant: `%s`\n", currentToken);
                    currentState = ST_INITIAL_STATE;
                }
                break;
            }
            case ST_COMMENT: {
                currentChar = getc(fp);
                if(currentChar == '/')
                    currentState = ST_COMMENT_SINGLE_LINE;
                if(currentChar == '*')
                    currentState = ST_COMMENT_MULTI_LINE;
                break;
            }
            case ST_COMMENT_SINGLE_LINE: {
                currentChar = getc(fp);
                if(currentChar != '\n')
                    currentToken[numCharactersRead++] = currentChar;
                else {
                    printf("single line comment: `%s`\n", currentToken);
                    currentState = ST_INITIAL_STATE;
                }
                break;
            }
            case ST_COMMENT_MULTI_LINE: {
                currentChar = getc(fp);
                if(currentChar != '*')
                    currentToken[numCharactersRead++] = currentChar;
                else {
                    currentChar = getc(fp);
                    if(currentChar == '/')
                    {
                        printf("Multi line comment: `%s`\n", currentToken);
                        currentState = ST_INITIAL_STATE;
                    }
                    else{
                        currentToken[numCharactersRead++] = '*';
                        fseek(fp, -1, SEEK_CUR);
                    }
                }
                break;
            }
            case ST_IDENTIFIER: {
                printf("identifier: `%s`\n", currentToken);
                currentState = ST_INITIAL_STATE;
                break;
            }
            case ST_PROCESS_DIGIT: {  
                // printf("Process digit\n");
                currentChar = getc(fp);
                if(isdigit(currentChar))
                    currentToken[numCharactersRead++] = currentChar;
                else if(currentChar == '.') {
                    currentToken[numCharactersRead++] = currentChar;
                    currentState = ST_FLOATING_POINT_CONSTANT;
                }
                else {
                    if(!feof(fp))
                        fseek(fp, -1, SEEK_CUR);
                    currentState = ST_INTEGER_CONSTANT;
                }
                break;
            }
            case ST_PROCESS_ALPHA: {
                currentChar = getc(fp);
                if(currentChar != '_' && !isalnum(currentChar)) {
                    if(!feof(fp))
                        fseek(fp, -1, SEEK_CUR);
                    if(is_keyword(currentToken))
                        currentState = ST_KEYWORD;
                    else
                        currentState = ST_IDENTIFIER;
                }
                else {
                    currentToken[numCharactersRead++] = currentChar;
                }
                break;
            }
        }
    }
    return 0;
}

int lexer_init(const char *file_path)
{
    FILE *fp = fopen(file_path, "r");
    if(fp == NULL)
    {
        perror(file_path);
        return -1;
    }

    int retVal = lexer(fp);

    fclose(fp);

    return retVal;
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if(lexer_init(argv[1]) == -1)
    {
        fprintf(stderr, "Error while running the lexer\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}