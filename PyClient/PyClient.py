
import socket, struct
from enum import IntEnum
from threading import Thread
from time import sleep
from datetime import datetime
import sys

class Addresses(IntEnum):
	BROKER = 0,
	ALL_USER = -1,
	STORAGE = 666,

class MessageTypes(IntEnum):
    INIT = 0,
    EXIT = 1,
    CONFIRM = 2,
    GETDATA = 3,
    TEXT = 4,
    NODATA = 5,
    GETALLDATA = 6,


class MsgHeader():
    def __init__(self, msgFrom=0, msgTo=0, msgType=0, msgSize=0):
        self.msgFrom=msgFrom
        self.msgTo=msgTo
        self.msgType=msgType
        self.msgSize=msgSize
    def HeaderInit(self, header):
        self.msgFrom=header[0]
        self.msgTo=header[1]
        self.msgType=header[2]
        self.msgSize=header[3]

class Message():
    def __init__(self, To=0, From=0, Type=MessageTypes.TEXT, Data=''):
        self.msgHeader=MsgHeader()
        self.msgHeader.msgFrom=From;
        self.msgHeader.msgTo=To;
        self.msgHeader.msgType=Type;
        self.msgHeader.msgSize=int(len(Data))
        self.msgData=Data
    def SendData(self, s):
        s.send(struct.pack('i', self.msgHeader.msgFrom))
        s.send(struct.pack('i', self.msgHeader.msgTo))
        s.send(struct.pack('i', self.msgHeader.msgType))
        s.send(struct.pack('i', self.msgHeader.msgSize))
        if self.msgHeader.msgSize>0:
            # Это «форматированные строковые литералы», f-strings являются строковыми литералами с «f» в начале и фигурные скобки, 
            # содержащие выражения, которые в дальнейшем будут заменены своими значениями.
            # Динамическая подстановка данных
            s.send(struct.pack(f'{self.msgHeader.msgSize}s', self.msgData.encode('utf-8')))
    def ReceiveData(self, s):
        self.msgHeader=MsgHeader()
        try:
            self.msgHeader.HeaderInit(struct.unpack('iiii', s.recv(64)))
            if self.msgHeader.msgSize>0:
                 self.msgData=struct.unpack(f'{self.msgHeader.msgSize}s', s.recv(self.msgHeader.msgSize))
            return self.msgHeader.msgType
        except Exception:
            return 0;
        
def SendMessage(Socket, From, To, Type=MessageTypes.TEXT, Data=''):
    m=Message(From, To, Type, Data)
    m.SendData(Socket)

def Receive(Socket):
    m=Message()
    res=m.ReceiveData(Socket)
    return res

def connect(Socket):
    Socket.connect(('127.0.0.1', 12345))
def disconnect(Socket):
    Socket.close()

def GetData(ID):
    while True:
        clientSock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connect(clientSock)
        SendMessage(clientSock, 0, ID, MessageTypes.GETDATA)
        msg=Message()
        if (msg.ReceiveData(clientSock)==MessageTypes.TEXT):
            print('Сообщение от '+str(msg.msgHeader.msgFrom)+': '+msg.msgData[0].decode('utf-8'))
        disconnect(clientSock)

myID=0

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    connect(s)
    SendMessage(s, 0, 0, MessageTypes.INIT)
    msg=Message()
    if (msg.ReceiveData(s)==MessageTypes.CONFIRM):
        myID=msg.msgHeader.msgTo
        print("Клиент запущен\n");
        print('ID: '+str(myID))
        t=Thread(target=GetData, args=(myID,), daemon=True)
        t.start()      
        disconnect(s)
    else:
        print('Ошибка')
        sys.exit()
while True:
    # domain - AF_INET для сетевого протокола IPv4
    # type - SOCK_STREAM (надёжная потокоориентированная служба (сервис) или потоковый сокет)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        choice=int(input('[1] Отправить сообщения\n[2] Получить все сообщения\n[3] Выйти\n'))
        if choice==1:
            choice2=int(input('[1] Только для одного клиента\n[2] Для всех клиентов\n'))
            if choice2==1:
                ClientID=int(input('\nВведите ID клиента\n'))
            else:
                ClientID=Addresses.ALL_USER
            message=input('\nВведите сообщение\n')
            connect(s)
            SendMessage(s, ClientID, myID, MessageTypes.TEXT, message)
            if msg.ReceiveData(s)==MessageTypes.CONFIRM:
                print('\nУспешно\n')
            else: print('\nОшибка\n')
            disconnect(s)
        elif choice==2:
            connect(s)

            SendMessage(s, myID, myID, MessageTypes.GETALLDATA);
            msg.ReceiveData(s)

            if msg.ReceiveData(s)==MessageTypes.NODATA:
                print("\nАрхив пуст\n");
            else:
                messagesLen = int(msg.msgData[0].decode('utf-8'))

                print("\nИстория сообщений:\n");
                for number in range(messagesLen):
                    msg.ReceiveData(s)
                    if ((msg.msgHeader.msgTo == myID) and (msg.msgHeader.msgTo!=0)):
                        print("Сообщение от клиента ", str(msg.msgHeader.msgFrom), " : ", msg.msgData[number].decode('ascii'));
                    elif msg.msgHeader.msgTo!=0:
                        print("Мой ответ клиенту ", str(msg.msgHeader.msgTo), " : ",msg.msgData[number].decode('ascii'));

            disconnect(s)
        elif choice==3:
           connect(s)
           SendMessage(s,myID, 0, MessageTypes.EXIT)
           if msg.ReceiveData(s)==MessageTypes.CONFIRM:
              print('\nУспешно\n')
           else: print('\nОшибка\n')
           disconnect(s)
           sys.exit(0)
        else:
            print('Пожалуйста, введите 1 или 2')




