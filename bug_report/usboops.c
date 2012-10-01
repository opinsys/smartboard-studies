#include <usb.h>

/* Check `lsusb -v' for correct values on your machine. An USB mouse is
 * a good choice. */
static const int ID_VENDOR = 0x046d;
static const int ID_PRODUCT = 0xc52b;
static const int B_INTERFACE_NUMBER = 2;
static const int B_ENDPOINT_ADDRESS = 0x83; /* Any interrupt IN endpoint. */
static const int W_MAX_PACKET_SIZE = 32;

int main(void)
{
	struct usb_bus *bus;
	struct usb_device *device;
	usb_dev_handle *dev_handle = NULL;

	usb_init();
	usb_find_busses();
	usb_find_devices();
	for (bus = usb_get_busses(); bus; bus = bus->next) {
		for (device = bus->devices; device; device = device->next) {
			if (device->descriptor.idVendor == ID_VENDOR
			    && device->descriptor.idProduct == ID_PRODUCT) {
				dev_handle = usb_open(device);
				break;
			}
		}
	}
	usb_detach_kernel_driver_np(dev_handle, B_INTERFACE_NUMBER);
	usb_claim_interface(dev_handle, B_INTERFACE_NUMBER);

	while (1) {
		char buf[W_MAX_PACKET_SIZE];
		usb_interrupt_read(dev_handle, B_ENDPOINT_ADDRESS,
				   buf, sizeof(buf), 1);
	}

	return 0;
}
