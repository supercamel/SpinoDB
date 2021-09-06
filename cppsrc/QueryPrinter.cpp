/*
#include "QueryPrinter.h"

using namespace std;
using namespace Spino;

void QueryPrinter::Visit(NumericValue* f) { 
	print_indent();
	cout << "Numeric value: " << f->value << endl;
}

void QueryPrinter::Visit(StringValue* s) { 
	print_indent();
	cout << "String value: " << s->value << endl;
}

void QueryPrinter::Visit(BoolValue* b) {
	print_indent();
	cout << "Bool value: " << b->value << endl;
}

void QueryPrinter::Visit(Field* f) { 
	print_indent();
	cout << "Field: ";
	for(auto& f : f->name) {
		cout << f << ".";
	}
	cout << endl;
	indent++;
	f->operation->Accept(this);
	indent--;
}

void QueryPrinter::Visit(LogicalExpression* l) { 
	print_indent();
	cout << "Logical expression: " << l->op << endl;
	indent++;
	for(auto& f : l->fields) {
		f->Accept(this);
	}
	indent--;
}

void QueryPrinter::Visit(List* l) {
	print_indent();
	cout << "List" << endl;
	indent++;
	for(auto& l : l->list) {
		l->Accept(this);
	}
	indent--;
}

void QueryPrinter::Visit(Operator* a) { 
	print_indent();
	cout << "Operator: " << a->op << endl;
	indent++;
	a->cmp->Accept(this);
	indent--;
}	

*/
