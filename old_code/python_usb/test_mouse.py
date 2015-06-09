#!/usr/bin/python
import sys
import usb.core
import usb.util
# decimal vendor and product values
#dev = usb.core.find(idVendor=1118, idProduct=1917)
# or, uncomment the next line to search instead by the hexidecimal equivalent
dev = usb.core.find(idVendor=0x046D, idProduct=0xC062)
# first endpoint
interface = 0
endpoint = dev[0][(0,0)][0]
# if the OS kernel already claimed the device, which is most likely true
# thanks to http://stackoverflow.com/questions/8218683/pyusb-cannot-set-configuration
#if dev.is_kernel_driver_active(interface) is True:
  # tell the kernel to detach
#  dev.detach_kernel_driver(interface)
  # claim the device
#  usb.util.claim_interface(dev, interface)

USBRQ_HID_GET_REPORT = 0x01;
USBRQ_HID_SET_REPORT = 0x09;

buf=[0]*33;
buf[0]=1;
buf[1]=2;


dev.ctrl_transfer(0x21, USBRQ_HID_SET_REPORT, 0x0380, 0, data_or_wLength=buf)

collected = 0
attempts = 50
while collected < attempts :
    try:
        data = dev.read(endpoint.bEndpointAddress,endpoint.wMaxPacketSize);
        collected += 1;
        print data;
    except usb.core.USBError as e:
        data = None
        if e.args == ('Operation timed out',):
            continue
# release the device
usb.util.release_interface(dev, interface)
# reattach the device to the OS kernel
#dev.attach_kernel_driver(interface)
