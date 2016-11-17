/*bool x86runtest(X86Emitter* emitter, char* code) {
	for (int i = 0;i < emitter->getLength();i++) {
		if ((char)emitter->buffer[i] != code[i]) {
			cout << "Test failed! expected: " << code[i] << " got " << emitter->buffer[i] << endl;
			return false;
		}
	}
	cout << "Test passed" << endl;
	return true;
}

void test() {
	X64Emitter* emitter = new X64Emitter();
	emitter->movr64rm64(REG_RAX, new MEM_REF(REG_RCX, 1, 1)); // mov rax, [rcx+1]
	x64runtest(emitter, "\x48\x8B\x41\x01");
	emitter->clear();

	emitter->movr64rm64(REG_RAX, new MEM_REF(REG_R9, 1, 0)); // mov rax, [r9]
	x64runtest(emitter, "\x49\x8B\x01");
	emitter->clear();

	emitter->movr64rm64(REG_RAX, new MEM_REF(REG_R12, 2, 9)); // mov rax, [r12*2+9]
	x64runtest(emitter, "\x4A\x8B\x04\x65\x09\x00\x00\x00");
	emitter->clear();

	emitter->movr64rm64(REG_RAX, new MEM_REF(REG_R13, 1, 0)); // mov rax, [r13]
	x64runtest(emitter, "\x49\x8B\x45\x00");
	emitter->clear();

	emitter->movr64rm64(REG_RAX, new MEM_REF(REG_R13, 1, 1)); // mov rax, [r13+1]
	x64runtest(emitter, "\x49\x8B\x45\x01");
	emitter->clear();

	emitter->movr64rm64(REG_RAX, new MEM_REF(REG_R13, 2, 1)); // mov rax, [r13*2+1]
	x64runtest(emitter, "\x4A\x8B\x04\x6D\x01\x00\x00\x00");
	emitter->clear();

	cout << "X64 Emitter tests completed" << endl;
}*/