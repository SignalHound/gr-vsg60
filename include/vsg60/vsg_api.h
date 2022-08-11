// Copyright (c) 2022, Signal Hound, Inc.
// For licensing information, please see the API license in the software_licenses folder

/*!
 * \file vsg_api.h
 * \brief API functions for the VSG60A vector signal generator.
 *
 * This is the main file for user accessible functions for controlling the
 * VSG60A vector signal generator.
 *
 */

#ifndef VSG_API_H
#define VSG_API_H

#if _WIN32
    #ifdef VSG_EXPORT
        #define VSG_API __declspec(dllexport)
    #else
        #define VSG_API
    #endif

    // bare minimum stdint typedef support
    #if _MSC_VER < 1700 // For VS2010 or earlier
        typedef signed char        int8_t;
        typedef short              int16_t;
        typedef int                int32_t;
        typedef long long          int64_t;
        typedef unsigned char      uint8_t;
        typedef unsigned short     uint16_t;
        typedef unsigned int       uint32_t;
        typedef unsigned long long uint64_t;
    #else
        #include <stdint.h>
    #endif
#else // Linux
    #include <stdint.h>
    #define VSG_API __attribute__((visibility("default")))
#endif

/** Maximum number of devices that can be managed by the API. */
#define VSG_MAX_DEVICES (8)
/** Minimum configurable center frequency in Hz. */
#define VSG60_MIN_FREQ (30.0e6)
/** Maximum configurable center frequency in Hz. */
#define VSG60_MAX_FREQ (6.0e9)
/** Minimum configurable sample (DAC) rate in Hz. */
#define VSG_MIN_SAMPLE_RATE (12.5e3)
/** Maximum configurable sample (DAC) rate in Hz. */
#define VSG_MAX_SAMPLE_RATE (54.0e6)
/** Minimum configurable output level in dBm. */
#define VSG_MIN_LEVEL (-120.0)
/** Maximum configurable output level in dBm. */
#define VSG_MAX_LEVEL (10.0)
/** Minimum configurable I/Q offset */
#define VSG_MIN_IQ_OFFSET (-1024)
/** Maximum configurable I/Q offset */
#define VSG_MAX_IQ_OFFSET (1024)
/** Minimum configurable trigger length in seconds. */
#define VSG_MIN_TRIGGER_LENGTH (0.1e-6)
/** Maximum configurable trigger length in seconds. */
#define VSG_MAX_TRIGGER_LENGTH (0.1)

/**
 * Used to indicate the source of the timebase reference for the device.
 */
typedef enum VsgTimebaseState {
    /** Use the internal 10MHz timebase. */
    vsgTimebaseStateInternal = 0, // default
    /** Use an external 10MHz timebase on the `10 MHz In` port. */
    vsgTimebaseStateExternal = 1
} VsgTimebaseState;

/**
 * Boolean type. Used in public facing functions instead of `bool` to improve
 * API use from different programming languages.
 */
typedef enum VsgBool {
    /** False */
    vsgFalse = 0,
    /** True */
    vsgTrue = 1
} VsgBool;

/**
 * Status code returned from all API functions.
 */
typedef enum VsgStatus {
    // Internal use only
    vsgFileIOErr = -1000,
    vsgMemErr = -999,

    vsgInvalidOperationErr = -11,

    /** The auto pattern output is already active. */
    vsgWaveformAlreadyActiveErr = -10,
    /** The waveform is already inactive. */
    vsgWaveformNotActiveErr = -9,

    /** There was a problem during USB data transfer. */
    vsgUsbXferErr = -5,
    /** Required parameter found to have invalid value. */
    vsgInvalidParameterErr = -4,
    /** One or more required pointer parameters were null. */
    vsgNullPtrErr = -3,
    /** User specified invalid device index. */
    vsgInvalidDeviceErr = -2,
    /** Unable to open device. */
    vsgDeviceNotFoundErr = -1,

    /** Function returned successfully. No error. */
    vsgNoError = 0,

    /** There are no pending operations. */
    vsgAlreadyFlushed = 1,
    /** One or more of the provided settings were adjusted. */
    vsgSettingClamped = 2
} VsgStatus;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns a string indicating the version of the API. The returned string is
 * of the form major.minor.revision. Ascii periods (‘.’) separate positive
 * integers. Major/minor/revision are not guaranteed to be a single decimal
 * digit. The string is null terminated. The string should not be modified or
 * freed by the user. An example string is below… [‘3’ | ‘.’ | ‘0’ | ‘.’ | ‘1’
 * | ‘1’ | ‘\0’] = “3.0.11”
 *
 * @return
 */
VSG_API const char* vsgGetAPIVersion();

/**
 * This function is used to retrieve the serial number of all unopened VSG60
 * devices connected to the PC. If any device is open in a different process,
 * it will be returned by this function. The serial numbers returned can then
 * be used to open specific devices with the #vsgOpenDeviceBySerial function.
 *
 * @param[out] serials Pointer to array.
 *
 * @param[inout] count Pointer to integer that should equal the size of the
 * serials array. If the function returns successfully, this value will be set
 * to the number devices returned in the serials array. It will not exceed the
 * size of the serials array.
 *
 * @return
 */
VSG_API VsgStatus vsgGetDeviceList(int *serials, int *count);

/**
 * Claim the first unopened VSG60 detected on the system. If the device is
 * opened successfully, a handle to the device will be returned. This handle
 * can then be used to interface this device for all future API calls. This
 * function has the same effect as calling #vsgGetDeviceList and using the first
 * device found to call #vsgOpenDeviceBySerial.
 *
 * @param[out] handle Returns device handle.
 *
 * @return
 */
VSG_API VsgStatus vsgOpenDevice(int *handle);

/**
 * This function operates similarly to #vsgOpenDevice except it allows you to
 * specify the device you wish to open. This function is often used in
 * conjunction with #vsgGetDeviceList when managing several VSG60 devices on
 * one PC.
 *
 * @param[out] handle If this function returns successfully, handle will point to an
 * integer that can be used to access this device through the API.
 *
 * @param[in] serialNumber The serial number of the device you wish to open.
 *
 * @return
 */
VSG_API VsgStatus vsgOpenDeviceBySerial(int *handle, int serialNumber);

/**
 * This function should be called when you are finished with the VSG60. It will
 * release all resources for the device and the device will become available
 * again for use in the current process. The device handle specified will no
 * longer point to a valid device and the device must be re-opened again to be
 * used. This function should be called before the process exits.
 *
 * @param[in] handle Device handle.
 *
 * @return
 */
VSG_API VsgStatus vsgCloseDevice(int handle);

/**
 * Performs a full device preset. When this function returns, the hardware will
 * have performed a full reset, the device handle will no longer be valid,
 * #vsgCloseDevice will have been called for the device handle, and the device
 * will need to be re-opened. This function can be used to recover from an
 * undesirable device state. This function takes roughly 3 seconds to complete.
 *
 * @param[in] handle Device handle.
 * @return
 */
VSG_API VsgStatus vsgPreset(int handle);

/**
 * When operating the VSG60 for long periods of time with a fixed
 * configuration, environmental changes leading to changes in the internal
 * operating temperature of the VSG can cause signal drift leading to loss of
 * amplitude accuracy and image rejection performance. This function aborts any
 * current operation, and updates internal temperature corrections for the
 * current configuration.
 *
 * @param[in] handle Device handle.
 *
 * @return
 */
VSG_API VsgStatus vsgRecal(int handle);

/**
 * This function returns the device to an idle state. If any waveform is being
 * generated, it will be stopped. If the device is streaming in the complex
 * generation mode, all I/Q data pending is discarded and all frequency/level
 * changes are finished before returning. When this function returns, the
 * device will be in an idle state.
 *
 * @param[in] handle Device handle.
 *
 * @return
 */
VSG_API VsgStatus vsgAbort(int handle);

/**
 * Retreive the device serial number.
 *
 * @param[in] handle Device handle.
 *
 * @param[out] serial Serial number.
 *
 * @return
 */
VSG_API VsgStatus vsgGetSerialNumber(int handle, int *serial);

/**
 * Retrieve the firmware version number. The version is a single integer value.
 *
 * @param[in] handle Device handle.
 *
 * @param[out] version Firmware version.
 *
 * @return
 */
VSG_API VsgStatus vsgGetFirmwareVersion(int handle, int *version);

/**
 * Retrieve the last calibration date as the seconds since epoch.
 *
 * @param[in] handle Device handle.
 *
 * @param[out] lastCalDate Calibration date as seconds since epoch.
 *
 * @return
 */
VSG_API VsgStatus vsgGetCalDate(int handle, uint32_t *lastCalDate);

/**
 * Retrive the device temperature in Celcius. If the device is not idle, the
 * last read temperature is returned, otherwise the device temperature is
 * queried before returning.
 *
 * @param[in] handle Device handle.
 *
 * @param[out] temp Device temperature in C.
 *
 * @return
 */
VSG_API VsgStatus vsgReadTemperature(int handle, float *temp);

/**
 * Use this function to disable the RF output of the VSG60. Even when the VSG
 * is not transmitting, it might still be emitting spurious energy related to
 * clock frequencies and the DC offset. Disabling the RF output will eliminate
 * most spurious signals. The RF output is enabled by default. When the RF
 * output is disabled the only way to enable it again is to call this function.
 * Until then, all actions will continue to be performed but with the RF output
 * disabled. Additionally, the #vsgAbort function is called when this function
 * is called. This means any signal actively being generated will be stopped
 * when this function is called.
 *
 * @param[in] handle Device handle.
 *
 * @param[in] enabled Set to vsgFalse to disable the RF output.
 *
 * @return
 */
VSG_API VsgStatus vsgSetRFOutputState(int handle, VsgBool enabled);

/**
 * Retrive the RF output state.
 *
 * @param[in] handle Device handle.
 *
 * @param[out] enabled Returns vsgTrue if the RF output is enabled.
 *
 * @return
 */
VSG_API VsgStatus vsgGetRFOutputState(int handle, VsgBool *enabled);

/**
 * Specify whether the VSG60 should use its internal 10MHz reference or one
 * provided on the 10MHz reference input port. If external reference is
 * selected and no reference is provided, the frequency error may be off by
 * several PPM. If the state provided matches the current state, the function
 * returns immediately. Any active waveforms are paused, and the stream is
 * flushed before this operation takes place.
 *
 * @param handle Device handle.
 *
 * @param state New timebase state.
 *
 * @return
 */
VSG_API VsgStatus vsgSetTimebase(int handle, VsgTimebaseState state);

/**
 * Retrieve the current timebase state.
 *
 * @param handle Device handle.
 *
 * @param state Returns current timebase state.
 *
 * @return
 */
VSG_API VsgStatus vsgGetTimebase(int handle, VsgTimebaseState *state);

/**
 * Adjust the VSG60 timebase. The adjustment is provided in parts per million
 * (PPM) This adjustment will only last until the device is closed via
 * #vsgCloseDevice or the program is terminated. If the value provided matches
 * the currently set value, this function returns immediately. Any active
 * waveforms are paused, and the stream is flushed before this operation takes
 * place.
 *
 * @param handle Device handle.
 *
 * @param ppm New timebase offset. The value will be clamped between [-2,+2]
 * ppm.
 *
 * @return
 */
VSG_API VsgStatus vsgSetTimebaseOffset(int handle, double ppm);

/**
 * Retrieve the current user configured timebase offset in parts per million
 * (PPM).
 *
 * @param handle Device handle.
 *
 * @param ppm Returns current user PPM offset.
 *
 * @return
 */
VSG_API VsgStatus vsgGetTimebaseOffset(int handle, double *ppm);

/**
 * Set the center frequency of the signal generator. This function is used for
 * both basic and streaming operation. In streaming operation, this operation
 * takes 200us to complete. This operation will occur even if the provided
 * frequency matches the current frequency. This operation may configure a
 * recalibration (at no time penalty).
 *
 * @param handle Device handle.
 *
 * @param frequency New center frequency in Hz.
 *
 * @return
 */
VSG_API VsgStatus vsgSetFrequency(int handle, double frequency);

/**
 * Retrieve the current center frequency. This function will return the last
 * user configured center frequency even if the hardware has yet to update to
 * the new value.
 *
 * @param handle Device handle.
 *
 * @param frequency Returns the center frequency in Hz.
 *
 * @return
 */
VSG_API VsgStatus vsgGetFrequency(int handle, double *frequency);

/**
 * Sets the I/Q (DAC) sample rate of the signal generator. The streaming queue
 * is flushed via #vsgFlushAndWait before the sample rate is updated. If the
 * supplied sample rate is the same as the current rate, this function returns
 * immediately and no operation occurs. A full sample rate change takes
 * approximately 200-250ms.
 *
 * @param handle Device handle.
 *
 * @param sampleRate New sample rate in Hz.
 *
 * @return
 */
VSG_API VsgStatus vsgSetSampleRate(int handle, double sampleRate);

/**
 * Retrieve the current I/Q (DAC) sample rate.
 *
 * @param handle Device handle.
 *
 * @param sampleRate Returns the sample rate in Hz.
 *
 * @return
 */
VSG_API VsgStatus vsgGetSampleRate(int handle, double *sampleRate);

/**
 * Set the output level of the signal generator. The output level represents
 * the output power at the RF output port when I/Q samples of magnitude 1.0 are
 * transmitted. Hardware attenuation, amplification, and digital scaling are
 * used to achieve the requested output level. What values are used depend on
 * the temperature calibration coefficients for individual device and the
 * frequency of the output. In streaming operation, this operation takes 10us
 * to complete. This operation will occur even if the provided level matches
 * the current level. This operation may configure a recalibration (at no time
 * penalty).
 *
 * @param handle Device handle.
 *
 * @param level New output level in dBm.
 *
 * @return
 */
VSG_API VsgStatus vsgSetLevel(int handle, double level);

/**
 * Retrieve the current output level.
 *
 * @param handle Device handle.
 *
 * @param level Output level in dBm.
 *
 * @return
 */
VSG_API VsgStatus vsgGetLevel(int handle, double *level);

/**
 * This function allows the customer to guarantee the configuration of the
 * internal attenuator and amplifier directly by specifying a fixed system
 * attenuation. Values that are positive utilize the amplifier to achieve the
 * desired setting. Calling this function overrides the output level configured
 * via #vsgSetLevel. A digital I/Q scale of 0.5 is used when attenuation is
 * manually set. See #vsgGetIQScale.
 *
 * @param handle Device handle.
 *
 * @param atten Attenuator value between [-50, 20] in 2dB steps. Must be an
 * even number.
 *
 * @return
 */
VSG_API VsgStatus vsgSetAtten(int handle, int atten);

/**
 * Returns the currently used digital scale applied to the I/Q data before
 * transmitting. The digital scaling is used in conjunction with the hardware
 * amplifier and attenuator to achieve the desired output level. This function
 * does not interrupt any active waveforms or streaming generation.
 *
 * @param handle Device handle.
 *
 * @param iqScale Returns floating point value between [0.0, 1.0].
 *
 * @return
 */
VSG_API VsgStatus vsgGetIQScale(int handle, double *iqScale);

/**
 * Specify an additional I/Q offset applied to the I/Q data before transmit.
 * Used to fine improve carrier feedthrough. The offset lasts until the device
 * is closed or the program is terminated. #vsgFlushAndWait is called at the
 * beginning of this function. If the supplied value matches the current value,
 * this function returns immediately.
 *
 * @param handle Device handle.
 *
 * @param iOffset I channel offset between [-1024,1024].
 *
 * @param qOffset Q channel offset between [-1024,1024].
 *
 * @return
 */
VSG_API VsgStatus vsgSetIQOffset(int handle, int16_t iOffset, int16_t qOffset);

/**
 * Retrieve the user configured I/Q offsets.
 *
 * @param handle Device handle.
 *
 * @param iOffset Returns I channel offset.
 *
 * @param qOffset Returns Q channel offset.
 *
 * @return
 */
VSG_API VsgStatus vsgGetIQOffset(int handle, int16_t *iOffset, int16_t *qOffset);

/**
 * If the value is provided is, this function returns immediately with no
 * effect. #vsgFlushAndWait is called before the operation occurs. See the
 * [VSG60 Product
 * Manual](https://signalhound.com/sigdownloads/VSG60/VSG60A-Product-Manual.pdf#page=6)
 * for a description of digital tuning.
 *
 * @param handle Device handle.
 *
 * @param enabled Set to vsgTrue to enable digital tuning.
 *
 * @return
 */
VSG_API VsgStatus vsgSetDigitalTuning(int handle, VsgBool enabled);

/**
 * Retrieve whether digital tuning is enabled.
 *
 * @param handle Device handle.
 *
 * @param enabled Returns vsgTrue if digital tuning is enabled.
 *
 * @return
 */
VSG_API VsgStatus vsgGetDigitalTuning(int handle, VsgBool *enabled);

/**
 * Set the length of time the output trigger port remains high when a trigger
 * is output. Default is 10us. (10.0e-6) The range of acceptable values is
 * [100ns, 1s] This function does not interrupt any active waveforms or
 * streaming operation.
 *
 * @param handle Device handle.
 *
 * @param seconds Trigger length in seconds.
 *
 * @return
 */
VSG_API VsgStatus vsgSetTriggerLength(int handle, double seconds);

/**
 * Retrieve the output trigger length.
 *
 * @param handle Device handle.
 *
 * @param seconds Returns the trigger length in seconds.
 *
 * @return
 */
VSG_API VsgStatus vsgGetTriggerLength(int handle, double *seconds);

/**
 * Submit an array of I/Q samples to be generated with the current
 * configuration. If an ARB waveform is currently being transmitted via the
 * #vsgRepeatWaveform function, it is aborted, and the device starts operating
 * in the streaming configuration. This function should only be used for
 * streaming operation. For generating simple waveforms, use the
 * #vsgOutputWaveform and #vsgRepeatWaveform functions. This function will
 * block until there is room in the processing and command queue. See @ref
 * complexSigGen for more information.
 *
 * @param handle Device handle.
 *
 * @param iq Pointer to array of interleaved I/Q values. The array must be len*2
 * floating point values.
 *
 * @param len Number of I/Q samples in the iq array
 *
 * @return
 */
VSG_API VsgStatus vsgSubmitIQ(int handle, float *iq, int len);

/**
 * Submit a streaming trigger event. If an ARB waveform is currently being
 * transmitted, it is aborted, and the device starts operating in the streaming
 * configuration. If a trigger is already active when the new trigger is
 * output, it is first toggled low before re-toggling high, resetting the
 * trigger high period in the process. See the @ref basigSigGen section for
 * more information.
 *
 * @param handle Device handle.
 *
 * @return
 */
VSG_API VsgStatus vsgSubmitTrigger(int handle);

/**
 * Pushes all pending operations in a stream out to the device. This function
 * should be called after a sequence of streaming events to ensure there are no
 * gaps in the output. Also see #vsgFlushAndWait.
 *
 * @param handle Device handle.
 *
 * @return
 */
VSG_API VsgStatus vsgFlush(int handle);

/**
 * Pushes all pending operations in a stream out to the device and waits for
 * all operations to complete. This function should be called after a sequence
 * of streaming events to ensure there are no gaps in the output. When this
 * function returns, the device will be idle.
 *
 * @param handle Device handle.
 *
 * @return
 */
VSG_API VsgStatus vsgFlushAndWait(int handle);

/**
 * Output the I/Q waveform once and returns. This function has the same effect
 * as calling #vsgSubmitIQ followed by #vsgFlushAndWait. This function returns
 * once the waveform has been transmitted. The device will be in an idle state
 * when returned. If an ARB waveform is active, it is aborted prior to
 * transmission.
 *
 * @param handle Device handle.
 *
 * @param iq Pointer to arry of interleaved I/Q values. The array must be len*2
 * floating point values.
 *
 * @param len The number of I/Q samples in the iq array.
 *
 * @return
 */
VSG_API VsgStatus vsgOutputWaveform(int handle, float *iq, int len);

/**
 * This function instructs the API to continually generate the provided
 * waveform. A full copy of the waveform is made before generation occurs. This
 * function blocks until signal generation has begun. The repeated waveform is
 * only stopped after calling the #vsgAbort function or by calling a function
 * that interrupts operation. Setting any other configuration value, such as
 * frequency, level, etc. will pause the waveform until the configuration
 * completes and then generation starts again from the beginning. See
 * #vsgIsWaveformActive.
 *
 * @param handle Device handle.
 *
 * @param iq Pointer to arry of interleaved I/Q values. The array must be len*2
 * floating point values.
 *
 * @param len The number of I/Q samples in the iq array.
 *
 * @return
 */
VSG_API VsgStatus vsgRepeatWaveform(int handle, float *iq, int len);

/**
 * Convenience function which outputs a CW signal with the current frequency,
 * level, and sample rate. This function has the same effect as calling
 * #vsgRepeatWaveform with a single I/Q value of {1,0}.
 *
 * @param handle Device handle.
 *
 * @return
 */
VSG_API VsgStatus vsgOutputCW(int handle);

/**
 * Detect whether a waveform is currently being generated via the
 * #vsgRepeatWaveform function.
 *
 * @param handle Device handle.
 *
 * @param active Returns vsgTrue if a waveform is currently being transmitted.
 *
 * @return
 */
VSG_API VsgStatus vsgIsWaveformActive(int handle, VsgBool *active);

/**
 * Return the USB status warning.
 *
 * @param handle Device handle.
 *
 * @return
 */
VSG_API VsgStatus vsgGetUSBStatus(int handle);

/**
 * Enable power saving CPU mode See @ref powerSaving for more information.
 *
 * @param enabled Set to vsgTrue to enable power saving CPU mode.
 */
VSG_API void vsgEnablePowerSavingCpuMode(VsgBool enabled);

/**
 * Retrieve an ascii description string for a given VsgStatus. The string is
 * useful for printing an debugging purposes.
 *
 * @param status Status returned from an API function.
 *
 * @return A pointer to a null terminated string. The memory should not be
 * freed/deallocated/modified.
 */
VSG_API const char* vsgGetErrorString(VsgStatus status);

#ifdef __cplusplus
} // Extern C
#endif

#endif
