import serial
import uinput

ser = serial.Serial('/dev/ttyACM0', 115200)

# Create new mouse device
device = uinput.Device([
    uinput.KEY_A,
    uinput.KEY_S,
    uinput.KEY_J,
    uinput.KEY_K,
    uinput.KEY_L
])


def parse_data(data):
    key = data[0]
    value = data[1]
    print(f"Received data: {data}")
    print(f"btn_number: {key}, value: {value}")
    return key, value


def press_key(key, value):
    if key == 0:
        device.emit(uinput.KEY_A, value)
    elif key == 1:
        device.emit(uinput.KEY_J, value)


try:
    # sync package
    while True:
        print('Waiting for sync package...')
        while True:
            data = ser.read(1)
            if data == b'\xff':
                break

        data = ser.read(2)
        key, value = parse_data(data)
        press_key(key, value)

except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    ser.close()