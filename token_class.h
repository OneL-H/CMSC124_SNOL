#include <iostream>
#include <variant>
#include <vector>

enum token_class {
    tkn_Integer,
    tkn_Float,
    tkn_Operator,
    tkn_Command,
    tkn_Variable
};

enum variable_type {
    var_Integer,
    var_Float,
    var_undeclared
};

union u_int_or_float {
    int int_val;
    float float_val;
};

struct int_or_float {
    u_int_or_float val;
    variable_type var_type;
};

class Token {
   private:
    int intValue;
    float floatValue;
    std::string stringValue;
    token_class tokenClass;
    variable_type variableType;
    // token class is either int, float, operator, command, or variable
    // variableType is
    // is either int, float, or undeclared

   public:
    // call these constructors to create a token
    Token(int init) {
        intValue = init;
        tokenClass = tkn_Integer;
        variableType = var_Integer;
    }  // creates an "integer" token

    Token(float init) {
        floatValue = init;
        tokenClass = tkn_Float;
        variableType = var_Float;
    }  // creates a float token

    Token(std::string init) {
        if (init == "EXIT!" || init == "BEG" || init == "PRINT") {
            stringValue = init;
            tokenClass = tkn_Command;
        } else if (init == "+" || init == "-" || init == "*" || init == "/" || init == "%" || init == "=") {
            stringValue = init;
            tokenClass = tkn_Operator;
        } else {
            stringValue = init;
            tokenClass = tkn_Variable;
            variableType = var_undeclared;
        }
    }

    int_or_float getValue() {
        if (variableType == var_Integer) {
            u_int_or_float temp;
            temp.int_val = intValue;

            int_or_float temp1;
            temp1.val = temp;
            temp1.var_type = var_Integer;

            return temp1;
        } else if (variableType == var_Float) {
            u_int_or_float temp;
            temp.float_val = floatValue;

            int_or_float temp1;
            temp1.val = temp;
            temp1.var_type = var_Float;

            return temp1;
        } else {
            throw std::runtime_error("ERROR: ATTEMPTING TO ACCESS VALUE OF NONVARIABLE/NONOPERAND");
        }
    }

    token_class getTokenClass() {
        return tokenClass;
    }

    std::string getStringValue() {
        return stringValue;
    }

    variable_type getVariableType() {
        return variableType;
    }

    void declareVar(int value) {
        intValue = value;
        variableType = var_Integer;
    }

    void declareVar(float value) {
        floatValue = value;
        variableType = var_Float;
    }
};