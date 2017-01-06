#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <stdlib.h>
#include <algorithm>

namespace compiler {
	enum TokType {
		TOKEN_NOP = 0,						
		TOKEN_ADD = 1,						// '+'
		TOKEN_SUB = 2,						// '-'
		TOKEN_MUL = 3,						// '*'
		TOKEN_DIV = 4,						// '/'
		TOKEN_OPEN_PARENTHESIS = 5,			// '('
		TOKEN_CLOSE_PARENTHESIS = 6,		// ')'
		TOKEN_CONSTANT = 7,					// '[0-9]'
		TOKEN_WHITESPACE=9,					// ' ' or '\t'
		/*TOKEN_TEXT=8,						// '[a-zA-Z]'
		TOKEN_TYPE_IDENTIFIER=10,			// ':'
		TOKEN_EQUALS=11,					// '='
		TOKEN_END_OF_STATEMENT=12,			// ';'
		TOKEN_STRUCT = 13,					// 'struct'
		TOKEN_SQUARE_BRACKET_OPEN=14,		// '['
		TOKEN_SQUARE_BRACKET_CLOSE=15,		// ']'
		TOKEN_COMMA=16,						// ','
		TOKEN_FUNC=17,						// 'func'
		TOKEN_FOR=18,						// 'for'
		TOKEN_IN=19,						// 'in'
		TOKEN_WHILE=20,						// 'while'
		TOKEN_IF=21,						// 'if'
		TOKEN_ELSE=22,						// 'else'
		TOKEN_ELSEIF=23,					// 'elseif'
		TOKEN_COMMENT=24,					// '$'
		TOKEN_SPEACH_MARK=25,				// '"'
		TOKEN_PERIOD=26,					// '.'
		TOKEN_AND=27,						// '&'
		TOKEN_OR=28,						// '|'
		TOKEN_NOT=29,						// '~'
		TOKEN_XOR=30,						// '^'
		TOKEN_LEFT_KARET=31,				// '<'
		TOKEN_RIGHT_KARET=32,				// '>'*/
	};

	struct Token {
		TokType type;
		int value; // Only for entered values.

		Token(TokType type) : type(type), value(0) {}
		Token(int value) : type(TOKEN_CONSTANT), value(value) {}
		Token() : type(TOKEN_NOP), value(0) {}
	};

	class Tokenizer {
	public:
		Tokenizer() {
			this->tokens = new vector<Token*>();
		}

		void tokenize(string* input) {
			this->input = input;
		
			string* constant = new string();

			for (this->iterator = 0;iterator < input->length();iterator++) {
				char token = input->at(iterator);
				if (iterator>0 && this->is_number(input->at(iterator-1)) && !this->is_number(token)) { // End of constant, push to token list.
					int val = atoi(constant->c_str()); // ascii to integer.
					this->tokens->push_back(new Token(val));
					cout << *constant << endl;
					constant->clear();
				}
				switch (token) {
				case '+':
					this->tokens->push_back(new Token(TOKEN_ADD));
					break;
				case '-':
					this->tokens->push_back(new Token(TOKEN_SUB));
					break;
				case '/':
					this->tokens->push_back(new Token(TOKEN_DIV));
					break;
				case '*':
					this->tokens->push_back(new Token(TOKEN_MUL));
					break;
				case '(':
					this->tokens->push_back(new Token(TOKEN_OPEN_PARENTHESIS));
					break;
				case ')':
					this->tokens->push_back(new Token(TOKEN_CLOSE_PARENTHESIS));
					break;
				case ' ':
				case '\t':
					this->tokens->push_back(new Token(TOKEN_WHITESPACE));
					break;
				default:
					{
						// if it is a number (0 to 9)
						if (is_number(token)) {
							*constant += token; // Append character to the string.
						} else {
							cout << "Unknown character: " << token << endl;
						}
					}
					break;
 				}
			}

			if (this->is_number(input->back())) { // End of constant, push to token list.
				int val = atoi(constant->c_str()); // ascii to integer.
				this->tokens->push_back(new Token(val));
			}
		}

		void printTokenStream() {
			for (vector<Token*>::iterator it = this->tokens->begin(); it < this->tokens->end(); it++) {
				Token* t = (Token*)(*it);
				cout << "Type: " << t->type << " Value: " << t->value << endl;
			}
		}

		bool is_number(char c) {
			return c>='0' && c<='9';
		}

		bool isChar(char c) {
			if ((c >= 'a' && c <= 'z')||
				(c >= 'A' && c <= 'Z'))
				return true;
			return false;
		}

		vector<Token*>* getTokens() {
			return this->tokens;
		}
	private:
		string* input;
		vector<Token*>* tokens;
		int iterator;
	};

}

#endif