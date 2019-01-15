/*
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file aws_iot_large_object_transfer.h
 * @brief The file provides an interface for large object transfer protocol. Large Object transfer is used
 * to send larger payloads, ( greater than link MTU size) directly to a peer.
 */


#ifndef AWS_IOT_LARGE_OBJECT_TRANSFER_H_
#define AWS_IOT_LARGE_OBJECT_TRANSFER_H_

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Events supported within a large object transfer session.
 */
typedef enum AwsIotLargeObjectTransferEvent
{
    AWS_IOT_LARGE_OBJECT_TRANSFER_SEND_COMPLETE = 0,  //!< AWS_IOT_LARGE_OBJECT_TRANSFER_COMPLETE  Event indicates the completion of a large object send session.
    AWS_IOT_LARGE_OBJECT_TRANSFER_SEND_FAILED,        //!< AWS_IOT_LARGE_OBJECT_TRANSFER_SEND_FAILED Event indicates a send session has failed and cannot be resumed.
    AWS_IOT_LARGE_OBJECT_TRANSFER_SEND_TIMEDOUT,      //!< AWS_IOT_LARGE_OBJECT_TRANSFER_SEND_TIMEDOUT Event indicates a send session is timedout.
    AWS_IOT_LARGE_OBJECT_TRANSFER_RECEIVE,            //!< AWS_IOT_LARGE_OBJECT_TRANSFER_RECEIVE  Event indicates that a new large object is received
    AWS_IOT_LARGE_OBJECT_TRANSFER_RECV_FAILED,        //!< AWS_IOT_LARGE_OBJECT_TRANSFER_RECV_FAILED Event indicates a receive session has failed or aborted by peer.

} AwsIotLargeObjectTransferEvent_t;

/**
 * @brief Error types returned by the large object transfer APIs.
 */
typedef enum AwsIotLargeObjectTransferError
{
    AWS_IOT_LARGE_OBJECT_TRANSFER_SUCCESS = 0,          //!< AWS_IOT_LARGE_OBJECT_TRANSFER_SUCCESS API is successful.
    AWS_IOT_LARGE_OBJECT_TRANSFER_INVALID_PARAM,        //!< AWS_IOT_LARGE_OBJECT_TRANSFER_INVALID_PARAM API failed due to an invalid param as input.
    AWS_IOT_LARGE_OBJECT_TRANSFER_SESSION_IN_PROGRESS,  //!< AWS_IOT_LARGE_OBJECT_TRANSFER_SESSION_IN_PROGRESS  API failed as there is already a session in progress.
    AWS_IOT_LARGE_OBJECT_TRANSFER_SESSION_NOT_FOUND,
    AWS_IOT_LARGE_OBJECT_TRANSFER_NO_MEMORY,            //!< AWS_IOT_LARGE_OBJECT_TRANSFER_NO_MEMORY API failed as there is no enough memory for allocation.
    AWS_IOT_LARGE_OBJECT_TRANSFER_BUFFER_TOO_SMALL,     //!< AWS_IOT_LARGE_OBJECT_TRANSFER_BUFFER_TOO_SMALL API failed as the input buffer provided by the user was too small.
    AWS_IOT_LARGE_OBJECT_TRANSFER_NETWORK_ERROR,        //!< AWS_IOT_LARGE_OBJECT_TRANSFER_NETWORK_ERROR Large Object transfer failed due to network error.
    AWS_IOT_LARGE_OBJECT_TRANSFER_INTERNAL_ERROR        //!< AWS_IOT_LARGE_OBJECT_TRANSFER_INTERNAL_ERROR API failed due to an internal error.
} AwsIotLargeObjectTransferError_t ;



/**
 * @brief Maximum window size supported for large object transfer.
 */
#define largeObjTransferMAX_WINDOW_SIZE                  ( 32768 )

/**
 * @brief Keys for control message for large object transfer.
 */
#define largeObjTransferMESSAGE_TYPE_KEY        "m"
#define largeObjTransferSESSION_IDENTIFIER_KEY  "i"
#define largeObjTransferSIZE_KEY                "s"
#define largeObjTransferBLOCK_SIZE_KEY          "b"
#define largeObjTransferWINDOW_SIZE_KEY         "w"
#define largeObjTransferTIMEOUT_KEY             "t"
#define largeObjTransferNUM_RERTANS_KEY         "r"
#define largeObjTransferSESSION_EXPIRY_KEY      "x"

/**
 * @brief Message types exchanged for large object transfer.
 */
#define largeObjTransferMESSAGE_TYPE_START   ( 0x1 )
#define largeObjTransferMESSAGE_TYPE_ABORT   ( 0x2 )
#define largeObjTransferMESSAGE_TYPE_RESUME  ( 0x3 )
#define largeObjTransferMESSAGE_TYPE_UPDATE  ( 0x4 )
#define largeObjTransferMESSAGE_TYPE_ACK     ( 0x5 )


/**
 * @brief Number of parameters for START Message.
 */
#define largeObjTransferNUM_START_MESSAGE_PARAMS                  ( 8 )

/**
 * @brief Parameters used for large object transfer.
 */
typedef struct AwsIotLargeObjectTransferParams
{
    uint32_t objectSize;                 //!< objectSize Size of the large object.
    uint16_t blockSize;                  //!< blockSize Size of each block for the transfer.
    uint16_t windowSize;                 //!< windowSize Number of blocks which can be transferred at once without receiving an acknowledgement.
    uint16_t timeoutMilliseconds;        //!< timeoutMilliseconds Timeout in milliseconds for one window of transfer.
    uint16_t numRetransmissions;         //!< numRetransmissions Number of retransmissions.
    uint32_t sessionExpiryMilliseconds;  //!< sessionTimeout Session timeout in milliseconds.
} AwsIotLargeObjectTransferParams_t;

/**
 * @brief Callback used to receive bytes from a physical network.
 *
 */
typedef int32_t ( * AwsIotLargeObjectNetworkReceiveCallback_t )(
        void * pvRecvContext,
        const void * pvReceivedData,
        size_t xOffset,
        size_t xDataLength,
        void ( *freeReceivedData )( void * )
);

/**
 * @brief Structure which wraps the underlying network stack for the large object transfer.
 * Structure contains the open network connection used to send/receive data, the function pointers to set
 * a receive callback and send data over the network connection.
 */
typedef struct AwsIotLargeObjectNetworkIface
{
    /** Pointer to network Connection **/
    void * pvNetworkConnection;

    /** Function pointer to send data over a network connection **/
    size_t ( * send )( void *pvNetworkConnection, const void * const pvMessage , size_t xLength );

    /** Function pointer to set the network receive callback **/
    int32_t ( * set_receive_callback )(
            void* pvNetworkConnection,
            void* pvRecvContext,
            AwsIotLargeObjectNetworkReceiveCallback_t xNetworkReceiveCb );

} AwsIotLargeObjectNetworkIface_t;

/**
 * @brief Type represents unique handle to a large object transfer session.
 */
typedef void * AwsIotLargeObjectTransferSession_t;

/**
 * @brief Callback used to receive events from large object transfer.
 */
typedef void ( *AwsIotLargeObjectTransferEventCallback_t ) ( AwsIotLargeObjectTransferSession_t xSession, AwsIotLargeObjectTransferEvent_t xEvent );

/**
 * @brief Callback invoked for each of the blocks of large object received.
 * Callback will be invoked multiple
 */
typedef void ( *AwsIotLargeObjectReceiveCallback_t ) (
        AwsIotLargeObjectTransferSession_t xSession,
        size_t offset,
        const uint8_t *pBlock,
        size_t blockLength );

/**
 * @brief Creates a new large object transfer session and saves the handle to the new session.
 *
 */
AwsIotLargeObjectTransferError_t AwsIotLargeObjectTransfer_CreateSession(
        AwsIotLargeObjectNetworkIface_t* pxNetworkInterface,
        AwsIotLargeObjectTransferEventCallback_t xCallback,
        AwsIotLargeObjectTransferSession_t* pxSession );

/**
 * @brief Initiates sending a large object to a peer.
 * Initiates transfer of a large object by sending START message to the peer.
 */
AwsIotLargeObjectTransferError_t AwsIotLargeObjectTransfer_Send(
        AwsIotLargeObjectTransferSession_t xSession,
        const uint8_t* pucObject,
        AwsIotLargeObjectTransferParams_t *pxParams );

/**
 * @brief API invoked by the application to indicate its ready to receive a large object.
 * Sends back an ACK message to the peer to start receiving the blocks. Each block received is
 * handled by the application using the receive callback.
 * This function should be called by the application only in response to a AWS_IOT_LARGE_OBJECT_TRANSFER_RECEIVE event notification.
 */
AwsIotLargeObjectTransferError_t AwsIotLargeObjectTransfer_Receive(
        AwsIotLargeObjectTransferSession_t xSession,
        AwsIotLargeObjectReceiveCallback_t xReceiveCb,
        AwsIotLargeObjectTransferParams_t *pParams );

/**
 * @brief Resumes a large object transfer session.
 * Only Sender can resume a previously timedout session. Failed or Aborted sessions cannot be resumed.
 *
 */
AwsIotLargeObjectTransferError_t AwsIotLargeObjectTransfer_ResumeSession( AwsIotLargeObjectTransferSession_t xSession );

/**
 * @brief Aborts a large object transfer session.
 * Aborts an ongoing large object transfer session. Both receiver and sender can abort a large object transfer sesssion.
 */
AwsIotLargeObjectTransferError_t AwsIotLargeObjectTransfer_AbortSession( AwsIotLargeObjectTransferSession_t xSession );

/**
 * @brief Destroys the handle to a Large Object Transfer session.
 * Frees the resources associated with the session. Session should be aborted, completed or failed state.
 */
AwsIotLargeObjectTransferError_t AwsIotLargeObjectTransfer_DestroySession( AwsIotLargeObjectTransferSession_t xSession );

/**
 * @brief Sets the parameters for an ongoing large object transfer session.
 *
 * The function is invoked to change the parameters for a large object transfer session.
 * Function sets the metadata locally and returns immediately.
 * The final negotiated metadata will be returned back in the AWS_IOT_LARGE_OBJECT_TRANSFER_METADATA_CHANGED event callback.
 */
AwsIotLargeObjectTransferError_t AwsIotLargeObjectTransfer_SetSessionParams(
        AwsIotLargeObjectTransferSession_t xSession,
        AwsIotLargeObjectTransferParams_t* pParams );

#endif /* AWS_IOT_LARGE_OBJECT_TRANSFER_H_ */
