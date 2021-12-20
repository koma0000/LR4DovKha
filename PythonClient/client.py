import threading
from dataclasses import dataclass
import socket, struct, time
from msg import *


def ProcessMessages():
	while True:
		m = Message.SendMessage(MR_BROKER, MT_GETDATA)
		if m.Header.Type == MT_DATA:
			print(m.Data)
		else:
			time.sleep(1)


def Client():
	Message.SendMessage(MR_BROKER, MT_INIT)
	t = threading.Thread(target=ProcessMessages)
	t.start()
	while True:
		print('Send message','1.Send All','2.Send only one client','3.Exit')
	choice = input()
	match choice:
		case "1":
			print("Write your message:")
			Message.SendMessage(MR_ALL, MT_DATA, input())
		case "2":
			print("Write users ID:")
			Message.SendMessage(MR_ME,INFO,"")
			selected_ID= input()
			Message.SendMessage(selected_ID, MT_DATA, input())
		case "3":
			print("Application is closing")
			Message.SendMessage(MR_ME,MT_EXIT)
			sys.exit(0)


Client()
