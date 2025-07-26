import pypeertalk
import time

devices = pypeertalk.get_connected_devices()
print(devices)

if len(devices) > 0:
    client = pypeertalk.PeerTalkClient(devices[0], 2345)
    last_timestamp = time.time()
    while True:
        
        # message = client.get_latest_message(1000)
        # current_timestamp = time.time()
        # print(f"Time interval: {current_timestamp - last_timestamp:.3f}s, fps: {1 / (current_timestamp - last_timestamp):.3f}, message size: {len(message)}")
        # last_timestamp = current_timestamp
        # print(len(message))
        time.sleep(1)