#include <iostream>
#include <vector>
#include <variant>

enum tkn_class{
    tkn_Integer, 
    tkn_Float,
    tkn_Operator,
    tkn_Command,
    tkn_Variable   
};

enum variable_type{
    var_Integer,
    var_Float,
    Undeclared
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
    std::string tokenClass;
    std::string variableType;
    // token class is either int, float, operator, command, or variable
    // variableType is
    // is either int, float, or undeclared

   public:
    // call these constructors to create a token
    Token(int init) {
        intValue = init;
        tokenClass = "Integer";
        variableType = "Integer";
    }  // creates an "integer" token

    Token(float init) {
        floatValue = init;
        tokenClass = "Float";
        variableType = "Float";
    }  // creates a float token

    Token(std::string init) {
        if (init == "EXIT!" || init == "BEG" || init == "FLOAT") {
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

    int_or_float getValue(){
        if(variableType == "Integer"){
            u_int_or_float temp;
            temp.int_val = intValue;

            int_or_float temp1;
            temp1.val = temp;
            temp1.var_type = var_Integer;

            return temp1;
        }

        if(variableType == "Float"){
            u_int_or_float temp;
            temp.int_val = floatValue;

            int_or_float temp1;
            temp1.val = temp;
            temp1.var_type = var_Float;

            return temp1;
        }
    }

    std::string getTokenClass(){
        return tokenClass;
    }

    std::string getStringValue(){
        return stringValue;
    }
    
    std::string getVariableType(){
        return variableType;
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