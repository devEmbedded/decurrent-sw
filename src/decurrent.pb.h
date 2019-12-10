/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.0-dev */

#ifndef PB_SRC_DECURRENT_PB_H_INCLUDED
#define PB_SRC_DECURRENT_PB_H_INCLUDED
#include <pb.h>

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _Command {
    Command_CMD_STATUS = 0,
    Command_CMD_CONFIG = 1,
    Command_CMD_AUTOCALIBRATE = 2,
    Command_CMD_START_SAMPLING = 3,
    Command_CMD_STOP_SAMPLING = 4
} Command;
#define _Command_MIN Command_CMD_STATUS
#define _Command_MAX Command_CMD_STOP_SAMPLING
#define _Command_ARRAYSIZE ((Command)(Command_CMD_STOP_SAMPLING+1))

typedef enum _TriggerType {
    TriggerType_TRIGGER_LOW = 0,
    TriggerType_TRIGGER_HIGH = 1,
    TriggerType_TRIGGER_RISE = 2,
    TriggerType_TRIGGER_FALL = 3,
    TriggerType_TRIGGER_EDGE = 4
} TriggerType;
#define _TriggerType_MIN TriggerType_TRIGGER_LOW
#define _TriggerType_MAX TriggerType_TRIGGER_EDGE
#define _TriggerType_ARRAYSIZE ((TriggerType)(TriggerType_TRIGGER_EDGE+1))

typedef enum _WaveType {
    WaveType_WAVE_DC = 0,
    WaveType_WAVE_SIN = 1,
    WaveType_WAVE_RECTANGLE = 2,
    WaveType_WAVE_TRIANGLE = 3
} WaveType;
#define _WaveType_MIN WaveType_WAVE_DC
#define _WaveType_MAX WaveType_WAVE_TRIANGLE
#define _WaveType_ARRAYSIZE ((WaveType)(WaveType_WAVE_TRIANGLE+1))

typedef enum _SweepType {
    SweepType_SWEEP_NONE = 0,
    SweepType_SWEEP_LIN = 1,
    SweepType_SWEEP_LOG = 2
} SweepType;
#define _SweepType_MIN SweepType_SWEEP_NONE
#define _SweepType_MAX SweepType_SWEEP_LOG
#define _SweepType_ARRAYSIZE ((SweepType)(SweepType_SWEEP_LOG+1))

typedef enum _Status {
    Status_STATUS_UNKNOWN = 0,
    Status_STATUS_IDLE = 1,
    Status_STATUS_WAITTRIG = 2,
    Status_STATUS_MEASURING = 3,
    Status_STATUS_STOPPING = 4,
    Status_STATUS_ERROR = 5
} Status;
#define _Status_MIN Status_STATUS_UNKNOWN
#define _Status_MAX Status_STATUS_ERROR
#define _Status_ARRAYSIZE ((Status)(Status_STATUS_ERROR+1))

typedef enum _ErrorCode {
    ErrorCode_ERR_NONE = 0,
    ErrorCode_ERR_BUFFER_FULL = 1,
    ErrorCode_ERR_VREF_OUT_OF_RANGE = 2,
    ErrorCode_ERR_GND_REF_OUT_OF_RANGE = 3
} ErrorCode;
#define _ErrorCode_MIN ErrorCode_ERR_NONE
#define _ErrorCode_MAX ErrorCode_ERR_GND_REF_OUT_OF_RANGE
#define _ErrorCode_ARRAYSIZE ((ErrorCode)(ErrorCode_ERR_GND_REF_OUT_OF_RANGE+1))

typedef enum _ChannelId {
    ChannelId_CHANNEL_DIGITAL = 0,
    ChannelId_CHANNEL_ANALOG_V1 = 8,
    ChannelId_CHANNEL_ANALOG_V2 = 9,
    ChannelId_CHANNEL_ANALOG_I1 = 10,
    ChannelId_CHANNEL_USB = 16
} ChannelId;
#define _ChannelId_MIN ChannelId_CHANNEL_DIGITAL
#define _ChannelId_MAX ChannelId_CHANNEL_USB
#define _ChannelId_ARRAYSIZE ((ChannelId)(ChannelId_CHANNEL_USB+1))

typedef enum _Compression {
    Compression_COMPRESSION_NONE = 0,
    Compression_COMPRESSION_DEFLATE = 1,
    Compression_COMPRESSION_DELTA_DEFLATE = 2
} Compression;
#define _Compression_MIN Compression_COMPRESSION_NONE
#define _Compression_MAX Compression_COMPRESSION_DELTA_DEFLATE
#define _Compression_ARRAYSIZE ((Compression)(Compression_COMPRESSION_DELTA_DEFLATE+1))

typedef enum _Unit {
    Unit_UNIT_Volt = 0,
    Unit_UNIT_Ampere = 1
} Unit;
#define _Unit_MIN Unit_UNIT_Volt
#define _Unit_MAX Unit_UNIT_Ampere
#define _Unit_ARRAYSIZE ((Unit)(Unit_UNIT_Ampere+1))

/* Struct definitions */
typedef struct _AnalogScale {
    float scaling;
    Unit unit;
/* @@protoc_insertion_point(struct:AnalogScale) */
} AnalogScale;


typedef struct _ChannelConfig {
    uint32_t samplerate;
    uint32_t channelmask;
/* @@protoc_insertion_point(struct:ChannelConfig) */
} ChannelConfig;


typedef struct _TriggerChannel {
    uint32_t channel;
    TriggerType type;
/* @@protoc_insertion_point(struct:TriggerChannel) */
} TriggerChannel;


typedef struct _WaveConfig {
    float hz;
    float amplitude;
    float offset;
    float duty;
/* @@protoc_insertion_point(struct:WaveConfig) */
} WaveConfig;


typedef struct _DACConfig {
    WaveType wave;
    SweepType sweep;
    float sweep_time;
    bool has_start;
    WaveConfig start;
    bool has_end;
    WaveConfig end;
/* @@protoc_insertion_point(struct:DACConfig) */
} DACConfig;


typedef struct _TriggerConfig {
    pb_size_t triggers_count;
    TriggerChannel triggers[8];
/* @@protoc_insertion_point(struct:TriggerConfig) */
} TriggerConfig;


typedef struct _USBResponse {
    Status status;
    pb_callback_t statusmsg;
    uint32_t cpu_usage;
    ChannelId channelidx;
    uint32_t samplerate;
    int64_t sampleidx;
    uint32_t bits_per_sample;
    bool has_scaling;
    AnalogScale scaling;
    Compression compression;
    pb_callback_t data;
/* @@protoc_insertion_point(struct:USBResponse) */
} USBResponse;


typedef struct _Config {
    bool has_digital;
    ChannelConfig digital;
    bool has_analog1;
    ChannelConfig analog1;
    bool has_analog2;
    ChannelConfig analog2;
    bool has_analog3;
    ChannelConfig analog3;
    bool has_usb;
    ChannelConfig usb;
    bool has_dac1;
    DACConfig dac1;
/* @@protoc_insertion_point(struct:Config) */
} Config;


typedef struct _USBRequest {
    Command cmd;
    bool has_cfg;
    Config cfg;
/* @@protoc_insertion_point(struct:USBRequest) */
} USBRequest;


/* Initializer values for message structs */
#define ChannelConfig_init_default               {0, 0}
#define TriggerChannel_init_default              {0, _TriggerType_MIN}
#define TriggerConfig_init_default               {0, {TriggerChannel_init_default, TriggerChannel_init_default, TriggerChannel_init_default, TriggerChannel_init_default, TriggerChannel_init_default, TriggerChannel_init_default, TriggerChannel_init_default, TriggerChannel_init_default}}
#define WaveConfig_init_default                  {0, 0, 0, 0}
#define DACConfig_init_default                   {_WaveType_MIN, _SweepType_MIN, 0, false, WaveConfig_init_default, false, WaveConfig_init_default}
#define Config_init_default                      {false, ChannelConfig_init_default, false, ChannelConfig_init_default, false, ChannelConfig_init_default, false, ChannelConfig_init_default, false, ChannelConfig_init_default, false, DACConfig_init_default}
#define USBRequest_init_default                  {_Command_MIN, false, Config_init_default}
#define AnalogScale_init_default                 {0, _Unit_MIN}
#define USBResponse_init_default                 {_Status_MIN, {{NULL}, NULL}, 0, _ChannelId_MIN, 0, 0, 0, false, AnalogScale_init_default, _Compression_MIN, {{NULL}, NULL}}
#define ChannelConfig_init_zero                  {0, 0}
#define TriggerChannel_init_zero                 {0, _TriggerType_MIN}
#define TriggerConfig_init_zero                  {0, {TriggerChannel_init_zero, TriggerChannel_init_zero, TriggerChannel_init_zero, TriggerChannel_init_zero, TriggerChannel_init_zero, TriggerChannel_init_zero, TriggerChannel_init_zero, TriggerChannel_init_zero}}
#define WaveConfig_init_zero                     {0, 0, 0, 0}
#define DACConfig_init_zero                      {_WaveType_MIN, _SweepType_MIN, 0, false, WaveConfig_init_zero, false, WaveConfig_init_zero}
#define Config_init_zero                         {false, ChannelConfig_init_zero, false, ChannelConfig_init_zero, false, ChannelConfig_init_zero, false, ChannelConfig_init_zero, false, ChannelConfig_init_zero, false, DACConfig_init_zero}
#define USBRequest_init_zero                     {_Command_MIN, false, Config_init_zero}
#define AnalogScale_init_zero                    {0, _Unit_MIN}
#define USBResponse_init_zero                    {_Status_MIN, {{NULL}, NULL}, 0, _ChannelId_MIN, 0, 0, 0, false, AnalogScale_init_zero, _Compression_MIN, {{NULL}, NULL}}

/* Field tags (for use in manual encoding/decoding) */
#define AnalogScale_scaling_tag                  1
#define AnalogScale_unit_tag                     2
#define ChannelConfig_samplerate_tag             1
#define ChannelConfig_channelmask_tag            2
#define TriggerChannel_channel_tag               1
#define TriggerChannel_type_tag                  2
#define WaveConfig_hz_tag                        1
#define WaveConfig_amplitude_tag                 2
#define WaveConfig_offset_tag                    3
#define WaveConfig_duty_tag                      4
#define DACConfig_wave_tag                       1
#define DACConfig_sweep_tag                      2
#define DACConfig_sweep_time_tag                 3
#define DACConfig_start_tag                      4
#define DACConfig_end_tag                        5
#define TriggerConfig_triggers_tag               1
#define USBResponse_status_tag                   1
#define USBResponse_statusmsg_tag                2
#define USBResponse_cpu_usage_tag                3
#define USBResponse_channelidx_tag               8
#define USBResponse_samplerate_tag               9
#define USBResponse_sampleidx_tag                10
#define USBResponse_bits_per_sample_tag          11
#define USBResponse_scaling_tag                  12
#define USBResponse_compression_tag              13
#define USBResponse_data_tag                     20
#define Config_digital_tag                       16
#define Config_analog1_tag                       17
#define Config_analog2_tag                       18
#define Config_analog3_tag                       19
#define Config_usb_tag                           20
#define Config_dac1_tag                          21
#define USBRequest_cmd_tag                       1
#define USBRequest_cfg_tag                       2

/* Struct field encoding specification for nanopb */
#define ChannelConfig_FIELDLIST(X, a) \
X(a, STATIC, SINGULAR, UINT32, samplerate, 1) \
X(a, STATIC, SINGULAR, UINT32, channelmask, 2)
#define ChannelConfig_CALLBACK NULL
#define ChannelConfig_DEFAULT NULL

#define TriggerChannel_FIELDLIST(X, a) \
X(a, STATIC, SINGULAR, UINT32, channel, 1) \
X(a, STATIC, SINGULAR, UENUM, type, 2)
#define TriggerChannel_CALLBACK NULL
#define TriggerChannel_DEFAULT NULL

#define TriggerConfig_FIELDLIST(X, a) \
X(a, STATIC, REPEATED, MESSAGE, triggers, 1)
#define TriggerConfig_CALLBACK NULL
#define TriggerConfig_DEFAULT NULL
#define TriggerConfig_triggers_MSGTYPE TriggerChannel

#define WaveConfig_FIELDLIST(X, a) \
X(a, STATIC, SINGULAR, FLOAT, hz, 1) \
X(a, STATIC, SINGULAR, FLOAT, amplitude, 2) \
X(a, STATIC, SINGULAR, FLOAT, offset, 3) \
X(a, STATIC, SINGULAR, FLOAT, duty, 4)
#define WaveConfig_CALLBACK NULL
#define WaveConfig_DEFAULT NULL

#define DACConfig_FIELDLIST(X, a) \
X(a, STATIC, SINGULAR, UENUM, wave, 1) \
X(a, STATIC, SINGULAR, UENUM, sweep, 2) \
X(a, STATIC, SINGULAR, FLOAT, sweep_time, 3) \
X(a, STATIC, OPTIONAL, MESSAGE, start, 4) \
X(a, STATIC, OPTIONAL, MESSAGE, end, 5)
#define DACConfig_CALLBACK NULL
#define DACConfig_DEFAULT NULL
#define DACConfig_start_MSGTYPE WaveConfig
#define DACConfig_end_MSGTYPE WaveConfig

#define Config_FIELDLIST(X, a) \
X(a, STATIC, OPTIONAL, MESSAGE, digital, 16) \
X(a, STATIC, OPTIONAL, MESSAGE, analog1, 17) \
X(a, STATIC, OPTIONAL, MESSAGE, analog2, 18) \
X(a, STATIC, OPTIONAL, MESSAGE, analog3, 19) \
X(a, STATIC, OPTIONAL, MESSAGE, usb, 20) \
X(a, STATIC, OPTIONAL, MESSAGE, dac1, 21)
#define Config_CALLBACK NULL
#define Config_DEFAULT NULL
#define Config_digital_MSGTYPE ChannelConfig
#define Config_analog1_MSGTYPE ChannelConfig
#define Config_analog2_MSGTYPE ChannelConfig
#define Config_analog3_MSGTYPE ChannelConfig
#define Config_usb_MSGTYPE ChannelConfig
#define Config_dac1_MSGTYPE DACConfig

#define USBRequest_FIELDLIST(X, a) \
X(a, STATIC, SINGULAR, UENUM, cmd, 1) \
X(a, STATIC, OPTIONAL, MESSAGE, cfg, 2)
#define USBRequest_CALLBACK NULL
#define USBRequest_DEFAULT NULL
#define USBRequest_cfg_MSGTYPE Config

#define AnalogScale_FIELDLIST(X, a) \
X(a, STATIC, SINGULAR, FLOAT, scaling, 1) \
X(a, STATIC, SINGULAR, UENUM, unit, 2)
#define AnalogScale_CALLBACK NULL
#define AnalogScale_DEFAULT NULL

#define USBResponse_FIELDLIST(X, a) \
X(a, STATIC, SINGULAR, UENUM, status, 1) \
X(a, CALLBACK, SINGULAR, STRING, statusmsg, 2) \
X(a, STATIC, SINGULAR, UINT32, cpu_usage, 3) \
X(a, STATIC, SINGULAR, UENUM, channelidx, 8) \
X(a, STATIC, SINGULAR, UINT32, samplerate, 9) \
X(a, STATIC, SINGULAR, INT64, sampleidx, 10) \
X(a, STATIC, SINGULAR, UINT32, bits_per_sample, 11) \
X(a, STATIC, OPTIONAL, MESSAGE, scaling, 12) \
X(a, STATIC, SINGULAR, UENUM, compression, 13) \
X(a, CALLBACK, SINGULAR, BYTES, data, 20)
#define USBResponse_CALLBACK pb_default_field_callback
#define USBResponse_DEFAULT NULL
#define USBResponse_scaling_MSGTYPE AnalogScale

extern const pb_msgdesc_t ChannelConfig_msg;
extern const pb_msgdesc_t TriggerChannel_msg;
extern const pb_msgdesc_t TriggerConfig_msg;
extern const pb_msgdesc_t WaveConfig_msg;
extern const pb_msgdesc_t DACConfig_msg;
extern const pb_msgdesc_t Config_msg;
extern const pb_msgdesc_t USBRequest_msg;
extern const pb_msgdesc_t AnalogScale_msg;
extern const pb_msgdesc_t USBResponse_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define ChannelConfig_fields &ChannelConfig_msg
#define TriggerChannel_fields &TriggerChannel_msg
#define TriggerConfig_fields &TriggerConfig_msg
#define WaveConfig_fields &WaveConfig_msg
#define DACConfig_fields &DACConfig_msg
#define Config_fields &Config_msg
#define USBRequest_fields &USBRequest_msg
#define AnalogScale_fields &AnalogScale_msg
#define USBResponse_fields &USBResponse_msg

/* Maximum encoded size of messages (where known) */
#define ChannelConfig_size                       12
#define TriggerChannel_size                      8
#define TriggerConfig_size                       80
#define WaveConfig_size                          20
#define DACConfig_size                           53
#define Config_size                              131
#define USBRequest_size                          136
#define AnalogScale_size                         7
/* USBResponse_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
