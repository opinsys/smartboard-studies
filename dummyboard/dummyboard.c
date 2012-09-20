#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <usb.h>

static const useconds_t DUMMY_LOOP_INTERVAL = 100000;

/* TODO: This should be replaced with a sniffing logic at some
 * point. For now, we can just rely on this magic number. */
static const int DUMMY_SB_IFACE = 0;

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

static volatile sig_atomic_t dummy_is_running = 1;

static void dummy_sigint_handler(const int signum)
{
	dummy_is_running = 0;
}

int main(void)
{
	struct usb_device *dev = NULL;
	usb_dev_handle *devh = NULL;
	bool is_detached = false;
	bool is_claimed = false;

	usb_init();

	signal(SIGINT, dummy_sigint_handler);
	signal(SIGTERM, dummy_sigint_handler);
	signal(SIGHUP, dummy_sigint_handler);
	signal(SIGUSR1, dummy_sigint_handler);
	signal(SIGUSR2, dummy_sigint_handler);

	while (dummy_is_running) {
		usleep(DUMMY_LOOP_INTERVAL);

		usb_find_busses();
		usb_find_devices();

		if (!dev) {
			dev = dummy_find_smartboard();
			continue;
		}

		if (!devh) {
			devh = usb_open(dev);
			continue;
		}

		if (!is_detached) {
			if (usb_detach_kernel_driver_np(devh, DUMMY_SB_IFACE))
				fprintf(stderr, "error: libusb: %s", usb_strerror());
			else
				is_detached = true;
			continue;
		}

		if (!is_claimed) {
			if (usb_claim_interface(devh, DUMMY_SB_IFACE))
				fprintf(stderr, "error: libusb: %s", usb_strerror());
			else
				is_claimed = true;
			continue;
		}
	}

	if (devh && is_claimed)
		if (usb_release_interface(devh, DUMMY_SB_IFACE))
			fprintf(stderr, "error: libusb: %s", usb_strerror());

	if (devh && usb_close(devh))
		fprintf(stderr, "error: libusb: %s", usb_strerror());

	printf("GracefulShutdown™!\n");

	return 0;
}
