import shutil, os
from os import path

Import("env")
variants_dir = env['PROJECT_PACKAGES_DIR'] + "\\framework-arduino-nrf52-mbedos\\"
boards_dir = env['PROJECT_DIR'] + "\\boards\\"

if not (path.exists(variants_dir + "variants\\SENSOGRIP_NINAB306")):
    print("Installing Sensogrip boards...")
    shutil.copy(boards_dir + "boards.txt", variants_dir)
    shutil.copytree(boards_dir + "DEV_NINAB306", variants_dir + "variants\\DEV_NINAB306")
    shutil.copytree(boards_dir + "SENSOGRIP_NINAB306", variants_dir + "variants\\SENSOGRIP_NINAB306")