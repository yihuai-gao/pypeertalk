import pypeertalk
import time
import cv2
import numpy as np
devices = pypeertalk.get_connected_devices()
print(devices)

if len(devices) > 0:
    client = pypeertalk.PeerTalkClient(devices[0], 2345)
    last_timestamp = time.time()
    while True:
        
        message = client.get_latest_message(1000)
        current_timestamp = time.time()
        # raw_data = np.frombuffer(message, dtype=np.uint8).reshape(720, -1)
        # yuv_data = raw_data[:, :3*1280].reshape(720, 1280, 3)
        # bgr_data = cv2.cvtColor(yuv_data, cv2.COLOR_YUV2BGR)
        # cv2.imshow("image", bgr_data)
        # cv2.waitKey(1)
        print(f"Time interval: {current_timestamp - last_timestamp:.3f}s, fps: {1 / (current_timestamp - last_timestamp):.3f}, message size: {len(message)}")
        last_timestamp = current_timestamp
        # time.sleep(1)