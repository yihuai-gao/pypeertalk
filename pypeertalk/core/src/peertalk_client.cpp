#include <stdint.h>
#include <stdio.h>
#include <usbmuxd.h>
#include <stdlib.h>

bool connect_to_device(const DeviceInfo &device)
{
    std::lock_guard<std::mutex> guard{ apiCallsMutex_ };

    // Do not reconnect if we are already streaming.
    if ( connectionEstablished_.load())
    { return false; }

    // Ensure we are indeed connected before continuing.
    auto socketNo = usbmuxd_connect( $device.handle, DEVICE_PORT );
    if ( socketNo < 0 )
    { return false; }

    // We are successfully connected, start runloop.
    connectionEstablished_.store( true );
    socketHandle_ = socketNo;

    // Create thread that is going to execute runloop.
    runloopThread_ = std::thread( [&]
                                    {
                                        StreamProcessingRunloop();
                                    } );
    runloopThread_.detach();
    return true;
}

uint32_t receive_whole_buffer(int socket_handle, uint8_t* output_buffer, uint32_t num_bytes_to_read)
{
    {
        uint32_t numTotalReceivedBytes = 0;
        while ( numTotalReceivedBytes < num_bytes_to_read )
        {
            uint32_t numRestBytes = num_bytes_to_read - numTotalReceivedBytes;
            uint32_t numActuallyReceivedBytes = 0;
            if ( 0 != usbmuxd_recv( socket_handle, (char*) (output_buffer + numTotalReceivedBytes), numRestBytes,
                                    &numActuallyReceivedBytes ))
            {
                return numTotalReceivedBytes;
            }
            numTotalReceivedBytes += numActuallyReceivedBytes;
        }

        return numTotalReceivedBytes;
    }
}