/*** PL0 COMPILER WITH CODE GENERATION ***/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

const  AL    =  10;  /* LENGTH OF IDENTIFIERS */
const  NORW  =  19;  /* # OF RESERVED WORDS */
const  TXMAX = 100;  /* LENGTH OF IDENTIFIER TABLE */
const  NMAX  =  14;  /* MAX NUMBER OF DEGITS IN NUMBERS */
const  AMAX  =2047;  /* MAXIMUM ADDRESS */
const  LEVMAX=   3;  /* MAX DEPTH OF BLOCK NESTING */
const  CXMAX = 200;  /* SIZE OF CODE ARRAY */

const int SYMNUM = 43; //SYM个数

typedef enum { NUL,     IDENT,    NUMBER,    PLUS,     MINUS,   TIMES,
            SLASH,    ODDSYM,   EQL,       NEQ,      LSS,     LEQ, 
              GTR,      GEQ,      LPAREN,    RPAREN,   COMMA,   SEMICOLON,
              PERIOD,   BECOMES,  BEGINSYM,  ENDSYM,  IFSYM,    THENSYM,
            WHILESYM, WRITESYM, READSYM,   DOSYM,   CALLSYM,      CONSTSYM,
              VARSYM,   PROCSYM,  PROGSYM, 
              ELSESYM, FORSYM, TOSYM, DOWNTOSYM, RETURNSYM,       // 共5个。ELSE，FOR，TO，DOWNTO，RETURN
      PLUSBECOMES, MINUSBECOMES, PLUSPLUS, MINUSMINUS,// +=, -=, ++, --
      COLON //冒号
      } SYMBOL;

char *SYMOUT[] = {"NUL", "IDENT", "NUMBER", "PLUS", "MINUS", "TIMES",
          "SLASH", "ODDSYM", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ",
          "LPAREN", "RPAREN", "COMMA", "SEMICOLON", "PERIOD",
          "BECOMES", "BEGINSYM", "ENDSYM", "IFSYM", "THENSYM",
          "WHILESYM", "WRITESYM", "READSYM", "DOSYM", "CALLSYM",
          "CONSTSYM", "VARSYM", "PROCSYM", "PROGSYM"
      , "ELSESYM", "FORSYM", "TOSYM", "DOWNTOSYM", "RETURNSYM",     // 共5个。ELSE，FOR，TO，DOWNTO，RETURN
      "PLUSBECOMES", "MINUSBECOMES", "PLUSPLUS", "MINUSMINUS",// +=, -=, ++, --
      "COLON"//数组保留字符号,冒号
       };

/*
NUL: 一个空符号，可能用于表示空值或无效的符号。
IDENT: 标识符（Identifier）的符号类型，用于表示变量、函数或其他命名实体的名称。
NUMBER: 数字（Number）的符号类型，用于表示整数或浮点数的字面值。
PLUS: 加号（+）的符号类型，用于表示加法运算。
MINUS: 减号（-）的符号类型，用于表示减法运算。
TIMES: 乘号（*）的符号类型，用于表示乘法运算。
SLASH: 斜杠（/）的符号类型，用于表示除法运算。
ODDSYM: 奇数判断符号的类型，用于表示奇偶判断操作。
EQL: 等于号（==）的符号类型，用于表示相等比较。
NEQ: 不等于号（!=）的符号类型，用于表示不等比较。
LSS: 小于号（<）的符号类型，用于表示小于比较。
LEQ: 小于等于号（<=）的符号类型，用于表示小于等于比较。
GTR: 大于号（>）的符号类型，用于表示大于比较。
GEQ: 大于等于号（>=）的符号类型，用于表示大于等于比较。
LPAREN: 左括号（(）的符号类型，用于表示分组或函数调用的开始。
RPAREN: 右括号（)）的符号类型，用于表示分组或函数调用的结束。
COMMA: 逗号（,）的符号类型，用于表示参数列表或变量列表中的分隔符。
SEMICOLON: 分号（;）的符号类型，用于表示语句结束。
PERIOD: 句号（.）的符号类型，用于表示程序结束。
BECOMES: 赋值符号（:=）的符号类型，用于表示变量赋值操作。
BEGINSYM: BEGIN关键字的符号类型，用于表示复合语句的开始。
ENDSYM: END关键字的符号类型，用于表示复合语句的结束。
IFSYM: IF关键字的符号类型，用于表示条件语句的开始。
THENSYM: THEN关键字的符号类型，用于表示条件语句的条件部分。
WHILESYM: WHILE关键字的符号类型，用于表示循环语句的开始。
WRITESYM: WRITE关键字的符号类型，用于表示写操作。
READSYM: READ关键字的符号类型，用于表示读操作。
DOSYM: DO关键字的符号类型，用于表示循环语句的循环体。
CALLSYM: CALL关键字的符号类型，用于表示过程调用
CONSTSYM: CONST关键字的符号类型，用于表示常量声明。
VARSYM: VAR关键字的符号类型，用于表示变量声明。
PROCSYM: PROCEDURE关键字的符号类型，用于表示过程声明。
PROGSYM: PROGRAM关键字的符号类型，用于表示程序的开始。
ELSESYM: ELSE关键字的符号类型，用于表示条件语句的否定分支。
FORSYM: FOR关键字的符号类型，用于表示循环语句的开始。
TOSYM: TO关键字的符号类型，用于表示循环语句的终止条件。
DOWNTOSYM: DOWNTO关键字的符号类型，用于表示循环语句的终止条件。
RETURNSYM: RETURN关键字的符号类型，用于表示函数返回。
PLUSBECOMES: 加等于符号（+=）的符号类型，用于表示加法赋值操作。
MINUSBECOMES: 减等于符号（-=）的符号类型，用于表示减法赋值操作。
PLUSPLUS: 自增符号（++）的符号类型，用于表示自增操作。
MINUSMINUS: 自减符号（--）的符号类型，用于表示自减操作。
*/

typedef  int *SYMSET; // SET OF SYMBOL;
typedef  char ALFA[11];

typedef  enum { CONSTANT, VARIABLE, PROCEDUR, ARRAY } OBJECTS ;
typedef  enum { LIT, OPR, LOD, STO, CAL, INI, JMP, JPC,
                STO2, LOD2 
                } FCT;
typedef struct {
       FCT F;     /*FUNCTION CODE*/
       int L;       /*0..LEVMAX  LEVEL*/
       int A;     /*0..AMAX    DISPLACEMENT ADDR*/
} INSTRUCTION;
        /* LIT O A -- LOAD CONSTANT A             */
        /* OPR 0 A -- EXECUTE OPR A               */
        /* LOD L A -- LOAD VARIABLE L,A           */
        /* STO L A -- STORE VARIABLE L,A          */
        /* CAL L A -- CALL PROCEDURE A AT LEVEL L */
        /* INI 0 A -- INCREMET T-REGISTER BY A    */
        /* JMP 0 A -- JUMP TO A                   */
        /* JPC 0 A -- JUMP CONDITIONAL TO A       */
char   CH;  /*LAST CHAR READ*/
SYMBOL SYM; /*LAST SYMBOL READ*/
ALFA   ID;  /*LAST IDENTIFIER READ*/
int    NUM; /*LAST NUMBER READ*/
int    CC;  /*CHARACTER COUNT*/
int    LL;  /*LINE LENGTH*/
int    CX;  /*CODE ALLOCATION INDEX*/
char   LINE[81];
INSTRUCTION  CODE[CXMAX];
ALFA    KWORD[NORW+1];
SYMBOL  WSYM[NORW+1];
SYMBOL  SSYM['^'+1];
ALFA    MNEMONIC[9];
SYMSET  DECLBEGSYS, STATBEGSYS, FACBEGSYS;
/*
DECLBEGSYS  声明的开始符号集合
STATBEGSYS  语句的开始符号集合，例如控制流语句（如"if"、"while"等）、赋值语句或函数调用语句等
FACBEGSYS   表达式因子的开始符号集合
*/
struct {
  ALFA NAME;
  OBJECTS KIND;
  int LAW; /*下界*/
  union {
    int VAL;   /*CONSTANT*/
    struct { int LEVEL,ADR,SIZE; } vp;  /*VARIABLE,PROCEDUR:, ARRAY*/
    
  };
  
} TABLE[TXMAX];

FILE *FIN,*FOUT;
int ERR;

void EXPRESSION(SYMSET FSYS, int LEV, int &TX, bool nowArray, int index);//表达式处理
void TERM(SYMSET FSYS, int LEV, int &TX); //项处理
int isConst(int index);
void ENTER(OBJECTS K, int LEV, int &TX, int &DX);
void EnterArray( int LEV, int &TX, int &DX, int start, int end, ALFA name);
//---------------------------------------------------------------------------
int SymIn(SYMBOL SYM, SYMSET S1) {
  return S1[SYM];
}
//---------------------------------------------------------------------------
SYMSET SymSetUnion(SYMSET S1, SYMSET S2) {
  SYMSET S=(SYMSET)malloc(sizeof(int)*SYMNUM);
  for (int i=0; i<SYMNUM; i++)
        if (S1[i] || S2[i]) S[i]=1;
        else S[i]=0;
  return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetAdd(SYMBOL SY, SYMSET S) {
  SYMSET S1;
  S1=(SYMSET)malloc(sizeof(int)*SYMNUM);
  for (int i=0; i<SYMNUM; i++) S1[i]=S[i];
  S1[SY]=1;
  return S1;
}

//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMNUM);
  for (i=0; i<SYMNUM; i++) S[i]=0;
  S[a]=1;
  return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a, SYMBOL b) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMNUM);
  for (i=0; i<SYMNUM; i++) S[i]=0;
  S[a]=1;  S[b]=1;
  return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMNUM);
  for (i=0; i<SYMNUM; i++) S[i]=0;
  S[a]=1;  S[b]=1; S[c]=1;
  return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c, SYMBOL d) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMNUM);
  for (i=0; i<SYMNUM; i++) S[i]=0;
  S[a]=1;  S[b]=1; S[c]=1; S[d]=1;
  return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c, SYMBOL d,SYMBOL e) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMNUM);
  for (i=0; i<SYMNUM; i++) S[i]=0;
  S[a]=1;  S[b]=1; S[c]=1; S[d]=1; S[e]=1;
  return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c, SYMBOL d,SYMBOL e, SYMBOL f) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMNUM);
  for (i=0; i<SYMNUM; i++) S[i]=0;
  S[a]=1;  S[b]=1; S[c]=1; S[d]=1; S[e]=1; S[f]=1;
  return S;
}
//---------------------------------------------------------------------------
SYMSET SymSetNULL() {
  SYMSET S; int i,n,k;
  S=(SYMSET)malloc(sizeof(int)*SYMNUM);
  for (i=0; i<SYMNUM; i++) S[i]=0;
  return S;
}
//---------------------------------------------------------------------------
void Error(int n) {
  String s = "***"+AnsiString::StringOfChar(' ', CC-1)+"^";
  Form1->printls(s.c_str(),n);   fprintf(FOUT,"%s%d\n", s.c_str(), n);
  ERR++;
} /*Error*/
//---------------------------------------------------------------------------
void GetCh() {
  if (CC==LL) {
    if (feof(FIN)) {
        Form1->printfs("PROGRAM INCOMPLETE");
        fprintf(FOUT,"PROGRAM INCOMPLETE\n");
        fclose(FOUT);
        exit(0);
        }
    LL=0; CC=0;
    CH=' ';
    while (!feof(FIN) && CH!=10)  //A码中10为LF  换行符
        { CH=fgetc(FIN);  LINE[LL++]=CH; }
    LINE[LL-1]=' ';  LINE[LL]=0;
    String s=IntToStr(CX);
    while(s.Length()<3) s=" "+s;
    s=s+" "+LINE;
    Form1->printfs(s.c_str());
    fprintf(FOUT,"%s\n",s);
  }
  CH=LINE[CC++];
} /*GetCh()*/
//---------------------------------------------------------------------------
void GetSym() {
  int i,J,K;   //
  ALFA  A;
  while (CH<=' ') GetCh();
  if (CH>='A' && CH<='Z') { /*ID OR RESERVED WORD*/
    K=0;
    do {
      if (K<AL) A[K++]=CH;
      GetCh();
    }while((CH>='A' && CH<='Z')||(CH>='0' && CH<='9'));
    A[K]='\0';
    strcpy(ID,A); i=1; J=NORW;
    do { //二分查找，在已排序字符串数组中使用二分查找算法查找目标字符串
      K=(i+J) / 2;
      if (strcmp(ID,KWORD[K])<=0) J=K-1;
      if (strcmp(ID,KWORD[K])>=0) i=K+1;
    }while(i<=J);
    if (i-1 > J) SYM=WSYM[K]; //找到目标 ij彼此越界，找到关键字
    else SYM=IDENT;
  }
  else
    if (CH>='0' && CH<='9') { /*NUMBER*/
      K=0; NUM=0; SYM=NUMBER;
        do {
          NUM=10*NUM+(CH-'0');
        K++; GetCh();
      }while(CH>='0' && CH<='9');
        if (K>NMAX) Error(30);
    }
    else if (CH==':') {
      GetCh();
      if (CH=='=') { SYM=BECOMES; GetCh(); }
      else SYM=COLON;
    }
    /* THE FOLLOWING TWO CHECK WERE ADDED
               BECAUSE ASCII DOES NOT HAVE A SINGLE CHARACTER FOR <= OR >= */
        else if (CH=='<') {
      GetCh();
      if (CH=='=') { SYM=LEQ; GetCh(); }
      else if(CH=='>') { SYM=NEQ; GetCh(); }      // 不等号运算符 <>
      else SYM=LSS;
      }
      else if (CH=='>'){
        GetCh();
      if (CH=='=') { SYM=GEQ; GetCh(); }
      else SYM=GTR;
    }
    else if(CH=='/') {      
      GetCh();
      if(CH=='*'){       // '/* */' 多行注释
        GetCh();
        i=CH;
        while (i!='*' || CH!='/') {
          i=CH;
          GetCh();
        }
        if(i!='*' && CH!='/') Error(19);
        else{
            GetCh();
            GetSym();
        }
      }
      else if(CH=='/'){       // '//' 单行注释
        i=CX;
        while (CC!=LL) {
            GetCh();
        }
        GetSym();
      }else SYM=SLASH;
          }
    else if(CH=='+') {
      GetCh();
      if(CH=='=') { SYM=PLUSBECOMES; GetCh(); }  // 运算符 +=
      else if(CH=='+') { SYM=PLUSPLUS; GetCh(); }   // 运算符 ++
      else SYM=PLUS;
    } 
    else if(CH=='-') {
      GetCh();
      if(CH=='=') { SYM=MINUSBECOMES; GetCh(); }  // 运算符 -=
      else if(CH=='-') { SYM=MINUSMINUS; GetCh(); } // 运算符 --
      else SYM=MINUS;
    } 
      else { SYM=SSYM[CH]; GetCh(); }
} /*GetSym()*/
//---------------------------------------------------------------------------
void GEN(FCT X, int Y, int Z) {
  if (CX>CXMAX) {
    Form1->printfs("PROGRAM TOO LONG");
    fprintf(FOUT,"PROGRAM TOO LONG\n");
    fclose(FOUT);
    exit(0);
  }
  CODE[CX].F=X; CODE[CX].L=Y; CODE[CX].A=Z;
  CX++;
} /*GEN*/
//---------------------------------------------------------------------------
void TEST(SYMSET S1, SYMSET S2, int N) {
  if (!SymIn(SYM,S1)) {
    Error(N);
        while (!SymIn(SYM,SymSetUnion(S1,S2))) GetSym();
  }
} /*TEST*/
//---------------------------------------------------------------------------

int POSITION(ALFA ID, int TX) { /*FIND IDENTIFIER IN TABLE*/
  int i=TX;
  strcpy(TABLE[0].NAME,ID);
  while (strcmp(TABLE[i].NAME,ID)!=0) i--;
  return i;
} /*POSITION*/
//---------------------------------------------------------------------------
void ConstDeclaration(int LEV,int &TX,int &DX) {
  if (SYM==IDENT) {
    GetSym();
    if (SYM==EQL||SYM==BECOMES) {
      if (SYM==BECOMES) Error(1);
      GetSym();
      if (SYM==NUMBER) { ENTER(CONSTANT,LEV,TX,DX); GetSym(); }
      else Error(2);
    }
    else Error(3);
  }
  else Error(4);
} /*ConstDeclaration()*/
//---------------------------------------------------------------------------
void ENTER(OBJECTS K, int LEV, int &TX, int &DX) { /*ENTER OBJECT INTO TABLE*/
  TX++;
  if (TX > TXMAX) Error(3);  // 爆表
  strcpy(TABLE[TX].NAME,ID); TABLE[TX].KIND=K;
  switch (K) {
        case CONSTANT:
             if (NUM>AMAX) { Error(31); NUM=0; }
             TABLE[TX].VAL=NUM;
             break;
        case VARIABLE:
             TABLE[TX].vp.LEVEL=LEV; TABLE[TX].vp.ADR=DX; DX++;
             break;
        case PROCEDUR:
             TABLE[TX].vp.LEVEL=LEV;
             break;
      }
} /*ENTER*/
//---------------------------------------------------------------------------
void VarDeclaration(int LEV,int &TX,int &DX) {
  /* 变量声明 var 变量名1,变量名2;
	 * 数组声明 var 数组名1[下界:上界],数组名2[下界:上界]; 
	 */
  if (SYM==IDENT) { 
    int down = 0, up = 0;
		bool is_not_const = false;
    // SYMBOL SYM1;
    // SYM1 = SYM;
    GetSym();
    if(SYM==LPAREN){
      ALFA mid;
      strcpy(mid, ID);
      GetSym();
      if(SYM==NUMBER || SYM==IDENT){
  				if (SYM == NUMBER)down = NUM; //直接赋值给下界
				  else{
					  down = isConst(POSITION(ID, TX)); //在名字表中查找下界的标识符,并判断是否为常量
					  if (down == -1){ is_not_const = true; Error(31); }	//下界不能是变量或过程
          }
          if(!is_not_const){
            GetSym();
            if(SYM!=COLON)Error(31);
            else{
              GetSym();
              if (SYM==NUMBER || SYM==IDENT){
                if (SYM == NUMBER) up = NUM; //直接赋值给上界
                else{
                  
                  up = isConst(POSITION(ID, TX)); //在名字表中查找上界的标识符,并判断是否为常量
                  if (up == -1){ is_not_const = true; Error(31); }	//上界不能是变量或过程
                }
                if (!is_not_const){
                  if (down<=up){
                    GetSym();
                    if(SYM==RPAREN){
                      EnterArray(LEV, TX, DX, down, up, mid);
                    }else Error(31);
                  }else Error(31);
                }
              }
            }
          }
      }
      GetSym(); // , or ;
    }else ENTER(VARIABLE,LEV,TX,DX); 
  }
  else Error(4);
} /*VarDeclaration()*/
//---------------------------------------------------------------------------
void EnterArray( int LEV, int &TX, int &DX, int start, int end, ALFA name){

	TX++; 
  strcpy(TABLE[TX].NAME, name);
	TABLE[TX].KIND = ARRAY;
  TABLE[TX].vp.LEVEL = LEV;
	TABLE[TX].vp.ADR = DX;
  TABLE[TX].LAW = start; //数组下界
	DX += (end - start + 1);//连续内存地址分配给该数组
}
//---------------------------------------------------------------------------
int isConst(int index){
	if (index == 0||TABLE[index].KIND!=CONSTANT||TABLE[index].VAL < 0){//在名字表中未找到 或 类型不是constant 或 常量值小于0
		Error(31);//常量未找到
		return -1;
	}
	else{
		return TABLE[index].VAL; //返回常量的值
	}
}
//---------------------------------------------------------------------------
void ListCode(int CX0) {  /*LIST CODE GENERATED FOR THIS Block*/
  if (Form1->ListSwitch->ItemIndex==0)
    for (int i=CX0; i<CX; i++) {
      String s=IntToStr(i);
      while(s.Length()<3)s=" "+s;
      s=s+" "+MNEMONIC[CODE[i].F]+" "+IntToStr(CODE[i].L)+" "+IntToStr(CODE[i].A);
        Form1->printfs(s.c_str());
        fprintf(FOUT,"%3d%5s%4d%4d\n",i,MNEMONIC[CODE[i].F],CODE[i].L,CODE[i].A);
    }
} /*ListCode()*/;
//---------------------------------------------------------------------------
void FACTOR(SYMSET FSYS, int LEV, int &TX) {
  int i;
  SYMSET nxtlev = SymSetNULL();
  TEST(FACBEGSYS,FSYS,24);
  while (SymIn(SYM,FACBEGSYS)) {
      if (SYM==IDENT) {
        i=POSITION(ID,TX);
        if (i==0) Error(11);
        else
          switch (TABLE[i].KIND){
            case CONSTANT: GEN(LIT,0,TABLE[i].VAL); break;
            case VARIABLE: GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR); break;
            case PROCEDUR: Error(21); break;
            case ARRAY: 
              GetSym(); //忽略左括号
              GetSym(); //获取数组下标
              EXPRESSION(nxtlev, LEV, TX, true, i); //下标的表达式，将相对偏移量（已经减掉下界值）放到栈顶
              GEN(LIT, 0, TABLE[i].vp.ADR); //将基地址放到栈顶
              GEN(OPR, 0, 2); //下标的相对偏移量+基地址=相对地址
              GEN(LOD2, LEV - TABLE[i].vp.LEVEL, 0);
              break;
          }
        GetSym();
          // 因子中的 i++ 和 i-- 运算 实现
        if(SYM==PLUSPLUS||SYM==MINUSMINUS){       
          GEN(LIT,0,1);                   // 将常数 1 放入栈顶
          if(SYM==PLUSPLUS){                // i++ 
            GEN(OPR,0,2);               // 次栈顶 = 次栈顶 + 栈顶
            GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);     // 将栈顶送入变量单元
            GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);     // 将变量送入栈顶
            // 加了 1 的减去 1
            GEN(LIT,0,1);
            GEN(OPR,0,3);
          }
          else if(SYM==MINUSMINUS){          // i-- 
            GEN(OPR,0,3);               // 次栈顶 = 次栈顶 - 栈顶
            GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);     // 将栈顶送入变量单元
            GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);     // 将变量送入栈顶
            // 减了 1 的加上 1
            GEN(LIT,0,1);
            GEN(OPR,0,2);
          }
          GetSym();
      }
      }
      else if (SYM==NUMBER) {
        if (NUM>AMAX) { Error(31); NUM=0; }
        GEN(LIT,0,NUM); GetSym();
      }
      else if (SYM==LPAREN) {
        GetSym(); EXPRESSION(SymSetAdd(RPAREN,FSYS),LEV,TX, false, 0);
        if (SYM==RPAREN) GetSym();
        else Error(22);
        TEST(FSYS,FACBEGSYS,23);
      }
      else if(SYM==ELSESYM){break;}//因子分析的过程中，将ELSESYM也当成其开始因子，没有对其操作会进入无限循环
      else if(SYM==TOSYM){ break;}//分析为To跳出
      else if(SYM==DOWNTOSYM){break;}//分析为DownTo跳出
      else if(SYM==DOSYM){break;}//分析为Do跳出
    }
}/*FACTOR*/
//---------------------------------------------------------------------------
void TERM(SYMSET FSYS, int LEV, int &TX) {  /*TERM*/
  SYMBOL MULOP;
  FACTOR(SymSetUnion(FSYS,SymSetNew(TIMES,SLASH)), LEV,TX);
  while (SYM==TIMES || SYM==SLASH) {
      MULOP=SYM;  GetSym();
      FACTOR(SymSetUnion(FSYS,SymSetNew(TIMES,SLASH)),LEV,TX);
      if (MULOP==TIMES) GEN(OPR,0,4);
      else GEN(OPR,0,5);
  }
} /*TERM*/;
//---------------------------------------------------------------------------
void EXPRESSION(SYMSET FSYS, int LEV, int &TX, bool nowArray, int index) {
  //FSYS 符号集合   LEV当前层次   TX符号表指针
  int i;
  SYMBOL ADDOP; //保存正负号
  if (SYM==PLUS || SYM==MINUS) {
    ADDOP=SYM; GetSym();
    TERM(SymSetUnion(FSYS,SymSetNew(PLUS,MINUS)),LEV,TX);
    if (ADDOP==MINUS) GEN(OPR,0,1);
  }
  else if(SYM==PLUSPLUS){     // ++i
    GetSym();
    if(SYM==IDENT){
      i=POSITION(ID,TX);
      if(i==0) Error(11);
      else if(TABLE[i].KIND!=VARIABLE){
        Error(12);
        i=0;
      }
      if(i!=0) GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
      GEN(LIT,0,1);
      GEN(OPR,0,2);
      if(i!=0){
        GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
      }
      GetSym();
    }
    else Error(45);
  }
  else if(SYM==MINUSMINUS){     // --i
    GetSym();
    if(SYM==IDENT){
      i=POSITION(ID,TX);
      if(i==0) Error(11);
      else if(TABLE[i].KIND!=VARIABLE){
        Error(12);
        i=0;
      }
      if(i!=0) GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
      GEN(LIT,0,1);
      GEN(OPR,0,3);
      if(i!=0){
        GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
      }
      GetSym();
    }
    else Error(45);
  }
  else TERM(SymSetUnion(FSYS,SymSetNew(PLUS,MINUS)),LEV,TX);
  while (SYM==PLUS || SYM==MINUS) {
    ADDOP=SYM; GetSym();
    TERM(SymSetUnion(FSYS,SymSetNew(PLUS,MINUS)),LEV,TX);
    if (ADDOP==PLUS) GEN(OPR,0,2);
    else GEN(OPR,0,3);
  }
  if (nowArray == true){
		GEN(LIT, 0, TABLE[index].LAW); //下界地址
		GEN(OPR, 0, 3);  //下标减去下界
	}
} /*EXPRESSION*/
//---------------------------------------------------------------------------
void CONDITION(SYMSET FSYS,int LEV,int &TX) {
  SYMBOL RELOP;
  if (SYM==ODDSYM) { GetSym(); EXPRESSION(FSYS, LEV, TX, false, 0); GEN(OPR,0,6); }
  else {
      EXPRESSION(SymSetUnion(SymSetNew(EQL,NEQ,LSS,LEQ,GTR,GEQ),FSYS),LEV,TX, false, 0);
      if (!SymIn(SYM,SymSetNew(EQL,NEQ,LSS,LEQ,GTR,GEQ))) Error(20);
      else {
        RELOP=SYM; GetSym(); EXPRESSION(FSYS, LEV, TX, false, 0);
        switch (RELOP) {
          case EQL: GEN(OPR,0,8);  break;
          case NEQ: GEN(OPR,0,9);  break;
          case LSS: GEN(OPR,0,10); break;
          case GEQ: GEN(OPR,0,11); break;
          case GTR: GEN(OPR,0,12); break;
          case LEQ: GEN(OPR,0,13); break;
        }
      }
  }
} /*CONDITION*/
//---------------------------------------------------------------------------
void STATEMENT(SYMSET FSYS,int LEV,int &TX) {   /*STATEMENT*/
  int i,CX1,CX2;
  /*
  CX1表示跳转语句（如if语句中的条件判断）需要跳转到的目标地址
  CX2表示无条件跳转语句（如goto语句）需要跳转到的目标地址
  */
  switch (SYM) {
    case IDENT:
      i=POSITION(ID,TX);
      if (i==0) Error(11);//找不到，说明未定义
      else if (TABLE[i].KIND!=VARIABLE && TABLE[i].KIND!=ARRAY) {Error(12); i=0;} /*ASSIGNMENT TO NON-VARIABLE*/
      if (TABLE[i].KIND==VARIABLE){
        GetSym();
        if (SYM==BECOMES){ 
          GetSym();
          EXPRESSION(FSYS,LEV,TX, false, 0);
          if (i!=0) GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        }
        else if(SYM==PLUSBECOMES) {    // +=
            GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            GetSym();
            EXPRESSION(FSYS,LEV,TX, false, 0);
            GEN(OPR,0,2);
            if (i!=0) GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        }
        else if(SYM==MINUSBECOMES) {    // -= 
            GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            GetSym();
            EXPRESSION(FSYS,LEV,TX, false, 0);
            GEN(OPR,0,3);
            if (i!=0) GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        }else if(SYM==PLUSPLUS) {          // i++ 逻辑运算
            if(i!=0) GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            GEN(LIT,0,1);
            GEN(OPR,0,2);
            if(i!=0) GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            GetSym();
        }
        else if(SYM==MINUSMINUS) {          // i-- 逻辑运算
            if(i!=0)
                GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            GEN(LIT,0,1);
            GEN(OPR,0,3);
            if(i!=0)
                GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            GetSym();
        }
        
        else Error(13);
      }
      else{ //数组 a[5]:=10;
        GetSym(); //忽略左括号
        GetSym(); //读取下标
        EXPRESSION(FSYS, LEV, TX,true,i); //下标的表达式，将相对偏移量（已经减掉下界值）放到栈顶
        GetSym(); //右括号
        GEN(LIT, 0, TABLE[i].vp.ADR); //将基地址放到栈顶
        GEN(OPR, 0, 2); //下标的相对偏移量+基地址=相对地址
        if (SYM==BECOMES){
          GetSym();
        }
        else{
          Error(13);
        }
        EXPRESSION(FSYS, LEV, TX, false, 0); /* 处理赋值符号右侧表达式 */
        if (i != 0){
          /* expression将执行一系列指令，但最终结果将会保存在栈顶，执行sto命令完成赋值 */
          GEN(STO2, LEV - TABLE[i].vp.LEVEL, 0);
        }
      }
      break;

    case READSYM:
      GetSym(); //左括号
      if (SYM!=LPAREN) Error(34);
      else
        do {
          GetSym();//标识符
          if (SYM==IDENT) i=POSITION(ID,TX);
          else i=0;
          if (i==0) Error(35);
          else if (TABLE[i].KIND != VARIABLE && TABLE[i].KIND!=ARRAY){Error(32);}/* read()参数表的标识符不是变量, thanks to amd */
          else {
            if (TABLE[i].KIND == VARIABLE){ //普通变量
							GEN(OPR, 0, 16);  /* 生成输入指令，读取值到栈顶 */
							GEN(STO, LEV-TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);   /* 储存到变量 */
							GetSym();
						}
            else{
              GetSym();
              EXPRESSION(FSYS, LEV, TX, true, i);
              GEN(LIT,0,TABLE[i].vp.ADR);
              GEN(OPR,0,2);
              GEN(OPR,0, 16);
              GEN(STO2,LEV-TABLE[i].vp.LEVEL,0);
            }
          }
        }while(SYM==COMMA);
      if (SYM!=RPAREN) {
        Error(33);
        while (!SymIn(SYM,FSYS)) GetSym();
      }
      else GetSym();
      break; /* READSYM */
    case WRITESYM:
      GetSym();
      if (SYM==LPAREN) {
        do {
        GetSym();
        EXPRESSION(SymSetUnion(SymSetNew(RPAREN,COMMA),FSYS),LEV,TX, false, 0);
        GEN(OPR,0,14);
        }while(SYM==COMMA);
        if (SYM!=RPAREN) Error(33);
        else GetSym();
      }
      GEN(OPR,0,15);
      break; /*WRITESYM*/
    case CALLSYM:
      GetSym();
      if (SYM!=IDENT) Error(14);
      else {
        i=POSITION(ID,TX);
        if (i==0) Error(11);
        else
        if (TABLE[i].KIND==PROCEDUR)
          GEN(CAL,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        else Error(15);
        GetSym();
      }
      break;
    case IFSYM:
      GetSym();
      CONDITION(SymSetUnion(SymSetNew(THENSYM,DOSYM),FSYS),LEV,TX);
      if (SYM==THENSYM) GetSym();
      else Error(16);
      CX1=CX;  GEN(JPC,0,0);      
      // STATEMENT(FSYS,LEV,TX);  CODE[CX1].A=CX; 注释掉
      STATEMENT(SymSetUnion(SymSetNew(ELSESYM),FSYS),LEV,TX);       
      if(SYM!=ELSESYM)
          CODE[CX1].A=CX;
      else {
          GetSym();
          CX2=CX;
          GEN(JMP,0,0);       //直接跳转，执行完Then后面的则跳转到条件语句最后面
          CODE[CX1].A=CX;       //回填条件跳转，填回else语句块中第一句
          STATEMENT(FSYS,LEV,TX);
          CODE[CX2].A=CX;         //回填直接跳转地址
      }
      break;
    case BEGINSYM:
      GetSym();
      STATEMENT(SymSetUnion(SymSetNew(SEMICOLON,ENDSYM),FSYS),LEV,TX);
      while (SymIn(SYM, SymSetAdd(SEMICOLON,STATBEGSYS))) {
        if (SYM==SEMICOLON) GetSym();
        else Error(10);
        STATEMENT(SymSetUnion(SymSetNew(SEMICOLON,ENDSYM),FSYS),LEV,TX);
      }
      if (SYM==ENDSYM) GetSym();
      else Error(17);
      break;
    case WHILESYM:
      CX1=CX; GetSym(); CONDITION(SymSetAdd(DOSYM,FSYS),LEV,TX);
      CX2=CX; GEN(JPC,0,0);
      if (SYM==DOSYM) GetSym();
      else Error(18);
      STATEMENT(FSYS,LEV,TX);
      GEN(JMP,0,CX1);
      CODE[CX2].A=CX;
      break;
    // 用来检验保留字是否添加成功的标志
    // 处理 ++i
    case PLUSPLUS:   
      GetSym();
      if(SYM==IDENT){
        i=POSITION(ID,TX);
        if(i==0) Error(11);
        else if(TABLE[i].KIND!=VARIABLE){
          Error(12);
          i=0;
        }
        if(i!=0) GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        GEN(LIT,0,1);
        GEN(OPR,0,2);
        if(i!=0) GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        GetSym();
      }
      else Error(45);
      break;
  // 处理 --i
    case MINUSMINUS: 
      GetSym();
      if(SYM==IDENT){
        i=POSITION(ID,TX);
        if(i==0) Error(11);
        else if(TABLE[i].KIND!=VARIABLE){
          Error(12);
          i=0;
        }
        if(i!=0) GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        GEN(LIT,0,1);
        GEN(OPR,0,3);
        if(i!=0) GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        GetSym();
      }
      else Error(45);
      break;
    case FORSYM:
      GetSym();//获取赋值语句表达式
      i=POSITION(ID,TX);//获取变量位置
      if (i==0) {
        Form1->printfs("following for must be an ident.");
        Error(11);
      }
      else if (TABLE[i].KIND!=VARIABLE) { /*ASSIGNMENT TO NON-VARIABLE*/
        Error(12);
      }
      STATEMENT(SymSetUnion(SymSetNew(TOSYM,DOWNTOSYM),FSYS),LEV,TX);//处理表达式，以TO或者DOWNTO结尾
      if(SYM==TOSYM){//判断后跟符号是否为TO
        GetSym();//获取TO后面的表达式
        CX1=CX;//记录每趟循环结束后回到的判断位置的地址
        GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        //将赋值语句的值（A）放到栈顶
        EXPRESSION(SymSetUnion(SymSetNew(TOSYM,DOWNTOSYM,DOSYM),FSYS), LEV,TX, false, 0);//对TO后面的表达式进行处理，其表达式的结果放置栈顶
        GEN(OPR,0,10);//比较次栈顶是否小于栈顶（A<B），AB出栈，结果入栈顶
        if(SYM==DOSYM){//判断后跟符号是否为DO
          CX2=CX;//记录DO执行开始时候的地址 
          GEN(JPC,0,0);//设置条件跳转
          GetSym();//获取DO后面的表达式
          STATEMENT(FSYS,LEV,TX);//处理DO后面编译操作
          GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          //将赋值语句的值（A）放到栈顶
          GEN(LIT,0,1);//设置步长为1
          GEN(OPR,0,2);//将栈顶两元素相加，结果放置栈顶（即A+1）
          GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          //修改A的值（完成A++操作）
          GEN(JMP,0,CX1);//无条件跳转至AB比较的地方（此时AB还未加载进进栈）
          CODE[CX2].A=CX;//确定JPC跳转的目的地址（若JPC判断为假，循环结束）
        }else Error(18);//缺少DO报错代号
      }
      else if(SYM==DOWNTOSYM){//判断后跟符号是否为DOWNTO
        GetSym();//获取DOWNTO后面的表达式
        CX1=CX;//记录每趟循环结束后回到的判断位置的地址
        EXPRESSION(SymSetUnion(SymSetNew(TOSYM,DOWNTOSYM,DOSYM),FSYS),LEV,TX, false, 0);//对DOWNTO后面的表达式进行处理，其表达式的结果放置栈顶
        if(SYM==DOSYM){//判断后跟符号是否为DO
          GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          //将赋值语句的值（A）放到栈顶
          GEN(OPR,0,10);//比较次栈顶是否小于栈顶（A<B），AB出栈，结果入栈顶
          CX2=CX;//记录DO执行开始时候的地址
          GEN(JPC,0,0);//设置条件跳转
          GetSym();//获取DO后面的表达式
          STATEMENT(FSYS,LEV,TX);//处理DO后面编译操作
          GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          //将赋值语句的值（A）放到栈顶
          GEN(LIT,0,1);//设置步长为1
          GEN(OPR,0,3);//将栈顶两元素相加，结果放置栈顶（即A-1）
          GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);//修改A的值（完成A--作）
          GEN(JMP,0,CX1);//无条件跳转至AB比较的地方（此时AB还未加载进进栈）
          CODE[CX2].A=CX;//确定JPC跳转的目的地址（若JPC判断为假，则循环结束）
        }else Error(18);//缺少DO报错代号
        }else Error(36);//缺少TO或者DOWNTO
        break;
  }
  TEST(FSYS,SymSetNULL(),19);
} /*STATEMENT*/
//---------------------------------------------------------------------------
void Block(int LEV, int TX, SYMSET FSYS) {
  int DX=3;    /*DATA ALLOCATION INDEX*/
  int TX0=TX;  /*INITIAL TABLE INDEX*/
  int CX0=CX;  /*INITIAL CODE INDEX*/
  TABLE[TX].vp.ADR=CX; GEN(JMP,0,0);
  if (LEV>LEVMAX) Error(32);
  do {
    if (SYM==CONSTSYM) {
      GetSym();
      do {
        ConstDeclaration(LEV,TX,DX);
        while (SYM==COMMA) {
          GetSym();  ConstDeclaration(LEV,TX,DX);
        }
        if (SYM==SEMICOLON) GetSym();
        else Error(5);
      }while(SYM==IDENT);
    }
    if (SYM==VARSYM) {
      GetSym();
      do {
        VarDeclaration(LEV,TX,DX);
        while (SYM==COMMA) { GetSym(); VarDeclaration(LEV,TX,DX); }
        if (SYM==SEMICOLON) GetSym();    
        else Error(5);
      }while(SYM==IDENT);
    }
    while (SYM==PROCSYM) {
      GetSym();
          if (SYM==IDENT) { ENTER(PROCEDUR,LEV,TX,DX); GetSym(); }
          else Error(4);
          if (SYM==SEMICOLON) GetSym();
          else Error(5);
          Block(LEV+1,TX,SymSetAdd(SEMICOLON,FSYS));
          if (SYM==SEMICOLON) {
            GetSym();
            TEST(SymSetUnion(SymSetNew(IDENT,PROCSYM),STATBEGSYS),FSYS,6);
          }
          else Error(5);
    }
    TEST(SymSetAdd(IDENT,STATBEGSYS), DECLBEGSYS,7);
  }while(SymIn(SYM,DECLBEGSYS));
  CODE[TABLE[TX0].vp.ADR].A=CX; //在循环结束后，将当前代码块的结束地址记录到符号表中，并生成一个初始化指令（INI）来分配数据段空间。
  TABLE[TX0].vp.ADR=CX;   /*START ADDR OF CODE*/
  TABLE[TX0].vp.SIZE=DX;  /*SIZE OF DATA SEGMENT*/
  GEN(INI,0,DX);
  STATEMENT(SymSetUnion(SymSetNew(SEMICOLON,ENDSYM),FSYS),LEV,TX);
  GEN(OPR,0,0);  /*RETURN*/
  TEST(FSYS,SymSetNULL(),8);
  ListCode(CX0);
} /*Block*/
//---------------------------------------------------------------------------
int BASE(int L,int B,int S[]) {
  int B1=B; /*FIND BASE L LEVELS DOWN*/
  while (L>0) { B1=S[B1]; L=L-1; } //根据指定的层次 按照访问链访问基地址
  return B1;
} /*BASE*/
//---------------------------------------------------------------------------
void Interpret() {
  const STACKSIZE = 500;
  int P,B,T;        /*PROGRAM BASE TOPSTACK REGISTERS*/
  INSTRUCTION I;
  int S[STACKSIZE];         /*DATASTORE*/
  Form1->printfs("----- RUN PL0 -----");
  fprintf(FOUT,"----- RUN PL0 -----\n");
  T=0; B=1; P=0;
  S[1]=0; S[2]=0; S[3]=0; //三个位置留出
  do {
    I=CODE[P]; P=P+1;
    // typedef  enum { LIT, OPR, LOD, STO, CAL, INI, JMP, JPC, STA, LDA, ARRCHECK } FCT;
    switch (I.F) {
      case LIT: T++; S[T]=I.A; break;/* 将a的值取到栈顶 */
      case OPR:
        switch (I.A) { /*OPERATOR*/
          case 0: /*RETURN*/ T=B-1; P=S[T+3]; B=S[T+2]; break; 
          case 1: S[T]=-S[T];  break;
          case 2: T--; S[T]=S[T]+S[T+1];   break;
          case 3: T--; S[T]=S[T]-S[T+1];   break;
          case 4: T--; S[T]=S[T]*S[T+1];   break;
          case 5: T--; S[T]=S[T] / S[T+1]; break; //bug  %改成了/
          case 6: S[T]=(S[T]%2!=0);        break;
          case 8: T--; S[T]=S[T]==S[T+1];  break;
          case 9: T--; S[T]=S[T]!=S[T+1];  break;
          case 10: T--; S[T]=S[T]<S[T+1];   break;
          case 11: T--; S[T]=S[T]>=S[T+1];  break;
          case 12: T--; S[T]=S[T]>S[T+1];   break;
          case 13: T--; S[T]=S[T]<=S[T+1];  break;
          case 14: Form1->printls("",S[T]); fprintf(FOUT,"%d\n",S[T]); T--;
                    break;
          case 15: /*Form1->printfs(""); fprintf(FOUT,"\n"); */ break;
          case 16: T++;  S[T]=InputBox("输入","请键盘输入：", 0).ToInt();
                    Form1->printls("? ",S[T]); fprintf(FOUT,"? %d\n",S[T]);
                break;
        }
        break;
      case LOD: T++; S[T]=S[BASE(I.L, B, S)+I.A]; break;/* 取相对当前过程的数据基地址为a的内存的值到栈顶 */
      case LOD2:S[T] = S[BASE(I.L, B, S) + S[T]];break;//覆盖基址和偏移量所在位置，以保证可以进行条件判断
      case STO: S[BASE(I.L,B,S)+I.A]=S[T]; T--; break; /* 栈顶的值存到相对当前过程的数据基地址为a的内存 */
      case STO2:S[BASE(I.L,B,S) + S[T-1]] = S[T]; T--; break;//[当前层地址+相对地址=真实地址] 赋值为 表达式的值
      case CAL:  /* 调用子过程 */
        S[T+1]=BASE(I.L,B,S); S[T+2]=B; S[T+3]=P;
        B=T+1; P=I.A; break;
      case INI: T=T+I.A;  break;
      case JMP: P=I.A; break;
      case JPC: if (S[T]==0) P=I.A;  T--;  break;
    } /*switch*/
  }while(P!=0);
  Form1->printfs("----- END PL0 -----");
  fprintf(FOUT,"----- END PL0 -----\n");
} /*Interpret*/
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonRunClick(TObject *Sender) {
  for (CH=' '; CH<='^'; CH++) SSYM[CH]=NUL;
  strcpy(KWORD[ 1],"BEGIN");    strcpy(KWORD[ 2],"CALL");
  strcpy(KWORD[ 3],"CONST");    strcpy(KWORD[ 4],"DO");
  strcpy(KWORD[ 5],"DOWNTO");  //增加保留字4. DOWNTO
  strcpy(KWORD[ 6],"ELSE"); // 增加保留字1。 ELSE
  strcpy(KWORD[ 7],"END");      
  strcpy(KWORD[ 8],"FOR");  // 增加保留字2。 FOR
  strcpy(KWORD[ 9],"IF");
  strcpy(KWORD[ 10],"ODD");      strcpy(KWORD[ 11],"PROCEDURE");
  strcpy(KWORD[ 12],"PROGRAM");  strcpy(KWORD[13],"READ");
  strcpy(KWORD[14],"RETURN");   // 增加保留字5。 RETURN
  strcpy(KWORD[15],"THEN");
  strcpy(KWORD[16],"TO");   // 增加保留字3。 TO
  strcpy(KWORD[17],"VAR");
  strcpy(KWORD[18],"WHILE");    strcpy(KWORD[19],"WRITE");

  // 新增5个保留字符号。 ELSESYM, FORSYM, TOSYM, DOWNTOSYM, RETURNSYM
  WSYM[ 1]=BEGINSYM;   WSYM[ 2]=CALLSYM;
  WSYM[ 3]=CONSTSYM;   WSYM[ 4]=DOSYM;
  WSYM[ 5]=DOWNTOSYM; //增加保留字符号4. DOWNTO
  WSYM[ 6]=ELSESYM; //增加保留字符号1. ELSESYM
  WSYM[ 7]=ENDSYM;     
  WSYM[ 8]=FORSYM;  //增加保留字符号2. FORSYM
  WSYM[ 9]=IFSYM;
  WSYM[ 10]=ODDSYM;     WSYM[ 11]=PROCSYM;
  WSYM[ 12]=PROGSYM;    WSYM[13]=READSYM;
  WSYM[ 14]=RETURNSYM;   //增加保留字符号5. RETURNSYM
  WSYM[15]=THENSYM;    
  WSYM[16]=TOSYM;   //增加保留字符号3. TOSYM
  WSYM[17]=VARSYM;
  WSYM[18]=WHILESYM;   WSYM[19]=WRITESYM;

  SSYM['+']=PLUS;      SSYM['-']=MINUS;
  SSYM['*']=TIMES;     SSYM['/']=SLASH;
  SSYM['(']=LPAREN;    SSYM[')']=RPAREN;
  SSYM['=']=EQL;       SSYM[',']=COMMA;
  SSYM['.']=PERIOD;    // SSYM['#']=NEQ; 去掉#，后面要改成<>
  SSYM[';']=SEMICOLON;
  SSYM[':']=COLON;

  strcpy(MNEMONIC[LIT],"LIT");   strcpy(MNEMONIC[OPR],"OPR");
  strcpy(MNEMONIC[LOD],"LOD");   strcpy(MNEMONIC[STO],"STO");
  strcpy(MNEMONIC[CAL],"CAL");   strcpy(MNEMONIC[INI],"INI");
  strcpy(MNEMONIC[JMP],"JMP");   strcpy(MNEMONIC[JPC],"JPC");
  strcpy(MNEMONIC[LOD2],"LOD2");   strcpy(MNEMONIC[STO2],"STO2");

  DECLBEGSYS=(int*)malloc(sizeof(int)*SYMNUM);
  STATBEGSYS=(int*)malloc(sizeof(int)*SYMNUM);
  FACBEGSYS =(int*)malloc(sizeof(int)*SYMNUM);
  for(int j=0; j<SYMNUM; j++) {
      DECLBEGSYS[j]=0;  STATBEGSYS[j]=0;  FACBEGSYS[j] =0;
  }
  DECLBEGSYS[CONSTSYM]=1;
  DECLBEGSYS[VARSYM]=1;
  DECLBEGSYS[PROCSYM]=1;
  STATBEGSYS[BEGINSYM]=1;
  STATBEGSYS[CALLSYM]=1;
  STATBEGSYS[IFSYM]=1;
  STATBEGSYS[WHILESYM]=1;
  STATBEGSYS[WRITESYM]=1;
  FACBEGSYS[IDENT] =1;
  FACBEGSYS[NUMBER]=1;
  FACBEGSYS[LPAREN]=1;
 

  if ((FIN=fopen((Form1->EditName->Text+".PL0").c_str(),"r"))!=0) {
    FOUT=fopen((Form1->EditName->Text+".COD").c_str(),"w");
    Form1->printfs("=== COMPILE PL0 ===");
    fprintf(FOUT,"=== COMPILE PL0 ===\n");
    ERR=0;
    CC=0; CX=0; LL=0; CH=' '; GetSym();
    if (SYM!=PROGSYM) Error(0);
    else {
      GetSym();
      if (SYM!=IDENT) Error(0);
      else {
        GetSym();
        if (SYM!=SEMICOLON) Error(5);
        else GetSym();
      }
    }
    Block(0,0,SymSetAdd(PERIOD,SymSetUnion(DECLBEGSYS,STATBEGSYS)));
    if (SYM!=PERIOD) Error(9);
    if (ERR==0) Interpret();
    else {
      Form1->printfs("ERROR IN PL/0 PROGRAM");
      fprintf(FOUT,"ERROR IN PL/0 PROGRAM");
    }
    fprintf(FOUT,"\n"); fclose(FOUT);
  }
}
//---------------------------------------------------------------------------

