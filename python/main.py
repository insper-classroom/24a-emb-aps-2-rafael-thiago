import serial
import uinput
import time

ser = serial.Serial('/dev/rfcomm0', 9600)

# Create new keyboard device
device = uinput.Device([
    uinput.KEY_A,
    uinput.KEY_S,
    uinput.KEY_J,
    uinput.KEY_K,
    uinput.KEY_L,
    uinput.KEY_SPACE , # Adicionando a tecla de espaço ao dispositivo
    uinput.KEY_ESC,
])


def parse_data(data):
    key = int(data[0])
    value = int(data[1])
    print(f"btn_number: {key}, value: {value}")
    return key, value

def press_key(key, value):
    if key == 0:
        device.emit(uinput.KEY_A, value)
    elif key == 1:
        device.emit(uinput.KEY_S, value)
    elif key == 2:
        device.emit(uinput.KEY_J, value)
    elif key == 5:
        device.emit(uinput.KEY_SPACE, value)
        time.sleep(0.1)
        device.emit(uinput.KEY_SPACE, 0)
    elif key == 6:
        device.emit(uinput.KEY_ESC, value)
        time.sleep(0.1)
        device.emit(uinput.KEY_ESC, 0)

try:
    # sync package
    while True:
        print('Waiting for sync package...')
        while True:
            data = ser.read(4)
            data = [chr(byte) for byte in data]
            if (data[2] == "-" and data[3] == '1'):
                break

        key, value = parse_data(data[0:2])
        press_key(key, value)

except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    ser.close()
