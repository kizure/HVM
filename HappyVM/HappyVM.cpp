// HappyVM.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <ctime>

#include "stack.h"
#include "vm.h"
#include "HInt.h"
#include "HFloat.h"
#include "HArray.h"
#include "HString.h"
#include "HPointer.h"
#include "assembler.h"
#include "jit.h"
#include "tokenizer.h"
#include "ast.h"
#include "vm_emitter.h"

using namespace std;

using namespace compiler;

/*
	Optimization techniques:
		https://en.wikipedia.org/wiki/Constant_folding
		https://en.wikipedia.org/wiki/Peephole_optimization
		https://en.wikipedia.org/wiki/Inline_expansion
		https://en.wikipedia.org/wiki/Dead_code_elimination
		https://en.wikipedia.org/wiki/Loop_unrolling
		https://en.wikipedia.org/wiki/Register_allocation

		Many more optimization techniques:
			https://en.wikipedia.org/wiki/Optimizing_compiler#Interprocedural_optimizations

	Programming Language Design:
		Strongely typed.
		Structs.
		Functions

		struct MyStruct(
			a:string
			b[]:int
		)

		p:int = 10;
		p+=12;

		func:int adder(a:int, b:int) (
			ret a+b;
		)

		for (0 to p) (
		)

		for (a:int in array) (
		)

		while (a<0) (
		)

		if (...) (
		)
*/

void StringAddBenchmark();
void StringSubBenchmark();

int main(int argc, _TCHAR* argv[])
{
	//testJit();
	//return 0;

	// AST testing
	/*cout << "Enter expression to be evaluated" << endl;
	while (true) {
		cout << "> ";
		string input;
		getline(cin, input);

		Tokenizer* tokenizer = new Tokenizer();
		tokenizer->tokenize(&input);
		tokenizer->printTokenStream();
		
		delete tokenizer;

		//AST* ast = new AST(tokenizer->getTokens());
		//ast->buildAst();
	}*/

	//	TODO:
	//		- Implement ARRAY skipInstruction feature.
	//		- Test POINTER data type. (Now supports pointer -> pointer -> ... -> value but needs testing.)
	//			A bug with pointers at the moment. Kinda understand the problem, don't really know how to fix it semi-easily.
	//		- Test skipInstruction to make sure it works correctly.
	//
	//		- Basic Optimization - Use peek for TEST instruction instead of popping and then pushing back onto the stack.
	//		- Possible optimization - For skipping instructions. We have to look at the next instructions data and skip through it all. (especially if it is an array)
	//			To solve this, could seperate the instruction and it's data and then add a fixed size pointer for that instruction (32 bit integer) which references the data.
	//			This would simplify skip instruction a lot but not sure if it would improve performance unless a lot of arrays were being skipped over in data which I doubt.
	//
	//	Long term:
	//		- Runtime optimization
	//		- Make compatible with any endianness.
	//		- make into a JIT-interpreter hybrid compiler.
	//		- Make some sort of programming language so it can be used easily.
	//			Implement the language slowly and build it up. for example start at 1+2
	//
	//	Bug:
	//		- An issue with HPointer has appeared (As the pointer gets de-referenced it changes to the actual value. As each operation deletes the value, it results in a deleted pointer on the 'dataStack')
	//			which try to get deleted again and doesn't work and crashes. (undefined-behaviour)
	//
	//	VM Limitations:
	//		Fixed sized stack for both call stack and data stack. (cannot dynamically shrink and grow)
	//		Limited to a max of 32 unqiue instructions (shouldn't be an issue I hope.)
	//		Max compiled string length is 65535 (runtime is 'unlimited')
	//		Max static array length is signed 32 bit integer. (probably changed to unsigned?)

	VmEmitter* emitter = new VmEmitter(64);
	
	HArray* testArray = new HArray(4);
	testArray->setValue(0, new HInt(4));
	testArray->setValue(1, new HFloat(1.2f));
	testArray->setValue(2, new HFloat(2.0f));
	testArray->setValue(3, new HFloat(4.0f));

	HArray* testArray2 = new HArray(4);
	testArray2->setValue(0, new HInt(1));
	testArray2->setValue(1, new HFloat(1.2f));
	testArray2->setValue(2, new HFloat(2.1f));
	testArray2->setValue(3, new HFloat(4.0f));

	emitter->push(testArray);
	emitter->push(testArray2);
	emitter->sub();
	emitter->end();

	// (2 + 4) * (5 * 2)
	// 2 4 5 2	operands
	// + * *	operators
	// 2 4 + 5 2 * * postfix to infix.
	
	/*emitter->push(2);
	emitter->push(4);
	emitter->add();
	emitter->push(5);
	emitter->push(2);
	emitter->mul();
	emitter->mul();
	emitter->pop(); // "Clean stack"
	emitter->jmp(start);
	emitter->end();
	*/
	/*
	char* potato = (char*)malloc(sizeof(char)*6);
	memcpy(potato, "potato", 6);
	
	char* isnice = (char*)malloc(sizeof(char)*8);
	memcpy(isnice, " is nice", 8);
	
	char* ontoast = (char*)malloc(sizeof(char)*9);
	memcpy(ontoast, " on toast", 9);

	emitter->push(potato, 6);
	emitter->push(isnice, 8);
	emitter->push(ontoast, 9);
	emitter->pushPtr(0);
	emitter->pushPtr(1);
	emitter->pushPtr(2);
	emitter->add();
	emitter->breakpoint();
	emitter->add();
	emitter->pop();
	emitter->pop();
	emitter->pop();
	emitter->end();*/

	HappyVM* testVm = new HappyVM(emitter->complete());
	
	testVm->run();

	cout << "Stack pointer: " << testVm->dataStack->getSize() << endl;

	HObject* o = testVm->dataStack->pop();

	HInt* result = reinterpret_cast<HInt*>(o);
	int iresult = *static_cast<int*>(result->getValue());
	cout << "Result: " << iresult << endl;
	
	/*
	const int iterations = 100000000; // 100 million operations
	cout << "Doing " << iterations << " iterations of the VM program" << endl;
	
	long double elapsed = 0;
	for (int i= 0;i < iterations;i++) {

		const long double start = time(0) * 1000;
		testVm->run();
		elapsed += ( time(0) * 1000 ) - start;
		
		testVm->stop(); // reset ip
		HObject* o = testVm->dataStack->pop();
		delete o;
	}
	
	const long double perIteration = elapsed / (double)iterations;
	
	cout << "Took: " << perIteration * 1000 << "u/s per execution" << endl;
	*/
	//StringAddBenchmark();
	//StringSubBenchmark();

	delete testVm;

	system("pause");

	return 0; // No return error.
}