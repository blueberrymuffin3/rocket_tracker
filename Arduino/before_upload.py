from time import sleep
from glob import glob
import os
from pathlib import Path
Import("env")

boards = [
    ("groundstation.usb", 1),
    ("rocket.usb", 2)
]

def get_path_for_port(port):
    ports = glob(f"/dev/serial/by-path/pci-????:??:??.?-usb-?:?.{port}:1.0")
    if(len(ports) == 0):
        return None
    else:
        return Path(ports[0]).resolve()

def before_upload(source, target, env):

    # This delay prevents the uploader from being confused
    # when one board loads while another is resetting
    # and trying to upload to the wrong board
    sleep(3)
    print()
    print("Updating symlinks...")

    for (filename, port) in boards:
        if os.path.exists(filename):
            os.remove(filename)

        port_path = get_path_for_port(port)
        if(port_path):
            print(f'Found board "{filename}" at {port_path}')
            os.symlink(port_path, filename)

    print()


env.AddPreAction("upload", before_upload)
