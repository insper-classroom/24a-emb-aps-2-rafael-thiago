import serial
import uinput

ser = serial.Serial('/dev/ttyACM0', 115200)

# Create new mouse device
device = uinput.Device([
    uinput.BTN_A
])


def parse_data(data):
    btn_number = data[0]  # 0 for X, 1 for Y
    # value = int.from_bytes(data[1:3], byteorder='little', signed=True)
    value = data[1]
    print(f"Received data: {data}")
    print(f"btn_number: {btn_number}, value: {value}")
    return btn_number, value


def move_mouse(btn_number, value):
    if btn_number == 0:    #
        device.emit(uinput.REL_X, value)
    elif btn_number == 1:  # 
        device.emit(uinput.REL_Y, value)


try:
    # sync package
    while True:
        print('Waiting for sync package...')
        while True:
            data = ser.read(1)
            if data == b'\xff':
                break

        # Read 4 bytes from UART
        data = ser.read(2)
        btn_number, value = parse_data(data)
        move_mouse(btn_number, value)

except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    ser.close()