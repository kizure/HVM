#ifndef AST_H
#define AST_H

namespace compiler {

	class ASTLeaf {
	private:
		Token* token;
		ASTLeaf* left;
		ASTLeaf* right;
	public:
		ASTLeaf() {
			this->token = nullptr;
			this->left = nullptr;
			this->right = nullptr;
		}

		~ASTLeaf() {
			delete this->token;
			delete this->left;
			delete this->right;
		}
	};

	// https://en.wikipedia.org/wiki/Shunting-yard_algorithm

	class AST {
	public:
		AST(vector<Token*>* tokens) {
			this->tokens = tokens;
		}

		// Need order of presendence for different operators (I know, bit of a ball ache)

		void buildAst() {
			//nodes = new ASTLeaf(
		}

	private:
		vector<Token*>* tokens;
		ASTLeaf* nodes;
	};

};

#endif