#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#define _KEY_WORDEND "waiting for your expanding"

using namespace std;

typedef struct{
	int typenum;
	char * word;
}WORD;
struct Quad{
    char result[8];
    char ag1[8];
    char op[8];
    char ag2[20];
};

int k=0;  //记录语意分析的临时变量的个数
WORD* oneword = new WORD;
char input[1000];
char token[255] = "";
int p_input; //指针
int p_token;
int kk;//标记是否已经出错，如果整个程序判断到最后没有出错，输出Success
int flag=0;
bool print_flag=true;
int row=1;
int error_row = 0;
char ch;
char * rwtab[] = {"main","if","then","while","int" ,"else","float","double","return",_KEY_WORDEND };
FILE *fpout = NULL;
struct Quad quad[20];   //记录四元式
int sum=1;   //记录四元式的数量
int schain = 0; //判断句子串是否正确，0为正确，1为错误
int nSuffix,ntc;
int conif=0;//记录语句是不是if语句，1代表是，0代表不是

WORD * scanner();//扫描,获取到每个单词的种别码
int Irparser();//程序的判断
int yucu();//语句串的分析
int statement(int * nChain);//语句的分析
char * expression();//表达式的判断
char * term();//项的分析
char * factor();//因子的分析
char m_getch();
char * newtemp();//获得一个临时变量名
void emit(char*result,char * arg1,char * op,char * arg2);//新增一个四元式
char *myitoa(int num,char *str,int radix) ;  //整数转换成字符串
void Match(char word);  //识别单词是否是需要的单词，不是报错和读取下一个单词
void Condition(int *etc,int *efc)  ;//获得if语句的四元式
void bp(int p,int t);//判断语句拼接代码块
void Statement_Block(int * nChain);
void  Statement_Sequence(int * nChain);
int merg(int p1,int p2);
char output[255];

int main(){

    int over=1;
    //以“#”结束
    p_input=0;
    printf("Open Your FIle(end with #):input.txt#\n");

    FILE *fp=NULL;
    fp = fopen("/Users/noteday/CLionProjects/compile/input.txt","r");
    if(fp == NULL){
        printf("Not found file!");
        return 0;
    }
    while((input[p_input]=getc(fp))!='#'){   //读取内容进input
        p_input++;
    }

    p_input=0;
    printf("Your words:\n%s \n",input);
    printf("词法分析序列：\n");
    while(over<1000&&over!=-1){
        oneword=scanner();
        if(oneword->typenum<1000 && oneword->typenum!=99){
            printf("(%d,%s)\n",oneword->typenum,oneword->word);
        }
        over=oneword->typenum;
    }

    printf("语法分析：\n");
    p_input=0;
    oneword=scanner();
    Irparser();

    printf("语意分析：\n");
    for (int i = 1; i < sum; i++){
        /* code */
        printf("(%d):(%s,%s,%s,%s)\n",i,quad[i].result,quad[i].ag1,quad[i].op,quad[i].ag2);
    }

    //结束提示
    printf("\npress q to exit:");
    scanf("%[^q]s",input);

    return 0;
}

//从输入缓冲区读取一个字符到ch中
char m_getch(){
	ch = input[p_input];
	p_input++;
	return ch;
}

//去掉空白符号，不包括回车
void getbc(){
	while (ch == ' ')
	{
		ch = input[p_input];
		p_input++;
	}
}

//拼接单词
void concat(){
	token[p_token] = ch;
	p_token++;
	token[p_token] = '\0';
}

//判断是否字母
int letter(){
	if (ch >= 'a'&&ch <= 'z' || ch >= 'A'&&ch <= 'Z')
		return 1;
	else
		return 0;
}

//判断是否数字
int digit(){
	if (ch >= '0'&&ch <= '9')
		return 1;
	else
		return 0;
}

//检索关键字表格
int reserve(){
	int i = 0;
	while(strcmp(rwtab[i], _KEY_WORDEND)){
		if (!strcmp(rwtab[i], token))
			return i + 1;
		i++;
	}
	return 10;//如果不是关键字，则返回种别码10
}

//回退一个字符
void retract(){

	p_input--;
}

//词法扫描程序
WORD * scanner(){
	WORD * myword = new WORD;
	myword->typenum = 10;  //初始值
	myword->word = "";
	p_token = 0;   //单词缓冲区指针
	m_getch();
	getbc();//去掉空白
	if (letter()){//判断读取到的首字母是字母
	        //如int
		while (letter() || digit()){
			concat(); //连接
			m_getch();
		}
		retract(); //回退一个字符
		myword->typenum = reserve();//判断是否为关键字，返回种别码
		myword->word = token;
		return myword;
	}else if (digit()){  //判断读取到的单词首字符是数字
		while (digit()){ //所有数字连接起来
			concat();
			m_getch();
		}
		retract();
		//数字单词种别码统一为11，单词自身的值为数字本身
		myword->typenum = 11;
		myword->word = token;
		return(myword);
	}
	else switch (ch){
	case '=':
		m_getch();//首字符为=,再读取下一个字符判断
		if (ch == '='){
			myword->typenum = 29;
			myword->word = "==";
			return(myword);
		}
		retract();//读取到的下个字符不是=，则要回退，直接输出=
		myword->typenum = 25;
		myword->word = "=";
		return(myword);
		break;
	case '+':
		myword->typenum = 13;
		myword->word = "+";
		return(myword);
		break;
	case '-':
		myword->typenum = 14;
		myword->word = "-";
		return(myword);
		break;
    case '/'://读取到该符号之后，要判断下一个字符是什么符号，判断是否为注释
        m_getch();//首字符为/,再读取下一个字符判断
		if (ch == '*'){ // 说明读取到的是注释
		     m_getch();
			while(ch != '*'){
                m_getch();//注释没结束之前一直读取注释，但不输出
                if(ch == '*'){
                    m_getch();
                    if(ch == '/'){//注释结束
                        myword->typenum = 999;
                        myword->word = "注释";
                        return (myword);
                        break;
                    }
                }

            }

		}else{
            retract();//读取到的下个字符不是*，即不是注释，则要回退，直接输出/
            myword->typenum = 16;
            myword->word = "/";
            return (myword);
            break;
            }
        case '*':
		myword->typenum = 15;
		myword->word = "*";
		return(myword);
		break;
	case '(':
		myword->typenum = 27;
		myword->word = "(";
		return(myword);
		break;
	case ')':
		myword->typenum = 28;
		myword->word = ")";
		return(myword);
		break;
	case '[':
		myword->typenum = 30;
		myword->word = "[";
		return(myword);
		break;
	case ']':
		myword->typenum = 31;
		myword->word = "]";
		return(myword);
		break;
	case '{':
		myword->typenum = 32;
		myword->word = "{";
		return(myword);
		break;
	case '}':
		myword->typenum = 33;
		myword->word = "}";
		return(myword);
		break;
	case ',':
		myword->typenum = 34;
		myword->word = ",";
		return(myword);
		break;
	case ':':

        retract();
        myword->typenum = 17;
        myword->word = ":";
        return(myword);
        break;

    case ';':
        myword->typenum = 26;
        myword->word = ";";
        return(myword);
        break;
	case '>':
		m_getch();
		if (ch == '='){
			myword->typenum = 24;
			myword->word = ">=";
			return(myword);
			break;
		}
		retract();
		myword->typenum = 23;
		myword->word = ">";
		return(myword);
		break;
	case '<':
		m_getch();
		if (ch == '='){
			myword->typenum = 22;
			myword->word = "<=";
			return(myword);
			break;
		}
		else if(ch == '<'){
             myword->typenum = 42;
			myword->word = "<<";
			return(myword);
			break;
        }else{
            retract();
            myword->typenum = 20;
            myword->word = "<";
            return (myword);
        }
	case '!':
		m_getch();
		if (ch == '='){
			myword->typenum = 36;
			myword->word = "!=";
			return(myword);
			break;
		}
		retract();
		myword->typenum = -1;
		myword->word = "ERROR";
		return(myword);
		break;
    case ' " ':
        myword->typenum = 37;
		myword->word = " \" ";
		return(myword);
		break;
    case '\n'://回车要作为单词返回，方便后面语法分析中一行一行判断
        myword->typenum = 50;
		myword->word = "回车";
		return(myword);
		break;
	case '\0':
		myword->typenum = 1000;
		myword->word = "OVER";
		return(myword);
		break;
    case '#':
        myword->typenum = 0;
        myword->word = "#";
        return (myword);
        break;
	default:
		myword->typenum = -1;
		myword->word = "ERROR";
		return(myword);
		break;
	}
}


void check_enter(){//检查到是回车，则将row++，取下一个字符
   if(oneword->typenum == 50){
       row++;
       oneword = scanner();
   }
   return;
}

void error_enter(){//出错之后检查回车直接跳到下一行

    while(oneword->typenum != 50){
        oneword = scanner();
    }
    row++;//注意该函数执行完之后，行数对应加1，但是此时oneword存储的是“回车”单词的信息
}


int Irparser(){
    if(oneword->typenum!=5){
        printf("main函数返回类型错误");
        return 0;
    }
    oneword = scanner();
    if(oneword->typenum == 1){//main单词的种别码就是1，该程序中是从main开始的

        oneword = scanner();
        check_enter();//检查下是否读取到回车了
        if(oneword->typenum!=27){
            printf("main缺少（");
        }else{
            oneword = scanner();
            if(oneword->typenum!=28){
                printf("main缺少）");
                return 0;
            }
            oneword = scanner();
            if(oneword->typenum!=32){
                printf("main缺少{");
            }
        }
        oneword = scanner();
        check_enter();
        yucu();//开始执行语句串的判断
        if(oneword->typenum == 33) {//检查到最后的}
            oneword = scanner();
            //cout << oneword->typenum <<endl;
            if(oneword->typenum == 0 && (kk == 0)){//取到了结束符,kk=0说明整个程序都是没有错误的
                printf("Success\n");
            }
        }else{//程序最后没有取到}，报错
                printf("第%d行：缺少‘}’\n",row);
                error_row++;
                kk=1;
        }
    }else{//处理缺少main的情况

        printf("开始处：缺少‘main’\n");
        error_row++;//即使第一行开始的不是begin，此时缺少begin也算是一个错误
        kk=1;//说明已经出错，即使后面没有报错，最后也不能报Success
        error_enter();
        oneword = scanner();
        yucu();//跳到下一行同样执行语句串的判断
        if(oneword->typenum == 33){

            oneword = scanner();
            if(oneword->typenum == 0)
                return 0;       //已经出错了，不可能输出Success，所以不需要判断kk

        }else{
            printf("第%d行：缺少‘}’\n",row);
            kk=1;
            error_row++;//错的个数增加1
        }
    }
    return 0;
}

int yucu(){  //语句串分析
    int nChain;
    statement(&nChain);//对语句串的判断转换成对语句的判断
    while(oneword->typenum == 26){//  ；的种别码
        oneword = scanner();
        check_enter();//判断是否读取到回车
        statement(&nChain);
    }
    if(oneword->typenum == 50){//50代表的是回车

        row++;
        oneword = scanner();
        if(oneword->typenum != 6 && oneword->typenum != 0){//读取到的是其他单词，说明缺少了分隔符

            printf("第%d行：缺少分隔符\n",row-1);
            error_row++;//错的个数增加1
            kk=1;//标志是否已经出错了
            yucu();//同样进行语句串的判断
        }
        else return 0;
    }
    if(oneword->typenum==2||oneword->typenum==5||oneword->typenum==4||oneword->typenum == 8){  //int ,float,while,if前面是if 语句或wile语句
        yucu();
    }
    return 0;
}

int statement(int *nChain){//不同语句的判断：赋值语句，分支，循环
    //printf("%s",oneword->word);
    char tt[8],eplace[8];
    int nChainTemp,nWouad,nfc;
    if(oneword->typenum == 5||oneword->typenum == 8){   //int float的赋值语句判断
        oneword= scanner();
        if(oneword->typenum==10){ //变量判断
            strcpy(tt,oneword->word);
            oneword = scanner();
            if(oneword->typenum == 25){  // “=”单词种别码是18

                oneword = scanner();
                strcpy(eplace,expression());//读取到赋值号之后进行表达式的判断
                if(schain==0){
                    emit(tt,eplace,"","");
                }else{
                    schain=0;
                }
               *nChain=0;
            }else{//读取到的赋值号不是=，报错
                printf("第%d行：赋值号错误\n",row);
                error_row++;//出错的个数加1
                kk=1;//标志已经出错
                error_enter();//跳到下一行再次处理
                oneword = scanner();
                yucu();
            }
        }

    }else if(oneword->typenum==2){
        oneword =scanner();
        conif=1;
        Match('(');
        Condition(&ntc,&nfc);
        bp(ntc,sum);
        Match(')');
        Statement_Block(&nChainTemp);
       // printf("nchaintemp:%d\n",nChainTemp);
        //printf("sum:%d\n",sum);
        bp(nfc,sum);
        //*nChain=merg(nChainTemp,nfc);
    }else if(oneword->typenum==4){
        oneword=scanner();
        nWouad=sum;
        Match('(');
        Condition(&ntc,&nfc);
        bp(ntc,sum);
        Match(')');
        Statement_Block(&nChainTemp);
        //bp(sum,nWouad);
        sprintf(tt,"%d",nWouad);
        emit(tt,"","j","");
        bp(nfc,sum);
        *nChain=nfc;
    }else if(oneword->typenum == 33 || oneword->typenum == 0){
        return 0;
    }else if(oneword->typenum==50){     //分号下一行语句是空行
        check_enter();
    }else{
        printf("第%d行：语句错误\n",row);
        error_row++;//出错个数加1
        kk=1;
        error_enter();
        oneword = scanner();
        yucu();
    }
    return 0;
}

char * expression(){//表达式的分析
    char *tp,*ep2,*eplace,*tt;
    tp = (char *)malloc(12);
    ep2 = (char *)malloc(12);
    eplace = (char *)malloc(12);
    tt = (char *)malloc(12);

    strcpy(eplace,term());
    while(oneword->typenum == 13 || oneword->typenum == 14){  //+，-
        if(oneword->typenum==13){
            strcpy(tt,"+");
        }else{
            strcpy(tt,"-");
        }
        oneword = scanner();
        strcpy(ep2,term());
        strcpy(tp,newtemp());
        emit(tp,eplace,tt,ep2);
        strcpy(eplace,tp);

    }
    return eplace;
}

char* term(){//项的判断

    char *tp,*ep2,*eplace,*tt;
    tp = (char *)malloc(12);
    ep2 = (char *)malloc(12);
    eplace = (char *)malloc(12);
    tt = (char *)malloc(12);

    strcpy(eplace,factor());//转入到因子的判断中
    while(oneword->typenum == 15 || oneword->typenum == 16){//*，/
        if(oneword->typenum==15){
            strcpy(tt,"*");
        }else{
            strcpy(tt,"/");
        }
        oneword = scanner();
        strcpy(ep2,factor()); //出现乘除之后继续进行因子的判断
        strcpy(tp,newtemp());
        emit(tp,eplace,tt,ep2);
        strcpy(eplace,tp);
    }
    return eplace;
}

char * factor(){//因子的判断
    char *fplace;
    fplace=(char *)malloc(12);
    strcpy(fplace,"");
    if(oneword->typenum == 10 || oneword->typenum == 11){   //字符串或者数字串
        strcpy(fplace,oneword->word);
        oneword = scanner();
        if(oneword->typenum == 27){  //字符串或者数字串后面直接跟括号而没有运算符
            printf("第%d行：缺少运算符错误\n",row);
            error_row++;//出错个数加1
            kk=1;
            error_enter();
            oneword = scanner();
            schain=1;
            yucu();

        }
    }
    else if(oneword->typenum == 27){   //(
        myitoa(sum,fplace,10);
        oneword = scanner();
        strcpy(fplace,expression());
        if(oneword->typenum==28){
            oneword = scanner();
        }else{
            printf("第%d行：缺少）错误\n",row);
            error_row++;//出错个数加1
            kk=1;
            error_enter();
            oneword = scanner();
            yucu();
        }

    }else if(conif==1){
        printf("1");
        return fplace;
    }else {
        printf("第%d行：表达式错误\n",row);
        error_row++;//出错个数加1
        kk=1;
        error_enter();
        oneword = scanner();
        yucu();
    }
    return fplace;
}

char *myitoa(int num,char *str,int radix) {  //整数转换成字符串
	/* 索引表 */
	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned unum; /* 中间变量 */
	int i=0,j,k;
	/* 确定unum的值 */
	if(radix==10&&num<0) /* 十进制负数 */
	{
		unum=(unsigned)-num;
		str[i++]='-';
	}
	else unum=(unsigned)num; /* 其它情况 */
	/* 逆序 */
	do
	{
		str[i++]=index[unum%(unsigned)radix];
		unum/=radix;
	}while(unum);
	str[i]='\0';
	/* 转换 */
	if(str[0]=='-') k=1; /* 十进制负数 */
	else k=0;
	/* 将原来的“/2”改为“/2.0”，保证当num在16~255之间，radix等于16时，也能得到正确结果 */
	char temp;
	for(j=k;j<=(i-k-1)/2.0;j++)
	{
		temp=str[j];
		str[j]=str[i-j-1];
		str[i-j-1]=temp;
	}
	return str;
}

char * newtemp(){  //生成新的临时变量
    char *p;
    char m[8];
    p= (char *)malloc(8);
    k++;
    myitoa(k,m,10);
    strcpy(p+1,m);
    p[0]='t';

    return p;
}

void emit(char*result,char * arg1,char * op,char * arg2)  {

    sprintf(quad[sum].op,op);
    sprintf(quad[sum].ag1,arg1);
    sprintf(quad[sum].ag2,arg2);
    sprintf(quad[sum].result,result);
    sum++;
    return;
}

void Match(char word){   //识别单词是否是需要的单词，不是报错和读取下一个单词
    if(*oneword->word==word){
        oneword=scanner();
    }else{
        printf("第%d行：缺少%c错误\n",row,word);
        error_row++;//出错个数加1
        kk=1;
        oneword=scanner();
    }
}

void Condition(int *etc,int *efc){
    char opp[3],*eplace1,*eplace2;
    char strTemp[4];

    eplace1 = expression();
    if(oneword->typenum==20||oneword->typenum==23){  //<,>
        sprintf(opp,"%s",oneword->word);
    }else{
        sprintf(opp,"%s",oneword->word);
    }

    oneword =scanner();
    eplace2=expression();
   // printf("%s",opp);
    *etc = sum;         //判断语句拼接代码块
    *efc = sum +1;
    sprintf(strTemp,"j%s",opp);
    emit("0",eplace1,strTemp,eplace2);
    emit("0","","j","");
    return;
}

void bp(int p,int t){
    int w,q=p;
    sprintf(quad[q].result,"%d",t);
}

void Statement_Block(int * nChain){
    Match('{');
    Statement_Sequence(nChain);
    Match('}');
    check_enter();
}

void  Statement_Sequence(int * nChain){
    yucu();
    //bp(*nChain,sum);

   // printf("nchain:%d\n",*nChain);
    return;
}

int merg(int p1,int p2){
    int p,nResult;
    if(p2==0){
        nResult=p1;

    }
    else{
        nResult=p=p2;
        sprintf(quad[p].result,"%d",p1);
    }
    return nResult;
}