from msg import *
from utils import *
import threading
import time
from api import *
import sys

server_api = Api();


def listenServer():
    while True:
        m = server_api.Get();
        if m['result'] != None:
            safe_print(f'Сообщение от {m["result"]["id"]}:\n{m["result"]["message"]}');
        time.sleep(2);


def connect():
    server_api.Init();
    safe_print(f'Клиент подключен');
    safe_print(f'ID: {server_api.ClientID}');
    messages_thread = threading.Thread(target=listenServer, daemon=True);
    messages_thread.start();

def GetRightNumber(Min,Max):
    answer = input();
    k=True;
    while(k):
            if not (str(answer).isdigit()):
                safe_print('Введите число!');
                answer = int(input());
            elif(int(answer)>Max) or (int(answer)<Min):
                safe_print(f'Введите число повторно от {Min} до {Max}:');
                answer = int(input());
            else:
                k=False;
    return int(answer)

def process():
    connect();
    while (True):
        safe_print('[1] Отправить сообщения\n[2] Получить все сообщения\n[3] Выйти\n');
        answer = GetRightNumber(1,3);

        if answer == 1:
            safe_print('[1] Только для одного клиента\n[2] Для всех клиентов\n');

            answer_2 = GetRightNumber(1,2);

            if answer_2 == 1:
                safe_print('\nВведите ID клиента\n ');
                c_id = int(input());
                
                safe_print('\nВведите сообщение\n ')
                message = input();
                
                server_api.Post(c_id, message);
                safe_print('\nУспешно\n');

                continue

            if answer_2 == 2:
                safe_print('\nВведите сообщение\n')
                message = input();
                
                server_api.Post(int(Addresses.ALL_USER), message);
                safe_print('\nУспешно\n');

                continue
            continue
        if answer == 2:
            print('История сообщений')
            messages = server_api.GetAll()['result'];
            for m in messages:
                if (m[1] != (int)(MessageTypes.CONFIRM) and m[1] != (int)(MessageTypes.NODATA)):
                    if(int(m[0])!=server_api.ClientID):
                        safe_print(f'Сообщение от клиента {m[0]}:\n{m[2]}');
                    else:  
                        safe_print(f'Сообщение для клиента {m[1]}:\n{m[2]}');
            continue
        if answer == 3:
            server_api.Exit();
            safe_print('Сессия отключена\n\n');
            return
        safe_print('Введите 1-3 \n\n');


if __name__ == '__main__':
    process();