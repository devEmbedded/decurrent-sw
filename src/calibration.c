#include "decurrent.pb.h"

// The firmware default calibration uses the schematic nominal values
// and assumes 1% tolerance for resistors.
static const CalibrationInfo g_firmware_default_calibration = {
    .calibrated_by = "Firmware default",
    .date = "",
    .has_ai0_calibration = true,
    .ai0_calibration = {
        .has_resistance = true, .resistance = {990000, 11000, 0, 0},
        .has_offset = true, .offset = {0.0f, 0.015f, 0, 0},
        .has_scale = true, .scale = {120.0f/990.0f, 0.003f, 0, 0}
    },
    .has_ai1_calibration = true,
    .ai1_calibration = {
        .has_resistance = true, .resistance = {990000, 11000, 0, 0},
        .has_offset = true, .offset = {0.0f, 0.015f, 0, 0},
        .has_scale = true, .scale = {120.0f/990.0f, 0.003f, 0, 0}
    },
    .has_ai2_calibration = true,
    .ai2_calibration = {
        .has_resistance = true, .resistance = {1.0f, 0.012f, 0, 0},
        .has_offset = true, .offset = {0.0f, 0.010f, 0, 0},
        .has_scale = true, .scale = {20.0f, 0.20f, 0, 0}
    },
    .has_ai3_calibration = true,
    .ai3_calibration = {
        .has_resistance = true, .resistance = {47.0f, 0.50f, 0, 0},
        .has_offset = true, .offset = {0.0f, 0.010f, 0, 0},
        .has_scale = true, .scale = {1.0f, 0.02f, 0, 0}
    },
    .has_dac_calibration = true,
    .dac_calibration = {
        .has_resistance = true, .resistance = {47.0f, 0.50f, 0, 0},
        .has_offset = true, .offset = {0.0f, 0.010f, 0, 0},
        .has_scale = true, .scale = {7.25f, 0.15f, 0, 0}
    },

    .has_usb_shunt = true,
    .usb_shunt = {0.05f, 0.0006f, 0, 0},

    .has_vref = true,
    .vref = {2.5f, 0.006f, 0, 0}
};
