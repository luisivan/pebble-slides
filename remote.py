from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import socket
import urlparse
from pykeyboard import PyKeyboard

k = PyKeyboard()

class HTTPRequestHandler(BaseHTTPRequestHandler):

    def do_GET(self):

        path = urlparse.urlparse(self.path).path
        print(path)
        if path == '/down':
            k.tap_key(k.right_key)
        elif path == '/up':
            k.tap_key(k.left_key)
        self.send_response(200)

def run():
    ip = socket.gethostbyname(socket.gethostname())
    print('Pebble Slides started. Your IP is ' + ip)

    server_address = (ip, 8686)
    httpd = HTTPServer(server_address, HTTPRequestHandler)
    httpd.serve_forever()

if __name__ == '__main__':
    run()