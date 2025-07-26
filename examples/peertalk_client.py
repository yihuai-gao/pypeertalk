import pypeertalk
import time

devices = pypeertalk.get_connected_devices()
print(devices)

if len(devices) > 0:
    client = pypeertalk.PeerTalkClient(devices[0], 2345)

    while True:
        message = client.get_latest_message(1000)
        print(message)
        time.sleep(1)