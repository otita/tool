//
//  JSON.cpp
//
//  Created by otita on 2016/02/08.
//
/*
The MIT License (MIT)
Copyright (c) 2016 otita.
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <cassert>

#include "JSON.h"

#define JSON_HANDLE_EXCEPTION(condition, err_msg) \
{ \
  try { \
    if (!(condition)) { \
      throw logic_error(err_msg); \
    } \
  } \
  catch (const logic_error &error) { \
    cerr << error.what() << endl; \
    throw; \
  } \
}

using namespace std;

namespace otita {

namespace tool {

class JSON_Parser {
public:
  JSON_Parser(const string &source);
  JSON parse();
private:
  unsigned long long _at;
  unsigned char _ch;
  const string &_text;
  unsigned char _next(unsigned char c  = 0);
  JSON _number();
  JSON _string();
  void _white();
  JSON _word();
  JSON _array();
  JSON _object();
  JSON _value();
};

JSON JSON::parse(const ::std::string &source) {
  return JSON_Parser(source).parse();
}

JSON::JSON() {
  _type = JSON_NULL;
}

JSON::JSON(double number) {
  _type = JSON_NUMBER;
  _field.number = number;
}

JSON::JSON(bool boolean) {
  _type = JSON_BOOLEAN;
  _field.boolean = boolean;
}

JSON::JSON(const json_string &string) {
  _type = JSON_STRING;
  _field.string_ptr = new json_string(string);
}

JSON::JSON(const char c_str[]) {
  _type = JSON_STRING;
  _field.string_ptr = new json_string(c_str);
}

JSON::JSON(json_array *array_ptr) {
  _type = JSON_ARRAY;
  _field.array_ptr = array_ptr;
}

JSON::JSON(json_object *object_ptr) {
  _type = JSON_OBJECT;
  _field.object_ptr = object_ptr;
}

JSON::JSON(const JSON &other) {
  _type = other._type;
  switch (other._type) {
    case JSON_NULL:
      break;
    case JSON_NUMBER:
      _field.number = other.number();
      break;
    case JSON_BOOLEAN:
      _field.boolean = other.boolean();
      break;
    case JSON_STRING:
      _field.string_ptr = new json_string(other.string());
      break;
    case JSON_ARRAY:
      _field.array_ptr = new json_array;
      _field.array_ptr->reserve(other._field.array_ptr->size());
      for (JSON *obj : *other._field.array_ptr) {
        _field.array_ptr->push_back(new JSON(*obj));
      }
      break;
    case JSON_OBJECT:
      _field.object_ptr = new json_object;
      _field.object_ptr->reserve(other._field.object_ptr->size());
      for (auto pair : *other._field.object_ptr) {
        _field.object_ptr->insert(make_pair(pair.first, new JSON(*pair.second)));
      }
      break;
    default:
      break;
  }
}

JSON::json_t JSON::type() const {
  return _type;
}

double JSON::number() const {
  JSON_HANDLE_EXCEPTION(
    _type == JSON_NUMBER,
    "Exception: JSON::number()\n \
    type must be JSON_NUMBER."
  );
  return _field.number;
}

bool JSON::boolean() const {
  JSON_HANDLE_EXCEPTION(
    _type == JSON_BOOLEAN,
    "Exception: JSON::boolean()\n \
    type must be JSON_BOOLEAN."
  );
  return _field.boolean;
}

const JSON::json_string &JSON::string() const {
  JSON_HANDLE_EXCEPTION(
    _type == JSON_STRING,
    "Exception: JSON::string()\n \
    type must be JSON_STRING."
  );
  return *(_field.string_ptr);
}

JSON &JSON::operator =(const JSON &other) {
  switch (_type) {
    case JSON_STRING:
      delete _field.string_ptr;
      break;
    case JSON_ARRAY:
      delete _field.array_ptr;
      break;
    case JSON_OBJECT:
      delete _field.object_ptr;
    default: ;
  }
  
  _type = other._type;
  switch (_type) {
    case JSON_NUMBER:
      _field.number = other._field.number;
      break;
    case JSON_BOOLEAN:
      _field.boolean = other._field.boolean;
      break;
    case JSON_STRING:
      _field.string_ptr = new json_string(other.string());
      break;
    case JSON_ARRAY:
      _field.array_ptr = new json_array(*other._field.array_ptr);
      break;
    case JSON_OBJECT:
      _field.object_ptr = new json_object(*other._field.object_ptr);
    default: ;
  }
  return *this;
}

JSON &JSON::operator [](size_t i) {
  if (_type==JSON_NULL) {
    _type = JSON_ARRAY;
    _field.array_ptr = new json_array;
  }
  
  JSON_HANDLE_EXCEPTION(
    _type == JSON_ARRAY,
    "Exception: JSON::operator [](size_t)\n \
    type must be JSON_ARRAY."
  );
  size_t size = _field.array_ptr->size();
  if (size <= i) {
    while (_field.array_ptr->size() <= i) {
      _field.array_ptr->push_back(new JSON());
    }
  }
  return *(*_field.array_ptr)[i];
}

const JSON &JSON::operator [](size_t i) const {
  JSON_HANDLE_EXCEPTION(
    _type == JSON_ARRAY,
    "Exception: JSON::operator [](size_t)\n \
    type must be JSON_ARRAY."
  );
  JSON_HANDLE_EXCEPTION(
    i < _field.array_ptr->size(),
    "Exception: JSON::operator [](size_t)\n \
    invalid index."
  );
  return *(*_field.array_ptr)[i];
}

JSON &JSON::operator [](const json_string &key) {
  if (_type == JSON_NULL) {
    _type = JSON_OBJECT;
    _field.object_ptr = new json_object;
  }

  JSON_HANDLE_EXCEPTION(
    _type == JSON_OBJECT,
    "Exception: JSON::operator [](const json_string &)\n \
    type must be JSON_OBJECT."
  );
  
  auto it = _field.object_ptr->find(key);
  if (it == _field.object_ptr->end()) {
    _field.object_ptr->insert({key, new JSON()});
    it = _field.object_ptr->find(key);
  }
  return *(it->second);
}

const JSON &JSON::operator [](const json_string &key) const {
  JSON_HANDLE_EXCEPTION(
    _type == JSON_OBJECT,
    "Exception: JSON::operator [](const json_string &)\n \
    type must be JSON_OBJECT."
  );

  auto it = _field.object_ptr->find(key);
  
  JSON_HANDLE_EXCEPTION(
    it != _field.object_ptr->end(),
    "Exception: JSON::operator [](const json_string &)\n \
    not registered key."
  );
  return *(it->second);
}

JSON::~JSON() {
  switch (_type) {
    case JSON_STRING:
      delete _field.string_ptr;
      break;
    case JSON_ARRAY:
      for (auto it : *_field.array_ptr) {
        delete it;
      }
      delete _field.array_ptr;
      break;
    case JSON_OBJECT:
      for (auto it=_field.object_ptr->begin(); it!=_field.object_ptr->end(); it++) {
        delete it->second;
      }
//      for (auto it : *_field.object_ptr) {
//        delete it.second;
//      }
      delete _field.object_ptr;
      break;
    default:
      break;
  }
}

struct escape_pair {
  char key;
  char value;
};

static escape_pair escapee[] = {
  {'\"', '\"'},
  {'\\', '\\'},
  {'b' , '\b'},
  {'f' , '\f'},
  {'n' , '\n'},
  {'r' , '\r'},
  {'t' , '\t'},
  {'\0', '\0'},
};

JSON_Parser::JSON_Parser(const string &source) : _text(source) {
  _at = 0;
  _ch = ' ';
}

JSON JSON_Parser::parse() {
  JSON result = _value();
  if (_ch) {
    // syntax error
    return JSON();
  }
  return result;
}

unsigned char JSON_Parser::_next(unsigned char c) {
  if (c && c != _ch) {
    // error
  }
  _ch = _text[_at++];
  return _ch;
}

JSON JSON_Parser::_number() {
  string str = "";
  if (_ch == '-') {
    str = _ch;
    _next('-');
  }
  
  while (_ch >= '0' && _ch <= '9') {
    str += _ch;
    _next();
  }
  
  if (_ch == '.') {
    str += _ch;
    while (_next() &&
           _ch >= '0' && _ch <= '9') {
      str += _ch;
    }
  }
  
  if (_ch == 'e' || _ch == 'E') {
    str += _ch;
    _next();
    if (_ch == '-' || _ch == '+') {
      str += _ch;
      _next();
    }
    while (_ch >= '0' && _ch == '9') {
      str += _ch;
      _next();
    }
  }
  return atof(str.c_str());
}

JSON JSON_Parser::_string() {
  string str = "";
  if (_ch == '"') {
    while (_next()) {
      if (_ch == '"') {
        _next();
        return str;
      }
      else if (_ch == '\\') {
        _next();
        bool is_escapee = false;
        for (int i = 0; escapee[i].key != '\0'; i++) {
          if (_ch == escapee[i].key) {
            str += escapee[i].value;
            is_escapee = true;
            break;
          }
        }
      }
      else {
        str += _ch;
      }
    }
  }
  return "error";
}
  
void JSON_Parser::_white() {
  while (_ch && _ch<=' ') {
    _next();
  }
}

JSON JSON_Parser::_word() {
  switch (_ch) {
    case 't':
      _next('t');
      _next('r');
      _next('u');
      _next('e');
      return true;
      break;
    case 'f':
      _next('f');
      _next('a');
      _next('l');
      _next('s');
      _next('e');
      return false;
      break;
    case 'n':
      _next('n');
      _next('u');
      _next('l');
      _next('l');
      return JSON();
      break;
    default:
      break;
  }
  // unexpected char
  return JSON();
}

JSON JSON_Parser::_array() {
  JSON::json_array *array_ptr = new JSON::json_array;
  
  if (_ch == '[') {
    _next('[');
    _white();
    if (_ch == ']') {
      _next(']');
      return array_ptr;
    }
    while (_ch) {
      array_ptr->push_back(new JSON(_value()));
      _white();
      if (_ch == ']') {
        _next(']');
        return array_ptr;
      }
      _next(',');
      _white();
    }
  }
  // bad array
  return JSON();
}

JSON JSON_Parser::_object() {
  JSON::json_object *object_ptr = new JSON::json_object;
  
  if (_ch == '{') {
    _next('{');
    _white();
    if (_ch == '}') {
      _next('}');
      return object_ptr;
    }
    while (_ch) {
      string key = _string().string();
      _white();
      _next(':');
      object_ptr->insert(make_pair(key, new JSON(_value())));
      _white();
      if (_ch == '}') {
        _next('}');
        return object_ptr;
      }
      _next(',');
      _white();
    }
  }
  // bad object
  return JSON();
}
  
JSON JSON_Parser::_value() {
  _white();
  switch (_ch) {
    case '{':
      return _object();
      break;
    case '[':
      return _array();
      break;
    case '"':
      return _string();
      break;
    case '-':
      return _number();
      break;
    default:
      return (_ch >= '0' && _ch <= '9') ? _number() : _word();
      break;
  }
}

} // tool

} // otita