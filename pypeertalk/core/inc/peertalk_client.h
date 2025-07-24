#ifndef PEERTALK_CLIENT_H
#define PEERTALK_CLIENT_H

#include <stdint.h>
#include <stdio.h>
#include <usbmuxd.h>
#include <stdlib.h>

class PeerTalkClient
{
    public:
        PeerTalkClient();
        ~PeerTalkClient();

        bool connect_to_device(const DeviceInfo &device);
        uint32_t receive_whole_buffer(int socket_handle, uint8_t* output_buffer, uint32_t num_bytes_to_read);

    private:
}

#endif