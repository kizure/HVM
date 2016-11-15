
#ifndef VARIABLE_ARRAY_H
#define VARIABLE_ARRAY_H

typedef struct {
	char* name;
} ELEMENT;

class Enum {
private:
	ELEMENT* elements;
	int elementPos;

public:
	Enum(int maxSize);
	~Enum();

	void add_element(char* name);
	ELEMENT get_element(char* name);
};

#endif