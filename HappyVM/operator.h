#ifndef OPERATOR_H
#define OPERATOR_H

namespace compiler {
	namespace operators {
		class Operator {
			Operator(int presendence){
				this->presedence = presendence;
			}
			~Operator();
		private:
			int presedence;
		};

		/**************** Operator Presendance table *******************
		The higher the number the higher the presedence the operator is.

		+	1
		-	1
		*	2
		/	2

		// Example input:
			1 + 2*3

			The * is more important so it gets evaluated first.
	

		****************************************************************/

		class PlusOperator : public Operator {
			PlusOperator() : Operator(1) {}
		};

		class MinusOperator : public Operator {
			MinusOperator() : Operator(1) {}
		};

		class MultiplyOperator: public Operator {
			MinusOperator() : Operator(2) {}
		};

		class DivideOperator: public Operator {
			MinusOperator() : Operator(2) {}
		};

	};
};
#endif