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


#ifndef INCLUDED_VSG60_IQIN_H
#define INCLUDED_VSG60_IQIN_H

#include <vsg60/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace vsg60 {

    /*!
     * This block feeds IQ data to the Signal Hound VSG60 vector signal generator.
     *
     *
     */
    class VSG60_API iqin : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<iqin> sptr;

      static sptr make(double frequency,
                       double level,
                       double srate,
                       bool repeat);

      virtual void set_frequency(double frequency) = 0;
      virtual void set_level(double level) = 0;
      virtual void set_srate(double srate) = 0;
      virtual void set_repeat(bool repeat) = 0;
    };

  } // namespace vsg60
} // namespace gr

#endif /* INCLUDED_VSG60_IQIN_H */

