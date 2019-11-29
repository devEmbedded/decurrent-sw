#include <hal.h>
#include <hal_usb_cdc.h>
#include "usbcfg.h"

static const uint8_t g_usb_device_descriptor_data[USB_DESC_DEVICE_SIZE] = {
    USB_DESC_DEVICE(
        0x0200, // bcdUSB = 2.0
        0xEF, // bDeviceClass in interface descriptors
        0x02, // bDeviceSubClass
        0x01, // bDeviceProtocol
        0x40, // bMaxPacketSize
        0x1d50, // idVendor (Openmoko)
        0xffff, // idProduct (to be assigned)
        0x0100, // bcdDevice
        1,      // iManufacturer
        2,      // iProduct
        3,      // iSerialNumber
        1       // bNumConfigurations
    )
};
static USBDescriptor g_usb_device_descriptor = {
    sizeof(g_usb_device_descriptor_data), g_usb_device_descriptor_data
};

#define CDC_FUNC_DESCRIPTOR_SIZE (5+5+4+5)
#define CDC_FUNC_DESCRIPTOR \
    USB_DESC_BYTE(5), USB_DESC_BYTE(0x24), USB_DESC_BYTE(0x00), USB_DESC_BCD(0x0110), \
    USB_DESC_BYTE(5), USB_DESC_BYTE(0x24), USB_DESC_BYTE(0x01), USB_DESC_BYTE(0x00), USB_DESC_BYTE(0x01), \
    USB_DESC_BYTE(4), USB_DESC_BYTE(0x24), USB_DESC_BYTE(0x02), USB_DESC_BYTE(0x02), \
    USB_DESC_BYTE(5), USB_DESC_BYTE(0x24), USB_DESC_BYTE(0x06), USB_DESC_BYTE(0), USB_DESC_BYTE(1)

#define USB_CONFIG_DESCRIPTOR_SIZE (USB_DESC_CONFIGURATION_SIZE + \
                                    USB_DESC_INTERFACE_ASSOCIATION_SIZE * 2 + \
                                    USB_DESC_INTERFACE_SIZE * 3 + \
                                    CDC_FUNC_DESCRIPTOR_SIZE + \
                                    USB_DESC_ENDPOINT_SIZE * 5)
static const uint8_t g_usb_config_descriptor_data[USB_CONFIG_DESCRIPTOR_SIZE] = {
    USB_DESC_CONFIGURATION(USB_CONFIG_DESCRIPTOR_SIZE, 3, 1, 0, 0x80, 200),
    USB_DESC_INTERFACE_ASSOCIATION(0, 2, CDC_COMMUNICATION_INTERFACE_CLASS, CDC_ABSTRACT_CONTROL_MODEL, 1, 0),
    USB_DESC_INTERFACE(0, 0, 1, 0x02, 0x02, 0x01, 0),
    CDC_FUNC_DESCRIPTOR,
    USB_DESC_ENDPOINT(USB_SERIAL_IRQ_ENDPOINT | 0x80, 0x03, 8, 10),
    USB_DESC_INTERFACE(1, 0, 2, 0x0A, 0x00, 0x00, 0),
    USB_DESC_ENDPOINT(USB_SERIAL_DATA_ENDPOINT, 0x02, 512, 0),
    USB_DESC_ENDPOINT(USB_SERIAL_DATA_ENDPOINT | 0x80, 0x02, 512, 0),
    USB_DESC_INTERFACE_ASSOCIATION(2, 1, 0xFF, 0, 0, 0),
    USB_DESC_INTERFACE(2, 0, 2, 0xFF, 0, 0, 0),
    USB_DESC_ENDPOINT(USB_PROTOBUF_ENDPOINT, 0x02, 512, 0),
    USB_DESC_ENDPOINT(USB_PROTOBUF_ENDPOINT | 0x80, 0x02, 512, 0)
};
static USBDescriptor g_usb_config_descriptor = {
    sizeof(g_usb_config_descriptor_data), g_usb_config_descriptor_data
};

static const uint8_t g_usb_string_langid[] = {
    USB_DESC_BYTE(4), USB_DESC_BYTE(USB_DESCRIPTOR_STRING), USB_DESC_WORD(0x0409)
};
static const uint8_t g_usb_string_vendor[] = {
    USB_DESC_BYTE(24), USB_DESC_BYTE(USB_DESCRIPTOR_STRING),
    'd', 0, 'e', 0, 'v', 0, 'E', 0, 'm', 0, 'b', 0, 'e', 0, 'd', 0,
    'd', 0, 'e', 0, 'd', 0
};
static const uint8_t g_usb_string_product[] = {
    USB_DESC_BYTE(20), USB_DESC_BYTE(USB_DESCRIPTOR_STRING),
    'D', 0, 'E', 0, 'C', 0, 'u', 0, 'r', 0, 'r', 0, 'e', 0, 'n', 0,
    't', 0
};
static uint8_t g_usb_string_serial[] = {
    USB_DESC_BYTE(18), USB_DESC_BYTE(USB_DESCRIPTOR_STRING),
    '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0
};
static const USBDescriptor g_usb_strings[] = {
    {sizeof(g_usb_string_langid),  g_usb_string_langid},
    {sizeof(g_usb_string_vendor),  g_usb_string_vendor},
    {sizeof(g_usb_string_product), g_usb_string_product},
    {sizeof(g_usb_string_serial),  g_usb_string_serial},
};

static USBInEndpointState g_usb_serial_irq_ep_instate;
static const USBEndpointConfig g_usb_serial_irq_ep_config = {
    .ep_mode = USB_EP_MODE_TYPE_INTR,
    .setup_cb = NULL,
    .in_cb = sduInterruptTransmitted,
    .out_cb = NULL,
    .in_maxsize = 16,
    .out_maxsize = 0,
    .in_state = &g_usb_serial_irq_ep_instate,
    .out_state = NULL,
    .in_multiplier = 1,
    .setup_buf = NULL
};


static USBInEndpointState g_usb_serial_data_ep_instate;
static USBOutEndpointState g_usb_serial_data_ep_outstate;
static const USBEndpointConfig g_usb_serial_data_ep_config = {
    .ep_mode = USB_EP_MODE_TYPE_BULK,
    .setup_cb = NULL,
    .in_cb = sduDataTransmitted,
    .out_cb = sduDataReceived,
    .in_maxsize = 512,
    .out_maxsize = 512,
    .in_state = &g_usb_serial_data_ep_instate,
    .out_state = &g_usb_serial_data_ep_outstate,
    .in_multiplier = 1,
    .setup_buf = NULL
};

static USBInEndpointState g_usb_protobuf_ep_instate;
static USBOutEndpointState g_usb_protobuf_ep_outstate;
static const USBEndpointConfig g_usb_protobuf_ep_config = {
    .ep_mode = USB_EP_MODE_TYPE_BULK,
    .setup_cb = NULL,
    .in_cb = NULL,
    .out_cb = NULL,
    .in_maxsize = 512,
    .out_maxsize = 512,
    .in_state = &g_usb_protobuf_ep_instate,
    .out_state = &g_usb_protobuf_ep_outstate,
    .in_multiplier = 3,
    .setup_buf = NULL
};

static void usb_event(USBDriver *usbp, usbevent_t event)
{
    if (event == USB_EVENT_CONFIGURED)
    {
        chSysLockFromISR();
        if (usbp->state == USB_ACTIVE)
        {
            usbInitEndpointI(usbp, USB_SERIAL_IRQ_ENDPOINT, &g_usb_serial_irq_ep_config);
            usbInitEndpointI(usbp, USB_SERIAL_DATA_ENDPOINT, &g_usb_serial_data_ep_config);
            usbInitEndpointI(usbp, USB_PROTOBUF_ENDPOINT, &g_usb_protobuf_ep_config);
            sduConfigureHookI(&g_usb_serial);
        }
        else if (usbp->state == USB_SELECTED)
        {
            usbDisableEndpointsI(usbp);
        }
        chSysUnlockFromISR();
    }
    else if (event == USB_EVENT_RESET || event == USB_EVENT_UNCONFIGURED || event == USB_EVENT_SUSPEND)
    {
        chSysLockFromISR();
        sduSuspendHookI(&g_usb_serial);
        chSysUnlockFromISR();
    }
    else if (event == USB_EVENT_WAKEUP)
    {
        chSysLockFromISR();
        sduWakeupHookI(&g_usb_serial);
        chSysUnlockFromISR();
    }
}

static const USBDescriptor *usb_get_descriptor(USBDriver *usbp, uint8_t dtype,
                                               uint8_t dindex, uint16_t lang)
{
    if (dtype == USB_DESCRIPTOR_DEVICE)
    {
        return &g_usb_device_descriptor;
    }
    else if (dtype == USB_DESCRIPTOR_CONFIGURATION)
    {
        return &g_usb_config_descriptor;
    }
    else if (dtype == USB_DESCRIPTOR_STRING)
    {
        if (dindex < sizeof(g_usb_strings) / sizeof(g_usb_strings[0]))
        {
            return &g_usb_strings[dindex];
        }
    }

    return NULL;
}

static bool usb_control_request_hook(USBDriver *usbp)
{
    if (((usbp->setup[0] & USB_RTYPE_RECIPIENT_MASK) == USB_RTYPE_RECIPIENT_INTERFACE) &&
        (usbp->setup[1] == USB_REQ_SET_INTERFACE))
    {
        usbSetupTransfer(usbp, NULL, 0, NULL);
        return true;
    }

    return sduRequestsHook(usbp);
}

static void usb_sof_hook(USBDriver *usbp)
{
    chSysLockFromISR();
    sduSOFHookI(&g_usb_serial);
    chSysUnlockFromISR();
}

const USBConfig g_usbcfg = {
    usb_event,
    usb_get_descriptor,
    usb_control_request_hook,
    usb_sof_hook
};

SerialUSBDriver g_usb_serial;

const SerialUSBConfig g_usb_serial_config = {
    &USBD2,
    USB_SERIAL_DATA_ENDPOINT,
    USB_SERIAL_DATA_ENDPOINT,
    USB_SERIAL_IRQ_ENDPOINT
};