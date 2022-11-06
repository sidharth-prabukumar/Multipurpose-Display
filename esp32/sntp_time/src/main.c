#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/net/sntp.h>
#include <zephyr/sys/timeutil.h>

#include <esp_wifi.h>

#define SPI2_NODE DT_NODELABEL(spi2)

#define SNTP_PORT 123

#define SERVER_ADDR "129.6.15.28"

#define WIFI_SSID_NAME "<Your_WiFi_SSID>"

#define WIFI_PASSWORD "<Your_WiFi_Password>"

/* size of stack area used by each thread */
#define STACKSIZE (2048)

/* scheduling priority used by each thread */
#define PRIORITY 7

/**
 * @brief Connect to the given WiFi network and retrieve
 * 	the time from the SNTP server
 */
void connect_sntp();

/**
 * @brief Send the retreived time via the SPI interface
 */
void spi_send_data();

K_MUTEX_DEFINE(mutex_sntp_time);

static struct sntp_time sntp_time_val;

static char dayofweek[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday",
								"Thursday", "Friday", "Saturday"};

void connect_sntp()
{
	wifi_config_t wifi_config;
	strcpy(wifi_config.sta.ssid, WIFI_SSID_NAME);
	strcpy(wifi_config.sta.password, WIFI_PASSWORD);

	esp_err_t ret = esp_wifi_set_mode(ESP32_WIFI_MODE_STA);

	ret |= esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
	ret |= esp_wifi_connect();
	if (ret != ESP_OK)
	{
		printk("connection failed\n");
	}
	struct sntp_ctx ctx;
	struct sockaddr_in addr;
	int rv;

	/* ipv4 */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SNTP_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &addr.sin_addr);

	rv = sntp_init(&ctx, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	if (rv < 0)
	{
		printk("Failed to init SNTP IPv4 ctx: %d\n", rv);
		goto end;
	}

	printk("Sending SNTP IPv4 request...\n");
	while (1)
	{
	start:
		k_mutex_lock(&mutex_sntp_time, K_FOREVER);
		rv = sntp_query(&ctx, 4 * MSEC_PER_SEC, &sntp_time_val);
		k_mutex_unlock(&mutex_sntp_time);
		if (rv < 0)
		{
			printk("SNTP IPv4 request failed: %d\n", rv);
			goto start;
		}

		struct tm tm = {
			/* Initialize an unset field */
			.tm_isdst = 1234,
		};
		time_t time = (time_t)(sntp_time_val.seconds - 14400);
		struct tm *tp = gmtime(&time);
		printk("%02d/%02d/%04d %s %02d:%02d:%02d\n", tp->tm_mon, tp->tm_mday,
			   (1900 + tp->tm_year), dayofweek[tp->tm_wday], tp->tm_hour, tp->tm_min,
			   tp->tm_sec);
		k_msleep(1000);
	}

end:
	sntp_close(&ctx);
}

void spi_send_data()
{
	const struct device *spi2_dev = DEVICE_DT_GET(SPI2_NODE);

	const struct spi_config spi_cfg = {
		.frequency = 1562500U,
		.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER};

	struct spi_buf tx_bufs[] = {
		{.buf = (uint8_t *)&(sntp_time_val.seconds), .len = 8},
	};

	struct spi_buf_set tx = {.buffers = tx_bufs, .count = 1};

	while (1)
	{
		k_mutex_lock(&mutex_sntp_time, K_FOREVER);
		spi_write(spi2_dev, &spi_cfg, &tx);
		k_mutex_unlock(&mutex_sntp_time);
		/* Send every 5 seconds */
		k_msleep(5000);
	}
}

K_THREAD_DEFINE(spi_send_data_id, STACKSIZE, spi_send_data, NULL, NULL, NULL, 8, 0, 0);
K_THREAD_DEFINE(connect_sntp_id, STACKSIZE, connect_sntp, NULL, NULL, NULL, PRIORITY, 0, 0);