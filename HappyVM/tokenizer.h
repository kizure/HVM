#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <stdlib.h>
#include <algorithm>

namespace compiler {
	enum TokType {
		TOKEN_NOP = 0,
		TOKEN_ADD = 1,
		TOKEN_SUB = 2,
		TOKEN_MUL = 3,
		TOKEN_DIV = 4,
		TOKEN_OPEN_PARENTHESIS = 5,
		TOKEN_CLOSE_PARENTHESIS = 6,
		TOKEN_CONSTANT = 7,
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
			input->erase(remove_if(input->begin(), input->end(), isspace), input->end()); // Remove white spaces.
		
			string* constant = new string();

			for (int i = 0;i < input->length();i++) {
				char token = input->at(i);
				if (i>0 && this->is_number(input->at(i-1)) && !this->is_number(token)) { // End of constant, push to token list.
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
			switch (c) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					return true;
				default:
					return false;
			}
		}

		vector<Token*>* getTokens() {
			return this->tokens;
		}
	private:
		string* input;
		vector<Token*>* tokens;
	};

}

#endif