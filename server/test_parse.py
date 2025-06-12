#!/usr/bin/env python3

# AGG_COMFORT 모드 데이터 파싱 테스트
import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

# 테스트 데이터 생성 (AGG_COMFORT 모드: 4바이트)
# temp=-4, humid=64, discomfort_index=31, weekday=5
test_data_4bytes = bytes([0xFC, 0x40, 0x1F, 0x05])  # -4, 64, 31, 5

# 테스트 데이터 생성 (AGG_AVG 모드: 5바이트)  
# temp=-4, humid=64, power=238, month=1
test_data_5bytes = bytes([0xFC, 0x40, 0x00, 0xEE, 0x01])  # -4, 64, 238, 1

def test_parse_data():
    print("Testing parse_data function...")
    
    # Mock logging
    class MockLogging:
        def info(self, msg):
            print(f"INFO: {msg}")
        def error(self, msg):
            print(f"ERROR: {msg}")
    
    # Mock server class
    class MockServer:
        def __init__(self):
            self.logging = MockLogging()
            
        def send_instance(self, lst, is_training):
            print(f"Sending to AI: {lst}, training={is_training}")
            
        def parse_data(self, buf, is_training):
            # AGG_COMFORT 모드: [temp(1), humid(1), discomfort_index(1), weekday(1)] = 4 bytes
            if len(buf) == 4:
                temp = int.from_bytes(buf[0:1], byteorder="big", signed=True)
                humid = int.from_bytes(buf[1:2], byteorder="big", signed=True)
                discomfort_index = int.from_bytes(buf[2:3], byteorder="big", signed=True)
                weekday = int.from_bytes(buf[3:4], byteorder="big", signed=True)

                lst = [temp, humid, discomfort_index, weekday]
                self.logging.info("[temp, humid, discomfort_index, weekday] = {}".format(lst))
            
            # AGG_AVG 모드: [temp(1), humid(1), power(2), month(1)] = 5 bytes
            elif len(buf) == 5:
                temp = int.from_bytes(buf[0:1], byteorder="big", signed=True)
                humid = int.from_bytes(buf[1:2], byteorder="big", signed=True)
                power = int.from_bytes(buf[2:4], byteorder="big", signed=True)
                month = int.from_bytes(buf[4:5], byteorder="big", signed=True)

                lst = [temp, humid, power, month]
                self.logging.info("[temp, humid, power, month] = {}".format(lst))
            
            else:
                self.logging.error("Invalid data length: expected 4 or 5 bytes, got {}".format(len(buf)))
                return

            self.send_instance(lst, is_training)
    
    server = MockServer()
    
    print("\n1. Testing AGG_COMFORT mode (4 bytes):")
    server.parse_data(test_data_4bytes, True)
    
    print("\n2. Testing AGG_AVG mode (5 bytes):")
    server.parse_data(test_data_5bytes, True)
    
    print("\n3. Testing invalid data length:")
    server.parse_data(b"123", True)

if __name__ == "__main__":
    test_parse_data()
