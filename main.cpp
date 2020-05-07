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
    LEX_FIN,      // //36
    POLIZ_LABEL,  // для ссылок на номера элементов ПОЛИЗа //37
	POLIZ_ADDRESS, // для обозначения операндов-адресов //38
	POLIZ_GO,	  // ! //39
	POLIZ_FGO     // F! //40
};


class Lex
{
	type_of_lex t_lex;
	int v_lex; // показывает, каким номером лексема лежит в соответствующей таблице (если не лежит ни в какой таблице - 0)
	int int_value;
	double real_value;
	std::string str_value;
	bool is_int, is_real, is_str;
	bool is_unary;
	int address; // нужно для перехода
public:
	Lex (type_of_lex t = LEX_NULL, int v = 0)
    {
    	is_unary=false;
        t_lex=t;
        v_lex=v;
        is_int=is_real=is_str=false;
    }
    Lex (type_of_lex t, int v, int i)
    {
    	is_unary=false;
    	t_lex=t;
        v_lex=v;
        is_real=is_str=false;
        is_int=true;
        int_value=i;
    }
    Lex (type_of_lex t, int v, double r)
    {
    	is_unary=false;
        t_lex=t;
        v_lex=v;
        is_int=is_str=false;
        is_real=true;
        real_value=r;
    }
    Lex (type_of_lex t, int v, std::string s)
    {
    	is_unary=false;
        t_lex=t;
        v_lex=v;
        is_int=is_real=false;
        is_str=true;
        str_value=s;
    }
    Lex (std::string, type_of_lex t, int addr)
    {
    	is_int=is_real=is_str=false;
    	if (t!=POLIZ_LABEL)
    	{
    		throw "Попытка использовать лексему POLIZ_LABEL не по назначению";
    	}
    	else
    	{
    		t_lex=POLIZ_LABEL;
    		address=addr;
    	}
    }
    type_of_lex get_type ()
    {
        return t_lex;
    }
    int get_value ()
    {
        return v_lex;
    }
    void unary()
    {
    	is_unary=true;
    }
    friend std::ostream& operator << (std::ostream &s, Lex l);
    friend std::ofstream& operator << (std::ofstream &s, Lex l);
};

class Ident
{
	// тут по-видимому придётся делать поле значений 
	char * name;
	bool declare;
	bool assign;
	type_of_lex type;
	bool is_used;
public:
	Ident ()
	{
		declare=false;
		assign=false;
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
	bool get_declare()
	{
		return declare;
	}
	void put_declare()
	{
		declare=true;
	}
	type_of_lex get_type()
	{
		return type;
	}
	bool get_assign()
	{
		return assign;
	}
	void put_assign()
	{
		assign=true;
	}
	void put_type(type_of_lex ctype)
	{
		type=ctype;
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
	int get_top()
	{
		return top;
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
		s << l.int_value << " " << std::endl;
	}
	else if (l.is_real==true)
	{
		s  << l.real_value << " " << std::endl;
	}
	else if (l.is_str==true)
	{
		s  << l.str_value << " " << std::endl;
	}
	else
	{
		if (l.t_lex==LEX_ID)
		{
			s  << TID[l.v_lex].get_name() << std::endl;
		}
		else if (l.t_lex==LEX_FIN)
		{
			s << "Сама лексема: \"EOF\", тип лексемы: " << l.t_lex << std::endl;
		}
		else if (l.t_lex==POLIZ_LABEL)
		{
			s << l.address << "          POLIZ_LABEL " << std::endl;
		}
		else if (l.t_lex==POLIZ_GO)
		{
			s << "!" << std::endl;
		}
		else if (l.t_lex==POLIZ_FGO)
		{
			s << "!F" <<  std::endl;
		}
		else if (l.t_lex==Scanner::dlms[l.v_lex])
		{
			if ((l.t_lex==LEX_MINUS||l.t_lex==LEX_PLUS)&&(l.is_unary))
			{
				s << Scanner::TD[l.v_lex] << " (унарный)" << std::endl;
				return s;
			}
			s << Scanner::TD[l.v_lex] << std::endl;
		}
		else
		{
			s << Scanner::TW[l.v_lex] << std::endl;
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
		else if (l.t_lex==POLIZ_LABEL)
		{
			s << "Адрес перехода в ПОЛИЗ-массиве \"" << l.address << "\", POLIZ_ADDRESS " << std::endl;
		}
		else if (l.t_lex==POLIZ_GO)
		{
			s << "Команда безусловного перехода ПОЛИЗ-массива: \"" << "!" << "\", POLIZ_GO " << std::endl;
		}
		else if (l.t_lex==POLIZ_FGO)
		{
			s << "Команда перехода по лжи ПОЛИЗ-массива: \"" << "!F" << "\", POLIZ_FGO " << std::endl;
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

/*
Эта шаблон нужен для семантического анализатора
*/

template <class T, int max_size> class Stack
{
	T s[max_size];
	int top;
public:
	Stack()
	{
		top=0;
	}	
	void reset()
	{
		top=0;
	}
	void push(T i);
	T pop();
	bool is_empty()
	{
		return top==0;
	}
	bool is_full()
	{
		return top==max_size;
	}
};

template <class T, int max_size>

void Stack <T, max_size>::push(T i)
{
	if (!is_full())
	{
		s[top]=i;
		++top;
	}
	else 
	{
		throw "Stack_is_full";
	}
}

template <class T, int max_size>

T Stack <T, max_size>::pop()
{
	if (!is_empty())
	{
		--top;
		return s[top];
	}
	else
	{
		throw 1; // надо выкидывать не строку для условия цикла for
	}
}

/*
ПОЛИЗ
*/

class Poliz
{
	Lex *p;
	int size;
	int free;
public:
	Poliz (int max_size)
	{
		p = new Lex [size=max_size];
		free=0;
	};
	~ Poliz ()
	{
		delete []p;
	}	
	void put_lex (Lex l)
	{
		p[free]=l;
		++free;
	}
	void put_lex (Lex l, int place)
	{
		p[place]=l;
	}
	void blank ()
	{
		++free;
	}
	int get_free ()
	{
		return free;
	}
	Lex& operator[] (int index)
	{
		if (index>size)
		{
			throw "Выход за пределы ПОЛИЗ массива";
		}
		else
		{
			if (index>free)
			{
				throw "Искомого элемента нет в ПОЛИЗ массиве";
			}
			else
			{
				return p[index];
			}
		}
	}
	void print ()
	{
		for (int i=0; i<free; i++)
		{
			std::cout << i << ") " << p[i];
		}
		std::cout<< std::endl; 
	}
};


class Parser
{
	Lex curr_lex;
	type_of_lex c_type;
	Scanner scan;
	Stack <Lex, 100> st_lex;
	int c_val; // показывает по каким номером лежит индентификатор в TID
	int curr_conditional; // показывает адрес условия текущего цикла (нужно для continue)

	/* 	нужно для обработки описаний */
	bool is_int, is_real, is_string;
	int var_in_program; // считаем, сколько переменных лежит в TID

	Lex standart_semicolon; // эталон ; для вставки в ПОЛИЗ

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
	void WRITE1(Lex l);
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
	/*контроль контекстных условий в описаниях*/
	void declare();
	/*контроль контекстных условий в выражении*/
	void check_id(Lex l);
	void check_op_eq();
	void check_op();
	void check_not();
	void check_unary_op();
	void check_id_in_read();
	/*Прочий контроль контекстных условий*/
	void check_if();
	void check_while();
	void check_for();

	void gl()
	{
		curr_lex=scan.get_lex();
		c_type=curr_lex.get_type();
		c_val=curr_lex.get_value();
		//std::cout<< curr_lex << std::endl;
	}

public:
	Poliz prog;
	Parser (const char* program) : scan ("./prog"), prog(1000)
	{ 
		curr_conditional=-1;
		standart_semicolon=Lex (LEX_SEMICOLON, 9);
	}
	void analyze();
};
	
void Parser::analyze()
{
	gl();
	PROG();
	std::cout << "Is correct" << std::endl;
	prog.print();
}

void Parser::PROG()
{
	var_in_program=0;
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
		if (c_type==LEX_INT)
		{
			is_real=is_string=false;
			is_int=true;
		}
		else if (c_type==LEX_STRING)
		{
			is_real=is_int=false;
			is_string=true;
		}
		else if (c_type==LEX_REAL)
		{
			is_string=is_int=false;
			is_real=true;
		}
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
		declare();
		prog.put_lex(curr_lex);
		gl();
		VAR2();
		prog.put_lex(standart_semicolon);
	}
	else
	{
		throw curr_lex;
	}
}

void Parser::VAR2()
{
	Lex l;
	l=curr_lex;
	if (c_type==LEX_EQUAL)
	{
		gl();
		CONST1();
		prog.put_lex(l);
	}
	else
	{
		/* переменная не инициализируется */ 
	}
}

void Parser::CONST1()
{
	Lex l;
	if (c_type==LEX_PLUS||c_type==LEX_MINUS)
	{
		l=curr_lex;
		gl();
		if (c_type!=LEX_CINT&&c_type!=LEX_CREAL)
		{
			throw curr_lex;
		}
		if ((is_int&&c_type==LEX_CINT)||(is_real&&c_type==LEX_CREAL)||(is_real&&c_type==LEX_CINT))
		{
			// OK
			prog.put_lex(curr_lex);
			prog.put_lex(l);
		}
		else
		{
			throw "Несоответствие типов операндов";
		}
		gl();
	}
	else if (c_type==LEX_CINT||c_type==LEX_CREAL||c_type==LEX_CSTRING)
	{
		if ((is_int&&c_type==LEX_CINT)||(is_real&&(c_type==LEX_CINT||c_type==LEX_CREAL))||(is_string&&c_type==LEX_CSTRING))
		{
			// OK
			prog.put_lex(curr_lex);
		}
		else
		{
			throw "Несоответствие типов операндов";
		}
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
		st_lex.reset();
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
		if (curr_conditional==-1)
		{
			throw "Встречен continue вне цикла";
		}
		prog.put_lex(Lex ("0_0", POLIZ_LABEL, curr_conditional));
		prog.put_lex(Lex (POLIZ_GO));
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
	int label1,label2;
	gl();
	if (c_type!=LEX_LPAREN)
	{
		throw curr_lex;
	}
	gl();
	EXPR();
	check_if();
	label1=prog.get_free();
	prog.blank();
	prog.put_lex(Lex (POLIZ_FGO));
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
	label2=prog.get_free();
	prog.blank();
	prog.put_lex(Lex (POLIZ_GO));
	prog.put_lex(Lex ("0_0", POLIZ_LABEL, prog.get_free()), label1);
	gl();
	OP1();
	prog.put_lex(Lex ("0_0", POLIZ_LABEL, prog.get_free()), label2);
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
	Lex lread,lid;
	lread=curr_lex;
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
	check_id(curr_lex);
	lid=curr_lex;
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
	prog.put_lex(lid);
	prog.put_lex(lread);
}

void Parser::WRITE()
{
	Lex l;
	l=curr_lex;
	gl();
	if (c_type!=LEX_LPAREN)
	{
		throw curr_lex;
	}
	gl();
	EXPR();
	prog.put_lex(l, prog.get_free()-1);
	prog.put_lex(standart_semicolon); // при реализации вычислений после вывода не убирать из стека вычислений то, что мы вывели, чтобы всё удалялост корректно
	WRITE1(l);
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

void Parser::WRITE1(Lex l)  // передаём лексему LEX_WRITE
{
	if (c_type!=LEX_COMMA)
	{
		/* ok - больше ничего не выводим */
	}
	else
	{
		gl();
		EXPR();
		prog.put_lex(l, prog.get_free()-1);
		prog.put_lex(standart_semicolon);
		WRITE1(l);
	}
}

void Parser::WHILE()
{
	int condition_addr, exit_addr, exit_label;
	gl();
	if (c_type!=LEX_LPAREN)
	{
		throw curr_lex;
	}
	condition_addr=prog.get_free();
	curr_conditional=condition_addr;
	gl();
	EXPR();
	check_while();
	exit_label=prog.get_free();
	prog.blank();
	prog.put_lex(Lex (POLIZ_FGO));
	if (c_type!=LEX_RPAREN)
	{
		throw curr_lex;
	}
	gl();
	OP1();
	prog.put_lex(Lex ("0_0", POLIZ_LABEL, condition_addr));
	prog.put_lex(Lex (POLIZ_GO));
	exit_addr=prog.get_free();
	prog.put_lex(Lex ("0_0", POLIZ_LABEL, exit_addr), exit_label);
	curr_conditional=-1;
}

void Parser::FOR() // for (A;B;C) D
{
	int addr_B, addr_C, addr_exit;
	int label_B, label_C;
	bool check=false;
	gl();
	if (c_type!=LEX_LPAREN)
	{
		throw curr_lex;
	}
	gl();
	if (c_type!=LEX_SEMICOLON)
	{
		FOR1(); // A
	}
	if (c_type!=LEX_SEMICOLON)
	{
		throw curr_lex;
	}
	st_lex.reset();
	gl();
	addr_B=prog.get_free();
	if (c_type!=LEX_SEMICOLON)
	{
		check=true;
		FOR1(); // B
	}
	if (check==false) // в цикле for нет условия
	{
		prog.put_lex (Lex (LEX_CINT, 0, 1)); // ложим на место условия истинное выражение 
	} 
	try
	{
		check_for();
	}
	catch(int)
	{
		// Ничего страшного - стек операций был пуст - в цикле for нет условия - работаем дальше
	}
	label_B=prog.get_free();
	prog.blank();
	prog.put_lex(Lex (POLIZ_FGO));
	label_C=prog.get_free();
	prog.blank();
	prog.put_lex(Lex (POLIZ_GO));
	if (c_type!=LEX_SEMICOLON)
	{
		throw curr_lex;
	}
	gl();
	curr_conditional=prog.get_free();
	addr_C=prog.get_free();
	if (c_type!=LEX_RPAREN)
	{
		FOR1(); // C
	}
	prog.put_lex(Lex ("0_0", POLIZ_LABEL, addr_B), prog.get_free());
	prog.blank();
	prog.put_lex(Lex (POLIZ_GO));
	if (c_type!=LEX_RPAREN)
	{
		throw curr_lex;
	}
	gl();
	prog.put_lex(Lex ("0_0", POLIZ_LABEL, prog.get_free()), label_C);
	OP1(); // D
	prog.put_lex(Lex ("0_0", POLIZ_LABEL, addr_C), prog.get_free());
	prog.blank();
	prog.put_lex(Lex (POLIZ_GO));
	addr_exit=prog.get_free();
	prog.put_lex(Lex ("0_0", POLIZ_LABEL, addr_exit), label_B);
	curr_conditional=-1;
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
	Lex l;
	int eq_num=0;
	EXPR1();
	while (c_type==LEX_EQUAL)
	{
		l=curr_lex;
		st_lex.push(curr_lex);
		eq_num++;
		gl();
		EXPR1();
	}
	while (eq_num>0)
	{
		prog.put_lex(l);
		check_op_eq();
		eq_num--;
	}
	prog.put_lex(standart_semicolon);
}

/*void Parser::EXPR11()
{
	if (c_type==LEX_EQUAL)
	{
		st_lex.push(curr_lex);
		gl();
		EXPR1();
		check_op();
		EXPR11();
	}
}*/

void Parser::EXPR1()
{
	EXPR2();
	EXPROR();
}

void Parser::EXPROR()
{
	Lex l;
	if (c_type==LEX_OR)
	{
		l=curr_lex;
		st_lex.push(curr_lex);
		gl();
		EXPR2();
		check_op();
		prog.put_lex(l);
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
	Lex l;
	if (c_type==LEX_AND)
	{
		l=curr_lex;
		st_lex.push(curr_lex);
		gl();
		EXPR3();
		check_op();
		prog.put_lex(l);
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
	Lex l;
	if (c_type==LEX_LS||c_type==LEX_GTR||c_type==LEX_DEQUAL||c_type==LEX_NEQUAL||c_type==LEX_LEQ||c_type==LEX_GEQ)
	{
		l=curr_lex;
		st_lex.push(curr_lex);
		gl();
		EXPR4();
		check_op();
		prog.put_lex(l);
		EXPR41();
	}
}

void Parser::EXPR4()
{
	EXPR5();
	EXPR51();
}

void Parser::EXPR51()
{
	Lex l;
	if (c_type==LEX_MINUS||c_type==LEX_PLUS)
	{
		l=curr_lex;
		st_lex.push(curr_lex);
		gl();
		EXPR5();
		check_op();
		prog.put_lex(l);
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
	Lex l;
	gl();
	if (c_type==LEX_TIMES||c_type==LEX_SLASH)
	{
		l=curr_lex;
		st_lex.push(curr_lex);
		gl();
		EXPR6();
		check_op();
		prog.put_lex(l);
		EXPR61();
	}
}

void Parser::EXPR6()
{
	Lex l;
	if (c_type==LEX_NOT)
	{
		l=curr_lex;
		l.unary();
		st_lex.push(curr_lex);
		gl();
		EXPR6();
		check_not();
		prog.put_lex(l);
	}
	else if (c_type==LEX_MINUS)
	{
		l=curr_lex;
		l.unary();
		st_lex.push(curr_lex);
		gl();
		EXPR6();
		check_unary_op();
		prog.put_lex(l);
	}
	else if (c_type==LEX_PLUS)
	{
		l=curr_lex;
		l.unary();
		st_lex.push(curr_lex);
		gl();
		EXPR6();
		check_unary_op();
		prog.put_lex(l);
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
		if (c_type==LEX_ID)
		{
			check_id(curr_lex);
		}
		st_lex.push(curr_lex);
		prog.put_lex(curr_lex);
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
	else
	{
		throw curr_lex;
	}
}

void Parser::declare()
{
	var_in_program++;
	if (var_in_program!=(TID.get_top()-1))
	{
		throw "second description";
	}
	else
	{
		TID[var_in_program].put_declare();
		if (is_string==true)
		{
			TID[var_in_program].put_type(LEX_STRING);
		}
		else if (is_real==true)
		{
			TID[var_in_program].put_type(LEX_REAL);
		}
		else if (is_int==true)
		{
			TID[var_in_program].put_type(LEX_INT);
		}
	}
}

/*
Проверяем, описан ли идентификатор
*/

void Parser::check_id(Lex l)
{
	if (TID[l.get_value()].get_declare())
	{
		/*OK*/
	}
	else
	{
		throw "Идентификатор не задекларирован";
	}
}

void Parser::check_op_eq()
{
	Lex lex1,lex2,op; 
	type_of_lex temp_type1, temp_type2;
	lex2=st_lex.pop();
	op=st_lex.pop();
	lex1=st_lex.pop(); // lex1 op lex2 
	/* похоже, что для = как правоассоциативной операции надо делать свой check_op потому, что нужно сначала пройти до конца цепочки равенств, записав
	лексемы в стек, и потом начинать выуживать   */
	if (op.get_type()==LEX_EQUAL)
	{
		if (lex1.get_type()!=LEX_ID)
		{
			throw "Несоответствие типов операндов: слева от = стоит не идентификатор";
		}
		else
		{
			temp_type1=TID[lex1.get_value()].get_type(); // получили, какой тип имеет данная переменная
			temp_type2=lex2.get_type();
			if (lex2.get_type()==LEX_ID)
			{
				temp_type2=TID[lex2.get_value()].get_type();
			}
			if (temp_type1==LEX_REAL)
			{
				if (temp_type2==LEX_INT||temp_type2==LEX_CINT||temp_type2==LEX_CREAL||temp_type2==LEX_REAL)
				{
					// OK
					st_lex.push(LEX_CREAL);
				}
				else
				{
					throw "Несоответствие типов операндов: неудачная попытка привести к real";
				}
			}
			else if (temp_type1==LEX_INT)
			{
				if (temp_type2==LEX_INT||temp_type2==LEX_CINT)
				{
					// OK
					st_lex.push(LEX_CINT);
				}
				else
				{
					throw "Несоответствие типов операндов: неудачная попытка привести к int";
				}
			}
			else if (temp_type1==LEX_STRING)
			{
				if (temp_type2==LEX_STRING||temp_type2==LEX_CSTRING)
				{
					// OK
					st_lex.push(LEX_CSTRING);
				}
				else
				{
					throw "Несоответствие типов операндов: неудачная попытка привести к string";
				}
			}
		}
	}
}

void Parser::check_op()
{
	Lex lex1,lex2,op; 
	type_of_lex temp_type1, temp_type2;
	lex2=st_lex.pop();
	op=st_lex.pop();
	lex1=st_lex.pop(); // lex1 op lex2 
	if (lex1.get_type()!=LEX_ID)
	{
		temp_type1=lex1.get_type();
	}
	else
	{
		temp_type1=TID[lex1.get_value()].get_type();
	}
	if (lex2.get_type()!=LEX_ID)
	{
		temp_type2=lex2.get_type();
	}
	else
	{
		temp_type2=TID[lex2.get_value()].get_type();
	}
	if (op.get_type()==LEX_OR) // or
	{
		if (temp_type1!=LEX_INT&&temp_type1!=LEX_CINT)
		{
			throw "Некорректный операнд перед or";
		}
		if (temp_type2!=LEX_INT&&temp_type2!=LEX_CINT)
		{
			throw "Некорректный операнд после or";
		}
		st_lex.push(LEX_CINT);
	}
	else if (op.get_type()==LEX_AND) // and
	{
		if (temp_type1!=LEX_INT&&temp_type1!=LEX_CINT)
		{
			throw "Некорректный операнд перед and";
		}
		if (temp_type2!=LEX_INT&&temp_type2!=LEX_CINT)
		{
			throw "Некорректный операнд после and";
		}
		st_lex.push(LEX_CINT);
	}
	else if (op.get_type()==LEX_LS||op.get_type()==LEX_DEQUAL||op.get_type()==LEX_LEQ||op.get_type()==LEX_GTR||op.get_type()==LEX_GEQ||op.get_type()==LEX_NEQUAL) // сравнения
	{
		if (temp_type1==LEX_STRING||temp_type1==LEX_CSTRING)
		{
			if (temp_type1==LEX_STRING||temp_type1==LEX_CSTRING)
			{
				// OK
			}
			else
			{
				throw "Некорретный операнд: попытка сравнить строку с не-строкой";
			}
		}
		else if (temp_type1==LEX_INT||temp_type1==LEX_CINT||temp_type1==LEX_REAL||temp_type1==LEX_CREAL)
		{
			if (temp_type2==LEX_INT||temp_type2==LEX_CINT||temp_type2==LEX_REAL||temp_type2==LEX_CREAL)
			{
				// OK
			}
			else
			{
				throw "Некорретный операнд: попытка сравнить число с не-числом";
			}
		}
		st_lex.push(LEX_CINT);
	}
	else if (op.get_type()==LEX_PLUS) // бинарный плюс
	{
		if (temp_type1==LEX_STRING||temp_type1==LEX_CSTRING)
		{
			if (temp_type2==LEX_STRING||temp_type2==LEX_CSTRING)
			{
				// OK
				st_lex.push(LEX_CSTRING);
			}
			else
			{
				throw "Некорретный операнд: попытка прибавить к строке не-строку";
			}
		}
		else if (temp_type1==LEX_INT||temp_type1==LEX_CINT)
		{
			if (temp_type2==LEX_INT||temp_type2==LEX_CINT)
			{
				// OK
				st_lex.push(LEX_CINT);
			}
			else if (temp_type2==LEX_REAL||temp_type2==LEX_CREAL)
			{
				// OK
				st_lex.push(LEX_CREAL);
			}
			else
			{
				throw "Некорретный операнд: попытка прибавить к числу не-число";
			}
		}
		else if (temp_type1==LEX_REAL||temp_type1==LEX_CREAL)
		{
			if (temp_type2==LEX_INT||temp_type2==LEX_CINT||temp_type2==LEX_REAL||temp_type2==LEX_CREAL)
			{
				// OK
				st_lex.push(LEX_CREAL);
			}
			else
			{
				throw "Некорретный операнд: попытка прибавить к числу не-число";
			}
		}
	}
	else if (op.get_type()==LEX_MINUS) //бинарный минус
	{
		if (temp_type1==LEX_INT||temp_type1==LEX_CINT)
		{
			if (temp_type2==LEX_INT||temp_type2==LEX_CINT)
			{
				// OK
				st_lex.push(LEX_CINT);
			}
			else if (temp_type2==LEX_REAL||temp_type2==LEX_CREAL)
			{
				// OK 
				st_lex.push(LEX_CREAL);
			}
			else
			{
				throw "Некорретный операнд: попытка отнять от числа не-число";
			}
		}
		else if (temp_type1==LEX_REAL||temp_type1==LEX_CREAL)
		{
			if (temp_type2==LEX_REAL||temp_type2==LEX_CREAL||temp_type2==LEX_INT||temp_type2==LEX_CINT)
			{
				// OK
				st_lex.push(LEX_REAL);
			}
			else
			{
				throw "Некорретный операнд: попытка отнять от числа не-число";
			}
		}
		else
		{
			throw "Некорретный операнд: слева от знака - стоит не число";
		}
	}
	else if (op.get_type()==LEX_SLASH||op.get_type()==LEX_TIMES)
	{
		if (temp_type1==LEX_INT||temp_type1==LEX_CINT)
		{
			if (temp_type2==LEX_INT||temp_type2==LEX_CINT)
			{
				// OK
				st_lex.push(LEX_CINT);
			}
			else if (temp_type2==LEX_REAL||temp_type2==LEX_CREAL)
			{
				// OK
				st_lex.push(LEX_CREAL);
			}
			else
			{
				throw "Некорретный операнд: поделить (умножить) число на не-число";
			}
		}
		else if (temp_type1==LEX_REAL||temp_type1==LEX_CREAL)
		{
			if (temp_type2==LEX_INT||temp_type2==LEX_CINT||temp_type2==LEX_REAL||temp_type2==LEX_CREAL)
			{
				// OK
				st_lex.push(LEX_CREAL);
			}
			else
			{
				throw "Некорретный операнд: поделить (умножить) число на не-число";
			}
		}
		else
		{
			throw "Некорретный операнд: слева от знака / (*) стоит не число";
		}
	}
}

void Parser::check_unary_op()
{
	Lex lex1, op;
	type_of_lex temp_type1;
	lex1=st_lex.pop();
	op=st_lex.pop();
	if (lex1.get_type()!=LEX_ID)
	{
		temp_type1=lex1.get_type();
	}
	else
	{
		temp_type1=TID[lex1.get_value()].get_type();
	}
	if (op.get_type()==LEX_MINUS||op.get_type()==LEX_PLUS) // унарный минус
	{
		if (temp_type1==LEX_INT||temp_type1==LEX_CINT)
		{
			// OK
			st_lex.push(LEX_CINT);
		}
		else if (temp_type1==LEX_REAL||temp_type1==LEX_CREAL)
		{
			// OK
			st_lex.push(LEX_REAL);
		}		
		else
		{
			throw "Некорретный операнд: справа от унарного минуса (плюса) стоит не число";
		}
	}
}

void Parser::check_not()
{
	Lex lex1, op;
	type_of_lex temp_type1;
	lex1=st_lex.pop();
	op=st_lex.pop();
	if (lex1.get_type()!=LEX_ID)
	{
		temp_type1=lex1.get_type();
	}
	else
	{
		temp_type1=TID[lex1.get_value()].get_type();
	}
	if (op.get_type()==LEX_NOT)
	{
		if (temp_type1==LEX_INT||temp_type1==LEX_CINT)
		{
			// OK
			st_lex.push(LEX_CINT);
		}
		else
		{
			throw "Некорретный операнд: справа от not стоит не целое число";
		}

	}
}

void Parser::check_if()
{
	Lex lex1;
	type_of_lex temp_type1;
	lex1=st_lex.pop();
	if (lex1.get_type()!=LEX_ID)
	{
		temp_type1=lex1.get_type();
	}
	else
	{
		temp_type1=TID[lex1.get_value()].get_type();
	}
	if (temp_type1==LEX_INT||temp_type1==LEX_CINT)
	{
		// OK
	}
	else
	{
		throw "Внутри условия в операторе if стоит не целое число";
	}
}

void Parser::check_while()
{
	Lex lex1;
	type_of_lex temp_type1;
	lex1=st_lex.pop();
	if (lex1.get_type()!=LEX_ID)
	{
		temp_type1=lex1.get_type();
	}
	else
	{
		temp_type1=TID[lex1.get_value()].get_type();
	}
	if (temp_type1==LEX_INT||temp_type1==LEX_CINT)
	{
		// OK
	}
	else
	{
		throw "Внутри условия в операторе while стоит не целое число";
	}
}

void Parser::check_for()
{
	Lex lex1;
	type_of_lex temp_type1;
	lex1=st_lex.pop();
	if (lex1.get_type()!=LEX_ID)
	{
		temp_type1=lex1.get_type();
	}
	else
	{
		temp_type1=TID[lex1.get_value()].get_type();
	}
	if (temp_type1==LEX_INT||temp_type1==LEX_CINT)
	{
		// OK
	}
	else
	{
		throw "Внутри условия в операторе for стоит не целое число";
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
		std::cout << "Ошибка на этапе ЛА, либо семантического анализа (конкретная ошибка в файле output)" << std::endl;
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
	catch (int)
	{
		std::cout << "Ошибка на этапе ЛА, либо семантического анализа (конкретная ошибка в файле output)" << std::endl;
		fout.close();
		fout.open ("./Output", std::ios_base::out|std::ios_base::trunc);
		fout << "Попытка извлечения из пустого стека операций (скорее всего ошибка в условии цикла for) " << std::endl;
	}
	fout.close();
	return 0;
}