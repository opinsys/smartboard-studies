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
	struct usb_device *dev;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	busses = usb_get_busses();

	for (bus = busses; bus; bus = bus->next) {

		for (dev = bus->devices; dev; dev = dev->next) {
			if (is_smartboard(dev))
				goto smartboard_found;
		}
	}

	fprintf(stderr, "error: SMARTBoard not found\n");
	return 1;

smartboard_found:
	printf("%d\n", dev->devnum);

	return 0;
}
