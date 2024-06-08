import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import RadioButtons, Slider

# LED
command_led_resis = {'470Ω': 0.47, '2kΩ': 2, '10kΩ': 10}
command_led_resis_selected = 0.47
command_led_up_percent = 0.1

power_led_resis = {'470Ω': 0.47, '2kΩ': 2, '10kΩ': 10}
power_led_resis_selected = 0.47

input_vol = 3.3
cutoff_vol = 2

# Core
deep_sleep_mode_curr = 10 * 10**(-3)
light_sleep_mode_curr = 0.8 
modem_sleep_mode_curr = 25

# Battery Measurement
battery_meas_resis = {'470Ω': 0.47, '2kΩ': 2, '10kΩ': 10, '100kΩ': 100}
battery_meas_resis_selected = 0.47

# LDO
LDO_curr = 30 * 10**(-3)

# Servo
servo_stall_cur = 360 
servo_stall_time = 3.5
servo_typical_cur = 250
servo_typical_time = 1.5
servo_idle_cur = 10
servo_move_count = 2

# Sleep controller
active_time = 1
total_time = 24
sleep_time = total_time - active_time

# Battery
battery_cap = 400
charging_curr = 500

def cal_energy():
    # Energy Estimation
    command_led_energy_consumption = command_led_up_percent*active_time*(input_vol-cutoff_vol)/command_led_resis_selected
    power_led_energy_consumption = total_time*(input_vol-cutoff_vol)/power_led_resis_selected
    led_energy_consumption = command_led_energy_consumption +  power_led_energy_consumption
    modem_energy_consumption = active_time*modem_sleep_mode_curr
    deep_energy_consumption = sleep_time*deep_sleep_mode_curr 
    core_energy_consumption = modem_energy_consumption + deep_energy_consumption
    battery_meas_energy_consumption = total_time * input_vol/(battery_meas_resis_selected*2)
    LDO_energy_consumption = total_time*LDO_curr
    servo_energy_consumption = (servo_typical_cur*servo_typical_time/3600+servo_stall_cur*servo_stall_time/3600)*servo_move_count
    energy_consumption = led_energy_consumption + + core_energy_consumption + LDO_energy_consumption + battery_meas_energy_consumption + servo_energy_consumption
    time_last = (battery_cap/energy_consumption)
    charging_time = battery_cap/charging_curr*60
    print("Energy Consumption per day: %.2f mAh (%.2f%%)" % (energy_consumption, 100* energy_consumption/battery_cap))
    print("\t* LED: %.2f mAh (%.2f%%)" % (led_energy_consumption, 100*led_energy_consumption/energy_consumption))
    print("\t\t* Power LED: %.2f mAh (%.2f%%)" % (power_led_energy_consumption, 100*power_led_energy_consumption/energy_consumption))
    print("\t\t* Command LED: %.2f mAh (%.2f%%)" % (command_led_energy_consumption, 100*command_led_energy_consumption/energy_consumption))
    print("\t* Core: %.2f mAh (%.2f%%)" % (core_energy_consumption, 100*core_energy_consumption/energy_consumption))
    print("\t\t* Modem sleep: %.2f mAh (%.2f%%)" % (modem_energy_consumption, 100*modem_energy_consumption/energy_consumption))
    print("\t\t* Sleep sleep: %.2f mAh (%.2f%%)" % (deep_energy_consumption, 100*deep_energy_consumption/energy_consumption))
    print("\t* Bat meas: %.2f mAh (%.2f%%)" % (battery_meas_energy_consumption, 100*battery_meas_energy_consumption/energy_consumption))
    print("\t* LDO: %.2f mAh (%.2f%%)" % (LDO_energy_consumption, 100*LDO_energy_consumption/energy_consumption))
    print("\t* Servo: %.2f mAh (%.2f%%)" % (servo_energy_consumption, 100*servo_energy_consumption/energy_consumption))
    print("Time Last: %.2f day" % time_last)
    return {"command_led": command_led_energy_consumption,
            "power_led": power_led_energy_consumption,
            "total_led": led_energy_consumption,
            "modem": modem_energy_consumption,
            "deep": deep_energy_consumption,
            "core": core_energy_consumption,
            "battery": battery_meas_energy_consumption,
            "LDO": LDO_energy_consumption,
            "servo": servo_energy_consumption,
            "total_energy_consumption": energy_consumption,
            "time_last": time_last,
            "charging_time": charging_time
    }

def update_command_led_resistor(label):
    global command_led_resis_selected
    command_led_resis_selected = command_led_resis[label]
    print(command_led_resis_selected)
    update_plot()
    plt.draw()

def update_power_led_resistor(label):
    global power_led_resis_selected
    power_led_resis_selected = power_led_resis[label]
    print(power_led_resis_selected)
    update_plot()
    plt.draw()

def update_batt_meas_resistor(label):
    global battery_meas_resis_selected
    battery_meas_resis_selected = battery_meas_resis[label]
    print(battery_meas_resis_selected)
    update_plot()
    plt.draw()

def update_active_time(val):
    global active_time
    active_time = val
    print(active_time)
    update_plot()
    plt.draw()

def update_total_time(val):
    global total_time
    total_time = val
    print(total_time)
    update_plot()
    plt.draw()

def update_servo_move_count(val):
    global servo_move_count
    servo_move_count = val
    print(servo_move_count)
    update_plot()
    plt.draw()
    
def update_battery_cap(val):
    global battery_cap
    battery_cap = val
    print(battery_cap)
    update_plot()
    plt.draw()

def update_plot():
    energy_dict = cal_energy()
    vals = np.array([[energy_dict["command_led"], energy_dict["power_led"]], 
                    [energy_dict["modem"], energy_dict["deep"]], 
                    [energy_dict["battery"], 0], 
                    [energy_dict["LDO"], 0], 
                    [energy_dict["servo"], 0]])

    cmap = plt.colormaps["tab20c"]
    outer_colors = cmap(np.arange(5)*4)
    inner_colors = cmap([1, 2, 5, 6, 9, 10, 13, 14, 17, 18])

    labels_inner = ["Command", "Power", "Modem", "Deep", "", "", "", "", "", ""]
    labels_outer = ["LED", "Core", "Battery", "LDO", "Servo"]
    outer_labels = [f"{name}\n{energy:.1f}mAh({percentage:.1f}%)" for name, energy, percentage in zip(labels_outer, vals.sum(axis=1), (vals.sum(axis=1) / vals.sum() * 100))]

    size = 0.3
    ax.clear()
    ax.pie(vals.sum(axis=1), radius=1, colors=outer_colors, labels=outer_labels, rotatelabels=False, labeldistance=1,
        wedgeprops=dict(width=size, edgecolor='w'))
    ax.pie(vals.flatten(), radius=1-size, colors=inner_colors, labels=labels_inner, rotatelabels=False, labeldistance=0.5,
        wedgeprops=dict(width=size, edgecolor='w'))

    ax.set_title("Energy Consumption")
    ax.text(1.5, 1.2, "Energy Consumption per day: {:.2f}mAh({:.2f}%)".format(energy_dict["total_energy_consumption"], energy_dict["total_energy_consumption"]/battery_cap*100))
    ax.text(1.5, 1.1, "Time Last: {:.2f} days".format(energy_dict["time_last"]))
    ax.text(1.5, 1.0, "Charging Time: {:.1f} minutes".format(energy_dict["charging_time"]))

fig, ax = plt.subplots(figsize=(20,10))
axradio = plt.axes([0.05, 0.7, 0.05, 0.07])
axradio.set_title("Command LED")
radio = RadioButtons(axradio, ('470Ω', '2kΩ', '10kΩ'))
radio.on_clicked(update_command_led_resistor)

axradio2 = plt.axes([0.12, 0.7, 0.05, 0.07])
axradio2.set_title("Power LED")
radio2 = RadioButtons(axradio2, ('470Ω', '2kΩ', '10kΩ'))
radio2.on_clicked(update_power_led_resistor)

axradio3 = plt.axes([0.19, 0.7, 0.05, 0.07])
axradio3.set_title("Batt Meas")
radio3 = RadioButtons(axradio3, ('470Ω', '2kΩ', '10kΩ', '100kΩ'))
radio3.on_clicked(update_batt_meas_resistor)

axslider = plt.axes([0.05, 0.62, 0.10, 0.03])
axslider.set_title("Active Time")
active_time_slider = Slider(
    ax=axslider,
    label="",
    valmin=0,
    valmax=24,
    valinit=1,
    valstep=0.5,
)
active_time_slider.on_changed(update_active_time)

axslider2 = plt.axes([0.05, 0.54, 0.10, 0.03])
axslider2.set_title("Total Time")
active_time_slider2 = Slider(
    ax=axslider2,
    label="",
    valmin=0,
    valmax=24,
    valinit=24,
    valstep=0.5,
)
active_time_slider2.on_changed(update_total_time)

axslider3 = plt.axes([0.05, 0.47, 0.10, 0.03])
axslider3.set_title("Servo move count")
active_time_slider3 = Slider(
    ax=axslider3,
    label="",
    valmin=0,
    valmax=100,
    valinit=0,
    valstep=1,
)
active_time_slider3.on_changed(update_servo_move_count)

axslider4 = plt.axes([0.05, 0.4, 0.10, 0.03])
axslider4.set_title("Battery Capacity")
active_time_slider4 = Slider(
    ax=axslider4,
    label="",
    valmin=100,
    valmax=1500,
    valinit=400,
    valstep=50,
)
active_time_slider4.on_changed(update_battery_cap)

update_plot()
plt.show()
