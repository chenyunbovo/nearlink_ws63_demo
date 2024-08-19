#ifndef __BLUFI_ADV_H__
#define __BLUFI_ADV_H__

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    BLE_PUBLIC_DEVICE_ADDRESS,
    BLE_RANDOM_DEVICE_ADDRESS,
    BLE_PUBLIC_IDENTITY_ADDRESS,
    BLE_RANDOM_STATIC_IDENTITY_ADDRESS
} ble_address_t;

#define NOTIFY_HANDLE 17
#define WRITE_HANDLE 19

uint8_t blufi_set_adv_data(void);
uint8_t blufi_start_adv(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif