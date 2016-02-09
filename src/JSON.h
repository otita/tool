//
//  JSON.h
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

#ifndef _JSON_H_
#define _JSON_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace otita {

namespace tool {

class JSON {
public:
  using json_string = ::std::string;
  using json_array = ::std::vector<JSON *>;
  using json_object = ::std::unordered_map<::std::string, JSON *>;
  enum json_t {
    JSON_NULL,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT,
  };
  static JSON *parse(const ::std::string &source);
  JSON();
  JSON(double);
  JSON(bool);
  JSON(const json_string &);
  JSON(const char []);
  JSON(json_array *);
  JSON(json_object *);
  JSON(const JSON &);
  json_t type() const;
  double number() const;
  bool   boolean() const;
  const json_string &string() const;
  JSON &operator =(const JSON &other);
  JSON &operator [](size_t i);
  const JSON &operator [](size_t i) const;
  JSON &operator [](const json_string &key);
  const JSON &operator [](const json_string &key) const;
  virtual ~JSON();
private:
  union json_field {
    double number;
    bool boolean;
    json_string *string_ptr;
    json_array *array_ptr;
    json_object *object_ptr;
  };
  json_t _type;
  json_field _field;
};

} // tool

} // otita

#endif  // _JSON_H_
