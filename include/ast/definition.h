#ifndef _DEFINITION_SCHEMER
#define _DEFINITION_SCHEMER

#include <ast/defformals.h>

#include <deque>


class Definition : public AstList {
public:	
	Definition(std::deque<std::shared_ptr<AstTree>> &var) 
	:AstList(var) {
	}
	virtual std::shared_ptr<Variable> getVariable()=0;
};

class DefinitionNormal : public Definition {
public:	
	DefinitionNormal(std::deque<std::shared_ptr<AstTree>> &var) 
	:Definition(var) {
	}

	std::shared_ptr<Expression> getExpression() {
		return std::dynamic_pointer_cast<Expression>(child(1));
	}

	virtual std::shared_ptr<Variable> getVariable() override {
		return std::dynamic_pointer_cast<Variable>(child(0));
	}

	virtual std::string toString() override {
		std::string s;
		s += "(define";
		std::shared_ptr<AstTree> p;
		
		for(int i = 0; (i < numChildren()) && (p = child(i)); i++) {
			s += " ";
			s += p->toString();
		}
		s += ")";
		return s;
	}

	virtual Object* eval(EnvironmentObject* env) override {
		std::shared_ptr<Variable> var = getVariable();
		if (!var) {
			throw *new IllFormedException(this->toString());
		}
		std::shared_ptr<Expression> exp;
		if (numChildren() > 2) { 
			throw *new IllFormedException(this->toString());
		}
		exp = getExpression();
		if (exp) {
			Object* obj = exp->eval(env);
			env->put(var->getName(),obj);
		} else {
			throw *new IllFormedException(this->toString());
		}
		return nullptr;
	}
};



class DefinitionVarlist : public Definition {
public:
	DefinitionVarlist(std::deque<std::shared_ptr<AstTree>> &var) 
	:Definition(var) {

	}

	virtual std::shared_ptr<Variable> getVariable() override {
		return std::dynamic_pointer_cast<Variable>(child(0));
	}

	virtual std::string toString() override {
		std::string s;
		s += "(define";
		std::string sep(" ");
		std::shared_ptr<AstTree> p;
		
		for(int i = 0; (i < numChildren()) && (p = child(i)); i++) {
			s += sep;
			s += p->toString();
			//sep = " ";
		}
		s += ")";
		return s;
	}

	std::shared_ptr<DefFormals> getDefFormals() {
		return std::dynamic_pointer_cast<DefFormals>(child(1));
	}

	std::shared_ptr<Body> getBody() {
		return std::dynamic_pointer_cast<Body>(child(2));
	}

	virtual Object* eval(EnvironmentObject *env) override {
		std::shared_ptr<Variable> var = getVariable();
		if (!var) 
			throw *new IllFormedException(this->toString());
		std::shared_ptr<DefFormals> def = getDefFormals();
		std::shared_ptr<Body> body = getBody();

		if(!body || !body->sequence()->numChildren()) 
			throw *new IllFormedException(this->toString());

		NormalFunctionObject* funobj = NormalFunctionObject::allocNormalFunction(def,body,env);

		env->put(var->getName(),funobj);
		
		return nullptr;
	}
};


#endif/*_DEFINITION_SCHEMER*/
