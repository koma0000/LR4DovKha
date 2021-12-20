#pragma once
#include "pch.h"
#include "Message.h"

class Session
{
private:
	// ������������� ������ (�������� ��������)
	int session_id;
	// !!! ��� ������, � �� ������� !!!
	// ������  ���������
	vector<Message> messages;
	// ����������� ������ ��� ������ ������� �� �������������� �������������
	CRITICAL_SECTION criricalSection;
public:
	// ����������� �� ���������
	Session() {
		InitializeCriticalSection(&criricalSection);
	}

	// ����������� � �����������
	Session(int ID)
		:session_id(ID) {
		InitializeCriticalSection(&criricalSection);
	}

	// ���������� �������� �� ������������ ����������� ������ 
	~Session() {
		DeleteCriticalSection(&criricalSection);
	}

	// ������ �������������� ������
	int getId() {
		return session_id;
	}

	// ������ �������������� ������
	void setId(int i) {
		session_id = i;
	}
	
	// ���������� ��������� � ���������
	void Add(Message& m) {
		EnterCriticalSection(&criricalSection);

		messages.push_back(m);

		LeaveCriticalSection(&criricalSection);
	}

	// �������� ������ ���������
	void send(CSocket& s) {
		EnterCriticalSection(&criricalSection);

		if (messages.empty())
		{
			Message::send(s, session_id, MIDDLEWARE, NODATA);
		}
		else
		{
			Message::send(s, session_id, STORAGE, TEXT, to_string((int)messages.size()));
			for (Message m : messages)
				Message::send(s, m.getHeader().msgTo, m.getHeader().msgFrom, TEXT, m.getData());
		}

		LeaveCriticalSection(&criricalSection);
	}
};

