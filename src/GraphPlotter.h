//
//  GraphPlotter.h
//
//  Created by otita on 2015/11/20.
//

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
