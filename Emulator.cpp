#include "stdafx.h"
#include "Emulator.h"
#include "Instruction.h"

/*
NAME

	insertMemory - inserts the translated code in the specified location.

SYNOPSIS

	bool insertMemory(int a_location, long long a_contents);
		a_location	-> The location of the translated code to be stored.
		a_contents	-> The code which is translated to be stored.

DESCRIPTION

	Stores the translated code in the specified memory location.

*/

bool emulator::insertMemory(int a_location, long long a_contents)
{
	if (a_location >= MEMSZ)
	{
		return false;
	}
	else
	{
		m_memory[a_location] = a_contents;
		return true;
	}
}

/*
NAME

	runProgram - to run the emulator using the translated code.

SYNOPSIS

	bool runProgram();

DESCRIPTION

	This function executes the memory instruction and runs the emulator.

*/

bool emulator::runProgram()
{
	long long operandCode = 0;
	int address1 = 0;
	int address2 = 0;
	long long contents = 0;
	long long val = 0;

	for (int i = 0; i < MEMSZ; i++)
	{
		contents = m_memory[i];

		if (contents != 0)
		{
			operandCode = contents / 10'000'000'000;
			address1 = (contents / 100'000) % 100'000;
			address2 = contents % 100'000;

			switch (operandCode)
			{
			case 1:
				m_memory[address1] += m_memory[address2];
				break;

			case 2:
				m_memory[address1] -= m_memory[address2];
				break;


			case 3:
				m_memory[address1] *= m_memory[address2];
				break;

			case 4:
				m_memory[address1] /= m_memory[address2];
				break;


			case 5:
				m_memory[address1] = m_memory[address2];
				break;


			case 7:
				int input;
				
				cout<< "?";
				cin >> input;
				if (input < MEMSZ)
				{
					m_memory[address1] = input;
				}
				else {
					cout << "Too large value" << endl;
				}
				break;

			case 8:
				cout << m_memory[address1] << endl;
				break;

			case 9:
				i = address1;
				break;

			case 10:
				if (m_memory[address2] < 0) {
					i = address1 - 1;
				}
				break;

			case 11:
				if (m_memory[address2] == 0) {
					i = address1 - 1;
				}
				break;

			case 12:
				if (m_memory[address2] > 0) {
					i = address1 - 1;
				}
				break;

			case 13:
				i = MEMSZ;
				break;

			default:
				cout << "Operand Code Error" << endl;

				return false;
			}
		}
	}
	return false;
}

