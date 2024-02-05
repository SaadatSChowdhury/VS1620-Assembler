#include "stdafx.h"
#include "Errors.h"
#include "Instruction.h"
#include "Assembler.h"

/*
NAME

    Instruction::Instruction() - Initializes/resets the values

SYNOPSIS

    Instruction::Instruction() - constructor for the Instruction class.

DESCRIPTION

    This function is the constructor of the Instruction class and initializes/resets its variable values.

*/

Instruction::Instruction() 
{
    isFormatError = false;
    //nothing to do for now
}

/*
NAME

    ParseInstruction - return the instruction type.

SYNOPSIS

    Instruction::InstructionType Instruction::ParseInstruction(string a_line);
        a_line	-> a string to be parsed.

DESCRIPTION

    This function extracts different components of instruction.

*/

Instruction::InstructionType Instruction::ParseInstruction(string a_line)
{
    // Record the original statement.  This will be needed in the sceond pass.
    m_instruction = a_line;

    // Delete any comment from the line.
    a_line=RemoveComment(a_line);

    // Record label, opcode, and operands.  Up to you to deal with formatting errors.
    bool isFormatError = SymbolLabels(a_line);

    // Check if this is a comment.
    if (m_Label.empty() && m_OpCode.empty())
    {
        return InstructionType::ST_Comment;
    }
    // Return the instruction type.  This has to be handled in the code.
    return m_type;
}

/*
NAME

    LocationNextInstruction

SYNOPSIS

    int LocationNextInstruction(int a_loc);
        a_loc	-> the current instruction location

DESCRIPTION

    This function returns the calculated location of the next instruction.

*/

int Instruction::LocationNextInstruction(int a_loc)
{
    int myloc = 0;
    if (m_OpCode == "ORG" || m_OpCode == "DS")
    {
        return myloc = a_loc + m_NumValOperand1;
    }
    else
    {
        return myloc = a_loc + 1;
    }
}

/*
NAME

    bool Instruction::isStrNumber(const string& a_str)

SYNOPSIS

    bool isStrNumber(const string& a_str);
        a_str	-> check the string if it contains a number.

DESCRIPTION

    This function checks if the string contains any number.

*/

bool Instruction::isStrNumber(const string& a_str)
{
    if (a_str.empty())
    {
        return false;
    }

    // If there is - or +, make sure there are more characters.
    int ichar = 0;
    if (a_str[0] == '-' || a_str[0] == '+')
    {
        ichar++;
        if (a_str.length() < 2) return false;
    }

    // Make sure that the remaining characters are all digits
    for (; unsigned(ichar) < a_str.length(); ichar++)
    {
        if (!isdigit(a_str[ichar])) return false;
    }
    return true;
}

/*
NAME

    bool Instruction::SymbolLabels(const string& a_line)

SYNOPSIS

    bool SymbolLabels(const string& a_line);
        a_line	-> The line whose fields are to be recorded. Passes it as a const with 
                   reference to avoid it being copied.

DESCRIPTION

    This function stores the instruction type we are receiving.

*/

bool Instruction::SymbolLabels(const string& a_line)
{
    string line = a_line;

    // Get the fields that make up the instruction.
    isFormatError =! ParseLine(line, m_Label, m_OpCode, m_Operand1, m_Operand2);

    // if code was a comment, there is nothing to do.
    if (m_OpCode.empty() && m_Label.empty())
    {
        return isFormatError;
    }

    // For the sake of comparing, convert the op code to upper case.
    for (char& c : m_OpCode)
    {
        c = toupper(c);
    }

    //if the line is empty or has spaces then it is comment.
    if (line.length() == 0 || all_of(line.begin(), line.end(), isspace))
    {
        m_type = InstructionType::ST_Comment;
    }
    else if (m_OpCode == "END") // if the line ends.
    {
        m_type = InstructionType::ST_End;
    }
    
    //if the line is an Assembler Instruction
    if (find(AssemblyLang.begin(), AssemblyLang.end(), m_OpCode) != AssemblyLang.end())
    {
        m_type = InstructionType::ST_AssemblerInstr;
        m_NumOpCode = (int)distance(AssemblyLang.begin(), find(AssemblyLang.begin(), AssemblyLang.end(), m_OpCode)) + 1;
    }
    
    //if the line is a Machine Language Instruction 
    else if (find(MachineLang.begin(), MachineLang.end(), m_OpCode) != MachineLang.end())
    {
        m_type = InstructionType::ST_MachineLanguage;
        m_NumOpCode = (int)distance(MachineLang.begin(), find(MachineLang.begin(), MachineLang.end(), m_OpCode)) + 1;
    }
    else if (m_OpCode == "END") // if ends
    {
        m_type = InstructionType::ST_End;
    }
    else //if error
    {
        m_type = InstructionType::ST_Error;
    }

    // Record whether the operands are numeric and their value if they are.
    m_IsNumericOperand1 = isStrNumber(m_Operand1);
    if (m_IsNumericOperand1) m_NumValOperand1 = stoi(m_Operand1);

    m_IsNumericOperand2 = isStrNumber(m_Operand2);
    if (m_IsNumericOperand2) m_NumValOperand2 = stoi(m_Operand2);

    return isFormatError;
}

/*
NAME

    bool Instruction::ParseLine(string & line, string & label, string & opcode, 
                                string & operand1, string & operand2);

SYNOPSIS

    bool ParseLine(string & line, string & label, string & opcode, string & operand1, 
                   string & operand2);
        line      -> The line/string which is bring parsed.
        label     -> Stores the label in the instruction.
        opcode    -> Stores the opcode in the instruction.
        operand1  -> Stores the operand1 in the instruction.
        operand2  -> Stores the operand2 in the instruction.

DESCRIPTION

    This function parses the following instructions: line, label, opcode, operand1, operand2.

*/

bool Instruction::ParseLine(string & line, string & label, string & opcode, string & operand1, string & operand2)        
{
    replace(line.begin(), line.end(), ',', ' ');

    // Delete any comment from the line.
    line = RemoveComment(line);

    istringstream ins(line);
    label = opcode = operand1 = operand2 = "";
    if (line.empty()) return true;

    string extra;

    if (line[0] != ' ' && line[0] != '\t')
    {
        ins >> label;
    }
    ins >> opcode >> operand1 >> operand2 >> extra;

    return extra == "";
}
