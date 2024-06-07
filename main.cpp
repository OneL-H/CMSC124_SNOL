#include <string.h>
#include <stdexcept>
#include "token_class.h"

enum chr_type {
    chr_letter,
    chr_number,
    chr_point,
    chr_operator,
    chr_space
};

enum token_state {
    state_integer,
    state_float,
    state_string,
    state_operator,
    state_no_state
};

//Function Prototypes
Token tokenize(std::string inp, std::string curr_state, std::vector<Token>* varspace);
chr_type type_check(char inp);
std::vector<Token> scanner(std::string input_string, std::vector<Token>* varspace);
void exit_snol();
void identifier_prechecker(std::vector<Token> token_stream, std::vector<Token>* variable_space);
variable_type expr_prechecker(std::vector<Token> token_stream);
Token expression_evaluator(std::vector<Token> token_stream, variable_type expr_type);

int main() {
    std::vector<Token> variable_space;

    while (true) {
        std::string inp;
        std::cout << "\n\nREQUESTING INPUT: ";
        std::getline(std::cin, inp);
        try {
            std::vector<Token> token_stream1 = scanner(inp, &variable_space);
            identifier_prechecker(token_stream1, &variable_space);
        } catch (const std::exception& e) {
            std::cout << "SNOL> ERROR CAUGHT - " << e.what() << '\n';
        }
    }
}

/**
 * @brief Creates a token object from a string based on the current state and variable space.
 *
 * This function takes a string, the current token state (based on previous characters),
 * and the variable space, and creates a `Token` object based on the content of the string.
 *
 * @param inp The string to be tokenized.
 * @param curr_state The current token state (e.g., state_integer, state_float, etc.).
 * @param varspace A pointer to a vector of `Token` objects representing the variable space.
 * @return A `Token` object representing the extracted token from the input string.
 */
Token tokenize(std::string inp, token_state curr_state, std::vector<Token>* varspace) {
    if (curr_state == state_integer) {
        return Token(atoi(inp.c_str()));
    } else if (curr_state == state_float) {
        return Token((float)atof(inp.c_str()));
    } else if (curr_state == state_operator) {
        return Token(inp);
    } else if (curr_state == state_string) {
        Token temp = Token(inp);

        if (temp.getTokenClass() == tkn_Command) {
            return temp;
        } else {
            if ((*varspace).empty()) return Token(inp);

            //Checks whether input matches stringValue of token in variable space
            std::vector<Token>::iterator i;
            for (i = (*varspace).begin(); i != (*varspace).end(); ++i) {
                if ((*i).getStringValue() == inp) {
                    return *i;  // return matching token instead
                }  
            return Token(inp);
        }
    }
        return Token(inp); 
    }
}

/**
 * @brief Determines the type of a character.
 *
 * This function takes a character as input and returns an enumeration value representing its type.
 *
 * @param inp The character to be checked.
 * @return An enumeration value of type `chr_type` representing the character type.
 *   - `chr_letter`: If the character is a letter (a-z, A-Z, or !)
 *   - `chr_number`: If the character is a digit (0-9)
 *   - `chr_point`: If the character is a decimal point (.)
 *   - `chr_operator`: If the character is an operator (+, -, *, /, %, =)
 *   - `chr_space`: If the character is a space
 * @throws std::runtime_error If the character is invalid.
 */
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

/**
 * @brief Scans an input string and converts it into a vector of tokens.
 *
 * This function takes an input string and scans it, converting it into a vector of `Token` objects.
 * It also uses the provided `varspace` (variable space) to identify variables during scanning.
 *
 * @param input_string The input string to be scanned.
 * @param varspace A pointer to a vector of `Token` objects representing the variable space.
 * @return A vector containing the tokens extracted from the input string.
 * @throws std::runtime_error If the input string is empty, doesn't start/end with a valid character,
 *   or contains a lexical error.
 */
std::vector<Token> scanner(std::string input_string, std::vector<Token>* varspace) {
    std::vector<Token> token_stream;
    chr_type char_type;

    // ERROR CHECK 0: NO STRING
    if (input_string.empty()) {
        throw std::runtime_error("LEXICAL ERROR: NO INPUT STRING");
    }

    // Strip whitespace from input
    int start = (input_string.find_first_not_of(' ') != std::string::npos) ? input_string.find_first_not_of(' ') : -1;
    int end = (input_string.find_last_not_of(' ') != std::string::npos) ? input_string.find_last_not_of(' ') : -1;

    // ERROR CHECK 1: ALL SPACES
    if (start == -1 || end == -1) {
        throw std::runtime_error("LEXICAL ERROR: NO EXPRESSION DETECTED.");
    }

    // FIND SUBSTRING WHERE THE START ISN'T JUST SPACES
    input_string = input_string.substr(start, end - start + 1);

    // ERROR CHECK 2: INVALID STARTING CHARACTERS
    char_type = type_check(input_string.back());
    if (char_type == chr_point || char_type == chr_operator) {
        throw std::runtime_error("SYNTAX ERROR: INVALD ENDING CHARACTER");
    }

    char_type = type_check(input_string.front());
    if (char_type == chr_point || char_type == chr_operator) {
        throw std::runtime_error("SYNTAX ERROR: INVALD STARTING CHARACTER");
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
        // Assign state to current char
        if (curr_state == state_no_state) {
            buffer += input_string.at(pos);
            if (char_type == chr_letter) {
                curr_state = state_string;
            } else if (char_type == chr_number) {
                curr_state = state_integer;
            } else if (char_type == chr_operator) {
                curr_state = state_operator;
            } else if (char_type == chr_point) {
                throw std::runtime_error("LEXICAL ERROR: LONE POINT DETECTED");
            } else if (char_type == chr_space) {
                continue;
            } else {
                throw std::runtime_error("LEXICAL ERROR: UNKNOWN ERROR OCCURED");
            }
        } // SPACE IS A DELIMITER. CHANGE TYPE IMMEDIATELY.
        else if (char_type == chr_space) {
            if (!buffer.empty()) {
                token_stream.push_back(tokenize(buffer, curr_state, varspace));
                buffer = "";
                curr_state = state_no_state;
            }
            continue;
        } else if (curr_state == state_integer) {
            // ACTION 1: If alphanumeric add to buffer, and typecast to corresponding state
            // ACTION 2: tokenize and clear buffer, change type
            // ACTION 3: add current char

            if (char_type == chr_point) {
                buffer += input_string.at(pos);
                curr_state = state_float;
            } else if (char_type == chr_letter) {
                if (!buffer.empty()) token_stream.push_back(tokenize(buffer, curr_state, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_state = state_string;
            } 
            // ACTION 2: tokenize and clear buffer, change type
            else if (char_type == chr_operator) {
                if (!buffer.empty()) token_stream.push_back(tokenize(buffer, curr_state, varspace));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_state = state_operator;  
            } // ACTION 3: add current char
            else if (char_type == chr_number) {
                buffer += input_string.at(pos); 
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

void variable_assignment(Token* var, Token result) {
    if (result.getTokenClass() == tkn_Integer) {
        var->declareVar(result.getValue().val.int_val);
    } else if (result.getTokenClass() == tkn_Float) {
        var->declareVar(result.getValue().val.float_val);
    } else {
        throw std::runtime_error("VARIABLE ASSIGNMENT: INVALID INPUT");
    }
}

/**
 * @brief Pre-processes the token stream and performs actions based on commands or expressions.
 *
 * This function takes a vector of tokens obtained from the scanner and a variable space vector.
 * It identifies the type of command or expression in the token stream and performs the corresponding action.
 * Actions include processing commands like `EXIT!`, `BEG`, and `PRINT`,
 * handling variable assignment (`VAR = EXPR`), and evaluating expressions (`EXPR`).
 *
 * @param token_stream The vector of tokens obtained from the scanner.
 * @param variable_space A pointer to a vector of `Token` objects representing the variable space.
 * @throws std::runtime_error If there's a syntax error, invalid input, or an error
 *   during variable assignment/retrieval.
 */
void identifier_prechecker(std::vector<Token> token_stream, std::vector<Token>* variable_space) {
    // FIVE CASES:
    // COMMAND - EXIT
    // COMMAND - BEG
    // COMMAND - PRINT
    // VAR = EXPR
    // EXPR

    // CHECK ONE. ITS A COMMAND THING
    std::vector<Token>::iterator iter = token_stream.begin();
    if ((*iter).getTokenClass() == tkn_Command) {   // PROCESSING COMMANDS.
        if ((*iter).getStringValue() == "EXIT!") {  // COMMAND: EXIT!
            if (token_stream.size() != 1) {
                throw std::runtime_error("SYNTAX ERROR: INVALID EXIT SYNTAX");
            }

            exit_snol();
        } else if ((*iter).getStringValue() == "BEG") {  // COMMAND: BEG
            ++iter;                                      // increment iter. its now on the second to
            std::cout << token_stream.size();
            if ((*iter).getTokenClass() == tkn_Variable && token_stream.size() == 2) {
                // check here is (its a variable) AND (its the end)

                std::string inp;
                std::cout << "SNOL> Please enter value for [" << (*iter).getStringValue() << "]:\nInput: ";
                std::getline(std::cin, inp);
                std::vector<Token> temp_vec = scanner(inp, variable_space);

                if (temp_vec.size() != 1) {
                    throw std::runtime_error("INPUT ERROR: INVALID INPUT SIZE");
                }

                if (temp_vec[0].getTokenClass() != tkn_Float && temp_vec[0].getTokenClass() != tkn_Integer) {
                    throw std::runtime_error("INPUT ERROR: INVALID INPUT");
                }

                Token* var_to_assign = &(*iter);

                bool var_unassigned = (var_to_assign->getVariableType() == var_undeclared);

                // A FIX. THE TOKEN IS INDEPENDENT TO THE ONE IN THE VARSPACE. SEARCH FOR THE CORRECT ONE TO ASSIGN!
                if (!var_unassigned) {
                    std::vector<Token>::iterator varspace_iter = variable_space->begin();
                    for (; varspace_iter != variable_space->end(); ++varspace_iter) {
                        // check for matching names
                        if (var_to_assign->getStringValue() == (*varspace_iter).getStringValue()) {
                            var_to_assign = &(*varspace_iter);
                            break;
                        }  // change the reference of var_to_assign to the one in the varspace
                    }
                }

                variable_assignment(var_to_assign, temp_vec[0]);

                if (var_unassigned) {
                    (*variable_space).push_back(*var_to_assign);
                }

                // undeclared variable, push to variable space

                // this doesn't cause C++ to scream so im assuming this cursed syntax works

            } else {
                throw std::runtime_error("SYNTAX ERROR: BEG SHOULD BE FOLLOWED BY ONE VARIABLE");
            }
        } else if ((*iter).getStringValue() == "PRINT") {  // COMMAND: PRINT
            // only accepts literals / variables
            ++iter;
            if (token_stream.size() != 2) {
                throw std::runtime_error("SYNTAX ERROR: PRINT MUST BE FOLLOWED BY ONLY ONE VARIABLE/LITERAL");
            }
            
            token_class iter_token_class = (*iter).getTokenClass();
            variable_type iter_variable_type = (*iter).getVariableType();
            if (iter_token_class == tkn_Integer) {
                int iter_value = (*iter).getValue().val.int_val;
                std::cout << "SNOL> [" << iter_value << "] = " << iter_value;
            } else if (iter_token_class == tkn_Float) {
                float iter_value = (*iter).getValue().val.float_val;
                std::cout << "SNOL> [" << iter_value << "] = " << iter_value;
            } else if (iter_token_class == tkn_Variable) {
                if (iter_variable_type == var_undeclared) {
                    throw std::runtime_error("SYNTAX ERROR: VARIABLE IN PRINT IS UNDECLARED");
                } else if (iter_variable_type == var_Integer) {
                    int iter_value = (*iter).getValue().val.int_val;
                    std::cout << "SNOL> [" << (*iter).getStringValue() << "] = " << iter_value;
                } else if (iter_variable_type == var_Float) {
                    float iter_value = (*iter).getValue().val.float_val;
                    std::cout << "SNOL> [" << (*iter).getStringValue() << "] = " << iter_value;
                }
            } else {
                throw std::runtime_error("SYNTAX ERROR: DISALLOWED TOKEN TYPE AFTER PRINT COMMAND");
            }
        }
    } else if ((*iter).getTokenClass() == tkn_Variable) {
        Token* var_to_assign = &(*iter);  // store iter before we increment it.

        ++iter;

        if ((*iter).getTokenClass() == tkn_Operator && (*iter).getStringValue() == "=") {
            bool var_unassigned = (var_to_assign->getVariableType() == var_undeclared);

            // REPLACE var_to_assign to the correct reference (the one in the varspace)
            // A FIX. THE TOKEN IS INDEPENDENT TO THE ONE IN THE VARSPACE. SEARCH FOR THE CORRECT ONE TO ASSIGN!
            if (!var_unassigned) {
                std::vector<Token>::iterator varspace_iter = variable_space->begin();
                for (; varspace_iter != variable_space->end(); ++varspace_iter) {
                    // check for matching names
                    if (var_to_assign->getStringValue() == (*varspace_iter).getStringValue()) {
                        var_to_assign = &(*varspace_iter);
                        break;
                    }  // change the reference of var_to_assign to the one in the varspace
                }
            }

            ++iter;
            std::vector<Token> temp_token_stream;
            for (; iter != token_stream.end(); ++iter) {
                temp_token_stream.push_back(*iter);
            }

            variable_assignment(var_to_assign, expression_evaluator(temp_token_stream, expr_prechecker(temp_token_stream)));

            if (var_unassigned) {
                (*variable_space).push_back(*var_to_assign);
            }

        } else {
            expr_prechecker(token_stream);
        }
    } else {
        expr_prechecker(token_stream);
    }
}

std::string token_class_to_string(token_class tkn){
    switch (tkn)
    {
    case tkn_Command:
        return "COMMAND";
        break;
    
    case tkn_Float:
        return "FLOAT";
        break;
    
    case tkn_Integer:
        return "INTEGER";
        break;

    case tkn_Operator:
        return "OPERATOR";
        break;

    case tkn_Variable:
        return "VARIABLE";
        break;
    
    default:
        return "UNKNOWN TOKEN TYPE";
        break;
    }
}

/**
 * @brief Performs pre-checks on a token stream representing an expression.
 *
 * This function analyzes a vector of tokens (`token_stream`) to ensure it represents a valid expression.
 * It performs various checks, including:
 *   - Valid starting and ending tokens (must be integer, float, variable, or operator).
 *   - Undeclared variables in the expression.
 *   - Consistent variable types throughout the expression (no mixing integers and floats).
 *   - Valid operator usage (e.g., no consecutive operators, assignment operator not allowed).
 *   - Modulo (%) operation only allowed for integer expressions.
 *
 * @param token_stream The vector of tokens representing the expression.
 * @return The variable type of the expression (var_Integer or var_Float), determined by the first operand.
 * 
 */
variable_type expr_prechecker(std::vector<Token> token_stream) {
    // CHECK 1: VALID BEGINNING / ENDING
    Token beg = token_stream.front();  
    Token end = token_stream.back();
    token_class beg_class = beg.getTokenClass();
    token_class end_class = end.getTokenClass();
    if (!(beg_class == tkn_Integer || beg_class == tkn_Float || beg_class == tkn_Variable || beg_class == tkn_Operator)) {
        throw std::runtime_error("here");
        throw std::runtime_error("SYNTAX ERROR. EXPRESSION MUST END WITH A VALID OPERAND");
    }

    if (!(end_class == tkn_Integer || end_class == tkn_Float || end_class == tkn_Variable)) {

        throw std::runtime_error("SYNTAX ERROR. EXPRESSION MUST END WITH A VALID OPERAND");
    }

    // precheck to avoid screwing next step
    if (beg.getTokenClass() == tkn_Variable && beg.getVariableType() == var_undeclared) {
        throw std::runtime_error("SYNTAX ERROR. EXPRESSION CONTAINS INVALID VARIABLE");
    }

    variable_type expr_type = beg.getVariableType();
    // if there's a float at start all variables must be float, vice versa. divisions can just call a check again

    bool expect_operand = true;
    for (std::vector<Token>::iterator i = token_stream.begin(); i != token_stream.end(); ++i) {
        if (expect_operand) {
            // not integer, not float and not variable, send error
            //Check whether number is negative
            if (!((*i).getTokenClass() == tkn_Integer || (*i).getTokenClass() == tkn_Float || (*i).getTokenClass() == tkn_Variable)) {
                std::string error_msg = "SYNTAX ERROR. EXPECTED OPERAND SAW " + token_class_to_string((*i).getTokenClass());
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
                std::string error_msg = "SYNTAX ERROR. EXPECTED OPERATOR SAW " + token_class_to_string((*i).getTokenClass());
                throw std::runtime_error(error_msg);
            }
            // Check whether negative number
            // operator is =
            if ((*i).getStringValue() == "=") {
                std::string error_msg = "SYNTAX ERROR. EXPECTED OPERATOR SAW " + token_class_to_string((*i).getTokenClass());
                throw std::runtime_error(error_msg);
            }

            // operator is % and expression is assumed to be float
            if ((*i).getStringValue() == "%" && expr_type == var_Float) {
                throw std::runtime_error("SYNTAX ERROR. MODULO (%) OPERATION IS ONLY ALLOWED BETWEEN INTs");
            }
            
        }
        expect_operand = !expect_operand;  
    }

    // i know this check isn't necessary but i am still paranoid
    if (expect_operand) {
        throw std::runtime_error("SYNTAX ERROR. EXPRESSION MUST END WITH A VALID OPERAND");
    }

    return expr_type;
}

// evaluates a valid token stream
Token expression_evaluator(std::vector<Token> token_stream, variable_type expr_type) {
    // this is a horribly lazy method for expression evaluation because there's
    // no parentheses in the language description

    // initial push
    std::vector<Token>::iterator iter_base = token_stream.begin();
    for (; iter_base != token_stream.end(); ++iter_base) {
        if ((*iter_base).getTokenClass() == tkn_Operator) {
            if ((*iter_base).getStringValue() == "*") {
                if (expr_type == var_Integer) {
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
                if (expr_type == var_Integer) {  // SPECIAL CASE. MUST PERFORM RECHECK.
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
                if (expr_type == var_Integer) {
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
        if ((*iter_base).getTokenClass() == tkn_Operator) {
            if ((*iter_base).getStringValue() == "+") {
                if (expr_type == var_Integer) {
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
                if (expr_type == var_Integer) {
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

    if (token_stream.size() != 1) {
        throw std::runtime_error("UNKNOWN ERROR IN EXPRESSION EVALUATION. NO SINGULAR TOKEN RESULT.");
    } else {
        return *token_stream.begin();
    }
}