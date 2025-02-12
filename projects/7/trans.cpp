#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// 去除字符串首尾的空格
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t"); // 找到第一个非空格字符
    size_t last = str.find_last_not_of(" \t");   // 找到最后一个非空格字符
    if (first == string::npos) return ""; // 如果没有非空格字符，返回空字符串
    return str.substr(first, last - first + 1); // 返回去掉首尾空格的子串
}

void process_file(string in,string out)
{
    ifstream input(in);
    ofstream output(out);
    int END=0;
    int andCount = 0, orCount = 0, notCount = 0;
    int eqCount = 0, gtCount = 0, ltCount = 0,lclCount=0,argCount=0,thisCount=0,thatCount=0; // 用于生成唯一标签 
    int funretCount = 0,funCount=0,retCount=0;
    if(!input)
    {
        cerr<<"open inputfile failed"<<endl;
    }
    string line;
    while(getline(input,line))
    {

        // 去掉行中的注释
        size_t commentPos = line.find("//");
        if (commentPos != string::npos) {
            line = line.substr(0, commentPos); // 截取注释之前的部分
        }
        //跳过空行以及删除后的注释
        if (line.empty()||line=="") 
        {
            continue; // 跳过空行
        }
        // 去除首尾空格
        line = trim(line);
        if(line=="add")
        {
            output<<"@SP\nM=M-1\nA=M\nD=M\nA=A-1\nM=D+M"<<endl;
        }
        else if(line=="sub")
        {
            output<<"@SP\nM=M-1\nA=M\nD=M\nA=A-1\nM=M-D"<<endl;
        }
        else if(line=="neg")
        {
            output<<"@SP\nA=M\nA=A-1\nM=-M"<<endl;
        }
        else if (line == "eq") {
            output << "@SP\nM=M-1\nA=M\nD=M\nA=A-1\nD=D-M\nM=0\n@EQUAL" << eqCount << "\nD;JEQ\n@END" << END << "\n0;JMP\n(EQUAL" << eqCount << ")\n@SP\nA=M\nA=A-1\nM=-1\n(END" << END << ")" << endl;
            eqCount++; // 增加计数
            END++;
        }
        else if (line == "gt") {
            output << "@SP\nM=M-1\nA=M\nD=M\nA=A-1\nD=M-D\nM=0\n@GREATER" << gtCount << "\nD;JGT\n@END" << END << "\n0;JMP\n(GREATER" << gtCount << ")\n@SP\nA=M\nA=A-1\nM=-1\n(END" << END << ")" << endl;
            gtCount++;
            END++;
        }
        else if (line == "lt") {
            output << "@SP\nM=M-1\nA=M\nD=M\nA=A-1\nD=M-D\nM=0\n@LESS" << ltCount << "\nD;JLT\n@END" << END << "\n0;JMP\n(LESS" << ltCount << ")\n@SP\nA=M\nA=A-1\nM=-1\n(END" << END << ")" << endl;
            ltCount++;
            END++;
        }
        else if(line == "and") 
        {
            output << "@SP\nM=M-1\nA=M\nD=M\nA=A-1\nM=D&M"<< endl;
            andCount++;
            END++;
        }
        else if(line == "or") 
        {
            output << "@SP\nM=M-1\nA=M\nD=M\nA=A-1\nM=D|M"<< endl;
            orCount++;
            END++;
        }
        else if(line == "not") 
        {
            output << "@SP\nA=M\nA=A-1\nM=!M"<< endl;
            notCount++;
            END++;
        }
        else if(line.substr(0, 14) == "push constant ")
        {
            string numberstr=line.substr(14);
            output<<"@"<<numberstr<<"\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1"<<endl;
        }
        else if(line.substr(0,10)=="pop local ")
        {
            string numberstr=line.substr(10);
            output<<"@LCL\nD=M\n@"<<numberstr<<"\nD=D+A\n@ADDR_lcl"<<lclCount<<"\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@ADDR_lcl"<<lclCount<<"\nA=M\nM=D"<<endl;
            lclCount++;
        }
        else if(line.substr(0,11)=="push local ")
        {
            string numberstr=line.substr(11);
            output<<"@LCL\nD=M\n@"<<numberstr<<"\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1"<<endl;
        }
        else if(line.substr(0,13)=="pop argument ")
        {
            string numberstr=line.substr(13);
            output<<"@ARG\nD=M\n@"<<numberstr<<"\nD=D+A\n@ADDR_arg"<<argCount<<"\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@ADDR_arg"<<argCount<<"\nA=M\nM=D"<<endl;
            argCount++;
        }
        else if(line.substr(0,14)=="push argument ")
        {
            string numberstr=line.substr(14);
            output<<"@ARG\nD=M\n@"<<numberstr<<"\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1"<<endl;
        }
        else if(line.substr(0,9)=="pop this ")
        {
            string numberstr=line.substr(9);
            output<<"@THIS\nD=M\n@"<<numberstr<<"\nD=D+A\n@ADDR_this"<<thisCount<<"\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@ADDR_this"<<thisCount<<"\nA=M\nM=D"<<endl;
            thisCount++;
        }
        else if(line.substr(0,10)=="push this ")
        {
            string numberstr=line.substr(10);
            output<<"@THIS\nD=M\n@"<<numberstr<<"\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1"<<endl;
        }
        else if(line.substr(0,9)=="pop that ")
        {
            string numberstr=line.substr(9);
            output<<"@THAT\nD=M\n@"<<numberstr<<"\nD=D+A\n@ADDR_that"<<thatCount<<"\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@ADDR_that"<<thatCount<<"\nA=M\nM=D"<<endl;
            thatCount++;
        }
        else if(line.substr(0,10)=="push that ")
        {
            string numberstr=line.substr(10);
            output<<"@THAT\nD=M\n@"<<numberstr<<"\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1"<<endl;
        }
        else if(line.substr(0,11)=="pop static ")
        {
            string numberstr=line.substr(11);
            int number=stoi(numberstr);
            number+=100;
            output<<"@SP\nM=M-1\nA=M\nD=M\n@"<<number<<"\nM=D\n"<<endl;
        }
        else if(line.substr(0,12)=="push static ")
        {
            string numberstr=line.substr(12);
            int number=stoi(numberstr);
            number+=100;
            output<<"@"<<number<<"\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1"<<endl;
        }
        else if(line.substr(0,9)=="pop temp ")
        {
            string numberstr=line.substr(9);
            int number=stoi(numberstr);
            number+=5;
            output<<"@SP\nM=M-1\nA=M\nD=M\n@"<<number<<"\nM=D\n"<<endl;
        }
        else if(line.substr(0,10)=="push temp ")
        {
            string numberstr=line.substr(10);
            int number=stoi(numberstr);
            number+=5;
            output<<"@"<<number<<"\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1"<<endl;
        }
        else if(line.substr(0,12)=="pop pointer ")
        {
            string numberstr=line.substr(12);
            int number=stoi(numberstr);
            if(number==0)
            {
                output<<"@SP\nM=M-1\nA=M\nD=M\n@THIS\nM=D"<<endl;
            }
            if(number==1)
            {
                output<<"@SP\nM=M-1\nA=M\nD=M\n@THAT\nM=D"<<endl;
            }
        }
        else if(line.substr(0,13)=="push pointer ")
        {
            string numberstr=line.substr(13);
            int number=stoi(numberstr);
            if(number==0)
            {
                output<<"@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1"<<endl;
            }
            if(number==1)
            {
                output<<"@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1"<<endl;
            }
        }
        else if(line.substr(0,6)=="label ")
        {
            string numberstr=line.substr(6);
            output<<"("<<numberstr<<")"<<endl;
        }
        else if(line.substr(0,5)=="goto ")
        {
            string numberstr=line.substr(5);
            output<<"@"<<numberstr<<"\n0;JMP"<<endl;
        }
        else if(line.substr(0,8)=="if-goto ")
        {
            string numberstr=line.substr(8);
            output<<"@SP\nM=M-1\nA=M\nD=M\n@"<<numberstr<<"\nD;JNE"<<endl;
        }
        else if(line.substr(0,5)=="call ")
        {
            string numberstr=line.substr(5);

            // 找到第一个空格的位置
            std::size_t space = numberstr.find(' ');
             // 获取第一个部分 函数名
            std::string function_name = numberstr.substr(0, space);
            // 获取剩余部分 参数个数
            std::string nargs = numberstr.substr(space + 1);
            int number=stoi(nargs);
            number=number+5;
            /*
            push retAddrLabel // Generates and pushes this label
            push LCL // Saves the caller’s LCL
            push ARG // Saves the caller’s ARG
            push THIS // Saves the caller’s THIS
            push THAT // Saves the caller’s THAT
            ARG = SP – 5 – nArgs // Repositions ARG
            LCL = SP // Repositions LCL
            goto functionName // Transfers control to the callee
            (retAddrLabel) // Injects this label into the code
            */

            output<<"@"<<function_name<<"_ret"<<funretCount<<"\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@LCL\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@ARG\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@"<<number<<"\nD=A\n@SP\nD=M-D\n@ARG\nM=D\n@SP\nD=M\n@LCL\nM=D\n@"<<function_name<<"\n0;JMP\n("<<function_name<<"_ret"<<funretCount<<")"<<endl;
            ++funretCount;

        }
        else if(line.substr(0,9)=="function ")
        {
            string numberstr=line.substr(9);

            // 找到第一个空格的位置
            std::size_t space = numberstr.find(' ');
             // 获取第一个部分 函数名
            std::string function_name = numberstr.substr(0, space);
            // 获取剩余部分 参数个数
            std::string nvars = numberstr.substr(space + 1);
            int number=stoi(nvars);
            if(number==0)
                output<<"("<<function_name<<")"<<endl;
            else
                output<<"("<<function_name<<")\n@"<<number<<"\nD=A\n@nvars\nM=0\nM=D\n("<<function_name<<"_LOOP)\n@SP\nA=M\nM=0\n@SP\nM=M+1\n@nvars\nM=M-1\nD=M\n@"<<function_name<<"_LOOP\nD;JNE"<<endl;
        }
        else if(line.substr(0,7)=="return")
        {
            /*
            // The code below creates and uses two temporary variables:
            // endFrame and retAddr;
            // The pointer notation *addr is used to denote: RAM[addr].
            endFrame = LCL // gets the address at the frame’s end
            retAddr = *(endFrame – 5) // gets the return address
            *ARG = pop() // puts the return value for the caller
            SP = ARG + 1 // repositions SP
            THAT = *(endFrame – 1) // restores THAT
            THIS = *(endFrame – 2) // restores THIS
            ARG = *(endFrame – 3) // restores ARG
            LCL = *(endFrame – 4) // restores LCL

            goto retAddr // jumps to the return address*/

            output<<"@5\nD=A\n@LCL\nA=M-D\nD=M\n@retAddr\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@ARG\nA=M\nM=D\n@ARG\nD=M\n@SP\nM=D+1\n@1\nD=A\n@LCL\nA=M-D\nD=M\n@THAT\nM=D\n@2\nD=A\n@LCL\nA=M-D\nD=M\n@THIS\nM=D\n@3\nD=A\n@LCL\nA=M-D\nD=M\n@ARG\nM=D\n@4\nD=A\n@LCL\nA=M-D\nD=M\n@LCL\nM=D\n@retAddr\nA=M\n0;JMP"<<endl;
        }
        else
            continue;
    }
    input.close();
    output.close();

}


int main(int argc,char *argv[])
{
    string vmfile=argv[1];
    string asmfile=vmfile.substr(0,vmfile.find_last_of('.'))+".asm";
    process_file(vmfile,asmfile);
    return 0;
}