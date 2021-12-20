#pragma once
#include "pch.h"

enum Address
{
	BROKER = 0,
	ALL_USER = -1,
	STORAGE = 666,
};

enum MessageTypes {
	INIT,
	EXIT,
	CONFIRM,
	GETDATA,
	TEXT,
	NODATA,
	GETALLDATA,
};

struct MsgHeader
{
	int msgFrom;
	int msgTo;
	int msgType;
	int msgSize;
};

class Message
{
private:
	MsgHeader msgHeader;
	string msgData;
public:
	// Конструктор без параметров
	Message() {
		msgHeader = { 0 };
		msgData = "";
	}

	// Конструктор с парметрами
	Message(int to, int from, int type = TEXT, const string& data = "") {
		msgHeader.msgFrom = from;
		msgHeader.msgTo = to;
		msgHeader.msgType = type;
		msgHeader.msgSize = data.length();
		msgData = data;
	}

	// Геттер заголовка
	MsgHeader getHeader() {
		return msgHeader;
	}

	// Геттер текста сообщения
	string getData() {
		return msgData;
	}

	// Отправка сообщения
	void send(CSocket& s) {
		s.Send(&msgHeader, sizeof(MsgHeader));
		if (msgHeader.msgSize != 0) {
			s.Send(msgData.c_str(), msgHeader.msgSize + 1);
		}
	}


	// Получение сообщения
	int receive(CSocket& s) {
		s.Receive(&msgHeader, sizeof(MsgHeader));
		if (msgHeader.msgSize != 0) {
			char* pBuff = new char[msgHeader.msgSize + 1];

			s.Receive(pBuff, msgHeader.msgSize + 1);
			pBuff[msgHeader.msgSize] = '\0';
			msgData = pBuff;

			// Мы против утечек памяти
			delete[] pBuff;
		}

		return msgHeader.msgType;
	}

	static void send(CSocket& s, int to, int from, int type = TEXT, const string& data = "") {
		Message m(to, from, type, data);
		m.send(s);
	}
};
