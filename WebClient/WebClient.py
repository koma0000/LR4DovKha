import os, sys, threading, time
from http.server import HTTPServer, CGIHTTPRequestHandler

server_address = ("", 8080)



def main():
	print('Tht web-application is located at http://localhost:%s/cgi-bin/CSG_Client.py' % ( server_address[1]))
	httpd = HTTPServer(server_address, CGIHTTPRequestHandler)#В Python уже есть встроенный CGI сервер
	httpd.serve_forever() #HTTPServer(('', port), CGIHTTPRequestHandler).serve_forever() - более короткая запись через 1 строку

try:
	main()
except:
	print("Error in Main")
