# -*- coding: utf-8 -*-
#!/usr/bin/python

import os, sys, re, codecs, binascii, cgi, cgitb, datetime, pickle
import http.cookies
import json
from time import sleep
from msg import *

cgitb.enable()
sys.stdout = codecs.getwriter('utf-8')(sys.stdout.detach())

def PrintJson(json_data):
    print("Content-type: application-json; charset=utf-8\n\n")
    print(json.dumps(json_data))


class CGI_Client:
    def __init__(self, form):
        self.form = form

    def MsgInit(self):
        Message.SendMessage(Addresses.BROKER, MessageTypes.INIT);
        json_data = {'result': Message.ClientID};
        PrintJson(json_data);

    def MsgPost(self):
        c_id = int(self.form.getvalue('id'));
        message = self.form.getvalue('message');
        print(c_id);
        Message.SendMessage(c_id, MessageTypes.TEXT, message);
        json_data = {'result': True};
        PrintJson(json_data);

    def MsgGet(self):
        m = Message.SendMessage(Addresses.BROKER, MessageTypes.GETDATA);
        if m.Header.Type != MessageTypes.NODATA:
            json_data = {
                'result': {
                    'id': m.Header.From,
                    'message': m.Data
                }
            };
        else:
            json_data = {'result': None};
        PrintJson(json_data);

    def MsgGetAll(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            messages = [];
            s.connect(('localhost', 12345));
            m = Message(Message.ClientID,Message.ClientID, MessageTypes.GETALLDATA)
            m.Send(s)
            m.Receive(s)
            if (m.Header.Type != MessageTypes.NODATA):
               splited_data = m.Data.split('#');
               i=0;
               while i < len(splited_data)-1:
                    messages.append((splited_data[i+1],splited_data[i],splited_data[i+2]))
                    i+=3
        json_data = {'result': messages};
        PrintJson(json_data);

    def MsgExit(self):
        Message.SendMessage(Addresses.BROKER, MessageTypes.EXIT);
        json_data = {'result': True};
        PrintJson(json_data);



def main():
    form = cgi.FieldStorage();
    if (form.getvalue('ClientID') != None):
        Message.ClientID = int(form.getvalue('ClientID'))
    client = CGI_Client(form);

    MENU = {
        'init': client.MsgInit,
        'exit': client.MsgExit,
        'post': client.MsgPost,
        'get': client.MsgGet,
        'getAll': client.MsgGetAll
    }

    try:
        MENU[form.getvalue('type')]()
    except Exception as e:
        print(e);
        pass

try:
    main();
except Exception as e:
    print(e);
    print("Error in main");