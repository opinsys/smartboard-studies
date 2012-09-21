#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <usb.h>

static char *dummy_aget_hexstr(const char *const bytes, const size_t size)
{
	char *hexstr;
	char *hexstr_ptr;
	int i;

	hexstr = calloc(3 * size + 1, sizeof(char));
	hexstr_ptr = hexstr;

	for (i = 0; i < size; ++i)
		hexstr_ptr += sprintf(hexstr_ptr, "%02hhX ", bytes[i]);

	return hexstr;
}

static const useconds_t DUMMY_LOOP_INTERVAL = 100000;

/* TODO: This should be replaced with a sniffing logic at some
 * point. For now, we can just rely on this magic number. */
static const int DUMMY_SB_IFACE = 0;

static const int DUMMY_SB_EP_READ = 0x82;

/* This magic value is sniffed from a conversation between an authentic
   SMARTBoardService and an authentic SMARTBoard. */
static const int DUMMY_SB_TIMEOUT_READ = 100;

static int dummy_is_smartboard(const struct usb_device *const dev)
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
			if (dummy_is_smartboard(dev))
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
		const int read_buf_size = 32;
		char read_buf[read_buf_size];
		int read_size;

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
			const int driver_name_size = 256;
			char driver_name[driver_name_size];

			/* Assume the driver is detached if the driver's
			 * name cannot be found. */
			is_detached = usb_get_driver_np(devh, DUMMY_SB_IFACE,
							driver_name, driver_name_size);
			is_detached = is_detached || !usb_detach_kernel_driver_np(devh, DUMMY_SB_IFACE);
			if (!is_detached)
				fprintf(stderr, "error: libusb: %s\n", usb_strerror());
			continue;
		}

		if (!is_claimed) {
			is_claimed = !usb_claim_interface(devh, DUMMY_SB_IFACE);
			if (!is_claimed)
				fprintf(stderr, "error: libusb: %s\n", usb_strerror());
			continue;
		}

		do {
			read_size = usb_interrupt_read(devh, DUMMY_SB_EP_READ,
						       read_buf, read_buf_size,
						       DUMMY_SB_TIMEOUT_READ);
			if (read_size < 0) {
				fprintf(stderr, "error: libusb: %d %s\n", read_size, usb_strerror());
			} else {
				char *hexstr;
				hexstr = dummy_aget_hexstr(read_buf, read_size);
				printf("%s\n", hexstr);
				free(hexstr);
			}

		} while (read_size >= 0);
	}

	if (devh && is_claimed && usb_release_interface(devh, DUMMY_SB_IFACE))
		fprintf(stderr, "error: libusb: %s\n", usb_strerror());

	if (devh && usb_close(devh))
		fprintf(stderr, "error: libusb: %s\n", usb_strerror());

	printf("GracefulShutdown™!\n");

	return 0;
}
