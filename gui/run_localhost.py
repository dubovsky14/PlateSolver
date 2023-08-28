#!/usr/bin/env python3

from app import app as application

if __name__ == "__main__":
    application.run()

# run as:
# uwsgi --http-socket :9090 --plugin python3 --wsgi-file run_localhost.py --master
# access the gui in browser at address: localhost:9090