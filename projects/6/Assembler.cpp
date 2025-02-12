#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
//二进制处理
#include <bitset>
#include <cctype>

using namespace std;

std::string removeSpaces(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c != ' ') { // 如果不是空格，添加到结果
            result += c;
        }
    }
    return result;
}   



void parseInstruction(const std::string& line, std::string& dest, std::string& comp, std::string& jump)
{
    dest="null";
    comp="";
    jump="null";
    size_t pos1=line.find('=');
    size_t pos2=line.find(';');
    if(pos1!=string::npos) 
    {
        dest=line.substr(0,pos1);
        if(pos2!=string::npos)
        {
            comp=line.substr(pos1+1,pos2-pos1-1);
            jump=line.substr(pos2+1);
        }
        else
        {
            comp=line.substr(pos1+1);
        }
    }
    else
    {
        if(pos2!=string::npos)
        {
            comp=line.substr(0,pos2);
            jump=line.substr(pos2+1);
        }
        else
            comp=line;
    }


}

int main()
{
    ifstream infile("Pong.asm");
    ofstream outfile("Pong.hack");
    string line;

    //建立表格symbols
    unordered_map<string,int> symbols; 
    for(int i=0;i<=15;++i)
    {
        symbols["R"+ to_string(i)]=i;
    }
    symbols["SCREEN"] = 16384;
    symbols["KBD"] = 24576;
    symbols["SP"] = 0;
    symbols["LCL"] = 1;
    symbols["ARG"] = 2;
    symbols["THIS"] = 3;
    symbols["THAT"] = 4;
    int count_line=0;
    //建表记录label_symbols
    //建表记录value_symbols
    int value_count=0;
    
    //构建instruction16位指令,初始化为全零
    string instruction(16,'0'); 
    //c指令对应表
    unordered_map<std::string, std::string> destTable = {
    {"null", "000"},
    {"M", "001"},
    {"D", "010"},
    {"MD", "011"},
    {"A", "100"},
    {"AM", "101"},
    {"AD", "110"},
    {"ADM", "111"}
    };

    unordered_map<std::string, std::string> compTable = {
    {"0", "0101010"},
    {"1", "0111111"},
    {"-1", "0111010"},
    {"D", "0001100"},
    {"A", "0110000"},
    {"M", "1110000"},
    {"!D", "0001101"},
    {"!A", "0110001"},
    {"!M", "1110001"},
    {"-D", "0001111"},
    {"-A", "0110011"},
    {"-M", "1110011"},
    {"D+1", "0011111"},
    {"A+1", "0110111"},
    {"M+1", "1110111"},
    {"D-1", "0001110"},
    {"A-1", "0110010"},
    {"M-1", "1110010"},
    {"D+A", "0000010"},
    {"D+M", "1000010"},
    {"D-A", "0010011"},
    {"D-M", "1010011"},
    {"A-D", "0000111"},
    {"M-D", "1000111"},
    {"D&A", "0000000"},
    {"D&M", "1000000"},
    {"D|A", "0010101"},
    {"D|M", "1010101"}
    };

    unordered_map<std::string, std::string> jumpTable = {
    {"null", "000"},
    {"JGT", "001"},
    {"JEQ", "010"},
    {"JGE", "011"},
    {"JLT", "100"},
    {"JNE", "101"},
    {"JLE", "110"},
    {"JMP", "111"}
    };


    if(!infile)
    {
        cerr<<"无法打开文件"<<endl;
        return 1;
    }            
    //建表
    while(getline(infile,line))
    {
        size_t start=line.find_first_not_of(" \t");
        size_t end=line.find_last_not_of(" \t");
        if(start==string::npos||end==string::npos)
            continue;
        line=line.substr(start,end-start+1);
        //省略注释行
        if(line.find("//")==0) 
        {
            continue;
        }
        //省略空格
        line=removeSpaces(line);

        if (line[0] == '(') {
                // 去掉括号
                string label = line.substr(1, line.size() - 2);
                symbols[label] = count_line; // 记录标签及其行号
                continue;        
            }
        ++count_line;
    }
    infile.clear(); // 清除 EOF 标志
    infile.seekg(0); // 移动指针到文件开头 

    //逐行处理文件内容
    while(getline(infile,line))
    {
        size_t start=line.find_first_not_of(" \t");
        size_t end=line.find_last_not_of(" \t");
        if(start==string::npos||end==string::npos)
            continue;
        line=line.substr(start,end-start+1);
        //省略注释行
        if(line.find("//")==0) 
        {
            continue;
        }
        //省略空格
        line=removeSpaces(line);

        if (line[0] == '(') 
            continue;
        //处理a指令
        if(line[0]=='@')
        {
            string address_label=line.substr(1);
            if(symbols.count(address_label)==0)
            {
        
                if(isdigit(address_label[0]))
                {
                    int number=stoi(address_label);
                    instruction="0"+bitset<15>(number).to_string();
                    outfile<<instruction<<endl;
                }
                else
                {
                    symbols[address_label]=16+value_count;
                    ++value_count;
                    int value=symbols[address_label];
                    instruction="0"+bitset<15>(value).to_string();
                    outfile<<instruction<<endl;
                }
            }
            else
            {
                int value=symbols[address_label];
                instruction="0"+bitset<15>(value).to_string();
                outfile<<instruction<<endl;
            }
        }
        else
        { 
            //处理C指令
            //dest    comp     jump
            //dest = comp ; jump
            string dest, comp, jump;
            parseInstruction(line, dest, comp, jump);
            string dest_code=destTable[dest];
            string comp_code=compTable[comp];
            string jump_code=jumpTable[jump];
            instruction="111"+comp_code+dest_code+jump_code;
            outfile<<instruction<<endl;
        } 
    }
    infile.close(); 
    outfile.close();

    return 0;
}