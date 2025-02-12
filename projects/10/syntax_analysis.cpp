#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <string>
#include <regex>
#include <vector>

using namespace std;

enum token_type {
    KEYWORD,
    SYMBOL,
    INTEGER_CONSTANT,
    STRING_CONSTANT,
    IDENTIFIER
};

//声明
class TokenParser; 

class Tokenizer {
public:
    Tokenizer(const std::string &filename) : file(filename) {
        // 文件在初始化列表中打开
    }

    friend class TokenParser;

    bool hasMoreTokens() const{
        return !current.empty() || !file.eof();
    }

    void advance(){
        if (tokenQueue.empty()) {
            string line;
            while (tokenQueue.empty() && getline(file, line)) {
                processLine(line);
            }
            if (tokenQueue.empty()) {
                current.clear();
                return;
            }
        }
        current = tokenQueue.front();
        currentType = tokenTypes.front();
        tokenQueue.erase(tokenQueue.begin());
        tokenTypes.erase(tokenTypes.begin());
    }

    token_type tokentype() const {
        return currentType;
    }

    string token_string() const {
        return current;
    }

    int test() const
    {
        return tokens.size();
    }

private:
    string current;
    token_type currentType;
    ifstream file;
    vector<string> tokenQueue;
    vector<token_type> tokenTypes;
    vector<pair<string,token_type>> tokens;

    const unordered_set<string> keywords = {
        "class", "constructor", "function", "method", "field", "static",
        "var", "int", "char", "boolean", "void", "true", "false",
        "null", "this", "let", "do", "if", "else", "while", "return"
    };

    const unordered_set<string> symbols = {
        "{", "}", "(", ")", "[", "]", ".", ",", ";", "+", "-", "*",
        "/", "&", "|", "<", ">", "=", "~"
    };

    void processLine(string line) {
        size_t commentPos = line.find("//");
        if (commentPos != string::npos) {
            line = line.substr(0, commentPos);
        }

        size_t pos = 0;
        while (pos < line.length()) {
            while (pos < line.length() && isspace(line[pos])) {
                pos++;
            }
            if (pos >= line.length()) break;

            if (line[pos] == '"') {
                size_t endPos = line.find('"', pos + 1);
                string str = line.substr(pos, endPos - pos + 1);
                tokens.push_back({str, STRING_CONSTANT});
                tokenQueue.push_back(str);
                tokenTypes.push_back(STRING_CONSTANT);
                pos = endPos + 1;
            }
            else if (symbols.find(string(1, line[pos])) != symbols.end()) {
                tokens.push_back({string(1, line[pos]), SYMBOL});
                tokenQueue.push_back(string(1, line[pos]));
                tokenTypes.push_back(SYMBOL);
                pos++;
            }
            else if (isdigit(line[pos])) {
                string num;
                while (pos < line.length() && isdigit(line[pos])) {
                    num += line[pos++];
                }
                tokens.push_back({num, INTEGER_CONSTANT});
                tokenQueue.push_back(num);
                tokenTypes.push_back(INTEGER_CONSTANT);
            }
            else if (isalpha(line[pos]) || line[pos] == '_') {
                string identifier;
                while (pos < line.length() && 
                       (isalnum(line[pos]) || line[pos] == '_')) {
                    identifier += line[pos++];
                }
                if (keywords.find(identifier) != keywords.end()) {
                    tokens.push_back({identifier, KEYWORD});
                    tokenQueue.push_back(identifier);
                    tokenTypes.push_back(KEYWORD);
                } else {
                    tokens.push_back({identifier, IDENTIFIER});
                    tokenQueue.push_back(identifier);
                    tokenTypes.push_back(IDENTIFIER);
                }
            }
            else {
                pos++;
            }
        }
    }
};



//parser

class TokenParser
{
public:
    TokenParser(const string& outputFile)
    {
        outputFileStream.open(outputFile);
    }
    ~TokenParser()
    {
        // 关闭输出文件流
        if (outputFileStream.is_open()) {
            outputFileStream.close();
        }
    }

    //构建语法树
    void Parser(const Tokenizer& t)
    {
        tokens=t.tokens;
        compileClass();
    }


private:
    vector<pair<string,token_type>> tokens;
    size_t index=0;
    ofstream outputFileStream; // 输出到文件

    // 输出匹配的内容到文件
    void outputXML(const string& tag, const string& content) {
        outputFileStream << "<" << tag << "> " << content << " </" << tag << ">" << endl;
    }

     void compileClass() {
        outputFileStream << "<class>" << endl;
        
        // 'class'
        outputToken("keyword", tokens[index].first);
        index++;

        // className
        outputToken("identifier", tokens[index].first);
        index++;

        // '{'
        outputToken("symbol", tokens[index].first);
        index++;

        // classVarDec*
        while (index < tokens.size() && 
              (tokens[index].first == "static" || tokens[index].first == "field")) {
            compileClassVarDec();
        }

        // subroutineDec*
        while (index < tokens.size() && 
              (tokens[index].first == "constructor" || 
               tokens[index].first == "function" || 
               tokens[index].first == "method")) {
            compileSubroutineDec();
        }

        // '}'
        outputToken("symbol", tokens[index].first);
        
        outputFileStream << "</class>" << endl;
    }

    void compileClassVarDec() {
        outputFileStream << "<classVarDec>" << endl;
        
        // 'static' | 'field'
        outputToken("keyword", tokens[index].first);
        index++;

        // type
        compileType();

        // varName
        outputToken("identifier", tokens[index].first);
        index++;

        // (',' varName)*
        while (tokens[index].first == ",") {
            outputToken("symbol", tokens[index].first);
            index++;
            outputToken("identifier", tokens[index].first);
            index++;
        }

        // ';'
        outputToken("symbol", tokens[index].first);
        index++;

        outputFileStream << "</classVarDec>" << endl;
    }

    void compileType() {
        if (tokens[index].first == "int" || 
            tokens[index].first == "char" || 
            tokens[index].first == "boolean") {
            outputToken("keyword", tokens[index].first);
        } else {
            outputToken("identifier", tokens[index].first);
        }
        index++;
    }

    void compileSubroutineDec() {
        outputFileStream << "<subroutineDec>" << endl;
        
        // constructor | function | method
        outputToken("keyword", tokens[index].first);
        index++;

        // void | type
        if (tokens[index].first == "void") {
            outputToken("keyword", tokens[index].first);
            index++;
        } else {
            compileType();
        }

        // subroutineName
        outputToken("identifier", tokens[index].first);
        index++;

        // '('
        outputToken("symbol", tokens[index].first);
        index++;

        // parameterList
        outputFileStream << "<parameterList>" << endl;
        compileParameterList();
        outputFileStream << "</parameterList>" << endl;

        // ')'
        outputToken("symbol", tokens[index].first);
        index++;

        // subroutineBody
        compileSubroutineBody();

        outputFileStream << "</subroutineDec>" << endl;
    }

    void compileParameterList() {
        if (tokens[index].first != ")") {
            compileType();
            outputToken("identifier", tokens[index].first);
            index++;

            while (tokens[index].first == ",") {
                outputToken("symbol", tokens[index].first);
                index++;
                compileType();
                outputToken("identifier", tokens[index].first);
                index++;
            }
        }
    }

    void compileSubroutineBody() {
        outputFileStream << "<subroutineBody>" << endl;
        
        // '{'
        outputToken("symbol", tokens[index].first);
        index++;

        // varDec*
        while (tokens[index].first == "var") {
            compileVarDec();
        }

        // statements
        outputFileStream << "<statements>" << endl;
        compileStatements();
        outputFileStream << "</statements>" << endl;

        // '}'
        outputToken("symbol", tokens[index].first);
        index++;

        outputFileStream << "</subroutineBody>" << endl;
    }

    void compileVarDec() {
        outputFileStream << "<varDec>" << endl;
        
        // 'var'
        outputToken("keyword", tokens[index].first);
        index++;

        // type
        compileType();

        // varName
        outputToken("identifier", tokens[index].first);
        index++;

        // (',' varName)*
        while (tokens[index].first == ",") {
            outputToken("symbol", tokens[index].first);
            index++;
            outputToken("identifier", tokens[index].first);
            index++;
        }

        // ';'
        outputToken("symbol", tokens[index].first);
        index++;

        outputFileStream << "</varDec>" << endl;
    }

    void outputToken(const string& type, const string& value) {
        if (type == "symbol") {
            if (value == "<") outputFileStream << "<symbol> &lt; </symbol>" << endl;
            else if (value == ">") outputFileStream << "<symbol> &gt; </symbol>" << endl;
            else if (value == "&") outputFileStream << "<symbol> &amp; </symbol>" << endl;
            else outputFileStream << "<symbol> " << value << " </symbol>" << endl;
        } else {
            outputFileStream << "<" << type << "> " << value << " </" << type << ">" << endl;
        }
    } 

    void compileStatements() {
        while (index < tokens.size() && tokens[index].first != "}") {
            if (tokens[index].first == "let") {
                compileLetStatement();
            }
            else if (tokens[index].first == "if") {
                compileIfStatement();
            }
            else if (tokens[index].first == "while") {
                compileWhileStatement();
            }
            else if (tokens[index].first == "do") {
                compileDoStatement();
            }
            else if (tokens[index].first == "return") {
                compileReturnStatement();
            }
        }
    }

    void compileLetStatement() {
        outputFileStream << "<letStatement>" << endl;
        
        // 'let'
        outputToken("keyword", tokens[index].first);
        index++;

        // varName
        outputToken("identifier", tokens[index].first);
        index++;

        // ('[' expression ']')?
        if (tokens[index].first == "[") {
            outputToken("symbol", tokens[index].first);
            index++;
            
            compileExpression();
            
            outputToken("symbol", tokens[index].first); // ']'
            index++;
        }

        // '='
        outputToken("symbol", tokens[index].first);
        index++;

        // expression
        compileExpression();

        // ';'
        outputToken("symbol", tokens[index].first);
        index++;

        outputFileStream << "</letStatement>" << endl;
    }

    void compileIfStatement() {
        outputFileStream << "<ifStatement>" << endl;
        
        // 'if'
        outputToken("keyword", tokens[index].first);
        index++;

        // '('
        outputToken("symbol", tokens[index].first);
        index++;

        // expression
        compileExpression();

        // ')'
        outputToken("symbol", tokens[index].first);
        index++;

        // '{'
        outputToken("symbol", tokens[index].first);
        index++;

        // statements
        outputFileStream << "<statements>" << endl;
        compileStatements();
        outputFileStream << "</statements>" << endl;

        // '}'
        outputToken("symbol", tokens[index].first);
        index++;

        // ('else' '{' statements '}')?
        if (index < tokens.size() && tokens[index].first == "else") {
            outputToken("keyword", tokens[index].first);
            index++;

            outputToken("symbol", tokens[index].first); // '{'
            index++;

            outputFileStream << "<statements>" << endl;
            compileStatements();
            outputFileStream << "</statements>" << endl;

            outputToken("symbol", tokens[index].first); // '}'
            index++;
        }

        outputFileStream << "</ifStatement>" << endl;
    }

    void compileWhileStatement() {
        outputFileStream << "<whileStatement>" << endl;
        
        // 'while'
        outputToken("keyword", tokens[index].first);
        index++;

        // '('
        outputToken("symbol", tokens[index].first);
        index++;

        // expression
        compileExpression();

        // ')'
        outputToken("symbol", tokens[index].first);
        index++;

        // '{'
        outputToken("symbol", tokens[index].first);
        index++;

        // statements
        outputFileStream << "<statements>" << endl;
        compileStatements();
        outputFileStream << "</statements>" << endl;

        // '}'
        outputToken("symbol", tokens[index].first);
        index++;

        outputFileStream << "</whileStatement>" << endl;
    }

    void compileDoStatement() {
        outputFileStream << "<doStatement>" << endl;
        
        // 'do'
        outputToken("keyword", tokens[index].first);
        index++;

        // subroutineCall
        compileSubroutineCall();

        // ';'
        outputToken("symbol", tokens[index].first);
        index++;

        outputFileStream << "</doStatement>" << endl;
    }

    void compileReturnStatement() {
        outputFileStream << "<returnStatement>" << endl;
        
        // 'return'
        outputToken("keyword", tokens[index].first);
        index++;

        // expression?
        if (tokens[index].first != ";") {
            compileExpression();
        }

        // ';'
        outputToken("symbol", tokens[index].first);
        index++;

        outputFileStream << "</returnStatement>" << endl;
    }

    void compileExpression() {
        outputFileStream << "<expression>" << endl;
        
        // term
        compileTerm();

        // (op term)*
        while (index < tokens.size() && 
               (tokens[index].first == "+" || tokens[index].first == "-" ||
                tokens[index].first == "*" || tokens[index].first == "/" ||
                tokens[index].first == "&" || tokens[index].first == "|" ||
                tokens[index].first == "<" || tokens[index].first == ">" ||
                tokens[index].first == "=")) {
            
            outputToken("symbol", tokens[index].first);
            index++;
            compileTerm();
        }

        outputFileStream << "</expression>" << endl;
    }

    void compileTerm() {
        outputFileStream << "<term>" << endl;
        
        if (tokens[index].second == INTEGER_CONSTANT) {
            outputToken("integerConstant", tokens[index].first);
            index++;
        }
        else if (tokens[index].second == STRING_CONSTANT) {
            outputToken("stringConstant", tokens[index].first.substr(1, tokens[index].first.length() - 2));
            index++;
        }
        else if (tokens[index].first == "true" || tokens[index].first == "false" ||
                 tokens[index].first == "null" || tokens[index].first == "this") {
            outputToken("keyword", tokens[index].first);
            index++;
        }
        else if (tokens[index].first == "(") {
            // '(' expression ')'
            outputToken("symbol", tokens[index].first);
            index++;
            
            compileExpression();
            
            outputToken("symbol", tokens[index].first);
            index++;
        }
        else if (tokens[index].first == "-" || tokens[index].first == "~") {
            // unaryOp term
            outputToken("symbol", tokens[index].first);
            index++;
            compileTerm();
        }
        else if (tokens[index].second == IDENTIFIER) {
            // Look ahead to determine the case
            string currentToken = tokens[index].first;
            index++;
            
            if (index < tokens.size() && tokens[index].first == "[") {
                // varName '[' expression ']'
                outputToken("identifier", currentToken);
                outputToken("symbol", tokens[index].first);
                index++;
                
                compileExpression();
                
                outputToken("symbol", tokens[index].first);
                index++;
            }
            else if (index < tokens.size() && 
                     (tokens[index].first == "(" || tokens[index].first == ".")) {
                // subroutineCall
                index--;  // Go back to handle subroutineCall properly
                compileSubroutineCall();
            }
            else {
                // varName
                outputToken("identifier", currentToken);
            }
        }

        outputFileStream << "</term>" << endl;
    }

    void compileSubroutineCall() {
        // subroutineName | (className|varName)
        outputToken("identifier", tokens[index].first);
        index++;

        if (tokens[index].first == ".") {
            // '.'
            outputToken("symbol", tokens[index].first);
            index++;

            // subroutineName
            outputToken("identifier", tokens[index].first);
            index++;
        }

        // '('
        outputToken("symbol", tokens[index].first);
        index++;

        // expressionList
        compileExpressionList();

        // ')'
        outputToken("symbol", tokens[index].first);
        index++;
    }

    void compileExpressionList() {
        outputFileStream << "<expressionList>" << endl;
        
        if (tokens[index].first != ")") {
            compileExpression();
            
            while (tokens[index].first == ",") {
                outputToken("symbol", tokens[index].first);
                index++;
                compileExpression();
            }
        }

        outputFileStream << "</expressionList>" << endl;
    }

};




void outputfileToken(ofstream& outfile,const token_type& type, const string &value) {
    switch (type) {
    case KEYWORD:
        outfile << "<keyword> " << value << " </keyword>" << endl;
        break;
    case SYMBOL:
        if (value == "<") {
            outfile << "<symbol> &lt; </symbol>" << endl;
        }
        else if (value == ">") {
            outfile << "<symbol> &gt; </symbol>" << endl;
        }
        else if(value=="&")
        {
            outfile << "<symbol> &amp; </symbol>" << endl;
        }
        else {
            outfile << "<symbol> " << value << " </symbol>" << endl;
        }
        break;
    case INTEGER_CONSTANT:
        outfile << "<integerConstant> " << value << " </integerConstant>" << endl;
        break;
    case STRING_CONSTANT:
        outfile << "<stringConstant> " << value.substr(1, value.length()-2) << " </stringConstant>" << endl;
        break;
    case IDENTIFIER:
        outfile << "<identifier> " << value << " </identifier>" << endl;
        break;
    }
}

int main() {

    //begin begin
    //步骤一
    //词法分析
    //分词器

    Tokenizer tknzr("SquareGame.jack");
    ofstream outfile("SquareGameT.xml");
    outfile<<"<tokens>"<<endl;
    while (tknzr.hasMoreTokens()) {
        tknzr.advance();
        token_type temp = tknzr.tokentype();
        // 添加检查，确保 token 不为空
        if (!tknzr.token_string().empty()) {
            outputfileToken(outfile,temp,tknzr.token_string());
        } 
    }
    outfile<<"</tokens>"<<endl;
    outfile.close();

    //步骤一完成
    //输出一个xml文件，保存词法分析的结果
    //分析结果保留在tokens当中
    //end end


//___________________________________
//___________________________________    分割线
//___________________________________ 

  

    //步骤二
    //语法分析过程
    //根据ll文法构建语法分析树

    TokenParser parser("SquareGame.xml");
    parser.Parser(tknzr);


    //语法树构建完毕

    return 0;
}
