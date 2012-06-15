
/*  -*- C++ -*- */

%{
#include <ase/asecpnt.hpp>
#include <stdexcept>
#include <pthread.h>
%}

package "util"

class "component_thread"
public static method "yield"
%{
  sched_yield();
%}
public static method "sleep"
%{
  if (nargs > 0) {
    ase_int msec = args[0].get_int();
    usleep(msec * 1000);
  }
%}

(defclass ...)
(defmethod ...)
(var x...)

(do X...)
(if X Y Z)
(loop X Y)
(continue n)
(break n)
(let x Y)
(set x Y Z)
(get x Y)
(minvoke x Y Z...)
(sinvoke x Y...)

