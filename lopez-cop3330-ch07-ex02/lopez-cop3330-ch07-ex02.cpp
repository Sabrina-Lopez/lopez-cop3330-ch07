/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Sabrina Lopez
 */

#include <iostream>
#include "std_lib_facilities.h"
using namespace std;

const char let = '#';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char sqroot = 's';
//const char power = 'p';
const char constant = 'c';

const string let_key = "let";
const string quit_key = "exit";
const string sqrt_key = "sqrt";
//const string pow_key = "pow";
const string const_key = "const";

class Token {
  public:
    char kind;
  double value;
  string name;
  Token(char ch): kind(ch), value(0) {}
  Token(char ch, double val): kind(ch), value(val) {}
  Token(char ch, string name): kind(ch), name(name) {}
};

class Token_stream {
  public:
    Token_stream(): full(0), buffer(0) {}

  Token get();
  void putback(Token t) {
    buffer = t;
    full = true;
  }

  void ignore(char);

  private:
    bool full;
  Token buffer;
};

Token Token_stream::get() {

  if (full) {
    full = false;
    return buffer;
  }
  char ch {
    ' '
  };
  cin >> ch;
  switch (ch) {
  case '(':
  case ')':
  case '+':
  case '-':
  case '*':
  case '/':
  case '%':
  case ';':
  case '=':
    return Token(ch);

  case '.':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9': {

    cin.putback(ch);
    double val;
    cin >> val;
    return Token(number, val);
  }
  default:
    if ((isalpha(ch)) || (ch == '_')) {
      string s;
      s += ch;
      while (cin.get(ch) && ((isalpha(ch)) || (isdigit(ch)) || (ch == '_'))) s += ch;
      cin.putback(ch);
      if (s == let_key) return Token(let);
      if (s == quit_key) return Token(quit);
      if (s == sqrt_key) return Token(sqroot);
      //if (s == pow_key) return Token(power);
      if (s == const_key) return Token(constant);
      return Token(name, s);
    }

    error("Bad token");
  }
}

void Token_stream::ignore(char c) {

  if (full && c == buffer.kind) {
    full = false;
    return;
  }
  full = false;

  char ch;
  while (cin >> ch)
    if (ch == c) return;
}

class Variable {
  public:
    Variable(string n, double v): name(n), value(v) {}

  string name;
  double value;
};

vector < Variable > names;

double get_value(string s) {

  for (Variable & v: names)
    if (v.name == s) return v.value;
  error("get: undefined name ", s);
}

void set_value(string s, double d) {

  for (Variable & v: names)
    if (v.name == s) {
      v.value = d;
      return;
    }
  error("set: undefined name ", s);
}

bool is_declared(string s) {

  for (Variable & v: names)
    if (v.name == s) return true;
  return false;
}

Token_stream ts;

double expression();

double square_root() {

  Token t = ts.get();
  switch (t.kind) {

  case '(': {

    double d = expression();

    if (d <= 0) error(to_string(d), " cannot be square routed. Enter ';' to continue");

    t = ts.get();
    if (t.kind != ')') error(" ')' was expected. Enter ';' to continue");

    return sqrt(d);
  }
  default:
    error(" '(' was expected. Enter ';' to continue");
  }
}

/*double raise_pow() {

  Token t = ts.get();
  switch (t.kind) {

  case '(': {

    double d1 = expression();

    t = ts.get();
    if (t.kind != ',') error(" ',' was expected. Enter ';' to continue");

    double d2 = expression();

    t = ts.get();
    if (t.kind != ')') error(" ')' was expected. Enter ';' to continue");

    return pow(d1, d2);
  }
  default:
    error(" '(' was expected. Enter ';' to continue");
  }
}*/

double primary() {

  Token t {
    ts.get()
  };
  switch (t.kind) {

  case '(': {
    double d {
      expression()
    };
    t = ts.get();
    if (t.kind != ')') error("')' expected");
    return d;
  }
  case '-':
    return -primary();

  case number:
    return t.value;

  case name:
    return get_value(t.name);

  case sqroot:
    return square_root();

  /*case power:
    return raise_pow();*/

  default:
    error("primary expected");
  }
}

double term() {

  double left {
    primary()
  };
  while (true) {

    Token t {
      ts.get()
    };
    switch (t.kind) {

    case '*':
      left *= primary();
      break;

    case '/': {
      double d {
        primary()
      };
      if (d == 0) error("Cannot divide by zero");
      left /= d;
      break;
    }

    case '%': {
      int i1 {
        left
      };
      int i2 {
        primary()
      };
      if (i2 == 0) error("Cannot divide by zero");
      left = i1 % i2;
      break;
    }

    default:
      ts.putback(t);
      return left;
    }
  }
}

double expression() {

  double left {
    term()
  };
  while (true) {

    Token t {
      ts.get()
    };
    switch (t.kind) {
    case '+':
      left += term();
      break;

    case '-':
      left -= term();
      break;

    default:
      ts.putback(t);
      return left;
    }
  }
}

double declaration() {

  Token t {
    ts.get()
  };
  if (t.kind != 'a') error("name expected in declaration");

  string name = t.name;

  if (is_declared(name)) {
      cout << name + ", declared twice. Do you want to equate this variable to a new value (y/n)? > ";
      cin.clear();
      cin.ignore(10000, '\n');
      string newValueBool;
      getline(cin, newValueBool);

      if (newValueBool == "n") error(name, ", won't be equated to a new value; ");
      if (newValueBool == "y") {
           cout << "Please enter new value = ";
           int newVal;
           cin >> newVal;
           set_value(name, newVal);
           double d = newVal;
           return d;
      }
  }

  Token t2 {
    ts.get()
  };
  if (t2.kind != '=') error("= missing in declaration of ", name);

  double d {
    expression()
  };
  names.push_back(Variable(name, d));

  return d;
}

double statement() {

  Token t {
    ts.get()
  };
  switch (t.kind) {
  case let:
    return declaration();

  default:
    ts.putback(t);
    return expression();
  }
}

void clean_up_mess() {

  ts.ignore(print);
}

const string prompt {
  "> "
};
const string result {
  "= "
};

void calculate() {
  while (true) {
    try {

      cout << prompt;
      Token t = ts.get();
      while (t.kind == print) t = ts.get();
      if (t.kind == quit) return;
      ts.putback(t);
      cout << result << statement() << '\n';
    } catch (runtime_error & e) {
      cerr << e.what() << '\n';
      clean_up_mess();
    }
  }
}

int main() {
  try {
    names.push_back(Variable {
      "k",
      1000
    });
    calculate();
    return 0;
  } catch (exception & e) {
    cerr << "exception: " << e.what() << '\n';
    char c {
      ' '
    };
    while (cin >> c && c != ';');
    return 1;
  } catch (...) {
    cerr << "exception\n";
    char c {
      ' '
    };
    while (cin >> c && c != ';');
    return 2;
  }
}
