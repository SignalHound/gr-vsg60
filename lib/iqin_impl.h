/* -*- c++ -*- */
/*
 * Copyright (C) 2020 Signal Hound, Inc. <support@signalhound.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
      int d_handle;

      double d_frequency;
      double d_level;
      double d_srate;
      bool d_repeat;

      gr::thread::mutex d_mutex;
      bool d_param_changed;

      std::complex<float> *d_buffer;
      bool d_len;

    public:
      iqin_impl(double frequency,
              double level,
              double srate,
              bool repeat);
      ~iqin_impl();

      void set_frequency(double frequency);
      void set_level(double level);
      void set_srate(double srate);
      void set_repeat(bool repeat);

      void configure();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace vsg60
} // namespace gr

#endif /* INCLUDED_VSG60_IQIN_IMPL_H */

