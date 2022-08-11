/* -*- c++ -*- */
/*
 * Copyright 2022 Signal Hound.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_VSG60_IQIN_IMPL_H
#define INCLUDED_VSG60_IQIN_IMPL_H

#include <vsg60/iqin.h>
#include <vsg60/vsg_api.h>

namespace gr {
namespace vsg60 {

class iqin_impl : public iqin
{
private:
      int _handle;

      double _frequency;
      double _level;
      double _srate;
      bool _repeat;

      gr::thread::mutex _mutex;
      bool _param_changed;

      std::complex<float> *_buffer;
      int _len;

public:
    iqin_impl(double frequency, double level, double srate, bool repeat);
    ~iqin_impl();

      void set_frequency(double frequency);
      void set_level(double level);
      void set_srate(double srate);
      void set_repeat(bool repeat);

      void configure();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace vsg60
} // namespace gr

#endif /* INCLUDED_VSG60_IQIN_IMPL_H */
