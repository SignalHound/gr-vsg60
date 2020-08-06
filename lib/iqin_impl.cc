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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "iqin_impl.h"

namespace gr {
    namespace vsg60 {

        iqin::sptr
        iqin::make(double frequency,
                 double level,
                 double srate,
                 bool repeat)
        {
            return gnuradio::get_initial_sptr(
                new iqin_impl(frequency, level, srate, repeat)
            );
        }

        void ERROR_CHECK(VsgStatus status) {
            if(status != vsgNoError) {
                bool isWarning = status < vsgNoError ? false : true;
                std::cout << "** " << (isWarning ? "Warning: " : "Error: ") << vsgGetErrorString(status) << " **" << "\n";
                if(!isWarning) abort();
            }
        }

        iqin_impl::iqin_impl(double frequency,
                         double level,
                         double srate,
                         bool repeat) :
            gr::sync_block("iqin",
                           gr::io_signature::make(0, 0, 0),
                           gr::io_signature::make(1, 1, sizeof(gr_complex))),
            d_handle(-1),
            d_frequency(frequency),
            d_level(level),
            d_srate(srate),
            d_repeat(repeat),
            d_param_changed(true),
            d_buffer(0),
            d_len(0)
        {
            std::cout << "\nAPI Version: " << vsgGetAPIVersion() << "\n";

            // Open device
            ERROR_CHECK(vsgOpenDevice(&d_handle));

            int serial;
            ERROR_CHECK(vsgGetSerialNumber(d_handle, &serial));
            std::cout << "Serial Number: "<< serial << "\n";
        }

        iqin_impl::~iqin_impl()
        {
            vsgAbort(d_handle);
            vsgCloseDevice(d_handle);

            if(d_buffer) delete [] d_buffer;
        }

        void
        iqin_impl::set_frequency(double frequency) {
            gr::thread::scoped_lock lock(d_mutex);
            d_frequency = frequency;
            d_param_changed = true;
        }

        void
        iqin_impl::set_level(double level) {
            gr::thread::scoped_lock lock(d_mutex);
            d_level = level;
            d_param_changed = true;
        }

        void
        iqin_impl::set_srate(double srate) {
            gr::thread::scoped_lock lock(d_mutex);
            d_srate = srate;
            d_param_changed = true;
        }

        void
        iqin_impl::set_repeat(bool repeat) {
            gr::thread::scoped_lock lock(d_mutex);
            d_repeat = repeat;
            d_param_changed = true;
        }

        void
        iqin_impl::configure() {
            gr::thread::scoped_lock lock(d_mutex);

            // Configure
            ERROR_CHECK(vsgSetFrequency(d_handle, d_frequency));
            ERROR_CHECK(vsgSetLevel(d_handle, d_level));
            ERROR_CHECK(vsgSetSampleRate(d_handle, d_srate));
        }

        int
        iqin_impl::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
        {
            std::complex<float> *in = (std::complex<float> *)input_items[0];

            // Initiate new configuration if necessary
            if(d_param_changed) {
                configure();
                d_param_changed = false;
            }

            // Allocate memory if necessary
            if(!d_buffer || noutput_items != d_len) {
                if(d_buffer) delete [] d_buffer;
                d_buffer = new std::complex<float>[noutput_items];
                d_len = noutput_items;
            }

            // Move data to input buffer
            for(int i = 0; i < noutput_items; i++) {
                d_buffer[i] = in[i];
            }

            // Generate signal from IQ waveform
            if(d_repeat) {
                ERROR_CHECK(vsgRepeatWaveform(d_handle, (float *)d_buffer, noutput_items));
            } else {
                ERROR_CHECK(vsgOutputWaveform(d_handle, (float *)d_buffer, noutput_items));
            }

            return noutput_items;
        }

    } /* namespace vsg60 */
} /* namespace gr */

