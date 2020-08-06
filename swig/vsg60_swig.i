/* -*- c++ -*- */

#define VSG60_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "vsg60_swig_doc.i"

%{
#include "vsg60/iqin.h"
%}


%include "vsg60/iqin.h"
GR_SWIG_BLOCK_MAGIC2(vsg60, iqin);
