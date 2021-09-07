#include <iostream>
#include "QueryExecutor.h"
#include "QueryParser.h"


namespace Spino {
	void QueryExecutor::Visit(NumericValue* f) {
		Value& v = stack[stack_ptr++];
		if(stack_ptr == stack.size()) {
			stack.resize(stack.size() + 100);
		}

		v.numeric = f->value;
		v.type = TYPE_NUMERIC;
	}

	void QueryExecutor::Visit(StringValue* s) {
		Value& v = stack[stack_ptr++];
		if(stack_ptr == stack.size()) {
			stack.resize(stack.size() + 100);
		}

		v.str = s->value;
		v.type = TYPE_STRING;
	}

	void QueryExecutor::Visit(BoolValue* b) {
		Value& v = stack[stack_ptr++];
		if(stack_ptr == stack.size()) {
			stack.resize(stack.size() + 100);
		}

		v.boolean = b->value;
		v.type = TYPE_BOOLEAN;
	}

	void QueryExecutor::Visit(Field* f) {
		Value& v = stack[stack_ptr++];
		if(stack_ptr == stack.size()) {
			stack.resize(stack.size() + 100);
		}

		auto a = f->jp.Get(*doc);
		if(a) {
			if(a->IsNull()) {
				v.type = TYPE_NULL;
			} 
			else if(a->IsBool()) {
				v.type = TYPE_BOOLEAN;
				v.boolean = a->GetBool();
			} 
			else if(a->IsNumber()) {
				v.type = TYPE_NUMERIC;
				v.numeric = a->GetDouble();
			}
			else if(a->IsString()) {
				v.type = TYPE_STRING;
				v.str = a->GetString();
			}
			else if(a->IsArray()) {
				v.type = TYPE_ARRAY;
			}
			else if(a->IsObject()) {
				v.type = TYPE_OBJECT;
			}
		}
		else {
			v.type = TYPE_UNDEFINED;
		}

		f->operation->Accept(this);
	}

	void QueryExecutor::Visit(LogicalExpression* l) {
		for(auto& i : l->fields) {
			i->Accept(this);

			auto& a = stack[--stack_ptr];
			if((l->op == TOK_AND) && (a.boolean == false)) {
				a.type == TYPE_BOOLEAN;
				return;
			}
			if((l->op == TOK_OR) && (a.boolean == true)) {
				a.type == TYPE_BOOLEAN;
				return;
			}
		}
	}

	void QueryExecutor::Visit(List* l) {
		for(auto& item : l->list) {
			item->Accept(this);
		}
	}

	void QueryExecutor::Visit(Operator* a) {
		uint32_t init_stack_sz = stack_ptr;
		a->cmp->Accept(this);

		switch(a->op) {
			case TOK_EQUAL: 
				{
					auto& b = stack[--stack_ptr];
					auto& a = stack[--stack_ptr];

					Value& v = stack[stack_ptr++];	
					v.type = TYPE_BOOLEAN;
					v.boolean = (a == b);
				}
				break;
			case TOK_NE:
				{
					auto& b = stack[--stack_ptr];
					auto& a = stack[--stack_ptr];

					Value& v = stack[stack_ptr++];	
					v.type = TYPE_BOOLEAN;
					v.boolean = !(a == b);
				}
				break;
			case TOK_GREATER_THAN: 
				{
					auto& b = stack[--stack_ptr];
					auto& a = stack[--stack_ptr];
					bool boolean = false;

					if(a.numeric > b.numeric) {
						boolean = true;
					}	
					Value& v = stack[stack_ptr++];
					v.type = TYPE_BOOLEAN;
					v.boolean = boolean;
				}
				break;
			case TOK_LESS_THAN:
				{
					auto& b = stack[--stack_ptr];
					auto& a = stack[--stack_ptr];
					bool boolean = false;

					if(a.numeric < b.numeric) {
						boolean = true;
					}
					Value& v = stack[stack_ptr++];
					v.type = TYPE_BOOLEAN;
					v.boolean = boolean;
				}
				break;
			case TOK_IN:
				{
					bool result = false;
					Value& field = stack[init_stack_sz-1];
					for(uint32_t i = init_stack_sz; i < stack_ptr; i++) {
						Value& cmp = stack[i];
						if(field.type == cmp.type) {
							if(field.type == TYPE_BOOLEAN) {
								if(field.boolean == cmp.boolean) {
									result = true;
									break;
								}
							}
							else if(field.type == TYPE_NUMERIC) {
								if(fabs(field.numeric-cmp.numeric) < 0.000001) {
									result = true;
									break;
								}
							}
							else if(field.type == TYPE_STRING) {
								if(field.str == cmp.str) {
									result = true;
									break;
								}
							}
							else {
								result = true;
								break;
							}
						}
					}
					stack_ptr = init_stack_sz;
					Value& v = stack[stack_ptr++];
					v.type = TYPE_BOOLEAN;
					v.boolean = result;
				}
				break;
			case TOK_NIN:
				{
					Value& field = stack[init_stack_sz-1];
					if(field.type == TYPE_UNDEFINED) {
						field.type = TYPE_BOOLEAN;
						field.boolean = true;
						stack_ptr++;
					}
					else {
						bool result = true;
						for(uint32_t i = init_stack_sz; i < stack_ptr; i++) {
							Value& cmp = stack[i];
							if(field.type == cmp.type) {
								if(field.type == TYPE_BOOLEAN) {
									if(field.boolean == cmp.boolean) {
										result = false;
										break;
									}
								}
								else if(field.type == TYPE_NUMERIC) {
									if(fabs(field.numeric-cmp.numeric) < 0.000001) {
										result = false;
										break;
									}
								}
								else if(field.type == TYPE_STRING) {
									if(field.str == cmp.str) {
										result = false;
										break;
									}
								}
								else {
									result = false;
									break;
								}
							}
						}
						stack_ptr = init_stack_sz;
						Value& v = stack[stack_ptr++];
						v.type = TYPE_BOOLEAN;
						v.boolean = result;
					}
				}
				break;
			case TOK_EXISTS:
				{
					auto& b = stack[--stack_ptr];
					auto& a = stack[--stack_ptr];

					bool exists = ((a.type == TYPE_UNDEFINED) != b.boolean);
					Value& v = stack[stack_ptr++];
					v.type = TYPE_BOOLEAN;
					v.boolean = exists;
				}
				break;
			case TOK_TYPE:
				{
					--stack_ptr;
					auto& b = stack[--stack_ptr];

					bool type_match = false;
					StringValue* sval = reinterpret_cast<StringValue*>(a->cmp.get());
					if(b.type == TYPE_STRING && sval->value == "string") {
						type_match = true;	
					}
					else if(b.type == TYPE_NUMERIC && sval->value == "number") {
						type_match = true;
					}
					else if(b.type == TYPE_BOOLEAN && sval->value == "bool") {
						type_match = true;
					}
					else if(b.type == TYPE_OBJECT && sval->value == "object") {
						type_match = true;
					}
					else if(b.type == TYPE_ARRAY && sval->value == "array") {
						type_match = true;
					}

					Value& v = stack[stack_ptr++];
					v.type = TYPE_BOOLEAN;
					v.boolean = type_match;

				}
				break;
			case TOK_NOT:
				{
					Value& v = stack[stack_ptr-1];
					v.boolean = !v.boolean;
				}
				break;

		}
	}

	void QueryExecutor::Visit(BasicFieldComparison* b) {
		Value& v = stack[stack_ptr++];
		if(stack_ptr == stack.size()) {
			stack.resize(stack.size() + 100);
		}

		v.boolean = false;

		auto a = b->jp.Get(*doc);
		if(a) {
			if(a->IsNumber() && (b->v.type == TYPE_NUMERIC)) {
				if(fabs(a->GetDouble()-b->v.numeric) < 0.000001) {
					v.type = TYPE_BOOLEAN;
					v.boolean = true;
				}
			}
			else if(a->IsString() && (b->v.type == TYPE_STRING)) {
				if(a->GetString() == b->v.str) {
					v.type = TYPE_BOOLEAN;
					v.boolean = true;
				}
			}
		}
	}

	bool QueryExecutor::resolve(std::shared_ptr<QueryNode> b) {
		if(b == nullptr) {
			return true;
		}

		stack_ptr = 0;
		stack[0].boolean = false;
		b->Accept(this);
		return stack[0].boolean;
	}

}


