#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
using namespace std;

const char *TW[] = {"program","int","string","real","if","else","while","read","write","and","or","for","not","continue"};
const char *TD[] = {"EOF",";",",","(",")","<",">","{","}","=","+","-","==","*","<=",">=","!=","++", "--", "/"};

enum state {INIT,IDENT,NUMB,MULTCOM,SINGCOM,HELPCOM,SLSH,PLUS,MINUS,ALE,FINALE,QUOTE};

enum LexType
{                                                                                                    
    L_PROGRAM, L_INTEGER,L_STRING, L_REAL,L_IF, L_ELSE,L_WHILE,L_READ,L_WRITE,L_AND, L_OR, L_FOR, L_NOT, L_CONTINUE,                  
    // TD starts
    L_FIN,                                                                                                   
    L_SEMICOLON, L_COMMA, L_LPAREN, L_RPAREN, L_LQPAREN, L_RQPAREN, L_BEGIN, L_END,             
    L_EQ, L_PLUS, L_MINUS, L_TIMES_EQ, L_STAR, L_LEQ, L_GEQ, L_NOTEQ, L_DPLUS, L_DMINUS, L_SLASH,
    L_ID, L_NUMB_CONST, L_STR_CONST, L_REAL_CONST,
    L_NULL,

    POLIZ_GO, POLIZ_FGO, POLIZ_LABEL, POLIZ_ADDRESS
    
};


class Lex
{
    LexType t_lex;
    int value;
    float real;
    string astr;
    bool LexEmpty;
    LexType Lexl;
    private:
    Lex (LexType t, int v, float r, string s)
    {
        t_lex = t;
        value = v;
        real = r;
        astr = s;
    }
    public:
    Lex(){}
    static int count;
	Lex (LexType  t) : Lex(t,0,0,"")
    {}
    Lex (LexType t, int v):Lex(t,v,0,"") 
    {}
    Lex (LexType t, int , float r):Lex(t,0,r,"") 
    {}
    Lex (LexType t, string str):Lex(t,0,0,str)
    {}

	LexType get_type () const 
    { 
		return t_lex; 
	}
	int  get_value () const 
    { 
		return value; 
	}
    float get_real () const
    {
        return real;
    }

    string get_str() const 
    {
        return astr;
    }

    void push_lex(LexType l)
    {
        Lexl = l;
        LexEmpty = false;

    }

    bool isTW(Lex l)
    {
        if(l.get_type()<L_FIN)
            return true;
        else return false;
    }

    bool isTD(Lex l)
    {
        if((l.get_type()>=L_FIN) && (l.get_type() <= L_SLASH))
            return true;
        else return false;
    }

    bool isEOF(Lex l)
    {
        if(l.get_type()==L_FIN)
            return true;
        else return false;
    }

    bool isNumber(Lex l)
    {
        if(l.get_type()==L_NUMB_CONST)
            return true;
        else return false;
    }
    void SetValue(int v)
    {
        value = v;
    }
    void SetValue(float v)
    {
        
    }
};



class Ident 
{
		string   name;
		LexType   type;
		int   val;

	public:
		Ident (string  n )
        { 
            name=n;
            type = L_NULL;
        }
        bool operator== (const string& s) const 
        {
            return name == s;
        }

        string get_name () 
        { 
            return  name; 
        }
		LexType  get_type () 
        { 
            return  type; 
        }
		void  set_type (LexType t) 
        {
            type = t; 
        }
        bool get_declare()
        {
            return type!=L_NULL;
        }
		int  get_value () 
        {
            return  val; 
        }
		void  set_value (int v) 
        { 
            val = v;
        }     
};

vector<Ident> TID;
int addtoTID(const string& str)
{
    vector<Ident>::iterator i;
    i = find(TID.begin(), TID.end(), str);
    if (i != TID.end()) return(i - TID.begin());
    else
    {
        TID.push_back(Ident(str));
        return(TID.size() - 1);
    }
}

int Lex::count = 0;

ostream& operator<< (ostream &out, Lex l)
{
    l.count++;
    string type, adding;
    if (l.get_type() == L_ID || l.get_type() == POLIZ_ADDRESS)
    {
        out  << l.get_value();
    }
    else if (l.get_type() == L_STR_CONST)
    {
        out   << l.get_str();
    }
    else if (l.get_type() == L_NUMB_CONST)
    {
        out  << l.get_value();
    }
    else if(l.get_type() == L_REAL_CONST)
    {
        out<<l.get_real();
    }
    else if(l.get_type() == POLIZ_GO)
    {
        out<<"!";
    }
    else if(l.get_type() == POLIZ_FGO)
    {
        out<<"!F";
    }
    else if(l.get_type() == POLIZ_LABEL)
    {
        out<<"L"<<l.get_value();
    }

    else 
        out << l.get_type();
    out <<";"<< l.get_type();
    //out <<endl ;
    return out;
}


class Scanner {
		FILE *fp; 
		char c;
        bool readNextChar = true;

		int look(const string& buf, const char**  list) 
        {  
            int i = 0;
            while (list [i]) 
            {  
                if ( buf ==  list [ i ] )  
		            return i;
		        i++;  
            }
		    return -1;  
        }

		void gc()
        { 
              c = fgetc(fp); 
        }

        void push_and_gc(string& str){
            str.push_back(c);
            gc();
        }
	public:
        Scanner(){}

		Scanner (const char * program) 
        { 
            fp = fopen ( program, "r" );
            readNextChar = true;
        } 
        
		Lex get_lex ()
        {
            int dig, j;
            float r;
            state CS = INIT;
            string str;

            while(1)
            {
                if(readNextChar)
                    gc();
                readNextChar = true;
                int digreal;
                switch(CS)
                {
                    case INIT:
                        if (c ==' ' || c =='\n' || c=='\r' || c =='\t')
                            ;
                        else if (isalpha(c))
                        {
                            str.push_back(c);
                            CS = IDENT;
                        }
                        else if (isdigit(c))
                        {
                            dig = c-'0';
                            CS = NUMB;
                            digreal = 0;
                        }
                        else if (c == '+')
                        {
                            str.push_back(c);
                            CS = PLUS;
                        }
                        else if (c == '-')
                        {
                            str.push_back(c);
                            CS = MINUS;
                        }
                        else if (c == '/')
                        {
                            str.push_back(c);
                            CS = SLSH;
                        }
                        else if (c == '!' || c == '=')
                        {
                            str.push_back(c);
                            CS = ALE;
                        }
                        else if (c == '*' || c == '<' || c == '>')
                        {
                            str.push_back(c);
                            CS = FINALE;
                        }
                        else if (c == '"')
                        {
                            CS = QUOTE;
                        }  
                        else if (c == EOF)
                        {
                            readNextChar = false;
                            return(Lex((LexType)L_FIN));
                        }
                        else
                        {
                            str.push_back(c);
                            if ((j = look(str, TD)) >= 0)
                                return(Lex((LexType)(j + (int)L_FIN), j));
                            throw(c);
                        ;}
                        break;

                    case IDENT:
                        if (isalpha(c) || isdigit(c))
                        {
                            str.push_back(c);
                        }
                        else 
                        {
                            readNextChar = false;
                            if((j = look(str, TW)) >= 0)
                                return Lex((LexType)j, j);
                            j =  addtoTID(str);
                            return Lex(L_ID, j);  
                        }
                    break;

                    case NUMB:
                        if (isdigit(c))
                        {
                            if(digreal>0)
                            {
                                r += (c - '0')/digreal;
                                digreal*=10;
                            }
                            else dig = 10 * dig + (c - '0');
                        }
                        else if(c=='.')
                        {
                            digreal = 10;
                            r = dig;
                        }
                        else if (isalpha(c))
                        {
                            throw(c);
                        }
                        else {
                            readNextChar = false; 
                            
                            return digreal?Lex(L_REAL_CONST,0,r): Lex(L_NUMB_CONST, dig);
                        }
                    break;

                    case PLUS:
                    if ( c == '+')
                    {
                        str.push_back(c);
                        j = look(str, TD);
                        return Lex((LexType)(j + (int)L_FIN), j);
                    }
                    else 
                    {
                        readNextChar = false;
                        j = look(str, TD);
                        return Lex((LexType)(j + (int)L_FIN), j);
                    }
                    break;

                    case MINUS:
                    if ( c == '-')
                    {
                        str.push_back(c);
                        j = look(str, TD);
                        return Lex((LexType)(j + (int)L_FIN), j);
                    }
                    else 
                    {
                        readNextChar = false;
                        j = look(str, TD); 
                        return Lex((LexType)(j + (int)L_FIN), j);
                    }
                    break;  

                    case QUOTE:
                    if (c == '"') 
                    {
                        string qstr = "";
                        qstr += str;
                        return Lex(L_STR_CONST,qstr);
                    }
                    str.push_back(c);
                    break;

                    case SLSH:
                    if (c == '=')
                    {
                        str.push_back(c);
                        j = look(str, TD);
                        return Lex((LexType)(j + (int)L_FIN), j);
                    }
                    else if (c == '/')
                    {
                        str.pop_back();
                        CS = SINGCOM;
                    }
                    else if (c == '*')
                    {
                        str.pop_back();
                        CS = MULTCOM;
                    }
                    else
                    {
                        readNextChar = false;
                        j = look(str, TD);
                        return Lex((LexType)(j + (int)L_FIN), j);
                    }
                    break;

                    case SINGCOM:
                    if (c == '\n')
                        CS = INIT;
                    else if (c == EOF) throw(c);
                    break;
                    case MULTCOM:
                    if (c == '*')
                        CS = HELPCOM;
                    else if (c == EOF) throw(c);
                    break;
                    case HELPCOM:
                    if (c == '/')
                        CS = INIT;
                    else if (c == EOF) throw(c);
                    else  CS = MULTCOM;
                    break;
                    case ALE:
                    if (c == '=')
                    {
                        str.push_back(c);
                        CS = FINALE;
                    }  
                    else
                    {
                        readNextChar = false;
                        j = look(str, TD);
                        return Lex((LexType)(j + (int)L_FIN), j);
                    }
                    break;

                    case FINALE: 
                    if (c == '=')
                    {
                        str.push_back(c);
                        j = look(str, TD);
                        return Lex((LexType)(j + (int)L_FIN), j);            
                    }   
                    else
                    {
                        readNextChar = false;
                        j = look(str, TD);
                        return Lex((LexType)(j + (int)L_FIN), j);                       
                    }    
                    break;  
                }
            }
        }
};



template <class T, int MaxSize> class Stack
{
    T st[MaxSize];
    int top = 0;

    public:
    Stack()
    {
        top = 0;
    }
    void reset()
    {
        top = 0;
    }
    void push(T i);
    T pop();
    bool isEmpty()
    {
        if(top == 0) return true;
        else return false;
    }

    bool isFull()
    {
        if(top == MaxSize) return true;
        else return false;
    }
};

template <class T, int MaxSize>
void Stack <T, MaxSize>::push(T i)
{
    if (!isFull())
    {
        st[top] = i;
        top++;
    }
    else
        throw "Full stack";
}

template <class T, int MaxSize>
T Stack <T, MaxSize >::pop()
{
    if (!isEmpty())
    {
        top--;
        return st[top];
    }
    else
        throw "Empty stack";
}


class Poliz
{
    Lex *p;
    int size;
    int free;
public:
    Poliz(int MaxSize)
    {
        p = new Lex[size = MaxSize];
        free = 0;
    };
    ~Poliz(){delete []p;};
    void put_lex(Lex l) 
    {
        p[free]=l;
        free++;
    };
    void put_lex(Lex l, int place)
    {
        p[place]=l;
    };
    void blank()
    {
        free++;
    };
    int get_free()
    {
        return free;
    };
    Lex& operator[](int index)
    {
        if (index > size)
            throw "POLIZ:out of array";
        else if (index > free)
            throw "POLIZ:indefinite element of array";
        else
            return p[index];
    };
    void print()
    {
        for (int i = 0; i < free; ++i)
        cout << p[i];
    };
};



class Parser
{
    Lex curl;
    LexType ctype;
    int cval;
    Scanner scan;

    Stack <int,100> IntSt;
    Stack <LexType,1000> LexSt;
    int flag;
    int minus;

    public:
    void FlagRes()
    {
        flag = 0;
    }
    
    void IsInt()
    {
        flag = 1;
    }

    void IsReal()
    {
        flag = 2;
    }

    void IsString()
    {
        flag = 3;
    }
    vector <Lex> poliz;

    void gl()
    {
        curl=scan.get_lex();
        ctype=curl.get_type();
        cval=curl.get_value();
    }

    public:
    Parser(const char *pr): scan(pr)
    {   }

    void dec(LexType ctype)
    {
        int i;
        while(!IntSt.isEmpty())
        {
            i = IntSt.pop();
            if(TID[i].get_declare()) /////////////
                throw "twice";
            else
                {
                   // TID[i].set_value(ctype); /////////
                    TID[i].set_type(ctype); ////////////
                }
        }
    }

    void CheckId()
    {
        if (TID[cval].get_declare()) ///////////////
        LexSt.push(TID[cval].get_type());
    else
        throw "not declared";
    }

    void EqType()
    {
        if(LexSt.pop() == LexSt.pop())
           poliz.push_back(Lex(L_EQ));
        else throw "error types =";
    }

    void CheckNot()
    {
        if(LexSt.pop() == L_NUMB_CONST)
        {
            LexSt.push(L_NUMB_CONST);
            poliz.push_back(Lex(L_NOT));
        }
        else 
            throw "error types not";
    }

    void ExprCheck()
    {
        LexType T1,T2,op;
        T1 = LexSt.pop();
        op = LexSt.pop();
        T2 = LexSt.pop();

        if(op == L_PLUS || op == L_MINUS || op == L_STAR || op == L_SLASH)
        {
            if(T1 == T2)
            {
                if(T1 == L_INTEGER) LexSt.push(L_INTEGER);
                if(T1 == L_REAL) LexSt.push(L_REAL);
                if(T1 == L_STRING) LexSt.push(L_STRING);
            }
            else if((T1 == L_REAL && T2 == L_INTEGER)||(T2 == L_REAL && T1 == L_INTEGER))
            {
                if(T1 == L_INTEGER) LexSt.push(L_INTEGER);
                if(T1 == L_REAL) LexSt.push(L_REAL);
            }
            else throw "error types + | - | * | /";
        }

        else if(op == L_AND || op == L_OR)
        {
            if(T1 == L_INTEGER && T2 == L_INTEGER)
            {
                LexSt.push(L_INTEGER);
            }
            else throw "error types and|or";
        }

        else if(op == L_TIMES_EQ || op == L_NOTEQ || op == L_LEQ || op == L_GEQ || op == L_LQPAREN || op == L_RQPAREN)
        {
            if(T1 == T2)
            {
                if(T1 == L_INTEGER) LexSt.push(L_INTEGER);
                if(T1 == L_REAL) LexSt.push(L_REAL);
            }
            else if((T1 == L_REAL && T2 == L_INTEGER)||(T2 == L_REAL && T1 == L_INTEGER))
            {
                if(T1 == L_INTEGER) LexSt.push(L_INTEGER);
                if(T1 == L_REAL) LexSt.push(L_REAL);
            }
            else throw "error types == | != | < | > | <= | >=";
        }
        poliz.push_back(Lex(op));
    }



    bool isSemicolon()
    {
        if(ctype == L_SEMICOLON) return true;
        else return false;
    }

    void isConst()
    {
        if(ctype == L_STR_CONST)
        {
            LexSt.push(L_STRING);
            poliz.push_back(curl);
            EqType();
            gl();
        }
        else if(ctype == L_NUMB_CONST) IntConst();
        else if(ctype == L_REAL_CONST) RealConst();
        
    }
    void IntConst()
    {
        minus = 0;
        if(ctype == L_NUMB_CONST)
        {
            LexSt.push(L_INTEGER);
            poliz.push_back(curl);
            EqType();
            gl();
        }
        else throw curl;
    }

    void RealConst()
    {
        minus = 0;
        if(ctype == L_REAL_CONST)
        {
            LexSt.push(L_REAL);
            poliz.push_back(curl);
            EqType();
            gl();
        }
        else throw curl;
    }
    
    void analyze()
    {
        int i = 0;
        gl();
        CheckProgram();
        for (Lex l : poliz)
        {
          cout<<i<<": "<<l<<endl;
            i++;
        }
        cout<<"OK"<<endl;
    }

    void CheckProgram()
    {
        if (ctype == L_PROGRAM) gl();
        else throw(curl);

        if(ctype == L_BEGIN) gl();
        else throw(curl);

        Declarations();
        Operators();

        if (ctype == L_END) gl();
        else throw(curl);
    }

    bool Declaration()
    {
        gl();

        if (ctype == L_ID)
        {
            IntSt.push(cval);
            if (flag == 1)
            {   
                dec(L_INTEGER);
                LexSt.push(L_INTEGER);
            }
            else if (flag == 2)
            {
                dec(L_REAL);
                LexSt.push(L_REAL);
            }
            else if (flag == 3)
            {
                dec(L_STRING);
                LexSt.push(L_STRING);
            }
            poliz.push_back(Lex(POLIZ_ADDRESS, cval)); 

            gl();
            if(ctype == L_EQ)
            {
                gl();
               if(ctype == L_PLUS) gl();
               if(ctype == L_MINUS)
               {
                   minus = 1;
                   gl();
               }
                isConst();
                if (ctype == L_COMMA) Declaration();
               
            }
            else poliz.pop_back(); 
            }
            else throw(curl);
        return true;
    }

    void Declarations()
    {
        while (((ctype == L_INTEGER) || (ctype == L_STRING) || (ctype == L_REAL)))
        {
            FlagRes();
            if (ctype == L_INTEGER) IsInt();
            else if (ctype == L_REAL) IsReal();
            else if (ctype == L_STRING) IsString();
            Declaration();
            if (isSemicolon()) gl();
            else throw curl;
        }
        
    }

    void Operators()   
    {
            while(Oper());
    }

    bool Oper()
    {
        int pl0, pl1, pl2, pl3, pc,pl4,pl5,pl6,pl7,pl8;
        bool res = false;
        if(ctype == L_IF)
        {
            gl();
            Expr();
            pl2 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            Oper();
            pl3 = poliz.size();
            poliz[pl2] = Lex(POLIZ_LABEL,poliz.size());
           // ElseOper();
            if(ctype == L_ELSE)
            {
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_GO));
                poliz[pl2] = Lex(POLIZ_LABEL,poliz.size());
                gl();
                Oper();
                poliz[pl3] = Lex(POLIZ_LABEL,poliz.size());
            }
            res = true;
        }
        if(ctype == L_WHILE)
        {
            pl0 = poliz.size();
            pc = pl0;
            gl();
            Expr();
            pl1 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            Oper();
            poliz.push_back(Lex(POLIZ_LABEL,pl0));
            poliz.push_back(Lex(POLIZ_GO));
            poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
            res = true;
        }
        else if (ctype == L_FOR)
        {
            gl();
            if(ctype != L_LPAREN) throw curl;
            gl();
            Assign();
            if(!isSemicolon()) throw curl;
            pl4 = poliz.size();
            gl();
            Expr();
            if(!isSemicolon()) throw curl;
            pl5 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            gl();
            pl6 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_GO));
            pl8 = poliz.size();
            pc=pl8;
            Assign(); 
            poliz.push_back(Lex(POLIZ_LABEL,pl4));
            poliz.push_back(Lex(POLIZ_GO));
            if(ctype != L_RPAREN) throw curl;
            gl();
            pl7 = poliz.size();
            poliz[pl6] = Lex(POLIZ_LABEL, pl7);
            Oper();
            poliz.push_back(Lex(POLIZ_LABEL,pl8));
            poliz.push_back(Lex(POLIZ_GO));
            poliz[pl5] = Lex(POLIZ_LABEL, poliz.size());
            res = true; 
        }
        else if(ctype == L_READ)
        {
            gl();
            if(ctype == L_LPAREN)
            {
                gl();
                if(ctype == L_ID) 
                {
                    CheckId();
                    poliz.push_back(Lex(POLIZ_ADDRESS,cval));
                    gl();
                    if(ctype == L_RPAREN)
                    {
                        gl();
                        poliz.push_back(L_READ);
                        if(isSemicolon()) gl();
                        else throw(curl);
                    }
                    else throw(curl);
                }
                else throw(curl);
            }
            
        }
        else if (ctype == L_ID) 
        {
            Assign();
            if(isSemicolon()) gl();
            else throw (curl);
            res = true;
        }
        else if (ctype == L_WRITE)
        {
            gl();
            if(ctype == L_LPAREN)
            {
                gl();
                Expr();
                if(ctype == L_COMMA)
                {
                    gl();
                    Expr();
                    while (ctype != L_RPAREN) 
                    {
                        gl();
                        Expr();
                    }
                }
                if(ctype == L_RPAREN)
                {
                    gl();
                    poliz.push_back(Lex(L_WRITE));
                    if(isSemicolon())
                    {
                        gl();
                        res = true;
                    }
                    else throw curl;
                }
                
            }
        }
        else if (ctype == L_BEGIN)
        {
            gl();
            Operators();
            if(ctype == L_END) gl();
            else throw curl;
        }
        else if (ctype == L_CONTINUE)
        {
            poliz.push_back(Lex(POLIZ_LABEL,pc));
            poliz.push_back(Lex(POLIZ_GO));
            gl();
            if(isSemicolon()) gl();
            else throw curl;
            res = true;
            
        }
        return res;
    }

    void Expr()   //Expr -> Ex1 [ < | > | <= | >= | != | == ] Ex1 | Ex1
    {
        Ex1();
        if (ctype == L_LEQ || ctype == L_GEQ || ctype == L_LQPAREN || ctype == L_RQPAREN || ctype == L_NOTEQ || ctype == L_TIMES_EQ)
        {
            LexSt.push(ctype);
            gl();
            Ex1();
            ExprCheck();
        }
    }

    void Assign()
    {
        int v; 
        if (ctype == L_ID)
        {
            CheckId();
            v = cval; 
            poliz.push_back(Lex(L_ID,cval));
            gl();
            if(ctype == L_EQ)
            {
                poliz.pop_back();
                poliz.push_back(Lex(POLIZ_ADDRESS,v));
                gl();
                Ex1();
                poliz.push_back(Lex(L_EQ));
            }
            else if(ctype == L_DMINUS)
            {
                poliz.push_back(Lex(L_DMINUS));
                gl();
            }
            else if(ctype == L_DPLUS) 
            {
                poliz.push_back(Lex(L_DPLUS));
                gl();
            }

            else throw curl;
        }

    }

    void Ex1()    //Ex1 -> S { [ + | – | or ] S }
    {
        S();
        while (ctype == L_PLUS || ctype == L_MINUS || ctype == L_OR || ctype == L_STAR || ctype == L_SLASH)
        {
            LexSt.push(ctype);
            gl();
            S();
            ExprCheck();
        }
    }
    void S() // S -> F { [ * | / | and ] F }
    {
        F();
        while(ctype == L_STAR || ctype == L_SLASH || ctype == L_AND)
        {
            LexSt.push(ctype);
            gl();
            F();
        }
    }
    void F()    // F -> Ident | Number | not F | (Expr)
    {
        if(ctype == L_ID)
        {
            CheckId();
            poliz.push_back(Lex(L_ID,cval));
            gl();
        }
        else if (ctype == L_NUMB_CONST)
        {
            LexSt.push(L_INTEGER);
            poliz.push_back(curl);
            gl();
        }
        else if (ctype == L_REAL_CONST)
        {
            LexSt.push(L_REAL);
            poliz.push_back(curl);
            gl();
        }
        else if (ctype == L_NOT)
        {
            gl();
            F();
            CheckNot();
        }
        else if (ctype == L_LPAREN)
        {
            gl();
            Expr();
            if (ctype == L_RPAREN)
                gl();
            else
                throw (curl);
        }
        else throw curl;
    }

};


int main(int argc, char** argv)
{   
    try
    {
        Parser p("1.txt");
        p.analyze();
        return 0;
    }

    catch(char c)
    {
        cout<<"Error"<<c<<endl;
    }
    catch (Lex cl)
    {
        cout<<"Unexpected lexem: "<<cl.get_type()<<endl;
    }
    catch(const char* c)
    {
        cout<<c<<endl;
    }

}
