#!/bin/bash
# start modbus server
python3 eggs/osie_plc/osie_plc.py &

# start OCR which will control machine over modbus
cd eggs/osie_plc
python2 osie_ocr.py
