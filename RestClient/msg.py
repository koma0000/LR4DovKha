# -*- coding: utf-8 -*-
#!/usr/bin/python

import threading
from dataclasses import dataclass
import socket, struct, time
from enum import IntEnum

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

@dataclass
class MsgHeader(object):
	To: int = 0
	From: int = 0
	Type: int = 0
	Size: int = 0

	def Send(self, s):
		s.send(struct.pack(f'iiii', self.To, self.From, self.Type, self.Size))

	def Receive(self, s):
		try:
			(self.From, self.To, self.Type, self.Size) = struct.unpack('iiii', s.recv(1280));
		except:
			self.Size = 0
			self.Type = MessageTypes.NODATA

class Message:
	ClientID = 0;
	
	def __init__(self, msgFrom = 0, msgTo = 0, msgType = 0, data = ""):
		self.Header = MsgHeader(msgTo, msgFrom, msgType, len(data));
		self.Data = data;

	def Send(self, s):
		self.Header.Send(s)
		if self.Header.Size > 0:
			s.send(struct.pack(f'{self.Header.Size}s', self.Data.encode('utf-8')))

	def Receive(self, s):
		self.Header.Receive(s)
		if self.Header.Size > 0:
			self.Data = struct.unpack(f'{self.Header.Size}s', s.recv(self.Header.Size))[0].decode('utf-8')



	def SendMessage(To, Type = MessageTypes.TEXT, Data=""):
		HOST = 'localhost'
		PORT = 12345
		with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
			s.connect((HOST, PORT));
			m = Message(To, Message.ClientID, Type, Data);
			m.Send(s)
			m.Receive(s)
			if m.Header.Type == MessageTypes.CONFIRM:
				if (Message.ClientID == 0):
					Message.ClientID = m.Header.To;
			return m