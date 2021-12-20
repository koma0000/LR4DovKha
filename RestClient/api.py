from pip._vendor import requests
import json
from msg import *


def DoRequest(method, cmd="", data=""):
    try:
        url = 'http://localhost:8080/cgi-bin/CGI_Client.py'
        header = {"Content-type": 'application-json'}
        res = method(url + cmd, headers=header, data=json.dumps(data))
        if res.status_code == 200:
            if (res.content.decode('utf-8') == ''):
                return json.loads(json.dumps({"result": "null"}));
            else:
                return json.loads(res.content.decode('utf-8'));
    except Exception as ex:
        print(res.content.decode('utf-8') == '');
        print(ex);


def TransformToCmd(query_params):
    cmd = '?'
    i = 0
    for [key, value] in query_params.items():
        cmd += key + '=' + value
        i += 1
        if len(query_params.items()) != i:
            cmd += '&'
    return cmd


class Api:
    ClientID = 0

    def __init__(self):
        pass

    def Get(self):
        query_params = {
            'type': 'get',
            'ClientID': str(self.ClientID)
        }
        return DoRequest(requests.get, TransformToCmd(query_params))

    def GetAll(self):
        query_params = {
            'type': 'getAll',
            'ClientID': str(self.ClientID)
        }
        return DoRequest(requests.get, TransformToCmd(query_params))

    def Post(self, c_id=0, message=""):
        query_params = {
            'type': 'post',
            'ClientID': str(self.ClientID),
            'id': str(c_id),
            'message': message
        }
        return DoRequest(requests.get, TransformToCmd(query_params))

    def Init(self):
        query_params = {
            'type': 'init'
        }
        res = DoRequest(requests.get, TransformToCmd(query_params))
        self.ClientID = res['result']

    def Exit(self):
        query_params = {
            'type': 'exit'
        }
        return DoRequest(requests.get, TransformToCmd(query_params))