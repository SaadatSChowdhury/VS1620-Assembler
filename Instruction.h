//
// Class to parse and provide information about instructions.  Note: you will be adding more functionality.
//
#pragma once

// The elements of an instruction.
class Instruction {

public:

    Instruction( );
    ~Instruction( ) { };

    // Codes to indicate the type of instruction we are processing.  Why is this inside the
    // class?  We should make this an enum class.  We will do this during a lecture.
    enum InstructionType {
        ST_MachineLanguage, 	// A machine language instruction.
        ST_AssemblerInstr,      // Assembler Language instruction.
        ST_Comment,             // Comment or blank line
        ST_End,                 // end instruction.
        ST_Error               
    };
    // Parse the Instruction.
    InstructionType ParseInstruction(string a_line);

    // Compute the location of the next instruction.
    int LocationNextInstruction(int a_loc);

    //getter functions 
    // To access the label
    inline string &GetLabel( ) {

        return m_Label;
    };
    // To determine if a label is blank.
    inline bool isLabel( ) {

        return ! m_Label.empty();
    };
    
    // To access the original instruction
    inline string& GetInstruction() {
        return m_instruction;
    };

    //getter function for opcode
    inline string GetOperandCode() {
        return m_OpCode;
    };

    //return int opcode
    inline int& GetOperandCodeInt() {
        return m_NumOpCode;
    };

    // To access operand 1
    inline string& GetOperand1() {
        return m_Operand1;
    };

    // To check if operand 1 is numeric
    inline bool IsNumericOperand1() {
        return m_IsNumericOperand1;
    };

    // To access operand 2
    inline string& GetOperand2() {
        return m_Operand2;
    };

    // To check if operand 2 is numeric
    inline bool IsNumericOperand2() {
        return m_IsNumericOperand2;
    };

    // Getter function for NumValOperand1
    inline int NumValOperand1() {
        return m_NumValOperand1;
    };
    
    // Getter function for NumValOperand2
    inline int NumValOperand2() {
        return m_NumValOperand2;
    };

    //bool sunction that returns true if there is format error
    inline bool isError()
    {
        return isFormatError;
    }

private:


    // The elements of a instruction
    string m_Label;        // The label.
    string m_OpCode;       // The symbolic op code.
    string m_Operand1;     // The first operand
    string m_Operand2;     // The second operand

    string m_instruction;    // The original instruction.

    // Derived values.
    int m_NumOpCode = 0;     // The numerical value of the op code for machine language equivalents.
    InstructionType m_type = InstructionType::ST_Error; // The type of instruction.

    bool m_IsNumericOperand1 = false;// == true if the operand 1 is numeric.
    int m_NumValOperand1 = 0;   // The value of the operand 1 if it is numeric.

    bool m_IsNumericOperand2 = false;// == true if the operand 2 is numeric.
    int m_NumValOperand2 = 0;   // The value of the operand 2 if it is numeric.

    vector<string> AssemblyLang = { "DC","DS","ORG"};
    vector<string> MachineLang = { "ADD","SUB","MULT","DIV","COPY","","READ","WRITE","B","BM","BZ","BP","HALT"};
    
    //Deletes comments from a line.
    string RemoveComment(string line) 
    {
        size_t pos = line.find(';');
        if (pos == string::npos)
        {
            return line;
        }
        return line.erase(pos);
    }

    vector<string> m_parsedInstruction;
    
    //Records the format error
    bool isFormatError;

    // Records the fields that make up the statement
    bool ParseLine(string& line, string& label, string& opcode, string& operand1, string& operand2);

    // Record the fields of the instructions.
    bool SymbolLabels(const string& a_line);

    // Check if a string contains a number. 
    bool isStrNumber(const string& a_str);
};


