#!/usr/bin/env python

import sys
import socket
import netifaces
from wsgiref.simple_server import make_server
from ws4py.websocket import WebSocket
from ws4py.server.wsgirefserver import WSGIServer, WebSocketWSGIRequestHandler
from ws4py.server.wsgiutils import WebSocketWSGIApplication

# return true if Mac
def isMac():
	return sys.platform == 'darwin'

# If we can't get the IP address by hostname (as observed on a Mac), then try by interface
# Thanks to Gabriel Samfira (http://stackoverflow.com/questions/11735821/python-get-localhost-ip)
def get_lan_ip():
	ip = None
	try:
		ip = socket.gethostbyname(socket.gethostname())

	except socket.gaierror:
		interfaces = netifaces.interfaces()
		for i in interfaces:
			iface = netifaces.ifaddresses(i).get(netifaces.AF_INET)
			if iface != None:
				for j in iface:
					if j['addr'] != '127.0.0.1':
						ip = j['addr']
						break
	return ip

# PyKeyboard does not work on Mac
if isMac():
	import os
else:
	from pykeyboard import PyKeyboard
	k = PyKeyboard()

class PebbleWebSocket(WebSocket):

	def received_message(self, message):

		# Mac
		if isMac():
			if message.data == 'down':
				cmd = "osascript -e 'tell application \"System Events\" to keystroke (ASCII character 29)'"
				os.system(cmd)
			elif message.data == 'up':
				cmd = "osascript -e 'tell application \"System Events\" to keystroke (ASCII character 28)'"
				os.system(cmd)

		# Window, Linux
		else:
			if message.data == 'down':
				k.tap_key(k.right_key)
			elif message.data == 'up':
				k.tap_key(k.left_key)

# either take a port from arguments or serve on random port
port = 0
if len(sys.argv) == 2:
	port = int(sys.argv[1])
elif len(sys.argv) != 1:
	print("usage: %s [port]" % sys.argv[0])
	sys.exit()

server = make_server('', port, server_class=WSGIServer,
					 handler_class=WebSocketWSGIRequestHandler,
					 app=WebSocketWSGIApplication(handler_cls=PebbleWebSocket))

ip = get_lan_ip()

if ip is not None:
	print('Pebble Slides started. Your address is ' + ip + ':' + str(server.server_port))
else:
	print('Pebble Slides started on port ' + str(server.server_port) + ' (Please look up your IP address)')

server.initialize_websockets_manager()

try:
	server.serve_forever()
except KeyboardInterrupt:
	server.server_close()
