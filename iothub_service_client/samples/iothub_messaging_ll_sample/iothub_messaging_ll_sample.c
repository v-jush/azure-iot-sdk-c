// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/consolelogger.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/connection_string_parser.h"

#include "iothub_messaging_ll_sample.h"

#include "iothub_service_client_auth.h"
#include "iothub_messaging_ll.h"
#include "iothub_message.h"


/* String containing Hostname, SharedAccessKeyName and SharedAccessKey in the format:                       */
/* "HostName=<host_name>;SharedAccessKeyName=<shared_access_key_name>;SharedAccessKey=<shared_access_key>" */
static const char* connectionString = "[IoTHub Connection String]";
static const char* deviceId = "[Device Id]";

#define MESSAGE_COUNT 10
static size_t feedbackCount = 0;

static void openCompleteCallback(void* context)
{
    (void)printf("Open completed, context: %s\n", (char*)context);
}

static void sendCompleteCallback(void* context, IOTHUB_MESSAGING_RESULT messagingResult)
{
    (void)context;
    if (messagingResult == IOTHUB_MESSAGING_OK)
    {
        (void)printf("Message has been sent successfully\n");
    }
    else
    {
        (void)printf("Send failed\n");
    }
    feedbackCount++;
}

static void feedbackReceivedCallback(void* context, IOTHUB_SERVICE_FEEDBACK_BATCH* feedbackBatch)
{
    printf("Feedback received, context: %s\n", (char*)context);

    if (feedbackBatch != NULL)
    {
        (void)printf("Batch userId       : %s\r\n", feedbackBatch->userId);
        (void)printf("Batch lockToken    : %s\r\n", feedbackBatch->lockToken);
        if (feedbackBatch->feedbackRecordList != NULL)
        {
            LIST_ITEM_HANDLE feedbackRecord = singlylinkedlist_get_head_item(feedbackBatch->feedbackRecordList);
            while (feedbackRecord != NULL)
            {
                IOTHUB_SERVICE_FEEDBACK_RECORD* feedback = (IOTHUB_SERVICE_FEEDBACK_RECORD*)singlylinkedlist_item_get_value(feedbackRecord);
                if (feedback != NULL)
                {
                    (void)printf("Feedback\r\n");
                    (void)printf("    deviceId        : %s\r\n", feedback->deviceId);
                    (void)printf("    generationId    : %s\r\n", feedback->generationId);
                    (void)printf("    correlationId   : %s\r\n", feedback->correlationId);
                    (void)printf("    description     : %s\r\n", feedback->description);
                    (void)printf("    enqueuedTimeUtc : %s\r\n", feedback->enqueuedTimeUtc);

                    feedbackRecord = singlylinkedlist_get_next_item(feedbackRecord);
                }
            }
        }
    }
}

static IOTHUB_SERVICE_CLIENT_AUTH_HANDLE iotHubServiceClientHandle;
static IOTHUB_MESSAGING_HANDLE iotHubMessagingHandle;
static IOTHUB_MESSAGING_RESULT iotHubMessagingResult;

void iothub_messaging_ll_sample_run(void)
{
    xlogging_set_log_function(consolelogger_log);

    if (platform_init() != 0)
    {
        (void)printf("Failed to initialize the platform.\r\n");
    }
    else
    {
        (void)printf("Calling IoTHubServiceClientAuth_CreateFromConnectionString with connection string\n");
        iotHubServiceClientHandle = IoTHubServiceClientAuth_CreateFromConnectionString(connectionString);
        if (iotHubServiceClientHandle == NULL)
        {
            (void)printf("IoTHubServiceClientAuth_CreateFromConnectionString failed\n");
        }
        else
        {
            (void)printf("Service Client Authentication handle has been created successfully\n");

            (void)printf("Creating Messaging...\n");
            iotHubMessagingHandle = IoTHubMessaging_LL_Create(iotHubServiceClientHandle);
            if (iotHubMessagingHandle == NULL)
            {
                (void)printf("IoTHubMessaging_LL_Create failed\n");
            }
            else
            {
                (void)printf("Messaging has been created successfully\n");
                (void)printf("Opening Messaging...\n");

                iotHubMessagingResult = IoTHubMessaging_LL_SetFeedbackMessageCallback(iotHubMessagingHandle, feedbackReceivedCallback, "Context string for feedback");
                if (iotHubMessagingResult != IOTHUB_MESSAGING_OK)
                {
                    (void)printf("IoTHubMessaging_LL_SetFeedbackMessageCallback failed\n");
                }
                else
                {
                    iotHubMessagingResult = IoTHubMessaging_LL_Open(iotHubMessagingHandle, openCompleteCallback, "Context string for open");
                    if (iotHubMessagingResult != IOTHUB_MESSAGING_OK)
                    {
                        (void)printf("IoTHubMessaging_LL_Open failed\n");
                    }
                    else
                    {
                        for (int i = 0; i < MESSAGE_COUNT; i++)
                        {
                            double avgWindSpeed = 10.0;
                            static char msgText[1024];
                            sprintf_s(msgText, sizeof(msgText), "{\"deviceId\":%s,\"windSpeed\":%.2f, \"i\":%d}", deviceId, avgWindSpeed + (rand() % 4 + 2), i);
                            IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText));
                            if (messageHandle == NULL)
                            {
                                (void)printf("IoTHubMessage_CreateFromByteArray failed\n");
                                break;
                            }
                            else
                            {
                                IOTHUB_MESSAGE_RESULT iotHubMessageResult;
                                const char* MSG_ID = "Sample_MessageId";
                                const char* MSG_CORRELATION_ID = "Sample_MessageCorrelationId";
                                const char* MSG_PROP_KEYS[3] = { "Sample_Key1", "Sample_Key2", "Sample_Key3" };
                                const char* MSG_PROP_VALS[3] = { "Sample_Val1", "Sample_Val2", "Sample_Val3" };

                                iotHubMessageResult = IoTHubMessage_SetMessageId(messageHandle, MSG_ID);
                                if (iotHubMessageResult != IOTHUB_MESSAGE_OK)
                                {
                                    (void)printf("IoTHubMessage_SetMessageId failed. Exiting...\n");
                                    IoTHubMessage_Destroy(messageHandle);
                                    break;
                                }
                                else
                                {
                                    iotHubMessageResult = IoTHubMessage_SetCorrelationId(messageHandle, MSG_CORRELATION_ID);
                                    if (iotHubMessageResult != IOTHUB_MESSAGE_OK)
                                    {
                                        (void)printf("IoTHubMessage_SetCorrelationId failed. Exiting...\n");
                                        IoTHubMessage_Destroy(messageHandle);
                                        break;
                                    }
                                    else
                                    {
                                        MAP_HANDLE mapHandle = IoTHubMessage_Properties(messageHandle);
                                        for (size_t j = 0; j < 3; j++)
                                        {
                                            if (Map_AddOrUpdate(mapHandle, MSG_PROP_KEYS[j], MSG_PROP_VALS[j]) != MAP_OK)
                                            {
                                                (void)printf("ERROR: Map_AddOrUpdate failed for property %zu!\r\n", j);
                                            }
                                        }

                                        iotHubMessagingResult = IoTHubMessaging_LL_Send(iotHubMessagingHandle, deviceId, messageHandle, sendCompleteCallback, NULL);
                                        if (iotHubMessagingResult != IOTHUB_MESSAGING_OK)
                                        {
                                            (void)printf("IoTHubMessaging_LL_Send failed\n");
                                        }
                                        else
                                        {
                                            (void)printf("IoTHubMessaging_LL_Send accepted data for transmission to IoT Hub.\r\n");
                                        }
                                    }
                                }
                            }
                            IoTHubMessage_Destroy(messageHandle);
                        }

                        feedbackCount = 0;
                        while (feedbackCount < MESSAGE_COUNT)
                        {
                            IoTHubMessaging_LL_DoWork(iotHubMessagingHandle);
                            ThreadAPI_Sleep(1);
                        }

                        /* Wait for user to press a key. */
                        (void)printf("Press any key to exit the application. \r\n");
                        (void)getchar();

                        IoTHubMessaging_LL_Close(iotHubMessagingHandle);
                    }
                }
                (void)printf("Calling IoTHubMessaging_LL_Destroy...\n");
                IoTHubMessaging_LL_Destroy(iotHubMessagingHandle);
            }
            (void)printf("Calling IoTHubServiceClientAuth_Destroy...\n");
            IoTHubServiceClientAuth_Destroy(iotHubServiceClientHandle);
        }
        platform_deinit();
    }
}