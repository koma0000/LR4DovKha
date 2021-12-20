#pragma once
#include "pch.h"
#include "Message.h"

class Session
{
private:
	// ������������� ������ (�������� ��������)
	int session_id;
	// �������  ���������
	queue<Message> messages;
	// ����������� ������ ��� ������ ������� �� �������������� �������������
	CRITICAL_SECTION criricalSection;
	// ��������� ����� ���������� �������
	clock_t  time;
public:
	// ����������� �� ���������
	Session() {
		session_id = 0;
		InitializeCriticalSection(&criricalSection);
	}

	// ����������� � �����������
	Session(int ID, clock_t t)
		:session_id(ID), time(t) {
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

	// ������ ������� ��������� ����������
	clock_t getTime() {
		return time;
	}


	// ������ ������� ��������� ����������
	void setTime(clock_t t) {
		time = t;
	}

	// ���������� ��������� � �������
	void Add(Message& m) {
		EnterCriticalSection(&criricalSection);

		messages.push(m);

		LeaveCriticalSection(&criricalSection);
	}

	// �������� ���������
	void send(CSocket& s) {
		EnterCriticalSection(&criricalSection);

		if (messages.empty())
		{
			Message::send(s, session_id, BROKER, NODATA);
		}
		else
		{
			messages.front().send(s);
			messages.pop();
		}

		LeaveCriticalSection(&criricalSection);
	}
};

