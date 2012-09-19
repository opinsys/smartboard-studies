#include <stdio.h>

#include <usb.h>

static int is_smartboard(const struct usb_device *const dev)
{
	return dev->descriptor.idVendor == 0x0b8c
		&& dev->descriptor.idProduct == 0x0001;
}

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
			if (is_smartboard(dev))
				goto smartboard_found;
		}
	}

smartboard_found:

	return 0;
}
