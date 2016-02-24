//
//  GraphPlotter.cpp
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

#include <iostream>
#include <fstream>

#include "GraphPlotter.h"

using namespace ::std;
using namespace ::otita::tool;

namespace otita {

namespace tool {
  
class GraphPlotter::Impl {
public:
  Impl();
  Impl(const string &filepath);
  void plot(const string &equation);
  void plot(const double *x_begin,
            size_t len,
            const string &options);
  void plot(const double *x_begin,
            const double *y_begin,
            size_t len,
            const string &options);
  virtual ~Impl();
private:
  const char *_tmpName();
  FILE *_gnuplot_p;
  bool _plotted_any;
};
  
GraphPlotter::Impl::Impl() {
  string gnuplot = GNUPLOT;
  gnuplot = gnuplot + " -persist";
  _gnuplot_p = popen(gnuplot.c_str(), "w");
  fprintf(_gnuplot_p, "plot ");
  _plotted_any = false;
}
             
GraphPlotter::Impl::Impl(const string &filepath) {
  static const char cmd_format[] = "set terminal postscript;\
                                    set term postscript eps enhanced color;\
                                    set output '%s';\
                                    plot ";
  _gnuplot_p = popen(GNUPLOT, "w");
  fprintf(_gnuplot_p, cmd_format, filepath.c_str());
  _plotted_any = false;
}
  
void GraphPlotter::Impl::plot(const string &equation) {
  if (_plotted_any) {
    fputs((", " + equation).c_str(), _gnuplot_p);
  }
  else {
    fputs(equation.c_str(), _gnuplot_p);
    _plotted_any = true;
  }
}
  
void GraphPlotter::Impl::plot(const double *x_begin,
                              size_t len,
                              const string &options) {
  const char *tmp_name = _tmpName();
  if (tmp_name==nullptr) {
    cerr << "cannot get tmp name" << endl;
    return;
  }
  ofstream tmpstream(tmp_name);
  if (!tmpstream.is_open()) {
    cerr << "cannot open tmp file" << endl;
    return;
  }
  for (size_t i=0; i<len; i++) {
    tmpstream << *(x_begin+i) << endl;
  }
  
  if (_plotted_any) {
    fprintf(_gnuplot_p, ", '%s' %s",
            tmp_name,
            options.c_str());
  }
  else {
    fprintf(_gnuplot_p, "'%s' %s",
            tmp_name,
            options.c_str());
    _plotted_any = true;
  }
}

void GraphPlotter::Impl::plot(const double *x_begin,
                              const double *y_begin,
                              size_t len,
                              const string &options) {
  const char *tmp_name = _tmpName();
  if (tmp_name==nullptr) {
    cerr << "cannot get tmp name" << endl;
    return;
  }
  ofstream tmpstream(tmp_name);
  if (!tmpstream.is_open()) {
    cerr << "cannot open tmp file" << endl;
    return;
  }
  for (size_t i=0; i<len; i++) {
    tmpstream << *(x_begin+i) << ' ' << *(y_begin+i) << endl;
  }
  if (_plotted_any) {
    fprintf(_gnuplot_p, ", '%s' using 1:2 %s",
            tmp_name,
            options.c_str());
  }
  else {
    fprintf(_gnuplot_p, "'%s' using 1:2 %s",
            tmp_name,
            options.c_str());
    _plotted_any = true;
  }
}

GraphPlotter::Impl::~Impl() {
  if (_plotted_any) {
    fputs("\n", _gnuplot_p);
  }
  pclose(_gnuplot_p);
}
  
const char *GraphPlotter::Impl::_tmpName() {
  static const char tmp_filename_template[] = "/var/tmp/gnuplot_tmpdatafile_XXXXXX";
  char *tmp_filename = nullptr;
  size_t tmp_filelen = strlen(tmp_filename_template);
  tmp_filename = new char [tmp_filelen+1];
  if (tmp_filename==nullptr) {
    return nullptr;
  }
  strcpy(tmp_filename, tmp_filename_template);
  
  int unx_fd = mkstemp(tmp_filename);
  if (unx_fd==-1) {
    delete [] tmp_filename;
    return nullptr;
  }
  close(unx_fd);
  return tmp_filename;
}
  
GraphPlotter::GraphPlotter() {
  _impl = new Impl();
}

GraphPlotter::GraphPlotter(const string &filepath) {
  _impl = new Impl(filepath);
}

GraphPlotter &GraphPlotter::plot(const std::string &equation) {
  _impl->plot(equation);
  return *this;
}
  
GraphPlotter &GraphPlotter::plot(const double *x_begin,
                                 size_t len,
                                 const string &options) {
  _impl->plot(x_begin, len, options);
  return *this;
}

GraphPlotter &GraphPlotter::plot(const double *x_begin,
                                 const double *y_begin,
                                 size_t len,
                                 const string &options) {
  _impl->plot(x_begin, y_begin, len, options);
  return *this;
}
  
GraphPlotter::~GraphPlotter() {
  delete _impl;
}
  

} // tool

} // otita
