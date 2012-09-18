#include <stdio.h>

#include <usb.h>

int main(void)
{
	struct usb_bus *busses;
	struct usb_bus *bus;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	busses = usb_get_busses();

	for (bus = busses; bus; bus = bus->next) {
		struct usb_device *dev;

		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor == 0x0b8c
			    && dev->descriptor.idProduct == 0x0001) {
				printf("%d %d\n", bus->root_dev->devnum, dev->devnum);
				break;
			}
		}
	}

	return 0;
}
