#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <string>


enum type_of_lex  
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
    LEX_FIN       //  //36
};


class Lex
{
	type_of_lex t_lex;
	int v_lex; // показывает, каким номером лексема лежит в соответствующей таблице (если не лежит ни в какой таблице - 0)
	int int_value;
	double real_value;
	std::string str_value;
	bool is_int, is_real, is_str;
public:
	Lex (type_of_lex t = LEX_NULL, int v = 0)
    {
        t_lex=t;
        v_lex=v;
        is_int=is_real=is_str=false;
    }
    Lex (type_of_lex t, int v, int i)
    {
    	t_lex=t;
        v_lex=v;
        is_real=is_str=false;
        is_int=true;
        int_value=i;
    }
    Lex (type_of_lex t, int v, double r)
    {
        t_lex=t;
        v_lex=v;
        is_int=is_str=false;
        is_real=true;
        real_value=r;
    }
    Lex (type_of_lex t, int v, std::string s)
    {
        t_lex=t;
        v_lex=v;
        is_int=is_real=false;
        is_str=true;
        str_value=s;
    }
    type_of_lex get_type ()
    {
        return t_lex;
    }
    int get_value ()
    {
        return v_lex;
    }
    friend std::ostream& operator << (std::ostream &s, Lex l);
    friend std::ofstream& operator << (std::ofstream &s, Lex l);
};

class Ident
{
	char * name;
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
		top=1;  
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
	friend std::ostream& operator << (std::ostream &s, Lex l);
	friend std::ofstream& operator << (std::ofstream &s, Lex l);
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

const char * Scanner::TD[] = 
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
	unsigned long exp=10;
	CS=H;
	// Реализация автомата
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
				else if (isalpha(c)||c=='('||c=='{')
				{
					throw ("NUMB:ERR");
				}
				else 
				{
					return Lex(LEX_CINT,0,integer);
				}
				break;
			}
			case REALNUM:
			{
				double t;
				if (isdigit(c))
				{
					t=c-'0';
					real=real+double(t/exp);
					exp=10*exp;
					gc();
				}
				else if (isalpha(c)||c=='('||c=='{')
				{
					throw ("REALNUM:ERR");
				}
				else
				{
					return Lex(LEX_CREAL,0,real);
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
					return Lex(LEX_CSTRING,0,(std::string)buf);
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

std::ostream& operator << (std::ostream &s, Lex l)
{
	if (l.is_int==true)
	{
		s << "Сама лексема: \"" << l.int_value << "\", тип лексемы: " << l.t_lex << std::endl;
	}
	else if (l.is_real==true)
	{
		s << "Сама лексема: \"" << l.real_value << "\", тип лексемы: " << l.t_lex << std::endl;
	}
	else if (l.is_str==true)
	{
		s << "Сама лексема: \"" << l.str_value << "\", тип лексемы: " << l.t_lex << std::endl;
	}
	else
	{
		if (l.t_lex==LEX_ID)
		{
			s << "Сама лексема: \"" << TID[l.v_lex].get_name() << "\", тип лексемы: " << l.t_lex << " (номер в таблице идентификаторов: " << l.v_lex << ")" << std::endl;
		}
		else if (l.t_lex==LEX_FIN)
		{
			s << "Сама лексема: \"EOF\", тип лексемы: " << l.t_lex << std::endl;
		}
		else if (l.t_lex==Scanner::dlms[l.v_lex])
		{
			s << "Сама лексема: \"" << Scanner::TD[l.v_lex] << "\", тип лексемы: " << l.t_lex << std::endl;
		}
		else
		{
			s << "Сама лексема: \"" << Scanner::TW[l.v_lex] << "\", тип лексемы: " << l.t_lex << std::endl;
		}
	}
	return s;
}

std::ofstream& operator << (std::ofstream &s, Lex l)
{
	if (l.is_int==true)
	{
		s << "Сама лексема: \"" << l.int_value << "\", тип лексемы: " << l.t_lex << std::endl;
	}
	else if (l.is_real==true)
	{
		s << "Сама лексема: \"" << l.real_value << "\", тип лексемы: " << l.t_lex << std::endl;
	}
	else if (l.is_str==true)
	{
		s << "Сама лексема: \"" << l.str_value << "\", тип лексемы: " << l.t_lex << std::endl;
	}
	else
	{
		if (l.t_lex==LEX_ID)
		{
			s << "Сама лексема: \"" << TID[l.v_lex].get_name() << "\", тип лексемы: " << l.t_lex << " (номер в таблице идентификаторов: " << l.v_lex << ")" << std::endl;
		}
		else if (l.t_lex==LEX_FIN)
		{
			s << "Сама лексема: \"EOF\", тип лексемы: " << l.t_lex << std::endl;
		}
		else if (l.t_lex==Scanner::dlms[l.v_lex])
		{
			s << "Сама лексема: \"" << Scanner::TD[l.v_lex] << "\", тип лексемы: " << l.t_lex << std::endl;
		}
		else
		{
			s << "Сама лексема: \"" << Scanner::TW[l.v_lex] << "\", тип лексемы: " << l.t_lex << std::endl;
		}
	}
	return s;
}

class Parser
{
	Lex curr_lex;
	type_of_lex c_type;
	Scanner scan;

	void PROG();
	/* description */
	void DESCR();
	void DESCR1();
	void VAR();
	void VAR1();
	void VAR2();
	void CONST1();
	/* statement */ 
	void OP();
	void OP1();
	void IFELSE();
	void WHILE();
	void FOR();
	void FOR1();
	void READ();
	void WRITE();
	void WRITE1();
	void COMPLEXOP();
	void EXPRESSIONOP();
	/* expression */ 
	void EXPR();
	void EXPR1();
	void EXPR11();
	void EXPR2();
	void EXPROR();
	void EXPR3();
	void EXPRAND();
	void EXPR4();
	void EXPR41();
	void EXPR5();
	void EXPR51();
	void EXPR6();
	void EXPR61();
	void EXPR7();

	void gl()
	{
		curr_lex=scan.get_lex();
		c_type=curr_lex.get_type();
		std::cout<< curr_lex << std::endl;
	}

public:
	Parser (const char* program) : scan ("./prog") { }
	void analyze();
};
	
void Parser::analyze()
{
	gl();
	PROG();
	std::cout << "Is correct" << std::endl;
}

void Parser::PROG()
{
	if (c_type==LEX_PROGRAM)
	{
		gl();
		if (c_type==LEX_LBRACE)
		{
			gl();
			DESCR();
			OP();
			if (c_type!=LEX_RBRACE)
			{
				throw curr_lex;
			}
			gl();
			if (c_type!=LEX_FIN)
			{
				throw curr_lex;
			}
		}
		else 
		{
			throw curr_lex;
		}
	}
	else
	{
		throw curr_lex;
	}
}

void Parser::DESCR()
{
	if (c_type==LEX_INT||c_type==LEX_STRING||c_type==LEX_REAL)
	{
		gl();
		DESCR1();
		if (c_type==LEX_SEMICOLON)
		{
			gl();
			DESCR();
		}
		else
		{
			throw curr_lex;
		}
	}
	else
	{
		/* нет описаний -> идём в OP */
	}
}

void Parser::DESCR1()
{
	VAR();
	VAR1();
}

void Parser::VAR1()
{
	if (c_type==LEX_COMMA)
	{
		gl();
		VAR();
		VAR1();
	}
	else
	{
		/* в описании нет запятой */ 
	}
}

void Parser::VAR()
{
	if (c_type==LEX_ID)
	{
		gl();
		VAR2();
	}
	else
	{
		throw curr_lex;
	}
}

void Parser::VAR2()
{
	if (c_type==LEX_EQUAL)
	{
		gl();
		CONST1();
	}
	else
	{
		/* переменная не инициализируется */ 
	}
}

void Parser::CONST1()
{
	if (c_type==LEX_PLUS||c_type==LEX_MINUS)
	{
		gl();
		if (c_type!=LEX_CINT&&c_type!=LEX_REAL)
		{
			throw curr_lex;
		}
		gl();
	}
	else if (c_type==LEX_CINT||c_type==LEX_CREAL||c_type==LEX_CSTRING)
	{
		gl();
	}
	else
	{
		throw curr_lex;
	}
}

void Parser::OP()
{
	if (c_type==LEX_RBRACE)
	{
		/* переходим в Parser::PROG, либо в Parser::COMPLEXOP */ 
	}
	else 
	{
		OP1();
		OP();
	}
}

void Parser::OP1()
{
	if (c_type==LEX_IF)
	{
		IFELSE();
	}
	else if (c_type==LEX_LBRACE)
	{
		COMPLEXOP();
	}
	else if (c_type==LEX_CONT)
	{
		gl();
		if (c_type!=LEX_SEMICOLON)
		{
			throw curr_lex;
		}
		gl();
	}
	else if (c_type==LEX_READ)
	{
		READ();
	}
	else if (c_type==LEX_WRITE)
	{
		WRITE();
	}
	else if (c_type==LEX_WHILE)
	{
		WHILE();
	}
	else if (c_type==LEX_FOR)
	{
		FOR();
	}
	else
	{
		EXPRESSIONOP();
	}
}

void Parser::IFELSE()
{
	gl();
	if (c_type!=LEX_LPAREN)
	{
		throw curr_lex;
	}
	gl();
	EXPR();
	if (c_type!=LEX_RPAREN)
	{
		throw curr_lex;
	}
	gl();
	OP1();
	if (c_type!=LEX_ELSE)
	{
		throw curr_lex;
	}
	gl();
	OP1();
}

void Parser::COMPLEXOP()
{
	gl();
	OP();
	if (c_type!=LEX_RBRACE)
	{
		throw curr_lex;
	}
	gl();
}

void Parser::READ()
{
	gl();
	if (c_type!=LEX_LPAREN)
	{
		throw curr_lex;
	}
	gl();
	if (c_type!=LEX_ID)
	{
		throw curr_lex;
	}
	gl();
	if (c_type!=LEX_RPAREN)
	{
		throw curr_lex;
	}
	gl();
	if (c_type!=LEX_SEMICOLON)
	{
		throw curr_lex;
	}
	gl();
}

void Parser::WRITE()
{
	gl();
	if (c_type!=LEX_LPAREN)
	{
		throw curr_lex;
	}
	gl();
	EXPR();
	WRITE1();
	if (c_type!=LEX_RPAREN)
	{
		throw curr_lex;
	}
	gl();
	if (c_type!=LEX_SEMICOLON)
	{
		throw curr_lex;
	}
	gl();
}

void Parser::WRITE1()
{
	if (c_type!=LEX_COMMA)
	{
		/* ok - больше ничего не выводим */
	}
	else
	{
		gl();
		EXPR();
		WRITE1();
	}
}

void Parser::WHILE()
{
	gl();
	if (c_type!=LEX_LPAREN)
	{
		throw curr_lex;
	}
	gl();
	EXPR();
	if (c_type!=LEX_RPAREN)
	{
		throw curr_lex;
	}
	gl();
	OP1();
}

void Parser::FOR()
{
	gl();
	if (c_type!=LEX_LPAREN)
	{
		throw curr_lex;
	}
	gl();
	if (c_type!=LEX_SEMICOLON)
	{
		FOR1();
	}
	if (c_type!=LEX_SEMICOLON)
	{
		throw curr_lex;
	}
	gl();
	if (c_type!=LEX_SEMICOLON)
	{
		FOR1();
	}
	if (c_type!=LEX_SEMICOLON)
	{
		throw curr_lex;
	}
	gl();
	if (c_type!=LEX_SEMICOLON)
	{
		FOR1();
	}
	if (c_type!=LEX_RPAREN)
	{
		throw curr_lex;
	}
	gl();
	OP1();
}

void Parser::FOR1()
{
	EXPR();
}

void Parser::EXPRESSIONOP()
{
	EXPR();
	if (c_type!=LEX_SEMICOLON)
	{
		throw curr_lex;
	}
	gl();
}


void Parser::EXPR()
{
	EXPR1();
	EXPR11();
}

void Parser::EXPR11()
{
	if (c_type==LEX_EQUAL)
	{
		gl();
		EXPR1();
		EXPR11();
	}
}

void Parser::EXPR1()
{
	EXPR2();
	EXPROR();
}

void Parser::EXPROR()
{
	if (c_type==LEX_OR)
	{
		gl();
		EXPR2();
		EXPROR();
	}
}

void Parser::EXPR2()
{
	EXPR3();
	EXPRAND();
}

void Parser::EXPRAND()
{
	if (c_type==LEX_AND)
	{
		gl();
		EXPR3();
		EXPRAND();
	}
}

void Parser::EXPR3()
{
	EXPR4();
	EXPR41();
}

void Parser::EXPR41()
{
	if (c_type==LEX_LS||c_type==LEX_GTR||c_type==LEX_DEQUAL||c_type==LEX_NEQUAL||c_type==LEX_LEQ||c_type==LEX_GEQ)
	{
		gl();
		EXPR3();
	}
}

void Parser::EXPR4()
{
	if (c_type==LEX_MINUS||c_type==LEX_PLUS)
	{
		gl();
		EXPR5();
		EXPR51();
	}
	else
	{
		EXPR5();
		EXPR51();
	}
}

void Parser::EXPR51()
{
	if (c_type==LEX_MINUS||c_type==LEX_PLUS)
	{
		gl();
		EXPR5();
		EXPR51();
	}
}

void Parser::EXPR5()
{
	EXPR6();
	EXPR61();
}

void Parser::EXPR61()
{
	gl();
	if (c_type==LEX_TIMES||c_type==LEX_SLASH)
	{
		gl();
		EXPR6();
		EXPR61();
	}
}

void Parser::EXPR6()
{
	if (c_type==LEX_NOT)
	{
		gl();
		EXPR7();
	}
	else
	{
		EXPR7();
	}
}

void Parser::EXPR7()
{
	if (c_type==LEX_ID||c_type==LEX_CSTRING||c_type==LEX_CINT||c_type==LEX_CREAL)
	{
		/* ok */ 
	}
	else if (c_type==LEX_LPAREN)
	{
		gl();
		EXPR();
		if (c_type!=LEX_RPAREN)
		{
			throw curr_lex;
		}
	}
}



int main ()
{
	std::ofstream fout;
	fout.open ("./Output", std::ios_base::out|std::ios_base::trunc);
	// prog - исходная программа на модельном языке
	Parser pars("./prog");
	try
	{
		pars.analyze();
	}
	catch (Lex curr_lex)
	{
		std::cout << "некорректный синтаксис" << std::endl;
		fout.close();
		fout.open ("./Output", std::ios_base::out|std::ios_base::trunc);
		fout << curr_lex << std::endl;
	}
	catch (char const *message)
	{
		fout.close();
		fout.open ("./Output", std::ios_base::out|std::ios_base::trunc);
		fout << message << std::endl;
	}
	catch (std::bad_alloc message)
	{
		fout.close();
		fout.open ("./Output", std::ios_base::out|std::ios_base::trunc);
		fout << "bad_alloc" << std::endl;
	}
	fout.close();
	return 0;
}