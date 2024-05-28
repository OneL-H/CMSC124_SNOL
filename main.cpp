#include <string.h>

#include <stdexcept>

#include "token_class.h"

enum chr_type{
    chr_letter,
    chr_number,
    chr_point,
    chr_operator,
    chr_space
};

enum token_state{
    state_integer,
    state_float,
    state_string,
    state_operator
};

Token tokenize(std::string inp, std::string curr_state, std::vector<Token> varspace);
chr_type type_check(char inp);
std::vector<Token> scanner(std::string input_string, std::vector<Token> varspace);
void exit_snol();
void identifier_prechecker(std::vector<Token> token_stream);
bool expr_prechecker(std::vector<Token> token_stream);
Token expression_evaluator(std::vector<Token> token_stream, std::string expr_type);

int main() {
    std::vector<Token> variable_space;

    std::string inp = "";
    std::cout << "inp";
    try {
        std::vector<Token> token_stream1 = scanner("2 * 3 * 4 * 5", variable_space);
        expression_evaluator(token_stream1, "Integer");
    } catch (const std::exception& e) {
        std::cout << "ERROR CAUGHT: " << e.what() << '\n';
    }
}

// returns a token object with correct parameters
Token tokenize(std::string inp, token_state curr_state, std::vector<Token>* varspace) {
    if (curr_state == state_integer) {
        return Token(atoi(inp.c_str()));
    } else if (curr_state == state_float) {
        return Token((float)atof(inp.c_str()));
        // float here cause C++ complains if i dont
    } else if (curr_state == state_operator) {
        return Token(inp);
    } else if (curr_state == state_string) {
        std::vector<Token>::iterator i;
        for (i = (*varspace).begin(); i != (*varspace).end(); ++i) {
            if ((*i).getStringValue() == inp) {
                return *i;  // return matching token instead
            }  // check if inp matches stringValue of token in variable space
        }
        return Token(inp);
    }

    // if control reaches here you know ive fucked up
    return Token(inp);
}

// returns the four things a character can be

chr_type type_check(char inp) {
    if (isalpha(inp) || inp == '!') {
        return chr_letter;
    } else if (isdigit(inp)) {
        return chr_number;
    } else if (inp == '.') {
        return chr_point;
    } else if (inp == '=' || inp == '+' || inp == '-' || inp == '*' || inp == '/' || inp == '%') {
        return chr_operator;
    } else if (inp == ' ') {
        return chr_space;
    } else {
        throw std::runtime_error("LEXICAL ERROR: INVALID CHARACTER DETECTED");
    }
}

// scanner routine. string to token stream converter
std::vector<Token> scanner(std::string input_string, std::vector<Token>* varspace) {
    std::vector<Token> token_stream;
    chr_type char_type;

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
    if (char_type == chr_point || char_type == chr_operator) {
        throw std::runtime_error("SYNTAX ERROR: INVALD STARTING CHARACTER");
    }

    char_type = type_check(input_string.back());
    if (char_type == chr_point || char_type == chr_operator) {
        throw std::runtime_error("SYNTAX ERROR: INVALD ENDING CHARACTER");
    }

    // curr_state: what the scanner assumes is the type of thing its getting.
    token_state curr_state;

    if (char_type == chr_number) {
        curr_state = state_integer;
    } else if (char_type == chr_letter) {
        curr_state = state_string;
    }

    std::string buffer = "";
    buffer += input_string.at(0);

    // turns the string into tokens, and throws an error if it doesn't make sense
    int len = input_string.length();
    for (int pos = 1; pos < len; pos++) {
        char_type = type_check(input_string.at(pos));

        if (char_type == chr_space) {
            if (!buffer.empty()) {
                token_stream.push_back(tokenize(buffer, curr_state, varspace));
                buffer = "";
            }  // SPACE IS A DELIMITER. CHANGE TYPE IMMEDIATELY. */
            continue;
        } else if (curr_state == state_integer) {
            if (char_type == chr_point) {
                buffer += input_string.at(pos);
                curr_state = state_float;  // ACTION 1: add current char and change type
            } else if (char_type == chr_letter) {
                if (!buffer.empty()) token_stream.push_back(tokenize(buffer, curr_state, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_state = state_string;
            } else if (char_type == chr_operator) {
                if (!buffer.empty()) token_stream.push_back(tokenize(buffer, curr_state, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_state = state_operator;  // ACTION 2: tokenize and clear buffer, change type
            } else if (char_type == chr_number) {
                buffer += input_string.at(pos);  // ACTION 3: add current char
            }
        } else if (curr_state == state_string) {
            if (char_type == chr_point) {
                throw std::runtime_error("LEXICAL ERROR: VARIABLES CANNOT CONTAIN POINTS.");
                // A.B is not valid.
            } else if (char_type == chr_letter || char_type == chr_number) {
                buffer += input_string.at(pos);
                // will only reach here with LETTER, numbers after first are okay.
            } else if (char_type == chr_operator) {
                if (!buffer.empty()) token_stream.push_back(tokenize(buffer, curr_state, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_state = state_operator;
            }
        } else if (curr_state == state_operator) {
            if (char_type == chr_point) {
                throw std::runtime_error("LEXICAL ERROR: POINTS CANNOT FOLLOW OPERATORS.");

            } else if (char_type == chr_operator) {
                throw std::runtime_error("LEXICAL ERROR: DOUBLED OPERATORS.");

            } else if (char_type == chr_letter) {
                if (!buffer.empty()) token_stream.push_back(tokenize(buffer, curr_state, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_state = state_string;  // ACTION 2: tokenize and clear buffer, change type

            } else if (char_type == chr_number) {
                if (!buffer.empty()) token_stream.push_back(tokenize(buffer, curr_state, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_state = state_integer;  // ACTION 2: tokenize and clear buffer, change type
            }
        } else if (curr_state == state_float) {
            if (char_type == chr_point) {
                throw std::runtime_error("LEXICAL ERROR: FLOATING POINT NUMBER HAS TWO DECIMAL POINTS.");
            } else if (char_type == chr_letter) {
                if (!buffer.empty()) token_stream.push_back(tokenize(buffer, curr_state, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_state = state_string;
            } else if (char_type == chr_operator) {
                if (!buffer.empty()) token_stream.push_back(tokenize(buffer, curr_state, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_state = state_operator;  // ACTION 2: tokenize and clear buffer, change type
            } else if (char_type == chr_number) {
                buffer += input_string.at(pos);  // ACTION 3: add current char
            }
        }
    }

    token_stream.push_back(tokenize(buffer, curr_state, varspace));

    return token_stream;
}

void exit_snol() {
    std::cout << "\nInterpreter is now terminated...";
    exit(0);
}

void variable_assignment(Token* var, Token result){
    if(result.getTokenClass() == tkn_Integer){
        var->declareVar(result.getValue().val.int_val);
    }else if(result.getTokenClass() == tkn_Float){
        var->declareVar(result.getValue().val.float_val);
    }else{
        throw std::runtime_error("VARIABLE ASSIGNMENT: INVALID INPUT");
    }
}

// identifies command type and proceeds with execution.
// required variable space to be passed by reference, since we'll be changing it.
void identifier_prechecker(std::vector<Token> token_stream, std::vector<Token>* variable_space) {
    // CHECK ONE. ITS A COMMAND THING
    std::vector<Token>::iterator iter = token_stream.begin();
    if ((*iter).getTokenClass() == tkn_Command) { // PROCESSING COMMANDS.
        if ((*iter).getStringValue() == "EXIT!") { // COMMAND: EXIT!
            if (token_stream.begin() != token_stream.end()) {
                throw std::runtime_error("SYNTAX ERROR: INVALID EXIT SYNTAX");
            }

            exit_snol();
        } else if ((*iter).getStringValue() == "BEG") { // COMMAND: BEG
            ++iter;  // increment iter. its now on the second token
            if ((*iter).getTokenClass() == tkn_Variable && iter == token_stream.end()) {
                // check here is (its a variable) AND (its the end)
                std::string inp;

                std::getline(std::cin, inp);

                tokenize();
            } else {
                throw std::runtime_error("SYNTAX ERROR: BEG SHOULD BE FOLLOWED BY ONE VARIABLE");
            }
        } else if ((*iter).getStringValue() == "PRINT") {
            std::vector<Token> temp_token_stream;
                ++iter;
                for(; iter != token_stream.end(); ++iter){
                    temp_token_stream.push_back(*iter);
            }  
        }
    } else if ((*iter).getTokenClass() == tkn_Variable) {
        ++iter;
        if ((*iter).getTokenClass() == tkn_Operator && (*iter).getStringValue() == "=") {
        } else {
            throw std::runtime_error("SYNTAX ERROR: INVALID VARIABLE ASSIGNMENT/DECLARATION SCHEME");
        }
    }
}

// returns true if expression is valid, throws runtime error otheriwse
bool expr_prechecker(std::vector<Token> token_stream, std::vector<Token>::iterator token_iterator) {
    // CHECK 1: VALID BEGINNING / ENDING
    Token beg = *token_iterator; // i am smort
    Token end = *token_stream.end();
    token_class beg_class = beg.getTokenClass();
    token_class end_class = beg.getTokenClass();
    if (!(beg_class == tkn_Integer || beg_class == tkn_Float || beg_class == tkn_Variable)) {
        throw std::runtime_error("SYNTAX ERROR. EXPRESSION MUST BEGIN WITH A VALID OPERAND");
    }

    if (!(end_class == tkn_Integer || end_class == tkn_Float || end_class == tkn_Variable)) {
        throw std::runtime_error("SYNTAX ERROR. EXPRESSION MUST END WITH A VALID OPERAND");
    }

    // precheck to avoid screwing next step
    if (beg.getVariableType() == var_undeclared) {
        throw std::runtime_error("SYNTAX ERROR. EXPRESSION CONTAINS INVALID VARIABLE");
    }

    variable_type expr_type = beg.getVariableType();
    // if there's a float at start all variables must be float, vice versa. divisions can just call a check again

    bool expect_operand = true;
    for (std::vector<Token>::iterator i = token_stream.begin(); i != token_stream.end(); ++i) {
        if (expect_operand) {
            // expect operand. expect integer, float, or variable
            if (!((*i).getTokenClass() == tkn_Integer || (*i).getTokenClass() == tkn_Float || (*i).getTokenClass() == tkn_Operator)) {
                std::string error_msg = "SYNTAX ERROR. EXPECTED OPERAND SAW " + (*i).getTokenClass();
                throw std::runtime_error(error_msg);
            }

            // operand is undeclared variable
            if ((*i).getVariableType() == var_undeclared) {
                throw std::runtime_error("SYNTAX ERROR. EXPRESSION CONTAINS UNDECLARED VARIABLES");
            }

            // operand is of different variable type compared to start
            if ((*i).getVariableType() != expr_type) {
                throw std::runtime_error("SYNTAX ERROR. SNOL DISALLOWS MIXED VARIABLE TYPES IN EXPRESSION");
            }
        } else {
            // not an operator
            if (!((*i).getTokenClass() == tkn_Operator)) {
                std::string error_msg = "SYNTAX ERROR. EXPECTED OPERATOR SAW " + (*i).getTokenClass();
                throw std::runtime_error(error_msg);
            }

            // operator is =
            if (!((*i).getStringValue() == "=")) {
                std::string error_msg = "SYNTAX ERROR. EXPECTED OPERATOR SAW " + (*i).getStringValue();
                throw std::runtime_error(error_msg);
            }

            // operator is % and expression is assumed to be float
            if ((*i).getStringValue() == "%" && expr_type == var_Float) {
                throw std::runtime_error("SYNTAX ERROR. MODULO (%) OPERATION IS ONLY ALLOWED BETWEEN INTs");
            }
        }
    }

    // i know this check isn't necessary but i am still paranoid
    if (expect_operand) {
        throw std::runtime_error("SYNTAX ERROR. EXPRESSION MUST END WITH A VALID OPERAND");
    }

    return true;
}

Token expression_evaluator(std::vector<Token> token_stream, std::string expr_type) {
    // this is a horribly lazy method for expression evaluation because there's
    // no parentheses in the language description

    // initial push
    int len = token_stream.size();
    std::vector<Token>::iterator iter_base = token_stream.begin();
    for (; iter_base != token_stream.end(); ++iter_base) {
        if ((*iter_base).getTokenClass() == "Operator") {
            if ((*iter_base).getStringValue() == "*") {
                if (expr_type == "Integer") {
                    Token temp = Token((*(iter_base - 1)).getValue().val.int_val * (*(iter_base + 1)).getValue().val.int_val);
                    token_stream.erase(iter_base - 1, iter_base + 2);
                    token_stream.insert(iter_base - 1, temp);
                    iter_base -= 2;
                } else {
                    Token temp = Token((*(iter_base - 1)).getValue().val.float_val * (*(iter_base + 1)).getValue().val.float_val);
                    token_stream.erase(iter_base - 1, iter_base + 2);
                    token_stream.insert(iter_base - 1, temp);
                    iter_base -= 2;
                }
            } else if ((*iter_base).getStringValue() == "/") {
                if (expr_type == "Integer") {  // SPECIAL CASE. MUST PERFORM RECHECK.
                    int a = (*(iter_base - 1)).getValue().val.int_val;
                    int b = (*(iter_base + 1)).getValue().val.int_val;
                    Token temp = Token(a / b);
                    token_stream.erase(iter_base - 1, iter_base + 2);
                    token_stream.insert(iter_base - 1, temp);
                    iter_base -= 2;
                    if (a % b != 0) {
                        expr_prechecker(token_stream);
                        // throws an error if invalid, something something
                    }
                } else {  // IF INTEGER DIVISION AND ITS NOT WHOLE RESULTS IN BAD
                    Token temp = Token((*(iter_base - 1)).getValue().val.float_val / (*(iter_base + 1)).getValue().val.float_val);
                    token_stream.erase(iter_base - 1, iter_base + 2);
                    token_stream.insert(iter_base - 1, temp);
                    iter_base -= 2;
                }
            } else if ((*iter_base).getStringValue() == "%") {
                if (expr_type == "Integer") {
                    Token temp = Token((*(iter_base - 1)).getValue().val.int_val % (*(iter_base + 1)).getValue().val.int_val);
                    token_stream.erase(iter_base - 1, iter_base + 2);
                    token_stream.insert(iter_base - 1, temp);
                    iter_base -= 2;
                } else {
                    throw std::runtime_error("EXPRESSION ERROR: MODULO PERFORMED ON NON-WHOLE NUMBERS");
                }
            }
        }
    }

    iter_base = token_stream.begin();
    for (; iter_base != token_stream.end(); ++iter_base) {
        if ((*iter_base).getTokenClass() == "Operator") {
            if ((*iter_base).getStringValue() == "+") {
                if (expr_type == "Integer") {
                    Token temp = Token((*(iter_base - 1)).getValue().val.int_val + (*(iter_base + 1)).getValue().val.int_val);
                    token_stream.erase(iter_base - 1, iter_base + 2);
                    token_stream.insert(iter_base - 1, temp);
                    iter_base -= 2;
                } else {
                    Token temp = Token((*(iter_base - 1)).getValue().val.float_val + (*(iter_base + 1)).getValue().val.float_val);
                    token_stream.erase(iter_base - 1, iter_base + 2);
                    token_stream.insert(iter_base - 1, temp);
                    iter_base -= 2;
                }
            } else if ((*iter_base).getStringValue() == "-") {
                if (expr_type == "Integer") {
                    Token temp = Token((*(iter_base - 1)).getValue().val.int_val - (*(iter_base + 1)).getValue().val.int_val);
                    token_stream.erase(iter_base - 1, iter_base + 2);
                    token_stream.insert(iter_base - 1, temp);
                    iter_base -= 2;
                } else {
                    Token temp = Token((*(iter_base - 1)).getValue().val.float_val - (*(iter_base + 1)).getValue().val.float_val);
                    token_stream.erase(iter_base - 1, iter_base + 2);
                    token_stream.insert(iter_base - 1, temp);
                    iter_base -= 2;
                }
            }
        }
    }

    if(token_stream.begin() != token_stream.end()){
        throw std::runtime_error("UNKNOWN ERROR IN EXPRESSION EVALUATION. NO SINGULAR TOKEN RESULT.");
    }else{
        return *token_stream.begin();
    }
}