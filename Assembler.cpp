//
//      Implementation of the Assembler class.
//
#include "stdafx.h"
#include "Assembler.h"
#include "Errors.h"
#include "Instruction.h"
#include "SymTab.h"
#include <iostream>

/*
NAME

    Assembler::Assembler( int argc, char *argv[] )

SYNOPSIS

    Assembler::Assembler( int argc, char *argv[] );
        argc	-> the total number of command line arguments
        argv    -> Array of strings containing user input

DESCRIPTION

    This function is the constructer of the Assembler class.

*/
 
Assembler::Assembler( int argc, char *argv[] )
: m_facc( argc, argv )
{
    // Nothing else to do here at this point.
}  


// Destructor currently does nothing.  You might need to add something as you develope this project.  If not, we can delete it.
Assembler::~Assembler( )
{
}
// Pass I establishes the location of the labels.  You will write better function comments according to the coding standards.


/*
NAME

    Assembler::PassI() - Helps in filling the symbol table.

SYNOPSIS

     Assembler::PassI();

DESCRIPTION

    This function establishes the location of the labels.

*/

void Assembler::PassI( ) 
{
    int loc = 0;        // Tracks the location of the instructions to be generated.

    // Successively process each line of source code.
    for( ; ; ) {

        // Read the next line from the source file.
        string line; 
        if( ! m_facc.GetNextLine( line ) ) {

            // If there are no more lines, we are missing an end statement.
            // We will let this error be reported by Pass II.
            return;
        }
        // Parse the line and get the instruction type.
        Instruction::InstructionType st = m_inst.ParseInstruction( line );

        // If this is an end statement, there is nothing left to do in pass I.
        // Pass II will determine if the end is the last statement and report an error if it isn't.
        if( st == Instruction::ST_End ) return;

        // Labels can only be on machine language and assembler language
        // instructions.  So, skip comments.
        if( st == Instruction::ST_Comment )  
        {
        	continue;
	    }
        // If the instruction has a label, record it and its location in the
        // symbol table.
        if( m_inst.isLabel( ) ) {

            m_symtab.AddSymbol( m_inst.GetLabel( ), loc );
        }
        // Compute the location of the next instruction.
        loc = m_inst.LocationNextInstruction( loc );
    }
}

/*
NAME

    Assembler::PassII - Translate the code line by line and records errors.

SYNOPSIS

    Assembler::PassII();


DESCRIPTION

    This function translates each line of the input and looks for errors. 
    If any errors are found, it reports immediately after the statement. Otherwise,
    calculates the value.

*/

void Assembler::PassII()
{
    cin.ignore();
    m_facc.rewind();  //goes to the beginning of the file
    
    int loc = 0;      //Location of the instructions to be generated

    cout << "Translation of Program: " << endl << endl;
    cout << left << setw(10) << "Location" << left << setw(22) << "Contents" << left << setw(30) << "Original Statement" << endl;

    while (true)
    {
        // to check for errors in each line of instructions.
        Errors::InitErrorReporting(); // initialize error reporting
        long long values = 0;  //values of the contents

        string line;     //records the next line.
        if (!m_facc.GetNextLine(line))
        {
            Errors::RecordError("ERROR! Missing End");
            m_numErrors++;
            Errors::DisplayErrors();
            return;
        }

        //parse the line and get the type of instruction
        Instruction::InstructionType st = m_inst.ParseInstruction(line);

        //determine if end is the last element 
        if (st == Instruction::InstructionType::ST_End)
        {
            if (m_facc.GetNextLine(line))
            {
                if (!line.empty())  //if there is something after end statement
                {
                    //if the line after end statement is not comment, report error
                    if (m_inst.ParseInstruction(line) != Instruction::InstructionType::ST_Comment)
                    {
                        Errors::RecordError("ERROR! End statement is not the last statement");
                        m_numErrors++;
                        Errors::DisplayErrors();
                       
                    }
                }
            }
            cout << "\t\t\t\t" << m_inst.GetInstruction() << endl;
            break;
        }

        //if the line is comment, just print the line as is.
        if (st == Instruction::InstructionType::ST_Comment)
        {
            cout << " \t\t\t\t" << m_inst.GetInstruction() << endl;
            continue;
        }
        if (m_inst.isError())
        {
            Errors::RecordError("ERROR! Extra operand found: " + m_inst.GetInstruction());
            m_numErrors++;
        }

        //check for bad instruction type
        if (st == Instruction::InstructionType::ST_Error)
        {
            Errors::RecordError(" ERROR! BAD opcode " + m_inst.GetInstruction());
            m_numErrors++;
        }

        //check to see if label start with number and report error if true
        if (m_inst.GetLabel()[0] >= '0' && m_inst.GetLabel()[0] <= '9')
        {
            Errors::RecordError("ERROR: Label " + m_inst.GetLabel() + " cannot start with number");
            m_numErrors++;
        }

        //making sure label is not too big
        if (m_inst.GetLabel().length() > 10)
        {
            Errors::RecordError("ERROR! The label size is too large for" + m_inst.GetLabel());
            m_numErrors++;
        }

        //if the next location exceeds 999999, then report error
        if (m_inst.LocationNextInstruction(loc) > 99999)
        {
            Errors::RecordError("ERROR! Memory too large...Insufficient memory for translation");
            m_numErrors++;
        }
        int temp_loc = 0;
        if (m_inst.isLabel())
        {
            if (m_symtab.LookupSymbol(m_inst.GetLabel(), temp_loc) && temp_loc == m_symtab.multiplyDefinedSymbol)
            {
                Errors::RecordError("ERROR! Multiple labels with same name");
                m_numErrors++;
            }
        }
        //if operand value > 10'000 report error
        if ((m_inst.IsNumericOperand1() > 10'000) || (m_inst.IsNumericOperand2() > 10'000))
        {
            Errors::RecordError("ERROR! Value too large for " + m_inst.GetOperandCode());
            m_numErrors++;
        }

        values = 0;

        //if assembly instruction call AssemblyInst function
        if (st == Instruction::InstructionType::ST_AssemblerInstr) {
            AssemblyInstructions(line, values);
        }

        int location = 0;

        //if it is machine, call MachineInst function
        if (st == Instruction::InstructionType::ST_MachineLanguage)
            MachineInstructions(line, values, location);

        DisplayTerminal(values, loc);
        Errors::DisplayErrors();

    }

}

/*
NAME

    Assembler::AssemblyInstructions() - runs the assembly instructions and records/reports error
    if there are any.

SYNOPSIS

    void Assembler::AssemblyInstructions(const string& line, long long& values)
        line	-> instructs the assembly line to process the instructions. Passed
                   off as a reference to avoid copy.
        values	-> value of the content. Passed as a reference so that the DisplayContent
                   function, which calls the passII function, can change its value, too.

DESCRIPTION

    This function processes the assembly instructions and reports errors if found. 
    Otherwise it updates the contents.

*/

void Assembler::AssemblyInstructions(const string& line, long long& values)
{
    if (m_inst.GetOperandCode() != "END" && m_inst.GetOperand1().empty())
    {
        Errors::RecordError("ERROR! No operand found for:" + line);
        m_numErrors++;
    }

    //operand 1 must be address, if not report error
    if (!m_inst.IsNumericOperand1() && !m_inst.GetOperand1().empty())
    {
        Errors::RecordError("ERROR! Operand 1 must be address for : " + line);
        m_numErrors++;
    }

    //assembly instruction do not need operand2, so if it exists report error
    if (!m_inst.GetOperand2().empty())
    {
        Errors::RecordError("ERROR! Operand 2 not needed for assembly instruction: " + line);
        m_numErrors++;
    }

    //ORG does not need label, if found report error
    if (m_inst.GetOperandCode() == "ORG" && !m_inst.GetLabel().empty())
    {
        Errors::RecordError("ERROR! Label found for ORG which is not needed:" + line);
        m_numErrors++;
        values = 0;
    }

    //all but ORG needs label, if not found report error
    if (m_inst.GetOperandCode() != "ORG" && m_inst.GetOperandCode() != "END" && m_inst.GetLabel().empty())
    {
        Errors::RecordError("ERROR! No label defined called " + m_inst.GetOperandCode());
        m_numErrors++;
    }

    //calculating value
    if (m_inst.GetOperandCode() == "DC")
    {
        values = m_inst.NumValOperand1();

    }
    else
    {
        values = 0;
    }
}

/*
NAME

    Assembler::MachineInstructions() - runs the machine instructions of the program and records/reports
    errors if there any. 

SYNOPSIS

    void Assembler::MachineInstructions(const string& line, long long& values, int& location);
        line	 -> Gets the line to process machine instructions. Passed as a reference to avoid copy.
        values	 -> value of the content. Passed as a reference so that the DisplayContent
                    function, which calls the passII function, can change its value, too.
        location -> looks up the location and uses the value to calculate the contents. Passes as reference
                    to avoid copy.

DESCRIPTION

    This function processes the machine instructions and reports errors if found. 
    Otherwise it updates the contents.

*/

void Assembler::MachineInstructions(const string& line, long long& values, int& location)
{
    int temp = 0;

    //for 01 ADD  02 SUB  03 MULT 04 DIV 
    if (m_inst.GetOperandCodeInt() >= 1 && m_inst.GetOperandCodeInt() <= 4)
    {
        //needs both operands. If not found, report error
        if (m_inst.GetOperand1().empty() || m_inst.GetOperand2().empty())
        {
            Errors::RecordError("ERROR! Both operands needed for: " + m_inst.GetInstruction());
            m_numErrors++;
        }
        //operand 1 must be address. If not, report error
        if (m_inst.IsNumericOperand1())
        {
            Errors::RecordError("ERROR! Operand 1 must be address : " + m_inst.GetInstruction());
            m_numErrors++;
        }
        //operand 2 must be address. If not repport error
        if (m_inst.IsNumericOperand2())
        {
            Errors::RecordError("ERROR! Operand 2 must be address " + m_inst.GetInstruction());
            m_numErrors++;
        }
        //if operand 2/addr is undefined report error
        int temploc;
        if (m_symtab.LookupSymbol(m_inst.GetOperand2(), temploc) == 0)
        {
            Errors::RecordError("ERROR! Label " + m_inst.GetOperand2() + " undefined ");
            m_numErrors++;
        }
    }
    if (m_inst.GetOperandCode() == "COPY" || m_inst.GetOperandCode() == "READ" || m_inst.GetOperandCode() == "WRITE")
    {
        //at least operand1 needed for read and write, if not found report error
        if (m_inst.GetOperand1().empty())
        {
            Errors::RecordError("Operand 1 not found for " + m_inst.GetOperandCode());
            m_numErrors++;

        }
        //read write can have 1 or 2 operands. For 1 operand, it must be addr value
        else if (!m_inst.GetOperand1().empty() && m_inst.GetOperand2().empty())
        {
            if (m_inst.IsNumericOperand1())
            {
                Errors::RecordError("For read/write statement with 1 operand, the operand must be non numeric/label ");
                m_numErrors++;
            }
            //if addr is undefined report error
            int temploc;
            if (m_symtab.LookupSymbol(m_inst.GetOperand1(), temploc) == 0)
            {
                Errors::RecordError("Label " + m_inst.GetOperand1() + " undefined ");
                m_numErrors++;
            }

            int location1;
            m_symtab.LookupSymbol(m_inst.GetOperand1(), location1);


            //calculate value using locations of addr1 and addr2
            values = 10000000000 * (long long)m_inst.GetOperandCodeInt() + 100000 * (long long)location1;


        }
        //for read/write with 2 operands, first should be register and second should be addr. If not report error
        else if (!m_inst.GetOperand1().empty() && !m_inst.GetOperand2().empty())
        {
            //if addr is undefined report error
            int temploc;
            if (m_symtab.LookupSymbol(m_inst.GetOperand2(), temploc) == 0)
            {
                Errors::RecordError("Label " + m_inst.GetOperand2() + " undefined ");
                m_numErrors++;
            }
             int location1, location2;
            m_symtab.LookupSymbol(m_inst.GetOperand1(), location1);
            m_symtab.LookupSymbol(m_inst.GetOperand2(), location2);


            //calculate value using locations of addr1 and addr2
            values = 10000000000 *(long long)  m_inst.GetOperandCodeInt() + 100000 * (long long) location1 +(long long) location2;
        }

    }
    //for 9 B  10 BM  11 BZ  12 BP
    else if (m_inst.GetOperandCodeInt() >= 9 && m_inst.GetOperandCodeInt() <= 12)
    {
        //oprand 2 is required, if not report error
        if (m_inst.GetOperand2().empty())
        {
            Errors::RecordError("For " + m_inst.GetOperandCode() + " , the second operand must be present ");
            m_numErrors++;
        }
        //operand 2 must be address, if not report error
        if (m_inst.IsNumericOperand2())
        {
            Errors::RecordError("For " + m_inst.GetOperandCode() + " , the second one must be address ");
            m_numErrors++;
        }
        //if label is not defined report error
        int temploc;
        if (m_symtab.LookupSymbol(m_inst.GetOperand2(), temploc) == 0)
        {
            Errors::RecordError("Label " + m_inst.GetOperand2() + " undefined ");
            m_numErrors++;
        }
        //calculating value
        int location1, location2;
        if (m_inst.GetOperandCodeInt() == 9) {
            m_symtab.LookupSymbol(m_inst.GetOperand1(), location1);
            values = 10000000000 * (long long)m_inst.GetOperandCodeInt() + 100000 * (long long)location1;
        }
        else {
            m_symtab.LookupSymbol(m_inst.GetOperand1(), location1);
            m_symtab.LookupSymbol(m_inst.GetOperand2(), location2);

            values = 10000000000 * (long long)m_inst.GetOperandCodeInt() + 100000 * (long long)location1 + (long long)location2;
        }
        

    }
    else
    {
        if (m_symtab.LookupSymbol(m_inst.GetOperand2(), location))
        {
            values = 10000000000 * (long long)m_inst.GetOperandCodeInt() + 100000 * (long long)location;

        }
    }
    if (m_inst.GetOperandCode() == "HALT")
    {
        //HALT does not need label, if found report error

        if (!m_inst.GetLabel().empty())
        {
            Errors::RecordError("Label found in HALT");
            m_numErrors++;
        }
        //HALT does not need operand, if found report error
        if (!m_inst.GetOperand1().empty())
        {
            Errors::RecordError("Operand not needed in HALT");
            m_numErrors++;
        }
        values = 10000000000 * (long long)m_inst.GetOperandCodeInt() + 100000 * (long long)temp;

    }
}

/*
NAME

    Assembler::DisplayTerminal() - displays the contents of instruction in the terminal

SYNOPSIS

    Assembler::DisplayTerminal(long long& values, int& loc);
        values	-> The actual value of contents is processed and displayed.
        loc		-> the value of location of the instruction.

DESCRIPTION

    This function displays the contents in the terminal.

*/

void Assembler::DisplayTerminal(long long& values, int& loc)
{

    string location = to_string(loc);

     //values are integers so to match with 9 digit format converting it to string and adding required number of 0s
    stringstream ss;
    ss << values;
    string mystring = ss.str();
    
    size_t length = std::to_string(values).length();
    size_t difference = 12 - length;
    for (size_t i = 0; i < difference; i++)
    {
        mystring = "0" + mystring;
    }
    if (values == 0)
    {
        mystring = "";
    }

    if (m_inst.GetOperandCode() == "END") location = "";

    //passing to the emulaor
    m_emul.insertMemory(loc, values);

    //outputting the content and instructions
    cout << left << setw(10) << location << left << setw(22);
    cout << mystring << left << setw(35);
    cout << m_inst.GetInstruction() << endl;

    //finding out the location of next instruction
    loc = m_inst.LocationNextInstruction(loc);

}

/*
NAME

    Assembler::RunProgramInEmulator() - runs the emulator if there are no errors otherwise will not run.

SYNOPSIS

    void Assembler::RunProgramInEmulator()

DESCRIPTION

    This function will runs the emulator. It checks for errors, if found any, the emulator will not run.
*/

void Assembler::RunProgramInEmulator()
{
    cout << endl;
    cout << "Press Enter to continue" << endl << endl;
    cout << "__________________________________________________________" << endl << endl;
    cin.ignore();

    cout << "Results from emulating program:" << endl;

    //if errors, dont run the emulator
    if (m_numErrors != 0)
    {
        cout << "Emulator cannot start because of errors" << endl;
    }
    else
    {
        m_emul.runProgram();
    }
        cout << endl;
    cout << "End of emulation" << endl;
}

