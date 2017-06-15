/* Copyright (c) 2003-2017, LogMeIn, Inc. All rights reserved.
 *
 * This is part of the Xively C Client library,
 * it is licensed under the BSD 3-Clause license.
 */

#include <xi_control_message.h>
#include <stddef.h>
#include <xi_macros.h>
#include <xi_helpers.h>

#include <stdio.h>

xi_control_message_t* xi_control_message_create_file_info( const char** filenames,
                                                           const char** revisions,
                                                           uint16_t count )
{
    if ( NULL == filenames || NULL == *filenames || 0 == count )
    {
        return NULL;
    }

    xi_state_t state = XI_STATE_OK;

    XI_ALLOC( xi_control_message_t, sft_message, state );

    sft_message->file_info.common.msgtype = XI_CONTROL_MESSAGE_CS_FILE_INFO;
    sft_message->file_info.common.msgver  = 1;

    XI_ALLOC_BUFFER_AT( xi_control_message_file_desc_t, sft_message->file_info.list,
                        sizeof( xi_control_message_file_desc_t ) * count, state );

    sft_message->file_info.list_len = count;

    uint16_t id_file = 0;
    for ( ; id_file < count; ++id_file )
    {
        sft_message->file_info.list[id_file].name = xi_str_dup( *filenames );
        ++filenames;

        if ( NULL != revisions )
        {
            sft_message->file_info.list[id_file].revision = xi_str_dup( *revisions );
            ++revisions;
        }
    }

    return sft_message;

err_handling:

    xi_control_message_free( &sft_message );

    return sft_message;
}

xi_control_message_t* xi_control_message_create_file_get_chunk( const char* filename,
                                                                const char* revision,
                                                                uint32_t offset,
                                                                uint32_t length )
{
    if ( NULL == filename || NULL == revision )
    {
        return NULL;
    }

    xi_state_t state = XI_STATE_OK;

    XI_ALLOC( xi_control_message_t, sft_message, state );

    sft_message->common.msgtype               = XI_CONTROL_MESSAGE_CS_FILE_GET_CHUNK;
    sft_message->file_get_chunk.common.msgver = 1;

    sft_message->file_get_chunk.name     = xi_str_dup( filename );
    sft_message->file_get_chunk.revision = xi_str_dup( revision );
    sft_message->file_get_chunk.offset   = offset;
    sft_message->file_get_chunk.length   = length;

    return sft_message;

err_handling:

    xi_control_message_free( &sft_message );

    return sft_message;
}

void xi_control_message_free( xi_control_message_t** control_message )
{
    if ( NULL == control_message || NULL == *control_message )
    {
        return;
    }

    switch ( ( *control_message )->common.msgtype )
    {
        case XI_CONTROL_MESSAGE_CS_FILE_INFO:
        {
            uint16_t id_file = 0;
            for ( ; id_file < ( *control_message )->file_info.list_len; ++id_file )
            {
                /* free xi_control_message_file_desc_t */
                XI_SAFE_FREE( ( *control_message )->file_info.list[id_file].name );
                XI_SAFE_FREE( ( *control_message )->file_info.list[id_file].revision );
            }

            XI_SAFE_FREE( ( *control_message )->file_info.list );
        }

        break;

        case XI_CONTROL_MESSAGE_SC_FILE_UPDATE_AVAILABLE:
        {
            uint16_t id_file = 0;
            for ( ; id_file < ( *control_message )->file_update_available.list_len;
                  ++id_file )
            {
                /* free xi_control_message_file_desc_ext_t */
                XI_SAFE_FREE(
                    ( *control_message )->file_update_available.list[id_file].name );
                XI_SAFE_FREE(
                    ( *control_message )->file_update_available.list[id_file].revision );
                XI_SAFE_FREE( ( *control_message )
                                  ->file_update_available.list[id_file]
                                  .fingerprint );
            }

            XI_SAFE_FREE( ( *control_message )->file_update_available.list );
        }

        break;

        case XI_CONTROL_MESSAGE_CS_FILE_GET_CHUNK:

            XI_SAFE_FREE( ( *control_message )->file_get_chunk.name );
            XI_SAFE_FREE( ( *control_message )->file_get_chunk.revision );

            break;

        case XI_CONTROL_MESSAGE_SC_FILE_CHUNK:

            XI_SAFE_FREE( ( *control_message )->file_chunk.name );
            XI_SAFE_FREE( ( *control_message )->file_chunk.revision );
            XI_SAFE_FREE( ( *control_message )->file_chunk.chunk );

            break;

        case XI_CONTROL_MESSAGE_CS_FILE_STATUS:

            break;
    }

    XI_SAFE_FREE( *control_message );
}

#if 1 // XI_DEBUG_OUTPUT
void xi_debug_control_message_dump( const xi_control_message_t* control_message,
                                    const char* custom_label )
{
    if ( NULL == control_message )
    {
        return;
    }

    printf( "+++ xi_control_message_t: +++++++++++++++++++++++++ [%s] \n", custom_label );
    printf( "+++ msgtype: %d, msgver: %d\n", control_message->common.msgtype,
            control_message->common.msgver );

    switch ( control_message->common.msgtype )
    {
        case XI_CONTROL_MESSAGE_CS_FILE_INFO:
        {
            printf( "+++ SFT FILE_INFO, list_len %d\n",
                    control_message->file_info.list_len );
            printf( "+++ #  [name], [revision]\n" );

            uint16_t id_file = 0;
            for ( ; id_file < control_message->file_info.list_len; ++id_file )
            {
                printf( "+++ #%d [%s], [%s]\n", id_file + 1,
                        control_message->file_info.list[id_file].name,
                        control_message->file_info.list[id_file].revision );
            }
        }
        break;

        case XI_CONTROL_MESSAGE_SC_FILE_UPDATE_AVAILABLE:
        {
            printf( "+++ SFT FILE_UPDATE_AVAILABLE, list_len %d\n",
                    control_message->file_update_available.list_len );
            printf( "+++ #  [name], [revision], [file operation], [size in bytes], "
                    "[fingerprint]\n" );

            uint16_t id_file = 0;
            for ( ; id_file < control_message->file_update_available.list_len; ++id_file )
            {
                printf(
                    "+++ #%d [%s], [%s], [%d], [%d], [", id_file + 1,
                    control_message->file_update_available.list[id_file].name,
                    control_message->file_update_available.list[id_file].revision,
                    control_message->file_update_available.list[id_file].file_operation,
                    control_message->file_update_available.list[id_file].size_in_bytes );

                uint16_t id_byte = 0;
                for ( ; id_byte < control_message->file_update_available.list[id_file]
                                      .fingerprint_len;
                      ++id_byte )
                {
                    printf( "%c", control_message->file_update_available.list[id_file]
                                      .fingerprint[id_byte] );
                }

                printf( "]:[%d]\n", control_message->file_update_available.list[id_file]
                                        .fingerprint_len );
            }
        }
        break;

        case XI_CONTROL_MESSAGE_CS_FILE_GET_CHUNK:
            printf( "+++ XI_CONTROL_MESSAGE_CS_FILE_GET_CHUNK\n" );
            printf( "+++ name: [%s], revision: [%s]\n",
                    control_message->file_get_chunk.name,
                    control_message->file_get_chunk.revision );
            printf( "+++ offset: %d, length: %d\n",
                    control_message->file_get_chunk.offset,
                    control_message->file_get_chunk.length );
            break;

        case XI_CONTROL_MESSAGE_SC_FILE_CHUNK:
            printf( "+++ XI_CONTROL_MESSAGE_SC_FILE_CHUNK\n" );
            printf( "+++ name: [%s], revision: [%s]\n", control_message->file_chunk.name,
                    control_message->file_chunk.revision );
            printf( "+++ offset: %d, length: %d\n", control_message->file_chunk.offset,
                    control_message->file_chunk.length );
            printf( "+++ status: %d, chunk ptr: %p\n", control_message->file_chunk.status,
                    control_message->file_chunk.chunk );
            break;
        default:;
    }

    printf( "+++++++++++++++++++++++++++++++++++++++++++++++++++ [%s]\n", custom_label );
}
#endif