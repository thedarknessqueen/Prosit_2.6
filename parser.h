#pragma once

#include <iostream>
#include <stack>

#include "lexer.h"

#ifndef PARSER_H
#define PARSER_H

#endif // PARSER_H

using namespace std;

class Expression;

stack<Expression *> operators;
stack<int> operands;

class Expression {
public:
  virtual void interpret() = 0;
  virtual ~Expression() {}
};

class OpenExpression : public Expression {
  void interpret() { ; }
};

class UnaryOp : public Expression {
public:
  virtual int compute(int right) = 0;

  void interpret() override {
    //if (operands.empty())
      //throw runtime_error("Syntax error: missing operand for unary operator.");

    // Extraire l'opérande de la pile
    int right = operands.top();
    operands.pop();

    // Calculer le résultat de l'opération unaire
    int result = compute(right);

    // Empiler le résultat dans les opérandes
    operands.push(result);
  }
};

class MinusUnaryOp : public UnaryOp {
public:
  int compute(int right) override {
    return -right; // Appliquer l'opération unaire "-"
  }
};


class PlusUnaryOp : public UnaryOp {
public:
  int compute(int right) override {
    return +right; // Appliquer l'opération unaire "+"
  }
};


class BinaryOp : public Expression {
public:
  virtual int compute(int left, int right) = 0;

  void interpret() override {
    //if (operands.size() < 2)
      //throw runtime_error("Syntax error: missing operands for binary operator.");

    // Extraire les deux opérandes de la pile
    int right = operands.top();
    operands.pop();
    int left = operands.top();
    operands.pop();

    // Calculer le résultat de l'opération binaire
    int result = compute(left, right);

    // Empiler le résultat dans les opérandes
    operands.push(result);
  }
};

class MinusBinaryOp : public BinaryOp {
public:
  int compute(int left, int right) override {
    return left - right; // Appliquer l'opération binaire "-"
  }
};


class PlusBinaryOp : public BinaryOp {
public:
  int compute(int left, int right) override {
    return left + right; // Appliquer l'opération binaire "+"
  }
};




class StarBinaryOp : public BinaryOp {
public:
  int compute(int left, int right) override {
    return left * right; // Appliquer l'opération binaire "*"
  }
};


class DivBinaryOp : public BinaryOp {
public:
  int compute(int left, int right) override {
    if (right == 0) {
      throw runtime_error("Division by zero.");
    }
    return left / right; // Appliquer l'opération binaire "/"
  }
};



enum OperatorType { UNARY, BINARY, TERNARY };

class OperatorFactory {
public:
  static Expression *build(TokenType tokenType, OperatorType operatorType) {
    Expression *expr;
    switch (operatorType) {
    case OperatorType::UNARY:
      switch (tokenType) {
      case TokenType::PLUSOPERATOR:
        expr = new PlusUnaryOp();
        break;
      case TokenType::MINUSOPERATOR:
        expr = new MinusUnaryOp();
        break;
      default:
        throw new runtime_error("Unhandled unary operator.");
      }
      break;
    case OperatorType::BINARY:
      switch (tokenType) {
      case TokenType::PLUSOPERATOR:
        expr = new PlusBinaryOp();
        break;
      case TokenType::MINUSOPERATOR:
        expr = new MinusBinaryOp();
        break;
      case TokenType::STAROPERATOR:
        expr = new StarBinaryOp();
        break;
      case TokenType::SLASHOPERATOR:
        expr = new DivBinaryOp();
        break;
      default:
        throw new runtime_error("Unhandled binary operator.");
      }
      break;
    default:;
    }
    return expr;

  }
};

class ExtendedParser {
  vector<Token *> tokens;
  int idx;

public:
  ExtendedParser(vector<Token *> tokens) {
    this->tokens = tokens;

    reset();
  }

  Token *next() {
    if (idx < tokens.size()) {
      return tokens[idx++];
    }

    throw runtime_error("Syntax error.");
  }
  void rewind() { idx--; }
  void reset() { idx = 0; }
  int parse() {
    reset();
    consumeBaseExpression();

    return operands.top();
  }
  void consumeLeftParenthesis() {
    Token *curr = next();

    if (!curr->isType(TokenType::LPARENTHESIS)) {
      throw runtime_error("Expected opening parenthesis.");
    }
  }
  void consumeRightParenthesis() {
    Token *curr = next();

    if (!curr->isType(TokenType::RPARENTHESIS)) {
      throw runtime_error("Expected closing parenthesis.");
    }
  }
  bool consumeBinop() {
    Token *curr = next();

    if (curr->isType(TokenType::MINUSOPERATOR) ||
        curr->isType(TokenType::PLUSOPERATOR) ||
        curr->isType(TokenType::STAROPERATOR) ||
        curr->isType(TokenType::SLASHOPERATOR)) {
      // Créer un opérateur binaire à l'aide de la factory
      Expression *expr = OperatorFactory::build(curr->getType(), BINARY);

      // Ajouter l'opérateur à la pile
      operators.push(expr);

      return true;
        }

    rewind(); // Si ce n'est pas un opérateur binaire, revenir en arrière
    return false;
  }

  bool consumeUnop() {
    Token *curr = next();

    if (curr->isType(TokenType::MINUSOPERATOR) ||
        curr->isType(TokenType::PLUSOPERATOR)) {
      // Créer un opérateur unaire à l'aide de la factory
      Expression *expr = OperatorFactory::build(curr->getType(), UNARY);

      // Ajouter l'opérateur à la pile
      operators.push(expr);

      return true;
        }

    rewind(); // Si ce n'est pas un opérateur unaire, revenir en arrière
    return false;
  }

  bool consumeLitteral() {
    Token *curr = next();

    if (curr->isType(TokenType::INTEGER)) {
      // Convertir la valeur du littéral en entier et l'empiler
      operands.push(stoi(curr->getValue()));
      return true;
    }

    rewind(); // Si ce n'est pas un littéral, revenir en arrière
    return false;
  }

  void consumeBaseExpression() {

    if (!consumeLitteral()) {
      consumeLeftParenthesis();
      consumeExpression();
      consumeRightParenthesis();
      //cout << "Exiting consumeBaseExpression at idx = " << idx << endl;
      solve();
    }

  }


  void consumeExpression() {
    if (!consumeUnop()) {
      consumeBaseExpression();
      consumeBinop();
    }

    consumeBaseExpression();
  }

  void solve() {
    Expression *expr;

    if (operators.size() >= 1) {
      expr = operators.top();
      operators.pop();

      expr->interpret();
      delete expr;
    } else
      throw runtime_error("Syntax error.");
  }
};

