/* Copyright (c) 2003-2018, LogMeIn, Inc. All rights reserved.
 *
 * This is part of the Xively C Client library,
 * it is licensed under the BSD 3-Clause license.
 */

#include <xi_itest_gateway.h>
#include "xi_itest_helpers.h"

#include "xi_memory_checks.h"
#include "xi_globals.h"
#include "xi_backoff_status_api.h"
#include "xi_itest_layerchain_ct_ml_mc.h"
#include "xi_itest_mock_broker_layerchain.h"
#include "xi_handle.h"
#include <xi_context.h>
#include <xively_gateway.h>

/*********************************************************************************
 * test fixture ******************************************************************
 ********************************************************************************/
typedef struct xi_itest_gateway__test_fixture_s
{
    uint16_t loop_id__manual_disconnect;
    uint16_t max_loop_count;

    xi_context_t* xi_context;
    xi_context_handle_t xi_context_handle;

    xi_context_t* xi_context_mockbroker;

} xi_itest_gateway__test_fixture_t;


static xi_itest_gateway__test_fixture_t* _xi_itest_gateway__generate_fixture()
{
    xi_state_t state = XI_STATE_OK;

    XI_ALLOC( xi_itest_gateway__test_fixture_t, fixture, state );


    fixture->loop_id__manual_disconnect = 0xFFFF - 5;
    fixture->max_loop_count             = 0xFFFF;

    return fixture;

err_handling:
    fail();

    return NULL;
}

/*********************************************************************************
 * setup / teardown **************************************************************
 ********************************************************************************/
int xi_itest_gateway_setup( void** fixture_void )
{
    xi_memory_limiter_tearup();

    xi_itest_gateway__test_fixture_t* fixture = _xi_itest_gateway__generate_fixture();

    *fixture_void = fixture;

    xi_globals.backoff_status.backoff_lut_i = 0;
    xi_cancel_backoff_event();

    xi_initialize( "xi_itest_gateway_account_id", "xi_itest_gateway_device_id" );

    xi_state_t state = xi_create_context_with_custom_layers_and_evtd(
        &fixture->xi_context, itest_ct_ml_mc_layer_chain, XI_LAYER_CHAIN_CT_ML_MC,
        XI_LAYER_CHAIN_SCHEME_LENGTH( XI_LAYER_CHAIN_CT_ML_MC ), NULL, 1 );

    XI_CHECK_STATE( state );

    xi_find_handle_for_object( xi_globals.context_handles_vector, fixture->xi_context,
                               &fixture->xi_context_handle );

    state = xi_create_context_with_custom_layers_and_evtd(
        &fixture->xi_context_mockbroker, itest_mock_broker_codec_layer_chain,
        XI_LAYER_CHAIN_MOCK_BROKER_CODEC,
        XI_LAYER_CHAIN_SCHEME_LENGTH( XI_LAYER_CHAIN_MOCK_BROKER_CODEC ), NULL, 0 );

    XI_CHECK_STATE( state );

    return 0;

err_handling:
    fail();

    return 1;
}

int xi_itest_gateway_teardown( void** fixture_void )
{
    xi_itest_gateway__test_fixture_t* fixture =
        ( xi_itest_gateway__test_fixture_t* )*fixture_void;

    xi_delete_context_with_custom_layers(
        &fixture->xi_context, itest_ct_ml_mc_layer_chain,
        XI_LAYER_CHAIN_SCHEME_LENGTH( XI_LAYER_CHAIN_CT_ML_MC ) );

    xi_delete_context_with_custom_layers(
        &fixture->xi_context_mockbroker, itest_mock_broker_codec_layer_chain,
        XI_LAYER_CHAIN_SCHEME_LENGTH( XI_LAYER_CHAIN_MOCK_BROKER_CODEC ) );

    xi_shutdown();

    XI_SAFE_FREE( fixture );

    return !xi_memory_limiter_teardown();
}


/*********************************************************************************
 * act ***************************************************************************
 ********************************************************************************/

/*********************************************************************************
 * test cases ********************************************************************
 ********************************************************************************/
void xi_itest_gateway__first( void** fixture_void )
{
    xi_itest_gateway__test_fixture_t* fixture =
        ( xi_itest_gateway__test_fixture_t* )*fixture_void;

    xi_state_t state = XI_STATE_OK;

    state = xi_connect_ed( fixture->xi_context_handle, "edge application device id" );

    state = xi_disconnect_ed( fixture->xi_context_handle, "edge application device id" );

    XI_UNUSED( state );
}
