/* -*- c++ -*- */
/*
 * Copyright 2022 Signal Hound.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_VSG60_IQIN_H
#define INCLUDED_VSG60_IQIN_H

#include <gnuradio/sync_block.h>
#include <vsg60/api.h>

namespace gr {
namespace vsg60 {

/*!
 * \brief This block accepts I/Q data for the Signal Hound VSG60 vector signal generator to output.
 * \ingroup vsg60
 *
 */
class VSG60_API iqin : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<iqin> sptr;

    static sptr make(double frequency = 1e9,
                     double level = -10,
                     double srate = 50e6,
                     bool repeat = false);


    virtual void set_frequency(double frequency) = 0;
    virtual void set_level(double level) = 0;
    virtual void set_srate(double srate) = 0;
    virtual void set_repeat(bool repeat) = 0;                     
};

} // namespace vsg60
} // namespace gr

#endif /* INCLUDED_VSG60_IQIN_H */
