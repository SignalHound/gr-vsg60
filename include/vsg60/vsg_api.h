// Copyright (c) 2020, Signal Hound, Inc.
// For licensing information, please see the API license in the software_licenses folder

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

#define VSG_MAX_DEVICES (8)

#define VSG60_MIN_FREQ (30.0e6)
#define VSG60_MAX_FREQ (6.0e9)

#define VSG_MIN_SAMPLE_RATE (12.5e3)
#define VSG_MAX_SAMPLE_RATE (54.0e6)

#define VSG_MIN_LEVEL (-120.0)
#define VSG_MAX_LEVEL (10.0)

#define VSG_MIN_IQ_OFFSET (-1024)
#define VSG_MAX_IQ_OFFSET (1024)

#define VSG_MIN_TRIGGER_LENGTH (0.1e-6) // 100ns
#define VSG_MAX_TRIGGER_LENGTH (1.0) // 100ms

typedef enum VsgTimebaseState {
    vsgTimebaseStateInternal = 0, // default
    vsgTimebaseStateExternal = 1
} VsgTimebaseState;

typedef enum VsgBool {
    vsgFalse = 0,
    vsgTrue = 1
} VsgBool;

typedef enum VsgStatus {
    // Internal use only
    vsgFileIOErr = -1000,
    vsgMemErr = -999,

    vsgInvalidOperationErr = -11,

    // Indicates the auto pattern output is already active
    vsgWaveformAlreadyActiveErr = -10,
    vsgWaveformNotActiveErr = -9,

    vsgUsbXferErr = -5,
    vsgInvalidParameterErr = -4,
    vsgNullPtrErr = -3,
    vsgInvalidDeviceErr = -2,
    vsgDeviceNotFoundErr = -1,

    vsgNoError = 0,

    vsgAlreadyFlushed = 1,
    vsgSettingClamped = 2
} VsgStatus;

#ifdef __cplusplus
extern "C" {
#endif

VSG_API const char* vsgGetAPIVersion();

VSG_API VsgStatus vsgGetDeviceList(int *serials, int *count);

VSG_API VsgStatus vsgOpenDevice(int *handle);
VSG_API VsgStatus vsgOpenDeviceBySerial(int *handle, int serialNumber);
VSG_API VsgStatus vsgCloseDevice(int handle);
VSG_API VsgStatus vsgPreset(int handle);

VSG_API VsgStatus vsgRecal(int handle);

VSG_API VsgStatus vsgAbort(int handle);

VSG_API VsgStatus vsgGetSerialNumber(int handle, int *serial);
VSG_API VsgStatus vsgGetFirmwareVersion(int handle, int *version);
VSG_API VsgStatus vsgGetCalDate(int handle, uint32_t *lastCalDate);
VSG_API VsgStatus vsgReadTemperature(int handle, float *temp);

VSG_API VsgStatus vsgSetRFOutputState(int handle, VsgBool enabled);
VSG_API VsgStatus vsgGetRFOutputState(int handle, VsgBool *enabled);

VSG_API VsgStatus vsgSetTimebase(int handle, VsgTimebaseState state);
VSG_API VsgStatus vsgGetTimebase(int handle, VsgTimebaseState *state);

VSG_API VsgStatus vsgSetTimebaseOffset(int handle, double ppm);
VSG_API VsgStatus vsgGetTimebaseOffset(int handle, double *ppm);

// Frequency in Hz
VSG_API VsgStatus vsgSetFrequency(int handle, double frequency);
VSG_API VsgStatus vsgGetFrequency(int handle, double *frequency);

// Sample rate in Hz
VSG_API VsgStatus vsgSetSampleRate(int handle, double sampleRate);
VSG_API VsgStatus vsgGetSampleRate(int handle, double *sampleRate);

// Set output power in dBm
VSG_API VsgStatus vsgSetLevel(int handle, double level);
VSG_API VsgStatus vsgGetLevel(int handle, double *level);

VSG_API VsgStatus vsgSetAtten(int handle, int atten);

VSG_API VsgStatus vsgGetIQScale(int handle, double *iqScale);

VSG_API VsgStatus vsgSetIQOffset(int handle, int16_t iOffset, int16_t qOffset);
VSG_API VsgStatus vsgGetIQOffset(int handle, int16_t *iOffset, int16_t *qOffset);

VSG_API VsgStatus vsgSetDigitalTuning(int handle, VsgBool enabled);
VSG_API VsgStatus vsgGetDigitalTuning(int handle, VsgBool *enabled);

VSG_API VsgStatus vsgSetTriggerLength(int handle, double seconds);
VSG_API VsgStatus vsgGetTriggerLength(int handle, double *seconds);

// Submit 'len' count I/Q pairs
// buf should be len*2 floating point values
VSG_API VsgStatus vsgSubmitIQ(int handle, float *iq, int len);
VSG_API VsgStatus vsgSubmitTrigger(int handle);

// Wait until all commands are sent to the device
VSG_API VsgStatus vsgFlush(int handle);
// Wait for all pending actions to get sent to device
VSG_API VsgStatus vsgFlushAndWait(int handle);

// Play waveform once and wait until complete
VSG_API VsgStatus vsgOutputWaveform(int handle, float *iq, int len);
// Loop on waveform
VSG_API VsgStatus vsgRepeatWaveform(int handle, float *iq, int len);
// Convenience function with behavior similar to vsgRepeatWaveform
VSG_API VsgStatus vsgOutputCW(int handle);
// Is a waveform currently being generated with the vsgRepeatWaveform function
VSG_API VsgStatus vsgIsWaveformActive(int handle, VsgBool *active);

VSG_API VsgStatus vsgGetUSBStatus(int handle);

VSG_API void vsgEnablePowerSavingCpuMode(VsgBool enabled);

VSG_API const char* vsgGetErrorString(VsgStatus status);

#ifdef __cplusplus
} // Extern C
#endif

#endif 
