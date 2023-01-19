"""
python wrapper for the logger from C++ part of the package
"""


from sys import path

# module "lives" in bin folder
path.append("../bin")

import cppLogger

def log_message(message : str) -> None:
    cppLogger.log_message(message)

def set_log_file(log_address : str, recreate : bool) -> None:
    cppLogger.set_log_file(log_address, recreate)

def enable_logging() -> None:
    cppLogger.enable_logging()

def disable_logging() -> None:
    cppLogger.disable_logging()

def benchmark(message : str) -> None:
    cppLogger.benchmark(message)
