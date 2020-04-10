#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cmath>

enum type_of_lex  //кажется, тут далеко не полный список
{
    LEX_NULL,     // 
    LEX_PROGRAM,  // program
    LEX_ID,       // какой-то индентификатор
    LEX_INT,      // int (имя типа)
    LEX_CINT,     // целочисленная константа
    LEX_CSTRING,  // строковая коснтанта 
    LEX_CREAL,    // вещественная константа
    LEX_STRING,   // string (имя типа)
    LEX_IF,       // if 
    LEX_ELSE,     // else
    LEX_PLUS,     // +
    LEX_MINUS,    // -
    LEX_WHILE,    // while
    LEX_LPAREN,   // (
    LEX_RPAREN,   // )
    LEX_TIMES,    // *
    LEX_SLASH,    // / - деление (ну либо часть комментария)
    LEX_LBRACE,   // {
    LEX_RBRACE,	  // }
    LEX_READ,     // read
    LEX_WRITE,    // write
    LEX_SEMICOLON,// ; 
    LEX_NOT,      // not
    LEX_EQUAL,    // =
    LEX_NEQUAL,   // !=
   	LEX_DEQUAL,   // ==
    LEX_AND,      // and
    LEX_OR,       // or
    LEX_GTR,      // > (greater)
    LEX_GEQ,      // >=
    LEX_LS,       // < (less)
    LEX_LEQ,      // <=
    LEX_FOR, 	  // for
    LEX_CONT,     // continue
    LEX_REAL,     // real (имя типа)
    LEX_POINT,    // . (нужно ли? Ведь она может быть только в составе вещественного числа)
    LEX_COMMA,    // ,
    LEX_OPENCOM,  // /*  Вроде бы не нужно, т.к. мы их пропускаем
    LEX_CLOSECOM, // */  Вроде бы не нужно, т.к. мы их пропускаем
    LEX_LITERAL,  // запись вида "abc"
    LEX_FIN       // @
    // непонятно, стоит ли делать " отдельной лексемой, если она всегда идёт только вместе со строкой 
};

// нужно сделать класс Ident - родительским - унаследовать от него строковый класс, класс, описывающий real, и класс, описывающий целые числа

class Lex
{
	type_of_lex t_lex;
	int v_lex; // показывает, каким номером лексема лежит в соответствующей таблице (если не лежит ни в какой таблице - 0)
public:
	Lex (type_of_lex t = LEX_NULL, int v=0)
    {
        t_lex=t;
        v_lex=v;
    }
    type_of_lex get_type ()
    {
        return t_lex;
    }
    int get_value ()
    {
        return v_lex;
    }
    friend std::ostream& operator << (std::ostream &s, Lex l)
    {
        s << '(' << l.t_lex << ',' << l.v_lex << ");";
        return s;
    }
};

class Ident
{
	char * name;
	type_of_lex type;
public:
	char *get_name ()
	{
		return name;
	}	
	void put_name (const char * n)
	{
		name=new char [strlen(n)+1];
		strcpy(name, n);
	}
	type_of_lex get_type ()
	{
		return type;
	}
	void put_type (type_of_lex t)
	{
		type=t;
	}
};

class tabl_ident
{
	Ident * table;
	int size;
	int top;
public:
	tabl_ident (int max_size)	
	{
		table=new Ident[size=max_size];
		top=1;  // почему не 0?
	}
	~tabl_ident ()
	{
		delete []table;
	}
	Ident& operator[] (int k)
	{
		return table[k];
	}
	int put (const char *buf);	
};

int tabl_ident::put (const char *buf)
{
	for (int j=1; j<top; ++j)
	{
		if (!strcmp(buf, table[j].get_name()))
		{
			return j;
		}
	}
	table[top].put_name(buf);
	++top;
	return top-1;
}

tabl_ident TID(100); //таблица идентификаторов (можно было бы сделать вектор вместо таблицы, но зачем?)

class Scanner
{
	enum state
	{
		H, // начальное состояние
		IDENT, // состояние - идентификатор
		NUMB, // состояние - целое число
		ALE, // (above, less, equal) состояние - сравнения
		COMMENT, // состояние - комментария
		DELIM, // delimiters, состояние - разделителей
		EXCLMARK,
		EQUAL,
		REALNUM,
		SLASH,
		STR
	};
	static const char * TW[];   //таблица служебных слов
	static type_of_lex words[];
	static const char * TD[];   //таблица ограничителей
	static type_of_lex dlms[];
	state CS;
	FILE * fp;
	char c;
	char buf[80];
	int buf_top;

	void clear()
	{
		buf_top=0;
		for (int j=0; j<80; ++j)
		{
			buf[j]='\0';
		}
	}
	void add ()
	{
		buf[buf_top++]=c;
	}
	int look (const char* buf, const char **list)
	{
		int i=0;
		while (list[i])
		{
			if (!strcmp(buf,list[i]))
			{
			return i;
			}
			++i;
		}
		return 0;
	}
	void gc ()
	{
		c=fgetc(fp);
	}
public:
	Scanner (const char * program)
	{
		fp=fopen(program,"r");
		CS=H;
		clear();
		gc();
	}	
	~Scanner ()
	{
		fclose(fp);
	}
	Lex get_lex();
};

const char * Scanner::TW[] = 
{	
	"",       //позиция 0 не используется
	"string",
	"int",
	"real",
	"if",
	"else",
	"for",
	"while",
	"read",
	"write",
	"not",
	"and",
	"or",
	"program",
	"continue",
	NULL
};

const char * Scanner::TD[] = //нужно переделать так, чтобы порядок совпадал с type_of_lex Scanner::dlms[]
{
	"",   
	"+",  
	"-",  
	"(",  
	")",
	"*",  
	"/", 
	"{",  
	"}",  
	";",  
	"=",  
	"!=",  
	"==",  
	">",  
	">=",  
	"<",  
	"<=", 
	",", 
	"@", 
	NULL
};

type_of_lex Scanner::words[] =
{	
	LEX_NULL,  
	LEX_STRING,
	LEX_INT,
	LEX_REAL,
	LEX_IF,
	LEX_ELSE,
	LEX_FOR,
	LEX_WHILE,
	LEX_READ,
	LEX_WRITE,
	LEX_NOT,
	LEX_AND,
	LEX_OR,
	LEX_PROGRAM,
	LEX_CONT,
	LEX_NULL
};

type_of_lex Scanner::dlms[] =
{
	LEX_NULL,
	LEX_PLUS,
	LEX_MINUS,
	LEX_LPAREN,
	LEX_RPAREN,
	LEX_TIMES,
	LEX_SLASH,
	LEX_LBRACE,
	LEX_RBRACE,
	LEX_SEMICOLON,
	LEX_EQUAL,
	LEX_NEQUAL,
	LEX_DEQUAL,
	LEX_GTR,
	LEX_GEQ,
	LEX_LS,
	LEX_LEQ,
	LEX_COMMA,
	LEX_FIN,
	LEX_NULL
};

Lex Scanner::get_lex ()
{
	int lex_pos;
	int integer;
	double real;
	CS=H;
	do  //первое чтение осуществлено в конструкторе Scanner
	{
		switch (CS)
		{
			case H:
			{
				if (c==' '||c=='\n')
				{
					gc();
				}
				else if (isalpha(c)||c=='_')
				{
					clear();
					add();
					gc();
					CS=IDENT;
				}
				else if (isdigit(c))
				{
					integer=c-'0';
					gc();
					CS=NUMB;
				}
				break;
			}
			case IDENT:
			{
				if (isalpha(c)||(isdigit(c))||c=='_')
				{
					add();
					gc();
				}	
				else
				{
					lex_pos=look(buf,TW);
					if (lex_pos==0)
					{
						lex_pos=TID.put(buf);
						return Lex(LEX_ID,lex_pos);
					}
					else
					{
						return Lex(words[lex_pos],lex_pos);
					}
				}
				break;
			}
			case NUMB:
			{
				if (isdigit(c))
				{
					integer=integer*10+c-'0';
					gc();
				}
				else if (c=='.')
				{
					gc();
					real=integer;
					CS=REALNUM;
				}
				else 
				{
					return Lex(LEX_CINT,0);
				}
				break;
			}
			case REALNUM:
			{
				int exp=-1;
				if (isdigit(c))
				{
					real=real+(c-'0')*pow(10,exp--);
					gc();
				}
				else
				{
					return Lex(LEX_CREAL,0);
				}
				break;
			}	
		}
	} 
	while (true);
}

int main ()
{
	char c=EOF;
	Scanner scan("./prog");
	std::cout << scan.get_lex() << std::endl;
	return 0;
}