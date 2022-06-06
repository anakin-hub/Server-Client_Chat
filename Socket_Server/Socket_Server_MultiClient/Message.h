#pragma once
#include <stdio.h>
#include <iostream>

using namespace std;

class Message
{
public:
	int getId();
	const char* getText() const;
	void setId(int id);
	void setText(char* tex);

private:
	int id;
	char text[100];
};

