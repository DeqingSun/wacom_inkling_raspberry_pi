#!/usr/bin/python
import sys
import usb.core
import usb.util
# decimal vendor and product values
#dev = usb.core.find(idVendor=1118, idProduct=1917)
# or, uncomment the next line to search instead by the hexidecimal equivalent
dev = usb.core.find(idVendor=0x056A, idProduct=0x0221)

dev.set_configuration()

# first endpoint
interface = 0
endpoint = dev[0][(0,0)][0]

print "ENDPOINT"+str(endpoint.bEndpointAddress)


USBRQ_HID_GET_REPORT = 0x01;
USBRQ_HID_SET_REPORT = 0x09;



try:
    #buf[4], I guess it is ID. 01 is mouse and 02 are digitizer
    # seems only 1 and 2 are valid from reading function
    buf=[0x80,0x01,0x03,0x01,0x01];
    buf=[0x80,0x01,0x03,0x01,0x01];
    buf.extend([0]*(33-len(buf)));
    dev.ctrl_transfer(0x21, USBRQ_HID_SET_REPORT, 0x0380, 0, data_or_wLength=buf)
    #lack of this will using digitizer as output


    


    buf=[0x80,0x01,0x0A,0x01,0x01,0x0B,0x01];
    buf.extend([0]*(33-len(buf)));
    dev.ctrl_transfer(0x21, USBRQ_HID_SET_REPORT, 0x0380, 0, data_or_wLength=buf)

    print dev.ctrl_transfer(0xA1, USBRQ_HID_GET_REPORT, 0x0380, 0, data_or_wLength=33)
##
    buf=[0x80,0x01,0x0B,0x01];
    buf.extend([0]*(33-len(buf)));
    dev.ctrl_transfer(0x21, USBRQ_HID_SET_REPORT, 0x0380, 0, data_or_wLength=buf)
##

    #important to enable device, buf[4]=1 will enable output while 2 will disable
    # seems only 1 and 2 are valid from reading function
    buf=[0x80,0x01,0x02,0x01,0x01];
    buf.extend([0]*(33-len(buf)));
    dev.ctrl_transfer(0x21, USBRQ_HID_SET_REPORT, 0x0380, 0, data_or_wLength=buf)

    #try to read data back
    buf=[0x80,0x01,0x0A,0x01,0x01,0x02,0x01];
    buf.extend([0]*(33-len(buf)));
    dev.ctrl_transfer(0x21, USBRQ_HID_SET_REPORT, 0x0380, 0, data_or_wLength=buf)
    print dev.ctrl_transfer(0xA1, USBRQ_HID_GET_REPORT, 0x0380, 0, data_or_wLength=33)

    #try to read data back
    buf=[0x80,0x01,0x0A,0x01,0x01,0x03,0x01];
    buf.extend([0]*(33-len(buf)));
    dev.ctrl_transfer(0x21, USBRQ_HID_SET_REPORT, 0x0380, 0, data_or_wLength=buf)
    print dev.ctrl_transfer(0xA1, USBRQ_HID_GET_REPORT, 0x0380, 0, data_or_wLength=33)
    

##    buf=[0x80,0x01,0x0A,0x01,0x01,0x02,0x01];
##    buf.extend([0]*(33-len(buf)));
##    dev.ctrl_transfer(0x21, USBRQ_HID_SET_REPORT, 0x0380, 0, data_or_wLength=buf)
##
##    print dev.ctrl_transfer(0xA1, USBRQ_HID_GET_REPORT, 0x0380, 0, data_or_wLength=33)
except usb.core.USBError as e:
    print e.args
    
print "\n\ndata"



# if the OS kernel already claimed the device, which is most likely true
# thanks to http://stackoverflow.com/questions/8218683/pyusb-cannot-set-configurati
collected = 0
attempts = 50
while (collected < attempts or 1) :
    try:
        data = dev.read(endpoint.bEndpointAddress,endpoint.wMaxPacketSize)
        collected += 1
        if (data[0]==1):
            x=data[1]+data[2]*256;
            y=data[3]+data[4]*256;
            pressed=data[5]
            print 'x:%d\ty:%d\tpressed:%d' % (x,y,pressed)
        if (data[0]==2):
            x=data[1]+data[2]*256;
            y=data[3]+data[4]*256;
            button=data[5]
            pressure=data[6]+data[7]*256;
            x_tilt=data[8] if data[8]<=127 else data[8]-256;
            y_tilt=data[9] if data[9]<=127 else data[9]-256;
            
            #print data
            print 'x:%d\ty:%d\tb:%d\tp:%d\txt:%d\tyt:%d' % (x,y,button,pressure,x_tilt,y_tilt)
        
        
        #print 'x:'+x+'\ty:'+y+'\tpressed:'+pressed; 
    except usb.core.USBError as e:
        data = None
        if e.args == ('Operation timed out',):
            continue
# release the device
usb.util.release_interface(dev, interface)

