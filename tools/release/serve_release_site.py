import argparse
import functools
import http.server
import os
import socket
import socketserver


IGNORED_CLIENT_ERRORS = (
    BrokenPipeError,
    ConnectionAbortedError,
    ConnectionResetError,
)


class QuietThreadingHTTPServer(socketserver.ThreadingMixIn, http.server.HTTPServer):
    daemon_threads = True
    allow_reuse_address = True

    def handle_error(self, request, client_address):
        error = getattr(self, "_last_client_error", None)
        if isinstance(error, IGNORED_CLIENT_ERRORS):
            print(f"{client_address[0]} - client disconnected during download")
            self._last_client_error = None
            return
        super().handle_error(request, client_address)


class ReleaseRequestHandler(http.server.SimpleHTTPRequestHandler):
    def _client_allowed(self):
        allowed_ips = getattr(self.server, "allowed_ips", set())
        return not allowed_ips or self.client_address[0] in allowed_ips

    def _reject_forbidden(self):
        print(f"{self.client_address[0]} - blocked by update allowlist")
        self.send_response(403, "Forbidden")
        self.end_headers()
        self.wfile.write(b"Forbidden\n")

    def do_GET(self):
        if not self._client_allowed():
            self._reject_forbidden()
            return
        super().do_GET()

    def do_HEAD(self):
        if not self._client_allowed():
            self._reject_forbidden()
            return
        super().do_HEAD()

    def copyfile(self, source, outputfile):
        try:
            super().copyfile(source, outputfile)
        except IGNORED_CLIENT_ERRORS as exc:
            self.server._last_client_error = exc
            raise
        except socket.timeout as exc:
            self.server._last_client_error = exc
            raise


def main():
    parser = argparse.ArgumentParser(description="Serve DarkEden release files.")
    parser.add_argument("--root", required=True, help="Release site root directory.")
    parser.add_argument("--port", type=int, default=8080, help="Port to listen on.")
    parser.add_argument("--bind", default="0.0.0.0", help="Address to bind.")
    parser.add_argument("--allowed-ip", action="append", default=[], help="Allowed client IP. Can be repeated.")
    args = parser.parse_args()

    root = os.path.abspath(args.root)
    if not os.path.isdir(root):
        raise SystemExit(f"Release site folder does not exist: {root}")

    handler = functools.partial(ReleaseRequestHandler, directory=root)
    with QuietThreadingHTTPServer((args.bind, args.port), handler) as server:
        server.allowed_ips = set(args.allowed_ip)
        print(f"Serving {root}")
        print(f"URL: http://{args.bind}:{args.port}/")
        if server.allowed_ips:
            print("Allowed IPs: " + ", ".join(sorted(server.allowed_ips)))
        else:
            print("Allowed IPs: all")
        print("Press Ctrl+C to stop.")
        server.serve_forever()


if __name__ == "__main__":
    main()
