//
//  timer.cpp
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

#include <iostream>
#include <unordered_map>

#include <ctime>

#include "timer.h"

using namespace std;

namespace otita {

namespace tool {
  
const char TIMER_KEY_ALL[] = "hoge";

class Timer {
  using time_table = unordered_map<string, pair<clock_t, clock_t> >;
public:
  static Timer &getInstance() {
    static Timer timer;
    return timer;
  }
  void tic(const char name[]) {
    time_table::iterator it = _time_table.find(name);
    if (it==_time_table.end()) {
      clock_t t = clock();
      _time_table.insert(make_pair(name, make_pair(t, 0)));
    }
    else {
      it->second.first = clock();
    }
  }
  void toc(const char name[]) {
    time_table::iterator it = _time_table.find(name);
    if (it==_time_table.end()) {
      cerr << "unrecognized name: " << name << endl;
      return;
    }
    clock_t t = clock();
    it->second.second += t - it->second.first;
    it->second.first = t;
  }
  void show() {
    for (time_table::iterator it=_time_table.begin(); it!=_time_table.end(); it++) {
      string key = (it->first==TIMER_KEY_ALL) ? "all" : it->first;
      cout << key << ": "
           << double(it->second.second)/CLOCKS_PER_SEC << "(s)"
           << endl;
    }
  }
private:
  time_table _time_table;
  Timer() {}
  Timer(const Timer &other) {}
  Timer &operator=(const Timer &other) {
    return Timer::getInstance();
  }
};
    
void tic(const char name[]) {
  Timer::getInstance().tic(name);
}

void toc(const char name[]) {
  Timer::getInstance().toc(name);
}

void show_timer() {
  Timer::getInstance().show();
}

} // tool

} // otita
