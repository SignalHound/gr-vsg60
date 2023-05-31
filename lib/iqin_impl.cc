/* -*- c++ -*- */
/*
 * Copyright 2022 Signal Hound.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "iqin_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace vsg60 {

using input_type = gr_complex;
iqin::sptr iqin::make(double frequency, double level, double srate, bool repeat)
{
    return gnuradio::make_block_sptr<iqin_impl>(frequency, level, srate, repeat);
}

void ERROR_CHECK(VsgStatus status)
{
    if(status != vsgNoError) {
        bool isWarning = status > vsgNoError;
        std::cout << "** " << (isWarning ? "Warning: " : "Error: ") << vsgGetErrorString(status) << " **" << "\n";
        if(!isWarning) abort();
    }
}

iqin_impl::iqin_impl(double frequency, double level, double srate, bool repeat)
    : gr::sync_block("iqin",
                     gr::io_signature::make(1, 1, sizeof(input_type)),
                     gr::io_signature::make(0, 0, 0)),
    _handle(-1),
    _frequency(frequency),
    _level(level),
    _srate(srate),
    _repeat(repeat),
    _param_changed(true),
    _buffer(0),
    _len(0)
{
    std::cout << "\nAPI Version: " << vsgGetAPIVersion() << "\n";

    // Open device
    ERROR_CHECK(vsgOpenDevice(&_handle));

    int serial;
    ERROR_CHECK(vsgGetSerialNumber(_handle, &serial));
    std::cout << "Serial Number: "<< serial << "\n";
}

iqin_impl::~iqin_impl() 
{
    vsgAbort(_handle);
    vsgCloseDevice(_handle);

    if(_buffer) delete [] _buffer;
}

void
iqin_impl::set_frequency(double frequency) {
    gr::thread::scoped_lock lock(_mutex);
    _frequency = frequency;
    _param_changed = true;
}

void
iqin_impl::set_level(double level) {
    gr::thread::scoped_lock lock(_mutex);
    _level = level;
    _param_changed = true;
}

void
iqin_impl::set_srate(double srate) {
    gr::thread::scoped_lock lock(_mutex);
    _srate = srate;
    _param_changed = true;
}

void
iqin_impl::set_repeat(bool repeat) {
    gr::thread::scoped_lock lock(_mutex);
    _repeat = repeat;
    _param_changed = true;
}

void
iqin_impl::configure() {
    gr::thread::scoped_lock lock(_mutex);

    // Configure
    ERROR_CHECK(vsgSetFrequency(_handle, _frequency));
    ERROR_CHECK(vsgSetLevel(_handle, _level));
    ERROR_CHECK(vsgSetSampleRate(_handle, _srate));
}

int iqin_impl::work(int noutput_items,
                    gr_vector_const_void_star& input_items,
                    gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);

    // Initiate new configuration if necessary
    if(_param_changed) {
        configure();
        _param_changed = false;
    }

    // Allocate memory if necessary
    if(!_buffer || noutput_items != _len) {
        if(_buffer) delete [] _buffer;
        _buffer = new std::complex<float>[noutput_items];
        _len = noutput_items;
    }

    // Move data to input buffer
    for(int i = 0; i < noutput_items; i++) {
        _buffer[i] = in[i];
    }

    // Generate signal from I/Q waveform
    if(_repeat) {
        ERROR_CHECK(vsgRepeatWaveform(_handle, (float *)_buffer, noutput_items));
    } else {
        ERROR_CHECK(vsgSubmitIQ(_handle, (float *)_buffer, noutput_items));
    }

    return noutput_items;
}

} /* namespace vsg60 */
} /* namespace gr */
