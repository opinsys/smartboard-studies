#include <stdio.h>
#include <stdbool.h>
#include <usb.h>

static int is_smartboard(const struct usb_device *const dev)
{
	return dev->descriptor.idVendor == 0x0b8c
		&& dev->descriptor.idProduct == 0x0001;
}

static struct usb_device *dummy_find_smartboard(void)
{
	struct usb_bus *busses;
	struct usb_bus *bus;
	struct usb_device *dev;

	for (bus = usb_get_busses(); bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			if (is_smartboard(dev))
				return dev;
		}
	}
	return NULL;
}

int main(void)
{
	struct usb_device *dev = NULL;

	usb_init();

	for (;;) {
		usb_find_busses();
		usb_find_devices();

		if (!dev) {
			dev = dummy_find_smartboard();
			continue;
		}
	}

	return 0;
}
