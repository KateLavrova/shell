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
    L_ID, L_NUMB_CONST, L_STR_CONST,  
    L_NULL
    
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
    Lex(){}
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
    if (l.get_type() == L_ID)
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
    else 
        out << l.get_type();
    out <<";"<< l.get_type();
    out <<endl ;
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
                            dig = 10 * dig + (c - '0');
                        }
                        else if (isalpha(c))
                        {
                            throw(c);
                        }
                        else {
                            readNextChar = false; 
                            return(Lex(L_NUMB_CONST, dig));
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


class Parser
{
    Lex curl;
    LexType ctype;
    int cval;
    Scanner scan;

    void gl()
    {
        curl=scan.get_lex();
        ctype=curl.get_type();
        cval=curl.get_value();
    }

    public:
    Parser(const char *pr): scan(pr)
    {   }

    bool isSemicolon()
    {
        if(ctype == L_SEMICOLON) return true;
        else return false;
    }

    bool isConst()
    {
        if((ctype == L_STR_CONST)||(ctype == L_NUMB_CONST)) return true;
        else return false;
    }
    
    void analyze()
    {
        gl();
        CheckProgram();
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
            gl();
            if(ctype == L_EQ)
            {
                gl();
                if((ctype == L_MINUS)||(ctype == L_PLUS)) gl();
                if(isConst())
                {
                    gl();
                    if (isSemicolon()) gl();
                    else if (ctype == L_COMMA) Declaration();
                    else throw(curl);
                }
                else throw(curl);
            }
            else if (isSemicolon()) gl();
            else throw(curl);
            }
        return true;
    }

    void Declarations()
    {
        while (((ctype == L_INTEGER) || (ctype == L_STRING) || (ctype == L_REAL)) && Declaration());
    }

    void Operators()   
    {
            while(Oper());
    }

    bool Oper()
    {
        bool res = false;
        if(ctype == L_IF)
        {
            gl();
            Expr();
            Oper();
            ElseOper();
            res = true;
        }
        if(ctype == L_WHILE)
        {
            gl();
            Expr();
            Oper();
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
                    gl();
                    if(ctype == L_RPAREN)
                    {
                        gl();
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
            gl();
            if(isSemicolon()) gl();
            else throw curl;
            res = true;
            
        }
        else if (ctype == L_FOR)
        {
            gl();
            if(ctype == L_LPAREN)
            {
                gl();
                Assign();
                if(!isSemicolon()) throw curl;
                else gl();
                Expr();
                if(!isSemicolon()) throw curl;
                else gl();
                Assign();
                if(ctype == L_RPAREN) 
                {
                    gl();
                    res = true;
                }
                else throw curl;
            }
            else throw curl;
            Oper();
        }
        return res;
    }
    void ElseOper()
    {
        if(ctype == L_ELSE)
        {
            gl();
            Oper();
        }
    }

    void Expr()   //Expr -> Ex1 [ < | > | <= | >= | != | == ] Ex1 | Ex1
    {
        Ex1();
        if (ctype == L_LEQ || ctype == L_GEQ || ctype == L_LQPAREN || ctype == L_RQPAREN || ctype == L_NOTEQ || ctype == L_TIMES_EQ)
        {
            gl();
            Ex1();
        }
    }

    void Assign()
    {
            if (ctype == L_ID)
            {
                gl();
                if(ctype == L_EQ)
                {
                    gl();
                    Ex1();
                }
                else if(ctype == L_DMINUS) gl();
                else if(ctype == L_DPLUS) gl();
                else throw curl;
            }

    }

    void Ex1()    //Ex1 -> S { [ + | – | or ] S }
    {
        S();
        while (ctype == L_PLUS || ctype == L_MINUS || ctype == L_OR || ctype == L_STAR || ctype == L_SLASH)
        {
            gl();
            S();
        }
    }
    void S() // S -> F { [ * | / | and ] F }
    {
        F();
        while(ctype == L_STAR || ctype == L_SLASH || ctype == L_AND)
        {
            gl();
            F();
        }
    }
    void F()    // F -> Ident | Number | not F | (Expr)
    {
        if (ctype == L_ID || ctype == L_NUMB_CONST || ctype == L_STR_CONST)
            gl();
        else if (ctype == L_NOT)
        {
            gl();
            F();
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

}
