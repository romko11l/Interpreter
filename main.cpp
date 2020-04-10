#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cmath>

enum type_of_lex  //кажется, тут далеко не полный список
{
    LEX_NULL,     // //0
    LEX_PROGRAM,  // program //1
    LEX_ID,       // какой-то индентификатор //2
    LEX_INT,      // int (имя типа) //3
    LEX_CINT,     // целочисленная константа //4
    LEX_CSTRING,  // строковая коснтанта //5
    LEX_CREAL,    // вещественная константа //6
    LEX_STRING,   // string (имя типа) //7
    LEX_IF,       // if //8
    LEX_ELSE,     // else //9
    LEX_PLUS,     // + //10
    LEX_MINUS,    // - //11
    LEX_WHILE,    // while //12
    LEX_LPAREN,   // ( //13
    LEX_RPAREN,   // ) //14
    LEX_TIMES,    // * //15
    LEX_SLASH,    // / - деление (ну либо часть комментария) //16
    LEX_LBRACE,   // { //17
    LEX_RBRACE,	  // } //18
    LEX_READ,     // read //19
    LEX_WRITE,    // write //20
    LEX_SEMICOLON,// ; //21
    LEX_NOT,      // not //22
    LEX_EQUAL,    // = //23
    LEX_NEQUAL,   // != //24
   	LEX_DEQUAL,   // == //25
    LEX_AND,      // and //26
    LEX_OR,       // or //27
    LEX_GTR,      // > (greater) //28
    LEX_GEQ,      // >= //29
    LEX_LS,       // < (less) //30
    LEX_LEQ,      // <= //31
    LEX_FOR, 	  // for //32
    LEX_CONT,     // continue //33
    LEX_REAL,     // real (имя типа) //34
    LEX_COMMA,    // , //35
    LEX_FIN       // @ //36
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
	bool is_used;
public:
	Ident ()
	{
		is_used=false;
	}
	~Ident ()
	{
		if (is_used==true)
		{
			delete []name;
		}
	}
	char *get_name ()
	{
		return name;
	}	
	void put_name (const char * n)
	{
		is_used=true;
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
	int exp=-1;
	CS=H;
	do  //первое чтение осуществлено в конструкторе Scanner
	{
		switch (CS)
		{
			case H:
			{
				if (c==' '||c=='\n'||c=='\t')
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
				else if (c=='<'||c=='>')
				{
					clear();
					add();
					gc();
					CS=ALE;
				}
				else if (c=='!')
				{
					clear();
					add();
					gc();
					CS=EXCLMARK;
				}
				else if (c=='=')
				{
					clear();
					add();
					gc();
					CS=EQUAL;
				}
				else if (c==EOF)
				{
					return Lex(LEX_FIN,0);
				}
				else if (c=='/')
				{
					clear();
					add();
					gc();
					CS=SLASH;
				}
				else if (c=='"')
				{
					clear();
					add();
					gc();
					CS=STR;
				}
				else
				{
					CS=DELIM;
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
			case ALE:
			{
				if (c=='=')
				{
					add();
					gc();
					lex_pos=look(buf,TD);
					return Lex(dlms[lex_pos],lex_pos);
				}
				else
				{
					lex_pos=look(buf,TD);
					return Lex(dlms[lex_pos],lex_pos);
				}
				break;
			}
			case EXCLMARK:
			{
				if (c=='=')
				{
					add();
					gc();
					lex_pos=look(buf,TD);
					return Lex(dlms[lex_pos],lex_pos);
				}
				else
				{
					throw ("EXCLMARK:ERR");
				}
				break;
			}
			case EQUAL:
			{
				if (c=='=')
				{
					add();
					gc();
					lex_pos=look(buf,TD);
					return Lex(dlms[lex_pos],lex_pos);
				}
				else
				{
					lex_pos=look(buf,TD);
					return Lex(dlms[lex_pos],lex_pos);
				}
				break;
			}
			case SLASH:
			{
				if (c=='*')
				{
					gc();
					CS=COMMENT;
				}
				else
				{
					lex_pos=look(buf,TD);
					return Lex(dlms[lex_pos],lex_pos);
				}
				break;
			}
			case COMMENT:
			{
				if (c==EOF)
				{
					throw ("COMMENT:ERR");
				}
				else if (c=='*')
				{
					gc();
					if (c=='/')
					{
						CS=H;
						gc();
					}
				}
				else
				{
					gc();
				}
				break;
			}
			case STR:
			{
				if (c=='"')
				{
					add();
					gc();
					return Lex(LEX_CSTRING,0);
				}
				else if (c==EOF)
				{
					throw ("STR:ERR");
				}
				else 
				{
					add();
					gc();
				}
				break;
			}
			case DELIM:
			{
				if (c=='{'||c=='}'||c=='+'||c=='-'||c==','||c==';'||c=='('||c==')'||c=='*')
				{
					clear();
					add();
					gc();
					lex_pos=look(buf,TD);
					return Lex(dlms[lex_pos],lex_pos);
				}
				else
				{
					throw ("DELIM:ERR");
				}
				break;
			}
		}
	} 
	while (true);
}

int main ()
{
	Scanner scan("./prog");
	Lex curr_lex;
	while (true)
	{
		curr_lex=scan.get_lex();
		std::cout << curr_lex << std::endl;
		if (curr_lex.get_type()==LEX_FIN)
		{
			break;
		}
	}
	return 0;
}