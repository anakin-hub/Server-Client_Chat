#include "Message.h"

int Message::getId()
{
	return id;
}

const char* Message::getText() const
{
	const char* t;
	t = text;
	return t;
}

void Message::setId(int id)
{
	this->id = id;
}

void Message::setText(char* tex)
{
	strcpy_s(text, tex);
}
