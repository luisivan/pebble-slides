#!/usr/bin/env python
import os
import sys
import socket
from wsgiref.simple_server import make_server
from ws4py.websocket import WebSocket
from ws4py.server.wsgirefserver import WSGIServer, WebSocketWSGIRequestHandler
from ws4py.server.wsgiutils import WebSocketWSGIApplication

class PebbleWebSocket(WebSocket):

	def received_message(self, message):

		if message.data == 'down':
			cmd = "osascript -e 'tell application \"System Events\" to keystroke (ASCII character 29)'"
			os.system(cmd)
		elif message.data == 'up':
			cmd = "osascript -e 'tell application \"System Events\" to keystroke (ASCII character 28)'"
			os.system(cmd)

port = 12345

if ( 1 < len(sys.argv) < 3 and int(sys.argv[1] >= 1024) ):
	port = int(sys.argv[1])
else:
	print("usage: %s <port (1024+)>" % sys.argv[0])
	sys.exit()

server = make_server('', port, server_class=WSGIServer,
					 handler_class=WebSocketWSGIRequestHandler,
					 app=WebSocketWSGIApplication(handler_cls=PebbleWebSocket))

ip = socket.gethostbyname(socket.gethostname())
print('Pebble Slides started. Your address is ' + ip + ':' + str(server.server_port))
server.initialize_websockets_manager()
server.serve_forever()
