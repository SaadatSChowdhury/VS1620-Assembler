#include "stdafx.h"
#include "Errors.h"

vector<string> Errors::m_ErrorMsgs;
int Errors::errorCount;

/*
NAME
    DisplayErrors - displays the last error

SYNOPSIS
    void DisplayErrors()

DESCRIPTION
    This function prints out the latest errors while translating the code (if they find any).

*/

void Errors::DisplayErrors()
{
    for (int i = 0; i < m_ErrorMsgs.size(); i++)
    {
        cout << m_ErrorMsgs[i] << endl;
    }
}

/*
NAME
    InitErrorReporting - Initializes the error vector

SYNOPSIS
    void InitErrorReporting()

DESCRIPTION
    This function initializes the vector error components.

*/

void Errors::InitErrorReporting()
{
    m_ErrorMsgs = {};
    errorCount = 0;
}

/*
NAME
    RecordError - records the error

SYNOPSIS
    void RecordError(string a_emsg)
        a_emsg  -> Stores the error message.

DESCRIPTION
    This function stores the error message in a vector.

*/

void Errors::RecordError(string a_emsg)
{
    m_ErrorMsgs.push_back(a_emsg);
    errorCount++;
}
