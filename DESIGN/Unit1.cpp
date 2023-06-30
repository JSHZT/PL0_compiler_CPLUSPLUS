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

const int SYMNUM = 43; //SYM����

typedef enum { NUL,     IDENT,    NUMBER,    PLUS,     MINUS,   TIMES,
            SLASH,    ODDSYM,   EQL,       NEQ,      LSS,     LEQ, 
              GTR,      GEQ,      LPAREN,    RPAREN,   COMMA,   SEMICOLON,
              PERIOD,   BECOMES,  BEGINSYM,  ENDSYM,  IFSYM,    THENSYM,
            WHILESYM, WRITESYM, READSYM,   DOSYM,   CALLSYM,      CONSTSYM,
              VARSYM,   PROCSYM,  PROGSYM, 
              ELSESYM, FORSYM, TOSYM, DOWNTOSYM, RETURNSYM,       // ��5����ELSE��FOR��TO��DOWNTO��RETURN
      PLUSBECOMES, MINUSBECOMES, PLUSPLUS, MINUSMINUS,// +=, -=, ++, --
      COLON //ð��
      } SYMBOL;

char *SYMOUT[] = {"NUL", "IDENT", "NUMBER", "PLUS", "MINUS", "TIMES",
          "SLASH", "ODDSYM", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ",
          "LPAREN", "RPAREN", "COMMA", "SEMICOLON", "PERIOD",
          "BECOMES", "BEGINSYM", "ENDSYM", "IFSYM", "THENSYM",
          "WHILESYM", "WRITESYM", "READSYM", "DOSYM", "CALLSYM",
          "CONSTSYM", "VARSYM", "PROCSYM", "PROGSYM"
      , "ELSESYM", "FORSYM", "TOSYM", "DOWNTOSYM", "RETURNSYM",     // ��5����ELSE��FOR��TO��DOWNTO��RETURN
      "PLUSBECOMES", "MINUSBECOMES", "PLUSPLUS", "MINUSMINUS",// +=, -=, ++, --
      "COLON"//���鱣���ַ���,ð��
       };

/*
NUL: һ���շ��ţ��������ڱ�ʾ��ֵ����Ч�ķ��š�
IDENT: ��ʶ����Identifier���ķ������ͣ����ڱ�ʾ��������������������ʵ������ơ�
NUMBER: ���֣�Number���ķ������ͣ����ڱ�ʾ�����򸡵���������ֵ��
PLUS: �Ӻţ�+���ķ������ͣ����ڱ�ʾ�ӷ����㡣
MINUS: ���ţ�-���ķ������ͣ����ڱ�ʾ�������㡣
TIMES: �˺ţ�*���ķ������ͣ����ڱ�ʾ�˷����㡣
SLASH: б�ܣ�/���ķ������ͣ����ڱ�ʾ�������㡣
ODDSYM: �����жϷ��ŵ����ͣ����ڱ�ʾ��ż�жϲ�����
EQL: ���ںţ�==���ķ������ͣ����ڱ�ʾ��ȱȽϡ�
NEQ: �����ںţ�!=���ķ������ͣ����ڱ�ʾ���ȱȽϡ�
LSS: С�ںţ�<���ķ������ͣ����ڱ�ʾС�ڱȽϡ�
LEQ: С�ڵ��ںţ�<=���ķ������ͣ����ڱ�ʾС�ڵ��ڱȽϡ�
GTR: ���ںţ�>���ķ������ͣ����ڱ�ʾ���ڱȽϡ�
GEQ: ���ڵ��ںţ�>=���ķ������ͣ����ڱ�ʾ���ڵ��ڱȽϡ�
LPAREN: �����ţ�(���ķ������ͣ����ڱ�ʾ����������õĿ�ʼ��
RPAREN: �����ţ�)���ķ������ͣ����ڱ�ʾ����������õĽ�����
COMMA: ���ţ�,���ķ������ͣ����ڱ�ʾ�����б������б��еķָ�����
SEMICOLON: �ֺţ�;���ķ������ͣ����ڱ�ʾ��������
PERIOD: ��ţ�.���ķ������ͣ����ڱ�ʾ���������
BECOMES: ��ֵ���ţ�:=���ķ������ͣ����ڱ�ʾ������ֵ������
BEGINSYM: BEGIN�ؼ��ֵķ������ͣ����ڱ�ʾ�������Ŀ�ʼ��
ENDSYM: END�ؼ��ֵķ������ͣ����ڱ�ʾ�������Ľ�����
IFSYM: IF�ؼ��ֵķ������ͣ����ڱ�ʾ�������Ŀ�ʼ��
THENSYM: THEN�ؼ��ֵķ������ͣ����ڱ�ʾ���������������֡�
WHILESYM: WHILE�ؼ��ֵķ������ͣ����ڱ�ʾѭ�����Ŀ�ʼ��
WRITESYM: WRITE�ؼ��ֵķ������ͣ����ڱ�ʾд������
READSYM: READ�ؼ��ֵķ������ͣ����ڱ�ʾ��������
DOSYM: DO�ؼ��ֵķ������ͣ����ڱ�ʾѭ������ѭ���塣
CALLSYM: CALL�ؼ��ֵķ������ͣ����ڱ�ʾ���̵���
CONSTSYM: CONST�ؼ��ֵķ������ͣ����ڱ�ʾ����������
VARSYM: VAR�ؼ��ֵķ������ͣ����ڱ�ʾ����������
PROCSYM: PROCEDURE�ؼ��ֵķ������ͣ����ڱ�ʾ����������
PROGSYM: PROGRAM�ؼ��ֵķ������ͣ����ڱ�ʾ����Ŀ�ʼ��
ELSESYM: ELSE�ؼ��ֵķ������ͣ����ڱ�ʾ�������ķ񶨷�֧��
FORSYM: FOR�ؼ��ֵķ������ͣ����ڱ�ʾѭ�����Ŀ�ʼ��
TOSYM: TO�ؼ��ֵķ������ͣ����ڱ�ʾѭ��������ֹ������
DOWNTOSYM: DOWNTO�ؼ��ֵķ������ͣ����ڱ�ʾѭ��������ֹ������
RETURNSYM: RETURN�ؼ��ֵķ������ͣ����ڱ�ʾ�������ء�
PLUSBECOMES: �ӵ��ڷ��ţ�+=���ķ������ͣ����ڱ�ʾ�ӷ���ֵ������
MINUSBECOMES: �����ڷ��ţ�-=���ķ������ͣ����ڱ�ʾ������ֵ������
PLUSPLUS: �������ţ�++���ķ������ͣ����ڱ�ʾ����������
MINUSMINUS: �Լ����ţ�--���ķ������ͣ����ڱ�ʾ�Լ�������
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
DECLBEGSYS  �����Ŀ�ʼ���ż���
STATBEGSYS  ���Ŀ�ʼ���ż��ϣ������������䣨��"if"��"while"�ȣ�����ֵ��������������
FACBEGSYS   ���ʽ���ӵĿ�ʼ���ż���
*/
struct {
  ALFA NAME;
  OBJECTS KIND;
  int LAW; /*�½�*/
  union {
    int VAL;   /*CONSTANT*/
    struct { int LEVEL,ADR,SIZE; } vp;  /*VARIABLE,PROCEDUR:, ARRAY*/
    
  };
  
} TABLE[TXMAX];

FILE *FIN,*FOUT;
int ERR;

void EXPRESSION(SYMSET FSYS, int LEV, int &TX, bool nowArray, int index);//���ʽ����
void TERM(SYMSET FSYS, int LEV, int &TX); //���
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
    while (!feof(FIN) && CH!=10)  //A����10ΪLF  ���з�
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
    do { //���ֲ��ң����������ַ���������ʹ�ö��ֲ����㷨����Ŀ���ַ���
      K=(i+J) / 2;
      if (strcmp(ID,KWORD[K])<=0) J=K-1;
      if (strcmp(ID,KWORD[K])>=0) i=K+1;
    }while(i<=J);
    if (i-1 > J) SYM=WSYM[K]; //�ҵ�Ŀ�� ij�˴�Խ�磬�ҵ��ؼ���
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
      else if(CH=='>') { SYM=NEQ; GetCh(); }      // ���Ⱥ������ <>
      else SYM=LSS;
      }
      else if (CH=='>'){
        GetCh();
      if (CH=='=') { SYM=GEQ; GetCh(); }
      else SYM=GTR;
    }
    else if(CH=='/') {      
      GetCh();
      if(CH=='*'){       // '/* */' ����ע��
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
      else if(CH=='/'){       // '//' ����ע��
        i=CX;
        while (CC!=LL) {
            GetCh();
        }
        GetSym();
      }else SYM=SLASH;
          }
    else if(CH=='+') {
      GetCh();
      if(CH=='=') { SYM=PLUSBECOMES; GetCh(); }  // ����� +=
      else if(CH=='+') { SYM=PLUSPLUS; GetCh(); }   // ����� ++
      else SYM=PLUS;
    } 
    else if(CH=='-') {
      GetCh();
      if(CH=='=') { SYM=MINUSBECOMES; GetCh(); }  // ����� -=
      else if(CH=='-') { SYM=MINUSMINUS; GetCh(); } // ����� --
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
  if (TX > TXMAX) Error(3);  // ����
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
  /* �������� var ������1,������2;
	 * �������� var ������1[�½�:�Ͻ�],������2[�½�:�Ͻ�]; 
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
  				if (SYM == NUMBER)down = NUM; //ֱ�Ӹ�ֵ���½�
				  else{
					  down = isConst(POSITION(ID, TX)); //�����ֱ��в����½�ı�ʶ��,���ж��Ƿ�Ϊ����
					  if (down == -1){ is_not_const = true; Error(31); }	//�½粻���Ǳ��������
          }
          if(!is_not_const){
            GetSym();
            if(SYM!=COLON)Error(31);
            else{
              GetSym();
              if (SYM==NUMBER || SYM==IDENT){
                if (SYM == NUMBER) up = NUM; //ֱ�Ӹ�ֵ���Ͻ�
                else{
                  
                  up = isConst(POSITION(ID, TX)); //�����ֱ��в����Ͻ�ı�ʶ��,���ж��Ƿ�Ϊ����
                  if (up == -1){ is_not_const = true; Error(31); }	//�Ͻ粻���Ǳ��������
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
  TABLE[TX].LAW = start; //�����½�
	DX += (end - start + 1);//�����ڴ��ַ�����������
}
//---------------------------------------------------------------------------
int isConst(int index){
	if (index == 0||TABLE[index].KIND!=CONSTANT||TABLE[index].VAL < 0){//�����ֱ���δ�ҵ� �� ���Ͳ���constant �� ����ֵС��0
		Error(31);//����δ�ҵ�
		return -1;
	}
	else{
		return TABLE[index].VAL; //���س�����ֵ
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
              GetSym(); //����������
              GetSym(); //��ȡ�����±�
              EXPRESSION(nxtlev, LEV, TX, true, i); //�±�ı��ʽ�������ƫ�������Ѿ������½�ֵ���ŵ�ջ��
              GEN(LIT, 0, TABLE[i].vp.ADR); //������ַ�ŵ�ջ��
              GEN(OPR, 0, 2); //�±�����ƫ����+����ַ=��Ե�ַ
              GEN(LOD2, LEV - TABLE[i].vp.LEVEL, 0);
              break;
          }
        GetSym();
          // �����е� i++ �� i-- ���� ʵ��
        if(SYM==PLUSPLUS||SYM==MINUSMINUS){       
          GEN(LIT,0,1);                   // ������ 1 ����ջ��
          if(SYM==PLUSPLUS){                // i++ 
            GEN(OPR,0,2);               // ��ջ�� = ��ջ�� + ջ��
            GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);     // ��ջ�����������Ԫ
            GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);     // ����������ջ��
            // ���� 1 �ļ�ȥ 1
            GEN(LIT,0,1);
            GEN(OPR,0,3);
          }
          else if(SYM==MINUSMINUS){          // i-- 
            GEN(OPR,0,3);               // ��ջ�� = ��ջ�� - ջ��
            GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);     // ��ջ�����������Ԫ
            GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);     // ����������ջ��
            // ���� 1 �ļ��� 1
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
      else if(SYM==ELSESYM){break;}//���ӷ����Ĺ����У���ELSESYMҲ�����俪ʼ���ӣ�û�ж���������������ѭ��
      else if(SYM==TOSYM){ break;}//����ΪTo����
      else if(SYM==DOWNTOSYM){break;}//����ΪDownTo����
      else if(SYM==DOSYM){break;}//����ΪDo����
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
  //FSYS ���ż���   LEV��ǰ���   TX���ű�ָ��
  int i;
  SYMBOL ADDOP; //����������
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
		GEN(LIT, 0, TABLE[index].LAW); //�½��ַ
		GEN(OPR, 0, 3);  //�±��ȥ�½�
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
  CX1��ʾ��ת��䣨��if����е������жϣ���Ҫ��ת����Ŀ���ַ
  CX2��ʾ��������ת��䣨��goto��䣩��Ҫ��ת����Ŀ���ַ
  */
  switch (SYM) {
    case IDENT:
      i=POSITION(ID,TX);
      if (i==0) Error(11);//�Ҳ�����˵��δ����
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
        }else if(SYM==PLUSPLUS) {          // i++ �߼�����
            if(i!=0) GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            GEN(LIT,0,1);
            GEN(OPR,0,2);
            if(i!=0) GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            GetSym();
        }
        else if(SYM==MINUSMINUS) {          // i-- �߼�����
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
      else{ //���� a[5]:=10;
        GetSym(); //����������
        GetSym(); //��ȡ�±�
        EXPRESSION(FSYS, LEV, TX,true,i); //�±�ı��ʽ�������ƫ�������Ѿ������½�ֵ���ŵ�ջ��
        GetSym(); //������
        GEN(LIT, 0, TABLE[i].vp.ADR); //������ַ�ŵ�ջ��
        GEN(OPR, 0, 2); //�±�����ƫ����+����ַ=��Ե�ַ
        if (SYM==BECOMES){
          GetSym();
        }
        else{
          Error(13);
        }
        EXPRESSION(FSYS, LEV, TX, false, 0); /* ����ֵ�����Ҳ���ʽ */
        if (i != 0){
          /* expression��ִ��һϵ��ָ������ս�����ᱣ����ջ����ִ��sto������ɸ�ֵ */
          GEN(STO2, LEV - TABLE[i].vp.LEVEL, 0);
        }
      }
      break;

    case READSYM:
      GetSym(); //������
      if (SYM!=LPAREN) Error(34);
      else
        do {
          GetSym();//��ʶ��
          if (SYM==IDENT) i=POSITION(ID,TX);
          else i=0;
          if (i==0) Error(35);
          else if (TABLE[i].KIND != VARIABLE && TABLE[i].KIND!=ARRAY){Error(32);}/* read()������ı�ʶ�����Ǳ���, thanks to amd */
          else {
            if (TABLE[i].KIND == VARIABLE){ //��ͨ����
							GEN(OPR, 0, 16);  /* ��������ָ���ȡֵ��ջ�� */
							GEN(STO, LEV-TABLE[i].vp.LEVEL, TABLE[i].vp.ADR);   /* ���浽���� */
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
      // STATEMENT(FSYS,LEV,TX);  CODE[CX1].A=CX; ע�͵�
      STATEMENT(SymSetUnion(SymSetNew(ELSESYM),FSYS),LEV,TX);       
      if(SYM!=ELSESYM)
          CODE[CX1].A=CX;
      else {
          GetSym();
          CX2=CX;
          GEN(JMP,0,0);       //ֱ����ת��ִ����Then���������ת��������������
          CODE[CX1].A=CX;       //����������ת�����else�����е�һ��
          STATEMENT(FSYS,LEV,TX);
          CODE[CX2].A=CX;         //����ֱ����ת��ַ
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
    // �������鱣�����Ƿ���ӳɹ��ı�־
    // ���� ++i
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
  // ���� --i
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
      GetSym();//��ȡ��ֵ�����ʽ
      i=POSITION(ID,TX);//��ȡ����λ��
      if (i==0) {
        Form1->printfs("following for must be an ident.");
        Error(11);
      }
      else if (TABLE[i].KIND!=VARIABLE) { /*ASSIGNMENT TO NON-VARIABLE*/
        Error(12);
      }
      STATEMENT(SymSetUnion(SymSetNew(TOSYM,DOWNTOSYM),FSYS),LEV,TX);//������ʽ����TO����DOWNTO��β
      if(SYM==TOSYM){//�жϺ�������Ƿ�ΪTO
        GetSym();//��ȡTO����ı��ʽ
        CX1=CX;//��¼ÿ��ѭ��������ص����ж�λ�õĵ�ַ
        GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
        //����ֵ����ֵ��A���ŵ�ջ��
        EXPRESSION(SymSetUnion(SymSetNew(TOSYM,DOWNTOSYM,DOSYM),FSYS), LEV,TX, false, 0);//��TO����ı��ʽ���д�������ʽ�Ľ������ջ��
        GEN(OPR,0,10);//�Ƚϴ�ջ���Ƿ�С��ջ����A<B����AB��ջ�������ջ��
        if(SYM==DOSYM){//�жϺ�������Ƿ�ΪDO
          CX2=CX;//��¼DOִ�п�ʼʱ��ĵ�ַ 
          GEN(JPC,0,0);//����������ת
          GetSym();//��ȡDO����ı��ʽ
          STATEMENT(FSYS,LEV,TX);//����DO����������
          GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          //����ֵ����ֵ��A���ŵ�ջ��
          GEN(LIT,0,1);//���ò���Ϊ1
          GEN(OPR,0,2);//��ջ����Ԫ����ӣ��������ջ������A+1��
          GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          //�޸�A��ֵ�����A++������
          GEN(JMP,0,CX1);//��������ת��AB�Ƚϵĵط�����ʱAB��δ���ؽ���ջ��
          CODE[CX2].A=CX;//ȷ��JPC��ת��Ŀ�ĵ�ַ����JPC�ж�Ϊ�٣�ѭ��������
        }else Error(18);//ȱ��DO�������
      }
      else if(SYM==DOWNTOSYM){//�жϺ�������Ƿ�ΪDOWNTO
        GetSym();//��ȡDOWNTO����ı��ʽ
        CX1=CX;//��¼ÿ��ѭ��������ص����ж�λ�õĵ�ַ
        EXPRESSION(SymSetUnion(SymSetNew(TOSYM,DOWNTOSYM,DOSYM),FSYS),LEV,TX, false, 0);//��DOWNTO����ı��ʽ���д�������ʽ�Ľ������ջ��
        if(SYM==DOSYM){//�жϺ�������Ƿ�ΪDO
          GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          //����ֵ����ֵ��A���ŵ�ջ��
          GEN(OPR,0,10);//�Ƚϴ�ջ���Ƿ�С��ջ����A<B����AB��ջ�������ջ��
          CX2=CX;//��¼DOִ�п�ʼʱ��ĵ�ַ
          GEN(JPC,0,0);//����������ת
          GetSym();//��ȡDO����ı��ʽ
          STATEMENT(FSYS,LEV,TX);//����DO����������
          GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          //����ֵ����ֵ��A���ŵ�ջ��
          GEN(LIT,0,1);//���ò���Ϊ1
          GEN(OPR,0,3);//��ջ����Ԫ����ӣ��������ջ������A-1��
          GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);//�޸�A��ֵ�����A--����
          GEN(JMP,0,CX1);//��������ת��AB�Ƚϵĵط�����ʱAB��δ���ؽ���ջ��
          CODE[CX2].A=CX;//ȷ��JPC��ת��Ŀ�ĵ�ַ����JPC�ж�Ϊ�٣���ѭ��������
        }else Error(18);//ȱ��DO�������
        }else Error(36);//ȱ��TO����DOWNTO
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
  CODE[TABLE[TX0].vp.ADR].A=CX; //��ѭ�������󣬽���ǰ�����Ľ�����ַ��¼�����ű��У�������һ����ʼ��ָ�INI�����������ݶοռ䡣
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
  while (L>0) { B1=S[B1]; L=L-1; } //����ָ���Ĳ�� ���շ��������ʻ���ַ
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
  S[1]=0; S[2]=0; S[3]=0; //����λ������
  do {
    I=CODE[P]; P=P+1;
    // typedef  enum { LIT, OPR, LOD, STO, CAL, INI, JMP, JPC, STA, LDA, ARRCHECK } FCT;
    switch (I.F) {
      case LIT: T++; S[T]=I.A; break;/* ��a��ֵȡ��ջ�� */
      case OPR:
        switch (I.A) { /*OPERATOR*/
          case 0: /*RETURN*/ T=B-1; P=S[T+3]; B=S[T+2]; break; 
          case 1: S[T]=-S[T];  break;
          case 2: T--; S[T]=S[T]+S[T+1];   break;
          case 3: T--; S[T]=S[T]-S[T+1];   break;
          case 4: T--; S[T]=S[T]*S[T+1];   break;
          case 5: T--; S[T]=S[T] / S[T+1]; break; //bug  %�ĳ���/
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
          case 16: T++;  S[T]=InputBox("����","��������룺", 0).ToInt();
                    Form1->printls("? ",S[T]); fprintf(FOUT,"? %d\n",S[T]);
                break;
        }
        break;
      case LOD: T++; S[T]=S[BASE(I.L, B, S)+I.A]; break;/* ȡ��Ե�ǰ���̵����ݻ���ַΪa���ڴ��ֵ��ջ�� */
      case LOD2:S[T] = S[BASE(I.L, B, S) + S[T]];break;//���ǻ�ַ��ƫ��������λ�ã��Ա�֤���Խ��������ж�
      case STO: S[BASE(I.L,B,S)+I.A]=S[T]; T--; break; /* ջ����ֵ�浽��Ե�ǰ���̵����ݻ���ַΪa���ڴ� */
      case STO2:S[BASE(I.L,B,S) + S[T-1]] = S[T]; T--; break;//[��ǰ���ַ+��Ե�ַ=��ʵ��ַ] ��ֵΪ ���ʽ��ֵ
      case CAL:  /* �����ӹ��� */
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
  strcpy(KWORD[ 5],"DOWNTO");  //���ӱ�����4. DOWNTO
  strcpy(KWORD[ 6],"ELSE"); // ���ӱ�����1�� ELSE
  strcpy(KWORD[ 7],"END");      
  strcpy(KWORD[ 8],"FOR");  // ���ӱ�����2�� FOR
  strcpy(KWORD[ 9],"IF");
  strcpy(KWORD[ 10],"ODD");      strcpy(KWORD[ 11],"PROCEDURE");
  strcpy(KWORD[ 12],"PROGRAM");  strcpy(KWORD[13],"READ");
  strcpy(KWORD[14],"RETURN");   // ���ӱ�����5�� RETURN
  strcpy(KWORD[15],"THEN");
  strcpy(KWORD[16],"TO");   // ���ӱ�����3�� TO
  strcpy(KWORD[17],"VAR");
  strcpy(KWORD[18],"WHILE");    strcpy(KWORD[19],"WRITE");

  // ����5�������ַ��š� ELSESYM, FORSYM, TOSYM, DOWNTOSYM, RETURNSYM
  WSYM[ 1]=BEGINSYM;   WSYM[ 2]=CALLSYM;
  WSYM[ 3]=CONSTSYM;   WSYM[ 4]=DOSYM;
  WSYM[ 5]=DOWNTOSYM; //���ӱ����ַ���4. DOWNTO
  WSYM[ 6]=ELSESYM; //���ӱ����ַ���1. ELSESYM
  WSYM[ 7]=ENDSYM;     
  WSYM[ 8]=FORSYM;  //���ӱ����ַ���2. FORSYM
  WSYM[ 9]=IFSYM;
  WSYM[ 10]=ODDSYM;     WSYM[ 11]=PROCSYM;
  WSYM[ 12]=PROGSYM;    WSYM[13]=READSYM;
  WSYM[ 14]=RETURNSYM;   //���ӱ����ַ���5. RETURNSYM
  WSYM[15]=THENSYM;    
  WSYM[16]=TOSYM;   //���ӱ����ַ���3. TOSYM
  WSYM[17]=VARSYM;
  WSYM[18]=WHILESYM;   WSYM[19]=WRITESYM;

  SSYM['+']=PLUS;      SSYM['-']=MINUS;
  SSYM['*']=TIMES;     SSYM['/']=SLASH;
  SSYM['(']=LPAREN;    SSYM[')']=RPAREN;
  SSYM['=']=EQL;       SSYM[',']=COMMA;
  SSYM['.']=PERIOD;    // SSYM['#']=NEQ; ȥ��#������Ҫ�ĳ�<>
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

