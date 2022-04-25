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

const char *TW[] = {"program","int","string","real","if","else","while","read","write","and","or","for","not","continue",NULL};
const char *TD[] = {"EOF","*","/","+","-","==","<",">","<=",">=","!=",";","{","}","=",",","(",")",NULL};

enum state {INIT,IDENT,NUMB,MULTCOM,SINGCOM,HELPCOM,SLSH,PLUS,MINUS,ALE,FINALE,AMP,QUOTE};

enum LexType
{   
    L_NULL,                                                                                                
    L_REAL, L_BREAK, L_CONTINUE, L_DO, L_ELSE, L_FALSE, L_FOR, L_FUNCTION, L_GETENV, L_IF, L_IN,            
    L_NAN, L_NUMBER, L_NULLPTR, L_OBJECT, L_RETURN, L_STRING, L_TRUE, L_TYPEOF, 	                      
    L_UNDEFINED, L_VAR, L_WHILE, L_WRITE,
    // TD starts
    L_FIN,                                                                                                   
    L_SEMICOLON, L_COMMA, L_COLON, L_DOT, L_LPAREN, L_RPAREN, L_LQPAREN, L_RQPAREN, L_BEGIN, L_END,             
    L_EQ, L_DEQ, L_TEQ, L_LSS, L_GTR, L_PLUS, L_PLUS_EQ, L_DPLUS, L_MINUS, L_MINUS_EQ, L_DMINUS,                
    L_TIMES, L_TIMES_EQ, L_TIMES_SLASH, L_SLASH, L_SLASH_EQ, L_SLASH_TIMES, L_DSLASH, L_PERCENT, L_PERCENT_EQ,  
    L_LEQ, L_NOT, L_NEQ, L_NDEQ, L_GEQ, L_OR, L_DPIPE, L_AND, L_DAMP, 
    L_ID, L_NUMB, L_STR_CONST  
    
};


class Lex
{
    LexType t_lex;
    int value;
    string astr;
    bool LexEmpty;
    LexType Lexl;
    private:
    Lex (LexType t, int v, string s)
    {
        t_lex = t;
        value = v;
        astr = s;
    }
    public:
    static int count;
	Lex (LexType  t) : Lex(t,0,"")
    {}
    Lex (LexType t, int v):Lex(t,v,"") 
    {}
    Lex (LexType t, string str):Lex(t,0,str)
    {}
	LexType get_type () const 
    { 
		return t_lex; 
	}
	int  get_value () const 
    { 
		return value; 
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
        if((l.get_type()>=L_FIN) && (l.get_type() <= L_DAMP))
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
        if(l.get_type()==L_NUMBER)
            return true;
        else return false;
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

    if(l.isTW(l))
    {
        type = (string)TW[l.get_type()]; 
        adding = "TW:";
    }
    else if(l.isTD(l))
    {
        type = (string)TD[l.get_type() - L_FIN];
        adding = "TD:";
    }
    else if(l.isEOF(l))
    {
        type = "EOF";
        adding = "TD:";  
    }
    else if(l.isNumber(l))
    {
        type = "Number";
        adding = "";
    }

    if (l.get_type() == L_ID)
    {
        type = TID[l.get_value()].get_name();
        out << l.count << ' ' << type << " << " << "TID:" << l.get_value()  << "\n";
    }
    else if ((l.get_type() == L_STR_CONST) || (l.get_type() == L_STRING))
    {
        type = "String";
        out << l.count  << ' ' << type  << " >> "  << adding << l.get_str() <<  "\n";
    }
    else 
        out  << l.count << ' ' << type << " == "  << adding << l.get_value()  << "\n";
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
            state CS = INIT;
            string str;

            while(1)
            {
                if(readNextChar)
                    gc();
                readNextChar = true;
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
                        else if (c == '*' || c == '<' || c == '>' || c == '%')
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
                        }
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
                            dig = 10 * dig + (c - '0');
                        }
                        else if (isalpha(c))
                        {
                            throw(c);
                        }
                        else {
                            readNextChar = false; 
                            return(Lex(L_NUMB, dig));
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

                    case AMP:
                    if (c == '&')
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

int main(int argc, char** argv)
{   
    cout<<"begin LexAnalis "<<endl;
    Scanner A;
    if (argc == 2)
    {
        A = Scanner(argv[1]);
    }
    else A = Scanner();

    Lex a(L_NULL);

    for(;;)
    {
        try 
        { 
            a = A.get_lex();
        }
        catch (char c)
        { 
            cout << "ERROR: " << c << "\n"; 
            terminate(); 
        }
        cout << a;
        if (a.get_type() == L_FIN) break;
    }

    cout<<"end"<<endl;

}
