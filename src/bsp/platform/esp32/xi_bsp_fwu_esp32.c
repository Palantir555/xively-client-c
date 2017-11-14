/* Copyright (c) 2003-2017, LogMeIn, Inc. All rights reserved.
 *
 * This is part of the Xively C Client library,
 * it is licensed under the BSD 3-Clause license.
 */

#include <string.h>

#include <xi_bsp_fwu.h>
#include <xively_error.h>
#include <xi_bsp_debug.h>

#include "esp_system.h"
#include "esp_ota_ops.h"

extern void esp32_xibsp_notify_update_applied();

uint8_t xi_bsp_fwu_is_this_firmware( const char* const resource_name )
{
    return ( 0 == strcmp( "firmware.bin", resource_name ) ) ? 1 : 0;
}

xi_bsp_fwu_state_t xi_bsp_fwu_on_new_firmware_ok()
{
    /* ESP32 doesn't have a commit/rollback mechanism in place for freshly
       installed images yet */
    xi_bsp_debug_logger( "New firmware image OK. No commit action implemented" );
    return XI_BSP_FWU_ACTUAL_COMMIT_HAPPENED;
    // return XI_BSP_FWU_STATE_OK;
}

void xi_bsp_fwu_on_new_firmware_failure()
{
    /* Updated image couldn't be validated / Roll back to previous image */
    /* ESP32 doesn't have a commit/rollback mechanism in place for freshly
       installed images yet */
    xi_bsp_debug_logger( "Error validating new image. No rollback action implemented" );
}

void xi_bsp_fwu_on_package_download_failure()
{
    xi_bsp_debug_logger( "Firmware image download failed. OTA update aborted!" );
}

void xi_bsp_fwu_on_package_download_finished( const char* const firmware_resource_name )
{
    ( void )firmware_resource_name;
    xi_bsp_debug_logger( "Firmware package download finished. Rebooting" );
    const esp_partition_t* next_partition = esp_ota_get_next_update_partition( NULL );
    esp_err_t retv                        = ESP_OK;

    retv = esp_ota_set_boot_partition( next_partition );
    if ( ESP_OK != retv )
    {
        xi_bsp_debug_format( "esp_ota_set_boot_partition() failed with error %d", retv );
        return;
    }

    esp32_xibsp_notify_update_applied();

    /* reboot the device */
    esp_restart();
}