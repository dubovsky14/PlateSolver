#!/usr/bin/env python3

from app import app as application

if __name__ == "__main__":
    application.run()

# run as:
# uwsgi --http-socket :9090 --wsgi-file local_test.py --master