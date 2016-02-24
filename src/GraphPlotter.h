//
//  GraphPlotter.h
//
//  Created by otita on 2015/11/20.
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

#ifndef _GRAPH_PLOTTER_H_
#define _GRAPH_PLOTTER_H_

#include <string>

namespace otita {

namespace tool {

class GraphPlotter {
public:
  GraphPlotter();
  GraphPlotter(const ::std::string &filepath);
  GraphPlotter &plot(const ::std::string &equation);
  GraphPlotter &plot(const double *x_begin,
                     size_t len,
                     const ::std::string &options);
  GraphPlotter &plot(const double *x_begin,
                     const double *y_begin,
                     size_t len,
                     const ::std::string &options);
  virtual ~GraphPlotter();
private:
  class Impl;
  Impl *_impl;
};
  
} // tool

} // otita

#endif // _GRAPH_PLOTTER_H_ 
