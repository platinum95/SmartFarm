#include "btHelper.h"

#include <zephyr.h>
#include <stdio.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>

static bool bt_connected;

static
void bt_connect_cb(struct bt_conn *conn, u8_t err)
{
    bt_connected = true;
}

static
void bt_disconnect_cb(struct bt_conn *conn, u8_t reason)
{
    bt_connected = false;
    printf("bt disconnected (reason %u)\n", reason);
}

static
struct bt_conn_cb bt_conn_cb = {
    .connected = bt_connect_cb,
    .disconnected = bt_disconnect_cb,
};

int networkSetup(void)
{
    const char *progress_mark = "/-\\|";
    int i = 0;
    int rc;

    rc = bt_enable(NULL);
    if (rc && rc != -EALREADY) {
        printf("bluetooth init failed\n");
        return rc;
    }

    bt_conn_cb_register(&bt_conn_cb);

    printf("\nwaiting for bt connection: ");
    while (bt_connected == false) {
        k_sleep(250);
        printf("%c\b", progress_mark[i]);
        i = (i + 1) % (sizeof(progress_mark) - 1);
    }
    printf("\n");
    return 0;
}
