#include <string>
#include <sstream>
#include <vector>
#include "HObject.h"

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#define PREPROCESSOR_DEFINE "#define"
#define OPERATION_ADD		"ADD"
#define OPERATION_SUB		"SUB"
#define OPERATION_MUL		"MUL"
#define OPERATION_DIV		"DIV"
#define OPERATION_MOD		"MOD"
#define OPERATION_AND		"AND"
#define OPERATION_OR		"OR"
#define OPERATION_XOR		"XOR"
#define OPERATION_NOT		"NOT"
#define OPERATION_CALL		"CALL"
#define OPERATION_RET		"RET"
#define OPERATION_TEST		"TEST" // (Used for determining if a value ==, !=)
#define OPERATION_PUSH		"PUSH"
#define OPERATION_END		"END"
#define OPERATION_JMP		"JMP"
#define OPERATION_SHL		"SHL"
#define OPERATION_SHR		"SHR"

class Assembler {
	public:
	static char* assemble(std::string* str, int& asmlen) {
		// split the text by line.
		// preprocessors then the actual asm.
		// What preprocessors are needed

		// These are in a specific order so the id value joins with their index.
		char* availableInstructions[] = {
			OPERATION_ADD,
			OPERATION_SUB,
			OPERATION_MUL,
			OPERATION_DIV,
			OPERATION_MOD,
			OPERATION_OR,
			OPERATION_XOR,
			OPERATION_NOT,
			OPERATION_AND,
			OPERATION_JMP,
			OPERATION_CALL,
			OPERATION_RET,
			OPERATION_TEST,
			OPERATION_PUSH,
			OPERATION_END,
			OPERATION_SHL,
			OPERATION_SHR
		};

		auto lines = split(*str, '\n');
		for (int i =0;i < lines.size();i++) {
			std::string line = lines[i];

			if (line.find(PREPROCESSOR_DEFINE) != std::string::npos) {
				// Add to some list of preprocessors
			}
		}

		return nullptr;
	}

private:
	static void split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss;
		ss.str(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
	}


	static std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}
};

#endif