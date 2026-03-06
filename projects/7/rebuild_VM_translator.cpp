#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

enum CommandType
{
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
};

string trim(const string &s)
{
    size_t l = s.find_first_not_of(" \t");
    size_t r = s.find_last_not_of(" \t");

    if (l == string::npos)
        return "";

    return s.substr(l, r - l + 1);
}

vector<string> split(const string &line)
{
    vector<string> tokens;
    stringstream ss(line);
    string token;

    while (ss >> token)
        tokens.push_back(token);

    return tokens;
}

class Parser
{
private:
    ifstream input;
    string currentLine;
    vector<string> tokens;

public:
    Parser(string filename)
    {
        input.open(filename);
    }

    bool hasMoreCommands()
    {
        return !input.eof();
    }

    bool advance()
    {
        while (getline(input, currentLine))
        {
            size_t pos = currentLine.find("//");
            if (pos != string::npos)
                currentLine = currentLine.substr(0, pos);

            currentLine = trim(currentLine);

            if (!currentLine.empty())
            {
                tokens = split(currentLine);
                return true;
            }
        }
        return false;
    }

    CommandType commandType()
    {
        string op = tokens[0];

        if (op == "push")
            return C_PUSH;
        if (op == "pop")
            return C_POP;
        if (op == "label")
            return C_LABEL;
        if (op == "goto")
            return C_GOTO;
        if (op == "if-goto")
            return C_IF;
        if (op == "function")
            return C_FUNCTION;
        if (op == "call")
            return C_CALL;
        if (op == "return")
            return C_RETURN;

        return C_ARITHMETIC;
    }

    string arg1()
    {
        return tokens[1];
    }

    int arg2()
    {
        return stoi(tokens[2]);
    }

    string op()
    {
        return tokens[0];
    }
};

class CodeWriter
{
private:
    ofstream output;
    int labelIndex = 0;

public:
    CodeWriter(string filename)
    {
        output.open(filename);
    }

    void writeArithmetic(string op)
    {
        if (op == "add")
            output << "@SP\nM=M-1\nA=M\nD=M\nA=A-1\nM=D+M\n";

        else if (op == "sub")
            output << "@SP\nM=M-1\nA=M\nD=M\nA=A-1\nM=M-D\n";

        else if (op == "neg")
            output << "@SP\nA=M\nA=A-1\nM=-M\n";

        else if (op == "and")
            output << "@SP\nM=M-1\nA=M\nD=M\nA=A-1\nM=D&M\n";

        else if (op == "or")
            output << "@SP\nM=M-1\nA=M\nD=M\nA=A-1\nM=D|M\n";

        else if (op == "not")
            output << "@SP\nA=M\nA=A-1\nM=!M\n";

        else if (op == "eq" || op == "gt" || op == "lt")
        {
            string labelTrue = "TRUE_" + to_string(labelIndex);
            string labelEnd = "END_" + to_string(labelIndex);
            labelIndex++;

            output << "@SP\nM=M-1\nA=M\nD=M\nA=A-1\nD=M-D\n";

            output << "@" << labelTrue << "\n";

            if (op == "eq")
                output << "D;JEQ\n";
            if (op == "gt")
                output << "D;JGT\n";
            if (op == "lt")
                output << "D;JLT\n";

            output << "@SP\nA=M-1\nM=0\n";
            output << "@" << labelEnd << "\n0;JMP\n";

            output << "(" << labelTrue << ")\n";
            output << "@SP\nA=M-1\nM=-1\n";

            output << "(" << labelEnd << ")\n";
        }
    }

    void writePushPop(CommandType type, string segment, int index)
    {
        if (type == C_PUSH && segment == "constant")
        {
            output << "@" << index << "\n";
            output << "D=A\n";
            output << "@SP\nA=M\nM=D\n";
            output << "@SP\nM=M+1\n";
        }
    }

    void writeLabel(string label)
    {
        output << "(" << label << ")\n";
    }

    void writeGoto(string label)
    {
        output << "@" << label << "\n0;JMP\n";
    }

    void writeIf(string label)
    {
        output << "@SP\nM=M-1\nA=M\nD=M\n";
        output << "@" << label << "\nD;JNE\n";
    }
};

int main(int argc, char *argv[])
{
    string vmfile = argv[1];
    string asmfile = vmfile.substr(0, vmfile.find_last_of('.')) + ".asm";

    Parser parser(vmfile);
    CodeWriter writer(asmfile);

    while (parser.advance())
    {
        CommandType type = parser.commandType();

        if (type == C_ARITHMETIC)
        {
            writer.writeArithmetic(parser.op());
        }
        else if (type == C_PUSH || type == C_POP)
        {
            writer.writePushPop(type, parser.arg1(), parser.arg2());
        }
        else if (type == C_LABEL)
        {
            writer.writeLabel(parser.arg1());
        }
        else if (type == C_GOTO)
        {
            writer.writeGoto(parser.arg1());
        }
        else if (type == C_IF)
        {
            writer.writeIf(parser.arg1());
        }
    }

    return 0;
}