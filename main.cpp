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
char * rwtab[] = {"main","if","then","while","int","main","else","float","double","return","cout",_KEY_WORDEND };
FILE *fpout = NULL;

WORD * scanner();//扫描,获取到每个单词的种别码
int Irparser();//程序的判断
int yucu();//语句串的分析
int statement();//语句的分析
int expression();//表达式的判断
int term();//项的分析
int factor();//因子的分析
char m_getch();
char output[255];

int main(){
    int over=1;
    //以“#”结束
    p_input=0;
    printf("Open Your FIle(end with #):input.txt#\n");

    FILE *fp=NULL;
    fp = fopen("input.txt","r");
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
   if(oneword->typenum == 50)
   {
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

    statement();//对语句串的判断转换成对语句的判断
    while(oneword->typenum == 26){//  ；的种别码
    
        oneword = scanner();
        check_enter();//判断是否读取到回车
        statement();
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
    if(oneword->typenum != 33 && oneword->typenum != 0){//不同语句可以放在同一行的，此时读取到的不是回车
                                                     //而是其他变量，报错，缺少分隔符
        printf("第%d行：缺少分隔符\n",row);
        error_row++;//错的个数增加1
        kk=1;//标志是否已经出错了
        error_enter();//出错之后检查下回车，继续下一行的判断
        oneword = scanner();//读取下一行的首个字符
        yucu();//同样进行语句串的判断
    }
    return 0;
}

int statement(){//不同语句的判断：赋值语句，分支，循环
    //printf("%s",oneword->word);

    if(oneword->typenum == 5||oneword->typenum == 8){   //int float的赋值语句判断
        oneword= scanner();
        if(oneword->typenum==10){ //变量判断
            oneword = scanner();
            if(oneword->typenum == 25){  // “=”单词种别码是18
                oneword = scanner();
                expression();//读取到赋值号之后进行表达式的判断
            }else{//读取到的赋值号不是=，报错
                printf("第%d行：赋值号错误\n",row);
                error_row++;//出错的个数加1
                kk=1;//标志已经出错
                error_enter();//跳到下一行再次处理
                oneword = scanner();
                yucu();
            }
        }
        
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

int expression(){//表达式的分析
    term();
    while(oneword->typenum == 13 || oneword->typenum == 14){  //+，-
        oneword = scanner();
        term();
    }
    return 0;
}

int term(){//项的判断

    factor();//转入到因子的判断中
    while(oneword->typenum == 15 || oneword->typenum == 16){//*，/
        oneword = scanner();
        factor();//出现乘除之后继续进行因子的判断
    }
    return 0;
}

int factor(){//因子的判断

    if(oneword->typenum == 10 || oneword->typenum == 11)   //字符串或者数字串
        oneword = scanner();
    else if(oneword->typenum == 27){   //(
        oneword = scanner();
        expression();
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
        
    
    }else{
        printf("第%d行：表达式错误\n",row);
        error_row++;//出错个数加1
        kk=1;
        error_enter();
        oneword = scanner();
        yucu();
    }
    return 0;
}
