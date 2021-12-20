from threading import Lock

mutex = Lock();

def safe_print(text):
    mutex.acquire()
    print(text);
    mutex.release();


def connect(s):
    try:
        s.connect(('localhost', 12345))
    except ConnectionRefusedError:
        safe_print('Connection error')
        return False
    return True