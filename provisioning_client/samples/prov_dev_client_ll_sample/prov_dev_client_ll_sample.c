
/*
 *  Created on: Nov 26, 2018
 *      Author: Roman Schneider
 */

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <signal.h>
//#include <getopt.h>

#include "azureiot/iothub.h"
#include "azureiot/iothub_client.h"
#include "azureiot/iothub_client_options.h"
#include "azureiot/iothub_client_version.h"
#include "azureiot/iothubtransportamqp_websockets.h"
#include "azureiot/iothubtransportmqtt.h"
#include "azureiot/iothubtransportmqtt_websockets.h"
#include "azureiot/iothubtransportamqp.h"

#include <azure_c_shared_utility/platform.h>
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "azure_c_shared_utility/threadapi.h"

#include "azure_prov_client/prov_device_client.h"
#include "azure_prov_client/prov_security_factory.h"
#include "azure_prov_client/prov_transport_amqp_ws_client.h"
#include "azure_prov_client/prov_transport_http_client.h"

#define UNUSED(expr) do { (void)(expr); } while (0)

    DEFINE_ENUM_STRINGS(PROV_DEVICE_RESULT, PROV_DEVICE_RESULT_VALUE);
DEFINE_ENUM_STRINGS(PROV_DEVICE_REG_STATUS, PROV_DEVICE_REG_STATUS_VALUES);

static bool g_registration_complete = false;
static bool g_connection_complete = false;
static bool g_use_proxy = false;
static const char* PROXY_ADDRESS = "10.0.2.2";

#define PROXY_PORT                  3128
#define MESSAGES_TO_SEND            2
#define TIME_BETWEEN_MESSAGES       2

static char* g_iothub_uri;
static char* g_device_id;

static size_t g_messageTrackingId;

static bool g_sending;
static bool g_dps_reconnect = false;
static size_t g_reconnect_cnt = 0;

/* callback requests reconnect */
static bool g_please_reconnect;

// global Iothub & DPS variables
HTTP_PROXY_OPTIONS g_http_proxy;
char* g_prov_uri = "global.azure-devices-provisioning.net";
char* g_id_scope = "0ne0001782C";
bool g_traceOn = false;
static IOTHUB_CLIENT_HANDLE iothubclient;
static IOTHUB_CLIENT_TRANSPORT_PROVIDER  g_iothub_protocol = AMQP_Protocol_over_WebSocketsTls;

static int connect_DPS();
static int connect_IoTHub();
static void set_iothub_protocol(const char* protocol);


/******************************************************************************/

static void msg_confirmation_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result,
    void* data)
{
    size_t id = (size_t)data;

    UNUSED(data);
    printf("%s: result: %s for id %zd\n", __func__,
        ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result), id);
}

int method_invocation_callback(const char* method_name,
    const unsigned char* payload, size_t size, unsigned char** response,
    size_t* response_size, void* user_context) {

    UNUSED(user_context);

    printf("%s: %s payload_len = %zd\n", __func__, method_name, size);

    *response = (unsigned char*)malloc(size);
    *response_size = size;
    memcpy(*response, payload, size);

    return 0;
}

IOTHUBMESSAGE_DISPOSITION_RESULT msg_receive_callback(
    IOTHUB_MESSAGE_HANDLE message, void* user_context)
{

    UNUSED(user_context);

    IOTHUB_MESSAGE_RESULT msg_ret = IOTHUB_MESSAGE_OK;
    const char* buf = "";
    size_t buf_size = 0;
    int cb_result = IOTHUB_CLIENT_IOTHUB_METHOD_STATUS_ERROR;

    IOTHUBMESSAGE_CONTENT_TYPE payload_type =
        IoTHubMessage_GetContentType(message);

    switch (payload_type) {
    case IOTHUBMESSAGE_BYTEARRAY:
        msg_ret = IoTHubMessage_GetByteArray(
            message, (const unsigned char**)&buf, &buf_size);
        if (msg_ret != IOTHUB_MESSAGE_OK) {
            printf("%s: binary message parsing failed!\n", __func__);
        }
        break;
    case IOTHUBMESSAGE_STRING:
        buf = IoTHubMessage_GetString(message);
        if (buf != NULL) {
            buf_size = strlen(buf);
        }
        else {
            printf("%s: string message parsing failed!\n", __func__);
        }
        break;
    case IOTHUBMESSAGE_UNKNOWN:
    default:
        printf("%s: Unknown Message content type!\n", __func__);
    }

    if (buf != NULL) {
        printf("%s: received: %s\n", __func__, buf);
        cb_result = IOTHUB_CLIENT_IOTHUB_METHOD_STATUS_SUCCESS;
    }

    return cb_result;
}

static void connection_callback(IOTHUB_CLIENT_CONNECTION_STATUS result,
    IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context)
{
    UNUSED(user_context);

    printf("%s: %s, reason: %s\n", __func__,
        ENUM_TO_STRING(IOTHUB_CLIENT_CONNECTION_STATUS, result),
        ENUM_TO_STRING(IOTHUB_CLIENT_CONNECTION_STATUS_REASON, reason));

    if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED) {
        g_connection_complete = true;
        g_reconnect_cnt++;
    }
    else {
        /**
         * Disconnect looks like this:
         * IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED,
         * reason: IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR
         */
         // Do not trigger reconnect while callback is called during normal
         // connect
        if (g_dps_reconnect && !g_please_reconnect) {
            printf("\n Signaling reconnect request \n");
            g_please_reconnect = true;
        }
        g_connection_complete = false;
    }
}

static void registation_status_callback(PROV_DEVICE_REG_STATUS reg_status, void* user_context)
{
    UNUSED(user_context);

    printf("%s: Provisioning Status: %s\n", __func__,
        ENUM_TO_STRING(PROV_DEVICE_REG_STATUS, reg_status));
}

static void register_device_callback(PROV_DEVICE_RESULT register_result,
    const char* iothub_uri, const char* device_id, void* user_context)
{
    UNUSED(user_context);

    if (register_result == PROV_DEVICE_RESULT_OK) {
        printf("%s: Registration Information received from service: %s, deviceId: %s\n",
            __func__, iothub_uri, device_id);

        g_iothub_uri = _strdup(iothub_uri);
        g_device_id = _strdup(device_id);

        g_registration_complete = true;

    }
    else {
        (void)printf("%s: Failure registering device: %s\n", __func__,
            ENUM_TO_STRING(PROV_DEVICE_RESULT, register_result));
    }
}

static void sendMessage(IOTHUB_CLIENT_HANDLE iotHubClientHandle)
{
    char payload[32];

    snprintf(payload, sizeof(payload), "%zd", g_messageTrackingId);

    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(
        (unsigned char*)payload, strlen(payload));

    if (messageHandle == NULL) {
        printf("%s: unable to create a new IoTHubMessage\n", __func__);
    }
    else {
        if (IoTHubClient_SendEventAsync(iotHubClientHandle, messageHandle,
            msg_confirmation_callback, (void*)g_messageTrackingId)
            == IOTHUB_CLIENT_OK) {
            printf("%s: with id %zd (recconnect_cnt = %zd) \n", __func__,
                g_messageTrackingId, g_reconnect_cnt);
        }
        IoTHubMessage_Destroy(messageHandle);
    }
    g_messageTrackingId++;
}

static void sig_handler(int signo) {
    if (signo == SIGINT) {
        printf("\nreceived SIGINT - terminating\n");
        g_sending = false;
    }
    else if (signo == SIGTERM) {
        printf("\nreceived SIGTERM - terminating\n");
        g_sending = false;
    }
    else {
        printf("\nreceived unknown signal - ignoring\n");
        return;
    }
}

static void print_help(void)
{
    char *txt = ""
        "Usage: simplect[OPTIONS]\n"
        "simple azure iot test framework\n\n"
        "Options:\n"
        "-d, --dps-reconnect perform full reconnect via DPS if SAS token expired \n"
        "-P, --use-proxy    use http proxy \n"
        "-x, --protocol     iothub protocol [AMQP_WEBSOCKETS | MQTT_WEBSOCKETS]\n"
        "-t, --traceon      activate Azure IoT SDK traces\n"
        "-p, --period       period in seconds (example: -p 60)\n"
        "-s, --scope        DPS scope (example: -s 0ne0001205C)\n"
        "-h, --help         this\n\n";

    printf("%s", txt);
}

static void connect() {
    printf(">%s\n", __func__);
    g_connection_complete = false;

    if (connect_DPS() != EXIT_SUCCESS) {
        printf("connect_DPS() failed, aborting\n");
        exit(-1);
    }

    if (connect_IoTHub() != EXIT_SUCCESS) {
        printf("connect_IoTHub() failed, aborting\n");
        exit(-1);
    }
    printf("<%s\n", __func__);
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    /*int c;*/
    size_t period_sec = 2;

    //while (1) {
    //    static struct option long_options[] = {
    //        {"dps-reconnect", no_argument, 0, 'd'},
    //        {"help", no_argument, 0, 'h'},
    //        {"period", required_argument, 0, 'p'},
    //        {"protocol", required_argument, 0, 'x'},
    //        {"scope", required_argument, 0, 's'},
    //        {"traceon", no_argument, 0, 't'},
    //        {"use-proxy", no_argument, 0, 'P'}, {0, 0, 0, 0}
    //    };

    //    /* getopt_long stores the option index here. */
    //    int option_index = 0;

    //    c = getopt_long(argc, argv, "x:ts:p:Ph", long_options, &option_index);

    //    /* Detect the end of the options. */
    //    if (c == -1)
    //        break;

    //    switch (c) {
    //    case 0:
    //        /* If this option set a flag, do nothing else now. */
    //        if (long_options[option_index].flag != 0)
    //            break;
    //        printf("option %s", long_options[option_index].name);
    //        if (optarg)
    //            printf(" with arg %s", optarg);
    //        printf("\n");
    //        break;
    //    case 'd': /* dps-reconnect */
    //        g_dps_reconnect = true;
    //        break;

    //    case 'P': /* proxy */
    //        g_use_proxy = true;
    //        break;

    //    case 'x': /* protocol MQTT/AMQP */
    //        set_iothub_protocol("AMPQ_WEBSOCKETS");
    //        break;

    //    case 's':
    //        g_id_scope = optarg;
    //        break;

    //    case 'p':
    //        period_sec = atoi(optarg);
    //        break;

    //    case 't':
    //        g_traceOn = true;
    //        break;

    //    case 'h':
    //        print_help();
    //        return EXIT_SUCCESS;

    //    case '?':
    //        /* getopt_long already printed an error message. */
    //        break;

    //    default:
    //        abort();
    //    }
    //}
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    set_iothub_protocol("AMQP_WEBSOCKETS");

    if (platform_init() != 0) {
        printf("Platform_init failed!\n");
        return EXIT_FAILURE;
    }
    SECURE_DEVICE_TYPE hsm_type = SECURE_DEVICE_TYPE_X509;
    (void)prov_dev_security_init(hsm_type);

    //memset(&g_http_proxy, 0, sizeof(HTTP_PROXY_OPTIONS));


    printf(" Provisioning API Version: %s\n", Prov_Device_GetVersionString());
    printf("   IOT HUB Client Version: %s\n", IoTHubClient_GetVersionString());
    printf("         Provisioning URI: %s\n", g_prov_uri);
    printf("       Provisioning Scope: %s\n", g_id_scope);
    printf("Message interval (period): %zd\n", period_sec);

    /*if (g_use_proxy) {
        g_http_proxy.host_address = PROXY_ADDRESS;
        g_http_proxy.port = PROXY_PORT;
    }
*/
    connect();
    g_sending = true;

    do {
        if (g_dps_reconnect && g_please_reconnect) {
            printf("\n Reconnect requested - destroying IotHubClient \n");
            IoTHubClient_Destroy(iothubclient);
            ThreadAPI_Sleep(5000);
            printf("\n IotHubClient destroyed, trying reconnect\n");
            connect();
        }
        else if (g_connection_complete) {
            sendMessage(iothubclient);
        }

        ThreadAPI_Sleep(1000 * period_sec);
    } while (g_sending);

    IoTHubClient_Destroy(iothubclient);

    free(g_iothub_uri);
    free(g_device_id);

    prov_dev_security_deinit();

    // Free all the sdk subsystem
    platform_deinit();
    return EXIT_SUCCESS;
}

/******************************************************************************/

static void set_iothub_protocol(const char* protoname) {
    if (strcmp(protoname, "AMPQ_WEBSOCKETS") == 0) {
        g_iothub_protocol = AMQP_Protocol_over_WebSocketsTls;
        return;
    }
    if (strcmp(protoname, "MQTT_WEBSOCKETS") == 0) {
        g_iothub_protocol = MQTT_WebSocket_Protocol;
        return;
    }
}

/******************************************************************************/

static int connect_DPS() {
    PROV_DEVICE_TRANSPORT_PROVIDER_FUNCTION prov_transport;
    prov_transport = Prov_Device_HTTP_Protocol;
    PROV_DEVICE_RESULT prov_device_result;
    PROV_DEVICE_HANDLE prov_device_handle;
    g_please_reconnect = false;
    g_registration_complete = false;
    printf(">%s\n", __func__);


    if ((prov_device_handle = Prov_Device_Create(
        g_prov_uri, g_id_scope, prov_transport)) == NULL) {
        printf("failed calling Prov_Device_Create\n");
        return EXIT_FAILURE;
    }
    if (g_http_proxy.host_address != NULL) {
        Prov_Device_SetOption(
            prov_device_handle, OPTION_HTTP_PROXY, &g_http_proxy);
    }

    Prov_Device_SetOption(
        prov_device_handle, PROV_OPTION_LOG_TRACE, &g_traceOn);

    prov_device_result = Prov_Device_Register_Device(prov_device_handle,
        register_device_callback, NULL, registation_status_callback, NULL);

    if (prov_device_result != PROV_DEVICE_RESULT_OK) {
        printf("FATAL: Prov_Device_Register_Device failed\n");
        return EXIT_FAILURE;
    }

    printf("Registering Device ...\n");

    do {
        ThreadAPI_Sleep(1000);
    } while (!g_registration_complete);
    printf(" DPS registration complete\n");

    Prov_Device_Destroy(prov_device_handle);
    printf("<%s\n", __func__);

    return EXIT_SUCCESS;
}

/******************************************************************************/

static int connect_IoTHub() {
    printf(">%s\n", __func__);
    g_connection_complete = false;
    iothubclient = IoTHubClient_CreateFromDeviceAuth(
        g_iothub_uri, g_device_id, g_iothub_protocol);

    if (iothubclient == NULL) {
        printf("FATAL: can't IoTHubClient_CreateFromDeviceAuth.\n");
        return EXIT_FAILURE;
    }

    if (IoTHubClient_SetOption(iothubclient, OPTION_LOG_TRACE, &g_traceOn) !=
        IOTHUB_CLIENT_OK) {
        printf("IoTHubClient_SetOption OPTION_LOG_TRACE failed.\n");
    }

    if (IoTHubClient_SetRetryPolicy(iothubclient, IOTHUB_CLIENT_RETRY_INTERVAL,
        120) != IOTHUB_CLIENT_OK) {
        printf("IoTHubClient_SetRetryPolicy failed.\n");
    }

    if (IoTHubClient_SetConnectionStatusCallback(
        iothubclient, connection_callback, 0) != IOTHUB_CLIENT_OK) {
        printf("IoTHubClient_SetConnectionStatusCallback failed.\n");
    }

    if (IoTHubClient_SetMessageCallback(
        iothubclient, msg_receive_callback, 0) != IOTHUB_CLIENT_OK) {
        printf("IoTHubClient_SetMessageCallback failed.\n");
    }

    if (IoTHubClient_SetDeviceMethodCallback(
        iothubclient, method_invocation_callback, 0) != IOTHUB_CLIENT_OK) {
        printf("IoTHubClient_SetDeviceMethodCallback failed.\n");
    }

    printf("Waiting for Authentication ... \n");

    do {
        ThreadAPI_Sleep(1000);
    } while (!g_connection_complete);

    printf(" IoT Hub connection complete\n");
    return EXIT_SUCCESS;
}
