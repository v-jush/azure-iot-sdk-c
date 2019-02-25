// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_PROV_TRANSPORT_H
#define IOTHUB_PROV_TRANSPORT_H

#ifdef __cplusplus
extern "C" {
    #include <cstddef>
#else
    #include <stddef.h>
#endif

#include "azure_c_shared_utility/umock_c_prod.h"
//#include "azure_prov_client/prov_transport.h"
//#include "iothub_transport_ll.h"
//#include "internal/iothub_transport_ll_private.h"

typedef struct PROVISIONING_RESULT_DATA_TAG
{
    const char* iothub_uri;
    const char* device_id;
    const char* custom_hub_data;
} PROVISIONING_RESULT_DATA;

#define PROVISIONING_STATUS_VALUES  \
    PROV_STATUS_SUCCESS,            \
    PROV_STATUS_UNAUTHORIZIED,      \
    PROV_STATUS_ERROR

DEFINE_ENUM(PROVISIONING_STATUS, PROVISIONING_STATUS_VALUES);

#define PROV_SECURITY_TYPE_VALUES   \
    PROV_SECURITY_X509,             \
    PROV_SECURITY_TPM,              \
    PROV_SECURITY_SYMM_KEY

DEFINE_ENUM(PROV_SECURITY_TYPE, PROV_SECURITY_TYPE_VALUES);

typedef void(*PROV_REGISTRATION_CB)(void* user_ctx, PROVISIONING_STATUS status, const PROVISIONING_RESULT_DATA* result_data);

/*typedef struct IOTHUB_PROV_TRANSPORT_PROVIDER_TAG
{
    TRANSPORT_PROVIDER iothub_transport;
    //PROV_DEVICE_TRANSPORT_PROVIDER prov_dev_transport;
} IOTHUB_PROV_TRANSPORT_PROVIDER;

typedef const IOTHUB_PROV_TRANSPORT_PROVIDER*(*PROV_TRANSPORT_PROVIDER)(void);*/

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_PROV_TRANSPORT_H */

