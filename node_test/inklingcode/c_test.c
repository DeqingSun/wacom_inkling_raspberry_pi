#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <linux/usbdevice_fs.h>

#include <libusb-1.0/libusb.h>

#define VENDOR_ID 0x056A
#define PRODUCT_ID 0x0221

// Values for bmRequestType in the Setup transaction's Data packet.

static const int CONTROL_REQUEST_TYPE_IN = LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE;
static const int CONTROL_REQUEST_TYPE_OUT = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE;


static const int HID_GET_REPORT = 0x01;
static const int HID_SET_REPORT = 0x09;
static const int HID_REPORT_TYPE_INPUT = 0x01;
static const int HID_REPORT_TYPE_OUTPUT = 0x02;
static const int HID_REPORT_TYPE_FEATURE = 0x03;

static const int INTERFACE_NUMBER = 0;
static const int TIMEOUT_MS = 1000;

unsigned char data_in[64];
unsigned char data_out[64];

int send_control_transfer(libusb_device_handle *devh,char *buf, int len){
  memset (data_out,0,64);
  memcpy(data_out,buf,len);
  int r = libusb_control_transfer(devh,CONTROL_REQUEST_TYPE_OUT,HID_SET_REPORT,(HID_REPORT_TYPE_FEATURE<<8)|0x80,INTERFACE_NUMBER,data_out,33,TIMEOUT_MS);
  if (r>0) printf("send OK\n");
  return r;
}

int receive_control_transfer(libusb_device_handle *devh){
  int r = libusb_control_transfer(devh,CONTROL_REQUEST_TYPE_IN ,HID_GET_REPORT,(HID_REPORT_TYPE_FEATURE<<8)|0x80,INTERFACE_NUMBER,data_in,33,TIMEOUT_MS);
  int i;
  if (r >= 0){
    int data_len=r-1;
    for (;data_len>=0;data_len--){
	  if (data_in[data_len]!=0) break;
    }
    data_len=data_len+1;
    printf("Feature report data received: ");
    for(i = 0; i < data_len; i++){
      printf("%02x ",data_in[i]);
    }
	printf("\n");
  }
  return r;
}

int reset_wacom_inkling(){
  int bus_id=-1,device_id=-1;
  
  {//reset usb device
    FILE *fp;
    int status;
    char path[1024];
    /* Open the command for reading. */
    fp = popen("lsusb", "r");
    if (fp == NULL) {
      printf("Failed to run lsusb\n" );
      return -1;
    }
    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path)-1, fp) != NULL) {
      if (strstr(path,"056a:0221")!=NULL){//look for inkling
        //printf("%s", path);
		sscanf(path,"Bus %d Device %d",&bus_id,&device_id);
      }
    }
    /* close */
    pclose(fp);
  
    if (bus_id>=0 && device_id>=0){
	  char buf[64];
	  sprintf(buf, "/dev/bus/usb/%03d/%03d", bus_id, device_id);
	  printf("Try to reset Inkling on: %s\n",buf);
      int fd, rc;
      fd = open(buf, O_WRONLY);
      if (fd < 0) {
          perror("Error opening output file");
          return -1;
      }
      printf("Resetting USB device %s\n", buf);
      rc = ioctl(fd, USBDEVFS_RESET, 0);
      if (rc < 0) {
          perror("Error in ioctl");
          return -1;
      }
      printf("Reset successful\n");
      close(fd);
	}else{
	  printf("Inkling not found\n");	
	}
  }
  return 0;
}

int main(int argc, char **argv) {
  reset_wacom_inkling();
  
  const struct libusb_version *version = libusb_get_version();
  printf("Using libusb v%d.%d.%d.%d\n\n", version->major, version->minor, version->micro, version->nano);

  int r=1;

  r = libusb_init(NULL);
  if (r < 0) {
    fprintf(stderr, "Failed to initialise libusb\n");
    return 1;
  }

  // discover devices
  libusb_device **list;
  libusb_device *found = NULL;
  libusb_context *ctx = NULL;
  int attached = 0;

  ssize_t cnt = libusb_get_device_list(ctx, &list);
  ssize_t i = 0;
  int err = 0;
  if (cnt < 0){
    printf( "no usb devices found\n" );
    goto out;
  }

  // find our device
  for(i = 0; i < cnt; i++){
    libusb_device *device = list[i];
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor( device, &desc );

    if( desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID ){
      found=device;
    }
  }

  if (found == NULL){
    printf("Unable to find usb device\n");
    goto out;
  }

  libusb_device_handle *devh; 
  err = libusb_open(found, &devh);
  if (err){
    printf("Unable to open usb device\n");
    goto out;
  }

  printf("Successfully find device\n");

//#ifdef LINUX 
  if (libusb_kernel_driver_active(devh,0)==1){
    printf("Device busy...detaching...\n"); 
    attached = 1;
    libusb_detach_kernel_driver(devh, 0);   
  }
//#endif 


  err = libusb_claim_interface( devh, 0 );
  if (err){
    printf( "Failed to claim interface. " );
    switch( err ){
    case LIBUSB_ERROR_NOT_FOUND:	
      printf( "not found\n" );	break;
    case LIBUSB_ERROR_BUSY:		
      printf( "busy\n" );		break;
    case LIBUSB_ERROR_NO_DEVICE:	
      printf( "no device\n" );	break;
    default:			
      printf( "other\n" );		break;
    }
    goto out;
  } 
  printf( "interface claimed\n" );


  {	//try disable first?
    char buf[]={0x80,0x01,0x02,0x01,0x02};
    send_control_transfer(devh,buf,sizeof(buf));
  }  


  {
    char buf[]={0x80,0x01,0x03,0x01,0x02};
    send_control_transfer(devh,buf,sizeof(buf));
  }

  {
    char buf[]={0x80,0x01,0x0A,0x01,0x01,0x03,0x01};
    send_control_transfer(devh,buf,sizeof(buf));
  }
  receive_control_transfer(devh);

  {
    char buf[]={0x80,0x01,0x0B,0x01};
    send_control_transfer(devh,buf,sizeof(buf));
  }
  
  {
    char buf[]={0x80,0x01,0x02,0x01,0x01};
    send_control_transfer(devh,buf,sizeof(buf));
  }



  
  printf("Yeah\n");


  int bytes_transferred;

  while(1){
    r = libusb_bulk_transfer(devh,0x83,data_in,16,&bytes_transferred,TIMEOUT_MS);
    switch(r){
	case 0:	//success
	  {
	    if (data_in[0]==2){
          int x=data_in[1]+data_in[2]*256;
          int y=data_in[3]+data_in[4]*256;
          int button=data_in[5];
          int pressure=data_in[6]+data_in[7]*256;
          int x_tilt=(signed char)data_in[8];
          int y_tilt=(signed char)data_in[9];
            
          printf("x:%d\ty:%d\tb:%d\tp:%d\txt:%d\tyt:%d\n",x,y,button,pressure,x_tilt,y_tilt);
	    }
	  }
	  
	  
	  break;
	  
    case LIBUSB_ERROR_TIMEOUT :	
      printf( "LIBUSB_ERROR_TIMEOUT (Don't worry)\n" );	break;
    case LIBUSB_ERROR_PIPE :		
      printf( "LIBUSB_ERROR_PIPE \n" );		break;
    case LIBUSB_ERROR_OVERFLOW :	
      printf( "no LIBUSB_ERROR_OVERFLOW \n" );	break;
    case LIBUSB_ERROR_NO_DEVICE :	
      printf( "no LIBUSB_ERROR_NO_DEVICE \n" );	break;
    default:			
      printf( "other\n" );		break;
    } 
  }	
out:
  //libusb_reset_device(devh);
  libusb_close(devh);
  libusb_exit(NULL);

  return 0;
}




