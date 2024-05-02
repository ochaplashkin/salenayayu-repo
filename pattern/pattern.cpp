﻿#include <iostream>
#include <cassert>
#include <string>
#include<vector>

struct Transformer;
struct Number;
struct BinaryOperation;
struct FunctionCall;
struct Variable;

struct Expression //базовая абстрактная структура
{
	virtual ~Expression() { } //виртуальный деструктор
	virtual double evaluate() const = 0; //абстрактный метод «вычислить»
	virtual Expression* transform(Transformer* tr) const = 0;
	virtual std::string print() const = 0;//абстрактный метод печать
};

struct Transformer //pattern Visitor
{
	virtual ~Transformer() {}
	virtual Expression* transformNumber(Number const*) = 0;
	virtual Expression* transformBinaryOperation(BinaryOperation const*) = 0;
	virtual Expression* transformFunctionCall(FunctionCall const*) = 0;
	virtual Expression* transformVariable(Variable const*) = 0;
};

struct Number : Expression // стуктура «Число»
{
	Number(double value) : value_(value) {} //конструктор
	double value() const { return value_; } // метод чтения значения числа
	double evaluate() const { return value_; } // реализация виртуального метода «вычислить»
	~Number() {}//деструктор, тоже виртуальный
	std::string print() const { return std::to_string(this->value_); }
	Expression* transform(Transformer* tr) const
	{
		return tr->transformNumber(this);
	}
private:
	double value_; // само вещественное число
};


struct BinaryOperation : Expression // «Бинарная операция»
{
	enum { // перечислим константы, которыми зашифруем символы операций
		PLUS = '+',
		MINUS = '-',
		DIV = '/',
		MUL = '*'
	};
	// в конструкторе надо указать 2 операнда — левый и правый, а также сам символ операции
	BinaryOperation(Expression const* left, int op, Expression const* right) : left_(left), op_(op), right_(right)
	{
		assert(left_ && right_);
	}
	~BinaryOperation() //в деструкторе освободить занятую память
	{
		delete left_;
		delete right_;
	}
	
	Expression const* left() const { return left_; } // чтение левого операнда
	Expression const* right() const { return right_; } // чтение правого операнда
	int operation() const { return op_; } // чтение символа операции
	double evaluate() const // реализация виртуального метода «вычислить»
	{
		double left = left_->evaluate(); // вычисляем левую часть
		double right = right_->evaluate(); // вычисляем правую часть
		switch (op_) // в зависимости от вида операции, складываем, вычитаем, умножаем или делим левую и правую части
		{
		case PLUS: return left + right;
		case MINUS: return left - right;
		case DIV: return left / right;
		case MUL: return left * right;
		}
	}
	Expression* transform(Transformer* tr) const
	{
		return tr->transformBinaryOperation(this);
	}
	std::string print() const {
		return this->left_->print() + std::string(1, this->op_) + this->right_->print();
	}
private:
	Expression const* left_; // указатель на левый операнд
	Expression const* right_; // указатель на правый операнд
	int op_; // символ операции
};

struct FunctionCall : Expression // структура «Вызов функции»
{
	// в конструкторе надо учесть имя функции и ее аргумент
	FunctionCall(std::string const& name, Expression const* arg) : name_(name),
		arg_(arg)
	{
		assert(arg_);
		assert(name_ == "sqrt" || name_ == "abs");
	} // разрешены только вызов sqrt и abs
	std::string const& name() const
	{
		return name_;
	}
	Expression const* arg() const // чтение аргумента функции
	{
		return arg_;
	}
	~FunctionCall() { delete arg_; } // освободить память в деструкторе
	virtual double evaluate() const { // реализация виртуального метода
		//«вычислить»
		if (name_ == "sqrt")
			return sqrt(arg_->evaluate()); // либо вычисляем корень квадратный
		else return fabs(arg_->evaluate());
	} // либо модуль — остальные функции запрещены
	std::string print() const {
		return this->name_ + "(" + this->arg_->print() + ")";
	}
	Expression* transform(Transformer* tr) const
	{
		return tr->transformFunctionCall(this);
	}
private:
	std::string const name_; // имя функции
	Expression const* arg_; // указатель на ее аргумент
};
struct Variable : Expression // структура «Переменная»
{
	Variable(std::string const& name) : name_(name) { } //в конструкторе надо
	//указать ее имя
	std::string const& name() const { return name_; } // чтение имени переменной
	double evaluate() const // реализация виртуального метода «вычислить»
	{
		return 0.0;
	}
	std::string print() const {
		return this->name_;
	}
	Expression* transform(Transformer* tr) const
	{
		return tr->transformVariable(this);
	}
private:
	std::string const name_; // имя переменной
};


int main()
{
	
	Expression* e1 = new Number(1.234);
	Expression* e2 = new Number(-1.234);
	Expression* e3 = new BinaryOperation(e1, BinaryOperation::DIV, e2);
	std::cout << e3->evaluate() << std::endl;

	Expression* m32 = new Number(32.0);
	Expression* m16 = new Number(16.0);
	Expression* minus = new BinaryOperation(m32, BinaryOperation::MINUS, m16);
	Expression* callSqrt = new FunctionCall("sqrt", minus);
	Expression* m2 = new Number(2.0);
	Expression* mult = new BinaryOperation(m2, BinaryOperation::MUL, callSqrt);
	Expression* callAbs = new FunctionCall("abs", mult);
	std::cout << callAbs->evaluate() << std::endl;

	Number* n32_1 = new Number(32.0);
	Number* n16_1 = new Number(16.0);
	BinaryOperation* minus1 = new BinaryOperation(n32_1, BinaryOperation::MINUS, n16_1);
	FunctionCall* callSqrt1 = new FunctionCall("sqrt", minus);
	Variable* var = new Variable("var");
	BinaryOperation* mult1 = new BinaryOperation(var, BinaryOperation::MUL, callSqrt);
	FunctionCall* callAbs1 = new FunctionCall("abs", mult);
		
	return 0;
}


