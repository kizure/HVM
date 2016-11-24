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
	testJit();
	return 0;

	// AST testing
	/*cout << "Enter expression to be evaluated" << endl;
	while (true) {
		cout << "> ";
		string input;
		getline(cin, input);

		Tokenizer* tokenizer = new Tokenizer();
		tokenizer->tokenize(&input);
		tokenizer->printTokenStream();

		AST* ast = new AST(tokenizer->getTokens());
		ast->buildAst();
	}*/

	//	TODO:
	//		- Implement FLOATs reading from the binary (pushObject)
	//		- Implement ARRAYs reading from the binary (pushObject)
	//		- Finish ARRAY operations.
	//		- Implement ARRAY compare
	//		- Implement ARRAY skipInstruction feature.
	//		- Test STRING data type
	//		- Test array data type.
	//		- Test pointer data type. (Now supports pointer -> pointer -> ... -> value)
	//		- Test skipInstruction to make sure it works correctly.
	//
	//		- Basic Optimization - Use peek for test instead of popping and then pushing back onto the stack.
	//
	//	Long term:
	//		- Runtime optimization
	//		- Make compatible with any endianness.
	//		- make into a JIT-interpreter hybrid compiler.
	//		- Make some sort of programming language so it can be used easily.
	//			Implement the language slowly and build it up. for example start at 1+2
	//
	//	VM Limitations:
	//		Fixed sized stack for both call stack and data stack. (cannot dynamically shrink and grow)
	//		Limited to a max of 32 unqiue instructions (shouldn't be an issue I hope.)
	//		Max compiled string length is 65535 (runtime is 'unlimited')

	VmEmitter* emitter = new VmEmitter(64);
	int start = emitter->label();
	
	// (2 + 4) * (5 * 2)
	emitter->push(2);
	emitter->push(4);
	emitter->add();
	emitter->push(5);
	emitter->push(2);
	emitter->mul();
	emitter->mul();
	emitter->pop();
	emitter->jmp(start);
	emitter->end();

	HappyVM* testVm = new HappyVM(emitter->complete());
	
	testVm->run();
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

void StringAddBenchmark() {
	const int iterations = 100000000; // 100 million operations
	cout << "Doing " << iterations << " iterations of the HOP_ADD operation for two strings" << endl;
	
	const char* hello = "hello ";
	const char* world = "world";

	long double elapsed = 0;
	for (int i= 0;i < iterations;i++) {
		char* aBuf = (char*)malloc(6 * sizeof(char)); // Hello world
		char* bBuf = (char*)malloc(5 * sizeof(char)); // word
		memcpy(aBuf, hello, 6*sizeof(char));
		memcpy(bBuf, world, 5*sizeof(char));

		HString* sa = new HString(aBuf, 6);
		HString* sb = new HString(bBuf, 5);
		const long double start = time(0) * 1000;
		sa->operation(sb, HOP_ADD);
		elapsed += time(0) * 1000 - start;
		//cout << "Result: " << static_cast<char*>(sa->getValue()) << endl;

		delete sa;
		delete sb;
	}
	
	const long double perIteration = elapsed / (double)iterations;

	cout << "Took: " << perIteration * 1000 << "u/s per operation" << endl;
}

void StringSubBenchmark() {
	const int iterations = 100000000; // 100 million operations
	cout << "Doing " << iterations << " iterations of the HOP_SUB operation for two strings" << endl;
	
	const char* helloWord = "hello world";
	const char* world = "world";

	long double elapsed = 0;
	for (int i= 0;i < iterations;i++) {
		char* aBuf = (char*)malloc(11 * sizeof(char)); // Hello world
		char* bBuf = (char*)malloc(5 * sizeof(char)); // word
		memcpy(aBuf, helloWord, 11*sizeof(char));
		memcpy(bBuf, world, 5*sizeof(char));

		HString* sa = new HString(aBuf, 11);
		HString* sb = new HString(bBuf, 5);
		const long double start = time(0) * 1000;
		sa->operation(sb, HOP_SUB);
		elapsed += time(0) * 1000 - start;

		delete sa;
		delete sb;
	}
	
	const long double perIteration = elapsed / (double)iterations;

	cout << "Took: " << perIteration * 1000 << "u/s per operation" << endl;
}