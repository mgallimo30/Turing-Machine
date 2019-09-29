/*
 * Name        : turing.cpp
 * Author      : Matthew Ryan Gallimo
 *
 * License     : Copyright (C) 2019 All rights reserved
 */

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <stack>
#include <math.h>
using namespace std;

int main(int argc, char *argv[])
{
	bool debug = false;

	//Create var list
	//ALL varables are created but are initialized within their scope
	//Main vars
	int totalMoves = 0;
	int totalCycles = 0;
	int i = 0;
	ifstream bin, boxedOfRollsOfTape;
	uint16_t *PC, RAM[4096];
	char bites[2];
	list <string> unboxedRollsOfTape;
	string oneRollOfTape;
	
	//While vars
	char alphabet[2][256];
	int moves, instructions, cycles, move;
	bool done, halted, EQUALS;
	struct pieceOfTape {
		char letter;
		bool isBlank;
		int position;
	};
	list<pieceOfTape> cutUpRollOfTape;
	list<pieceOfTape>::iterator head;
	list<pieceOfTape>::iterator head2;
	


	//Do vars
	bool blank, on;
	int opCode;
	uint16_t *IR, temp, address, bTi;
	uint8_t letter;
	string instruction; //Used for debugging only
		
	//Init preprocess var
	bin.open(argv[1], ios::in | ios::binary);
	boxedOfRollsOfTape.open(argv[2], ios::in);
	PC = &RAM[0];

	if (bin.is_open()) {
		do {
			bin.read(bites, 2);
			RAM[i] = (bites[0] | bites[1] << 8);
			i++;
		} while (!bin.eof());
	}
	else {
		cout << "Unable to open *argv[1]" << endl;
	}

	if (boxedOfRollsOfTape.is_open()) {
		do {
			getline(boxedOfRollsOfTape, oneRollOfTape);
			unboxedRollsOfTape.push_back(oneRollOfTape);

		} while (!boxedOfRollsOfTape.eof());
	}
	else
	{
		cout << "Unable to open *argv[2]" << endl;
	}

	bin.close();
	boxedOfRollsOfTape.close();

	while (!unboxedRollsOfTape.empty()) {
		
		oneRollOfTape = unboxedRollsOfTape.front(); unboxedRollsOfTape.pop_front();
		for (i = 0; i < 256; i++) {
			alphabet[0][i] = 0;
			alphabet[1][i] = 0;
		}
		moves = 0, instructions = 0, cycles = 0, move = 0;
		done = false, halted = false, EQUALS = false;
		

		for (i = 0; i < oneRollOfTape.length(); i++) {
			cutUpRollOfTape.push_back(pieceOfTape{ oneRollOfTape.at(i), false, i });
		}
		if (cutUpRollOfTape.size() == 0) {
			cutUpRollOfTape.push_front({ 0 , true, 0 });
		}
		head = cutUpRollOfTape.begin();
		head2 = cutUpRollOfTape.begin();
		
		do {
			if (debug) {
				for (i = 0; i < cutUpRollOfTape.size(); i++) {
					cout << head2->letter;
					head2++;
				}
				cout << endl;
				head2 = cutUpRollOfTape.begin();
				for (i = 0; i < head->position; i++) {
					cout << " ";
				}
				cout << "^" << endl;
			}
			cycles++; if (debug) { cout << cycles << " RAM: "; }
			blank = false, on = false;
			temp = 0, address = 0, bTi = 0;
			letter = 0, opCode = -1;
			instruction = "";

			//Fetch
			IR = PC;
			PC++;

			//Decode
			temp = *IR;
			for (i = 0; i < 16; i++) {
				if (temp % 2 != 0) {
					bTi = pow(2, i);
					instruction = "1" + instruction; //Used for debugging only
					if (i < 8)
					{
						letter += bTi;
						address += bTi;
					}
					else if (i < 11)
					{
						address += bTi;
					}
					else if (i < 12) {
						blank = true;
						on = true;
						address += bTi;
					}
				}
				else {
					instruction = "0" + instruction; //Used for debugging only
				}
				if (i == 12) {
					opCode = temp;
				}
				temp /= 2;
			}

			//Execute
			switch (opCode)
			{
			case 0: //Left
				if (debug) { cout << "left" << endl; }
				if (head->position != 0) {
				head--;
				moves++;
				}
				else {
					cutUpRollOfTape.push_front({ 0 , true, 0 });
					for (i = 1; i < cutUpRollOfTape.size(); i++)
					{
						head->position = i;
					}
					head = cutUpRollOfTape.begin();
				}
				
				break;
			case 1: //right
				if (debug) { cout << "right" << endl; }
				if (head->position != cutUpRollOfTape.size() - 1) {
					head++;
					moves++;
				}
				else {
					cutUpRollOfTape.push_back({ 0 , true, head->position + 1 });
					head++;
					moves++;
				}
				break;
			case 2: //halt
				if (debug) { cout << "halt" << endl; }
				halted = true;
				done = true;
				break;
			case 3: //fail
				if (debug) { cout << "fail" << endl; }
				halted = false;
				done = true;
				break;
			case 4: //draw
				if (debug) { cout << "draw \"" << letter << "\"" << endl; }
				if (blank) {
					head->letter = letter;
					head->isBlank = blank;
				} else if (alphabet[1][letter] == 1) {
					head->letter = letter;
					head->isBlank = blank;
				}
				break;
			case 5: //alpha
				if (debug) { cout << "alpha \""<< letter << "\"" << endl; }
				if (on) {
					alphabet[0][letter] = letter;
					alphabet[1][letter] = 1;
				}
				else if (!on)
				{
					alphabet[0][letter] = 0;
					alphabet[1][letter] = 0;
				}
				break;
			case 6: //brae
				if (debug) { cout << "brae" << endl; }
				if (EQUALS)
				{
					PC = &RAM[address];
				}
				break;
			case 7: //brane
				if (debug) { cout << "brane" << endl; }
				if (!EQUALS)
				{
					PC = &RAM[address];
				}
				break;
			case 8: //bra
				if (debug) { cout << "bra" << endl; }
				PC = &RAM[address];
				break;
			case 9: //cmp
				if (debug) { cout << "cmp \"" << letter << "\"" << endl; }
				if (blank && head->isBlank == true) {
					EQUALS = true;
				}
				else if (!blank && head->isBlank == true) {
					EQUALS = false;
				}
				else if (letter == head->letter && head->isBlank == false) {
					EQUALS = true;
				}
				else {
					EQUALS = false;
				}
				break;
			default:
				break;
			}
			if (debug) { cout << endl; }
		} while (!done);

		totalMoves += moves;
		totalCycles += cycles;

		if (halted) {
			cout << "Halted after " << moves << " moves and " << cycles << " instructions executed" << endl;
			head2 = head;
			for (i = head2->position; i < cutUpRollOfTape.size(); i++) {
				cout << head2->letter;
				head2++;
			}
			cout << endl;
			head2 = head;
			for (i = head2->position; i < head->position; i++) {
				if (head2->isBlank) {
					cout << " ";
				}
			}
			cout << "^" << endl;
			cutUpRollOfTape.clear();
			PC = &RAM[0];
		}
	}

	cout << endl << endl << "Totals across all tapes..." << endl;
	cout << "       moves: " << totalMoves << endl;
	cout << "instructions: " << totalCycles << endl << endl;

	exit(EXIT_SUCCESS);
}