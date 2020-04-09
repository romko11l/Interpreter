#include <iostream>
#include <cstring>

enum type_of_lex  //кажется, тут далеко не полный список
{
    LEX_NULL,     // NULL
    LEX_PROGRAM,  // program
    LEX_ID,       // какой-то индентификатор
    LEX_INT,      //
    LEX_NUM,
    LEX_STRING,
    LEX_IF,       // if 
    LEX_ELSE,     // else
    LEX_PLUS,     // +
    LEX_MINUS,    // -
    LEX_WHILE,    // whille
    LEX_LPAREN,   // (
    LEX_RPAREN,   // )
    LEX_TIMES,    // *
    LEX_SLASH,    // / - деление
    LEX_LBRACE,   // {
    LEX_RBRACE,	  // }
    LEX_READ,     // read
    LEX_WRITE,    // write
    LEX_SEMICOLON,// ; 
    LEX_NOT,      // not
    LEX_EQUAL,    // =
    LEX_NEQUAL,   // !=
    LEX_AND,      // and
    LEX_OR,       // or
    LEX_GTR,      // > (greater)
    LEX_GEQ,      // >=
    LEX_LS,       // < (less)
    LEX_LEQ,      // <=
    LEX_EXCLMARK, // ! (Exclamation mark)
    LEX_FOR, 	  // for
    LEX_CONT,     // continue
    LEX_REAL,     // real (имя типа)
    LEX_COMMA     // .
    // непонятно, стоит ли делать " отдельной лексемой, если она всегда идёт только вместе со строкой 
};

// нужно сделать класс Ident - родительским - унаследовать от него строковый класс, класс, описывающий real, и класс, описывающий целые числа


int main ()
{
	return 0;
}