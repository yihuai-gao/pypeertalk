#include "peertalk_client.h"

namespace pypeertalk
{

std::vector<DeviceInfo> get_connected_devices()
{
    usbmuxd_device_info_t* device_info_list;
    int num_devices = usbmuxd_get_device_list( &device_info_list );
    std::vector<DeviceInfo> available_devices;

    for ( int dev_idx = 0; dev_idx < num_devices; dev_idx++ )
    {
        const auto &dev = device_info_list[ dev_idx ];
        if ( dev.conn_type != CONNECTION_TYPE_USB ) continue;

        DeviceInfo curr_dev_info(
            dev.product_id,
            std::string( dev.udid ),
            dev.handle
        );

        available_devices.push_back( curr_dev_info );
    }

    usbmuxd_device_list_free( &device_info_list );

    return available_devices;
}

PeerTalkClient::PeerTalkClient(const DeviceInfo &device, int port) : device_(device), port_(port)
{
    while(true)
    {
        if (connect_to_device_())
        {
            break;
        }
        printf("Failed to connect to device %s. Retrying...\n", device_.udid.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

PeerTalkClient::~PeerTalkClient()
{
    is_connected_ = false;
    communication_thread_.join();
    usbmuxd_disconnect(socket_handle_);
    printf("Disconnected from device %s\n", device_.udid.c_str());
}

bool PeerTalkClient::connect_to_device_()
{
    // Ensure we are indeed connected before continuing.
    int socket_no = usbmuxd_connect( device_.handle, port_ );
    if ( socket_no < 0 )
    { return false; }

    // We are successfully connected, start runloop.
    is_connected_ = true;
    socket_handle_ = socket_no;

    // Create thread that is going to execute runloop.
    communication_thread_ = std::thread( &PeerTalkClient::run_communication_thread_, this );
    return true;
}

pybind11::bytes PeerTalkClient::get_latest_message(int timeout_ms)
{
    auto start_time = std::chrono::steady_clock::now();
    while(true)
    {
        if (received_new_message_.load())
        {
            {
                std::lock_guard<std::mutex> guard(receiving_message_mutex_);
                received_new_message_.store(false);
                pybind11::bytes message_bytes(reinterpret_cast<char *>(message_buffer_.data()), message_buffer_.size());
                message_buffer_.clear();
                return message_bytes;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (std::chrono::steady_clock::now() - start_time > std::chrono::milliseconds(timeout_ms))
        {
            return pybind11::bytes();
        }
    }

}

uint32_t PeerTalkClient::receive_whole_buffer_(int socket_handle, uint8_t* output_buffer, uint32_t num_bytes_to_read)
{
    printf("Receiving %d bytes\n", num_bytes_to_read);
    std::lock_guard<std::mutex> guard(receiving_message_mutex_);
    uint32_t num_total_received_bytes = 0;
    while ( num_total_received_bytes < num_bytes_to_read )
    {
        uint32_t num_rest_bytes = num_bytes_to_read - num_total_received_bytes;
        uint32_t num_actually_received_bytes = 0;
        if ( 0 != usbmuxd_recv( socket_handle, (char*) (output_buffer + num_total_received_bytes), num_rest_bytes,
                                &num_actually_received_bytes ))
        {
            return num_total_received_bytes;
        }
        num_total_received_bytes += num_actually_received_bytes;
    }
    return num_total_received_bytes;
}

void PeerTalkClient::run_communication_thread_()
{

    while ( is_connected_ )
    {
        PeerTalkHeader header;
        uint32_t num_bytes_to_read = sizeof(PeerTalkHeader);
        uint8_t* buffer = new uint8_t[num_bytes_to_read];
        receive_whole_buffer_(socket_handle_, buffer, num_bytes_to_read);
        memcpy(&header, buffer, sizeof(PeerTalkHeader));
        delete[] buffer;

        if ( header.body_size > message_buffer_.size() )
        {
            message_buffer_.resize(header.body_size);
        }
        received_new_message_.store(false);
        receive_whole_buffer_(socket_handle_, message_buffer_.data(), header.body_size);
        received_new_message_.store(true);
    }
}

}