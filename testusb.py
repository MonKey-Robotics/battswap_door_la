import usb.core
import usb.util

# Find your USB relay
device = usb.core.find(idVendor=0x16C0, idProduct=0x05DF)
if device is None:
    raise ValueError("Device not found")

# Send a control command (ON/OFF)
device.ctrl_transfer(0x21, 0x09, 0x0200, 0, [0xFF, 0x01])  # Example payload
