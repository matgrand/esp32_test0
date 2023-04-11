# script to read from server at ip address

LOCAL_IP = '192.168.1.23' # 192.168.1.70
TIMEOUT_TIME = 1.0 #seconds

from time import time, sleep
from urllib.request import urlopen

def read_from_server():
    try:
        # read from server
        # open url
        with urlopen(f'http://{LOCAL_IP}/', timeout=TIMEOUT_TIME) as f:
            # read data
            data = f.read()
            #keep only last 20 characters
            data = data[460:463]
            #convert to string
            data = data.decode('utf-8')
        return data
    except:
        return None

if __name__ == '__main__':
    while True:
        # read from server
        # data = read_from_server()
        data = read_from_server()

        # print data
        print(data)

        #sleep for 1 second
        sleep(.5)

