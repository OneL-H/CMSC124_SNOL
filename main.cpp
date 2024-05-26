#include <string.h>
#include <stdexcept>
#include "token_class.h"

std::string type_check(char inp);
// returns a token object with correct parameters
Token tokenize(std::string inp, std::string curr_type, std::vector<Token> varspace) {
    if (curr_type == "INTEGER") {
        return Token(atoi(inp.c_str()));
    } else if (curr_type == "FLOAT") {
        return Token((float)atof(inp.c_str()));
        // float here cause C++ complains if i dont
    } else if (curr_type == "OPERATOR") {
        return Token(inp);
    } else if (curr_type == "STRING"){
        std::vector<Token>::iterator i;
        for(i = varspace.begin(); i != varspace.end(); ++i){
            if((*i).getStringValue() == inp){
                return *i; // return matching token instead
            } // check if inp matches stringValue of token in variable space
        }
        return Token(inp);
    }   

    // if control reaches here you know ive fucked up
    return Token(inp);
}

// scanner routine. string to token stream converter
std::vector<Token> scanner(std::string input_string, std::vector<Token> varspace) {
    std::vector<Token> token_stream;
    std::string char_type;

    // ERROR CHECK 0: NO STRING
    if (input_string.empty()) {
        throw std::runtime_error("LEXICAL ERROR: NO INPUT STRING");
    }

    // clear string of spaces at start / end
    int start = (input_string.find_first_not_of(' ') != std::string::npos) ? input_string.find_first_not_of(' ') : -1;
    int end = (input_string.find_last_not_of(' ') != std::string::npos) ? input_string.find_last_not_of(' ') : -1;

    // ERROR CHECK 1: ALL SPACES
    if (start == -1 || end == -1) {
        throw std::runtime_error("LEXICAL ERROR: NO EXPRESSION DETECTED.");
    }  // throw std::runtime_error( error if bad. otherwise generate substring.

    // FIND SUBSTRING WHERE THE START ISN'T JUST SPACES
    input_string = input_string.substr(start, end - start + 1);

    // ERROR CHECK 2: INVALID STARTING CHARACTERS
    char_type = type_check(input_string.front());
    if (char_type == "POINT" || char_type == "OPERATOR") {
        throw std::runtime_error("SYNTAX ERROR: INVALD STARTING CHARACTER");
    }

    char_type = type_check(input_string.back());
    if (char_type == "POINT" || char_type == "OPERATOR") {
        throw std::runtime_error("SYNTAX ERROR: INVALD ENDING CHARACTER");
    }

    // curr_type: what the scanner assumes is the type of thing its getting.
    std::string curr_type;

    if (char_type == "NUMBER") {
        curr_type = "INTEGER";
    } else if (char_type == "LETTER") {
        curr_type = "STRING";
    }

    std::string buffer = "";
    buffer += input_string.at(0);

    // turns the string into tokens, and throws an error if it doesn't make sense
    int len = input_string.length();
    for (int pos = 1; pos < len; pos++) {
        char_type = type_check(input_string.at(pos));

        if (char_type == "SPACE") {
            
            if(!buffer.empty()){
                token_stream.push_back(tokenize(buffer, curr_type, varspace));
                buffer = "";
            } // SPACE IS A DELIMITER. CHANGE TYPE IMMEDIATELY. */

            continue;
        } else if (curr_type == "INTEGER") {
            if (char_type == "POINT") {
                buffer += input_string.at(pos);
                curr_type = "FLOAT";  // ACTION 1: add current char and change type
            } else if (char_type == "LETTER") {
                token_stream.push_back(tokenize(buffer, curr_type, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "STRING";
            } else if (char_type == "OPERATOR") {
                token_stream.push_back(tokenize(buffer, curr_type, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "OPERATOR";  // ACTION 2: tokenize and clear buffer, change type
            } else if (char_type == "NUMBER") {
                buffer += input_string.at(pos);  // ACTION 3: add current char
            }
        } else if (curr_type == "STRING") {
            if (char_type == "POINT") {
                throw std::runtime_error("LEXICAL ERROR: VARIABLES CANNOT CONTAIN POINTS.");
                // A.B is not valid.
            } else if (char_type == "LETTER" || char_type == "NUMBER") {
                buffer += input_string.at(pos);
                // will only reach here with LETTER, numbers after first are okay.
            } else if (char_type == "OPERATOR") {
                token_stream.push_back(tokenize(buffer, curr_type, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "OPERATOR";
            }
        } else if (curr_type == "OPERATOR") {
            if (char_type == "POINT") {
                throw std::runtime_error("LEXICAL ERROR: POINTS CANNOT FOLLOW OPERATORS.");

            } else if (char_type == "OPERATOR") {
                throw std::runtime_error("LEXICAL ERROR: DOUBLED OPERATORS.");

            } else if (char_type == "LETTER") {
                token_stream.push_back(tokenize(buffer, curr_type, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "STRING";  // ACTION 2: tokenize and clear buffer, change type

            } else if (char_type == "NUMBER") {
                token_stream.push_back(tokenize(buffer, curr_type, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "INTEGER";  // ACTION 2: tokenize and clear buffer, change type
            }
        } else if (curr_type == "FLOAT") {
            if (char_type == "POINT") {
                throw std::runtime_error("LEXICAL ERROR: FLOATING POINT NUMBER HAS TWO DECIMAL POINTS.");
            } else if (char_type == "LETTER") {
                token_stream.push_back(tokenize(buffer, curr_type, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "STRING";
            } else if (char_type == "OPERATOR") {
                token_stream.push_back(tokenize(buffer, curr_type, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "OPERATOR";  // ACTION 2: tokenize and clear buffer, change type
            } else if (char_type == "NUMBER") {
                buffer += input_string.at(pos);  // ACTION 3: add current char
            }
        }
    }

    token_stream.push_back(tokenize(buffer, curr_type, varspace));

    return token_stream;
}

int main() {
    std::vector<Token> variable_space;

    std::string inp = "";
    std::cout << "inp";
    try {
        std::vector<Token> token_stream1 = scanner("1.2.3.4.5", variable_space);
    } catch (const std::exception& e) {
        std::cout << "ERROR CAUGHT: " << e.what() << '\n';
    }
}

// returns the four things a character can be
std::string type_check(char inp) {
    if (isalpha(inp) || inp == '!') { 
        return "LETTER";
    } else if (isdigit(inp)) {
        return "NUMBER";
    } else if (inp == '.') {
        return "POINT";
    } else if (inp == '=' || inp == '+' || inp == '-' || inp == '*' || inp == '/' || inp == '%') {
        return "OPERATOR";
    } else if (inp == ' ') {
        return "SPACE";
    } else {
        throw std::runtime_error("LEXICAL ERROR: INVALID CHARACTER DETECTED");
    }
}

void exit_snol(){
    std::cout << "\nInterpreter is now terminated...";
    exit(0);
}

// prechecker one. identifies type of command
void identifier_prechecker(std::vector<Token> token_stream){
    // CHECK ONE. ITS A COMMAND THING
    std::vector<Token>::iterator iter = token_stream.begin();
    if((*iter).getTokenClass() == "Command"){
        if((*iter).getStringValue() == "EXIT!"){
            if(token_stream.begin() != token_stream.end()){
                throw std::runtime_error("SYNTAX ERROR: INVALID EXIT SYNTAX");
            }

            exit_snol();
        }else if((*iter).getStringValue() == "BEG"){
            ++iter; // increment iter. its now on the second token
            if((*iter).getTokenClass() == "Variable" && iter == token_stream.end()){
                // check here is (its a variable) AND (its the end)

            }else{
                throw std::runtime_error("SYNTAX ERROR: BEG SHOULD BE FOLLOWED BY ONE VARIABLE");
            }
        }else if((*iter).getStringValue() == "PRINT"){

        }
    }else if((*iter).getTokenClass() == "Variable"){
        ++iter;
        if((*iter).getTokenClass() == "Operator" && (*iter).getStringValue() == "="){

        }else{
            throw std::runtime_error("SYNTAX ERROR: INVALID VARIABLE ASSIGNMENT/DECLARATION SCHEME");
        }
    }
}

// returns true if expression is valid, throws runtime error otheriwse
bool expr_prechecker(std::vector<Token> token_stream){
    
    // CHECK 1: VALID BEGINNING / ENDING
    Token beg = *token_stream.begin();
    Token end = *token_stream.end();
    std::string beg_class = beg.getTokenClass();
    std::string end_class = beg.getTokenClass();
    if(!(beg_class == "Integer" || beg_class == "Float" || beg_class == "Variable")){
        throw std::runtime_error("SYNTAX ERROR. EXPRESSION MUST BEGIN WITH A VALID OPERAND");
    }

    if(!(end_class == "Integer" || end_class == "Float" || end_class == "Variable")){
        throw std::runtime_error("SYNTAX ERROR. EXPRESSION MUST END WITH A VALID OPERAND");
    }

    // precheck to avoid screwing next step
    if(beg.getVariableType() == "Undeclared"){
        throw std::runtime_error("SYNTAX ERROR. EXPRESSION CONTAINS INVALID VARIABLE");
    }

    std::string expr_type = beg.getVariableType();
    // if there's a float at start all variables must be float, vice versa. divisions can just call a check again

    bool expect_operand = true;
    for(std::vector<Token>::iterator i = token_stream.begin(); i != token_stream.end(); ++i){
        if(expect_operand){
            // expect operand. expect integer, float, or variable
            if(!((*i).getTokenClass() == "Integer" || (*i).getTokenClass() == "Float" || (*i).getTokenClass() == "Variable")){
                std::string error_msg = "SYNTAX ERROR. EXPECTED OPERAND SAW " + (*i).getTokenClass();
                throw std::runtime_error(error_msg);
            }

            // operand is undeclared variable
            if((*i).getVariableType() == "Undeclared"){
                throw std::runtime_error("SYNTAX ERROR. EXPRESSION CONTAINS UNDECLARED VARIABLES");
            }

            // operand is of different variable type compared to start
            if((*i).getVariableType() != expr_type){
                throw std::runtime_error("SYNTAX ERROR. SNOL DISALLOWS MIXED VARIABLE TYPES IN EXPRESSION");
            }
        }else{
            // not an operator
            if(!((*i).getTokenClass() == "Operator")){
                std::string error_msg = "SYNTAX ERROR. EXPECTED OPERATOR SAW " + (*i).getTokenClass();
                throw std::runtime_error(error_msg);
            }

            // operator is =
            if(!((*i).getStringValue() == "=")){
                std::string error_msg = "SYNTAX ERROR. EXPECTED OPERATOR SAW " + (*i).getStringValue();
                throw std::runtime_error(error_msg);
            }

            // operator is % and expression is assumed to be float
            if((*i).getStringValue() == "%" && expr_type == "Float"){
                throw std::runtime_error("SYNTAX ERROR. MODULO (%) OPERATION IS ONLY ALLOWED BETWEEN INTs");
            }
        }
    }

    // i know this check isn't necessary but i am still paranoid
    if(expect_operand){
        throw std::runtime_error("SYNTAX ERROR. EXPRESSION MUST END WITH A VALID OPERAND");
    }

    return true;
}