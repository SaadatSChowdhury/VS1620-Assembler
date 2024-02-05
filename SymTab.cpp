//
//		Implementation of the symbol table class.  This is the format I want for commenting functions.
//
#include "stdafx.h"
#include "SymTab.h"

/*
NAME

    AddSymbol - adds a new symbol to the symbol table.

SYNOPSIS

    void AddSymbol( const string &a_symbol, int a_loc );
    	a_symbol	-> The name of the symbol to be added to the symbol table.
    	a_loc		-> the location to be associated with the symbol.

DESCRIPTION

    This function will place the symbol "a_symbol" and its location "a_loc"
    in the symbol table.

*/
void SymbolTable::AddSymbol(const string& a_symbol, int a_loc)
{
    // If the symbol is already in the symbol table, record it as multiply defined.
    map<string, int>::iterator st = m_symbolTable.find(a_symbol);
    if (st != m_symbolTable.end()) {

        st->second = multiplyDefinedSymbol;
        return;
    }
    // Record a the  location in the symbol table.
    m_symbolTable[a_symbol] = a_loc;
}

/*
NAME

    DisplaySymbolTable - displays the symbol table on the screen.

SYNOPSIS

    void DisplaySymbolTable();
        
DESCRIPTION

    This function will display the symbol table.

*/
void SymbolTable::DisplaySymbolTable() 
{
    int i = 0;  //This is the number of symbol in symbol table.
    cout << "Symbol Table: " << endl << endl;
    cout << setw(12) << left << "Symbol#" << setw(12) << left << "Symbol" << setw(12) << left << "Location" << endl;

    //Iterate through the map and display the symbols and their location
    map<string, int>::iterator it;
    for (it = m_symbolTable.begin(); it != m_symbolTable.end(); it++)
    {
        cout << setw(12) << left << i << setw(12) << left << it->first << setw(12) << left << it->second << endl;
        i++;
    }
    cout << endl;
    cout << "Press Enter to Continue" << endl;
    cout << "__________________________________________________________" << endl << endl;
}

/*
NAME

    LookupSymbol - checks if the symbol is present in the symbol table

SYNOPSIS

    bool LookupSymbol(const string& a_symbol, int& a_loc);
        a_symbol	-> The symbol we are looking for in the symbol table.
        a_loc		-> If symbol is found, the location will be changed.

DESCRIPTION

    This function look up the symbol "a_symbol" in the symbol table. "a_loc" is changed to the location of the symbol if
    it exists.

*/
bool SymbolTable::LookupSymbol(const string& a_symbol, int& a_loc) {

    //If the symbol is found, record the address and return true
    if (m_symbolTable.find(a_symbol) != m_symbolTable.end())
    {
        a_loc = m_symbolTable[a_symbol];
        return true;
    }

    //The symbol does not exist in the symbolTable so return false
    return false;
}