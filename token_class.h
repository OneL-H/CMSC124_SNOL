#include <iostream>
#include <vector>

class Token {
   private:
    int intValue;
    float floatValue;
    std::string stringValue;
    std::string tokenClass;
    std::string variableType;
    // token class is either int, float, operator, command, or variable
    // variableType is either int, float, or undeclared

   public:
    // call these constructors to create a token
    Token(int init) {
        intValue = init;
        tokenClass = "Integer";
    }  // creates an "integer" token

    Token(float init) {
        floatValue = init;
        tokenClass = "Float";
    }  // creates a float token

    Token(std::string init) {
        if (init == "EXIT" || init == "BEG" || init == "FLOAT") {
            stringValue = init;
            tokenClass = "Command";
        } else if (init == "+" || init == "-" || init == "*" || init == "/" || init == "%" || init == "=") {
            stringValue = init;
            tokenClass = "Operator";
        } else {
            stringValue = init;
            tokenClass = "Variable";
            variableType = "Undeclared";
        }
    }

    void declareVar(int value) {
        intValue = value;
        variableType = "Integer";
    }

    void declareVar(float value) {
        floatValue = value;
        variableType = "Float";
    }
};