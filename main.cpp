#include <string.h>

#include <stdexcept>

#include "token_class.h"

std::vector<Token> scanner(std::string input_string);

int main() {
    std::string inp = "";
    std::cout << "inp";
    try {
        std::vector<Token> token_stream1 = scanner("BEG MOMMY TO STOP PRINT ING LMAO");
    } catch (const std::exception& e) {
        std::cout << "ERROR CAUGHT: " << e.what() << '\n';
    }
}

// returns the four things a character can be
std::string type_check(char inp) {
    if (isalpha(inp)) {
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

// returns a token object with correct parameters
Token tokenize(std::string inp, std::string curr_type) {
    if (curr_type == "INTEGER") {
        return Token(atoi(inp.c_str()));
    } else if (curr_type == "FLOAT") {
        return Token((float)atof(inp.c_str()));
        // float here cause C++ complains if i dont
    } else if (curr_type == "OPERATOR" || curr_type == "STRING") {
        return Token(inp);
    }

    // if control reaches here you know ive fucked up
    return Token(inp);
}

std::vector<Token> scanner(std::string input_string) {
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
                token_stream.push_back(tokenize(buffer, curr_type));
                buffer = "";
            } // SPACE IS A DELIMITER. CHANGE TYPE IMMEDIATELY. */

            continue;
        } else if (curr_type == "INTEGER") {
            if (char_type == "POINT") {
                buffer += input_string.at(pos);
                curr_type = "FLOAT";  // ACTION 1: add current char and change type
            } else if (char_type == "LETTER") {
                token_stream.push_back(tokenize(buffer, curr_type));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "STRING";
            } else if (char_type == "OPERATOR") {
                token_stream.push_back(tokenize(buffer, curr_type));
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
                token_stream.push_back(tokenize(buffer, curr_type));
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
                token_stream.push_back(tokenize(buffer, curr_type));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "STRING";  // ACTION 2: tokenize and clear buffer, change type

            } else if (char_type == "NUMBER") {
                token_stream.push_back(tokenize(buffer, curr_type));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "INTEGER";  // ACTION 2: tokenize and clear buffer, change type
            }
        } else if (curr_type == "FLOAT") {
            if (char_type == "POINT") {
                throw std::runtime_error("LEXICAL ERROR: FLOATING POINT NUMBER HAS TWO DECIMAL POINTS.");
            } else if (char_type == "LETTER") {
                token_stream.push_back(tokenize(buffer, curr_type));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "STRING";
            } else if (char_type == "OPERATOR") {
                token_stream.push_back(tokenize(buffer, curr_type));
                buffer.clear();
                buffer += input_string.at(pos);
                curr_type = "OPERATOR";  // ACTION 2: tokenize and clear buffer, change type
            } else if (char_type == "NUMBER") {
                buffer += input_string.at(pos);  // ACTION 3: add current char
            }
        }
    }

    token_stream.push_back(tokenize(buffer, curr_type));

    return token_stream;
}
