# -*- coding: utf-8 -*-
#!/usr/bin/python

import sys, codecs, cgi, cgitb
from os import environ

from msg import *
import json

cgitb.enable()
sys.stdout = codecs.getwriter('utf-8')(sys.stdout.detach())

class Api:
    def __init__(self, form):
        self.form = form;

def main():
    form = cgi.FieldStorage()
    print(form.headers)

    print("Content-type: application-json; charset=utf-8\n\n");
    result = {'ids': [[environ['REQUEST_METHOD'].upper(), 1], ["two", 2]]}
    print(json.dumps(result));


main();


