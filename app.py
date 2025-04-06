from flask import Flask, render_template, request, redirect, flash
import serial
import serial.tools.list_ports

app = Flask(__name__)
app.secret_key = "svarog-secret"

ser = None
available_ports = [p.device for p in serial.tools.list_ports.comports()]

@app.route("/", methods=["GET", "POST"])
def index():
    global ser
    total_minutes = "N/A"
    current_temp = "N/A"

    if request.method == "POST":
        selected_port = request.form.get("serial_port")
        if selected_port:
            try:
                ser = serial.Serial(selected_port, 9600, timeout=1)
                flash(f"Connected to {selected_port}")
            except Exception as e:
                flash(f"Failed to connect to {selected_port}: {e}")
                ser = None

        if "reset" in request.form:
            if ser and ser.is_open:
                ser.write(b"RESET\n")
                flash("Pump time reset.")
            else:
                flash("Arduino not connected.")
        elif "min_temp" in request.form and "max_temp" in request.form:
            min_temp = request.form["min_temp"]
            max_temp = request.form["max_temp"]
            if ser and ser.is_open:
                cmd = f"SET {min_temp} {max_temp}\n"
                ser.write(cmd.encode())
                flash("Thresholds sent to Arduino!")
            else:
                flash("Arduino not connected.")
        return redirect("/")

    if ser and ser.is_open:
        try:
            ser.write(b"READ\n")
            total_minutes = ser.readline().decode().strip()

            ser.write(b"TEMP\n")
            current_temp = ser.readline().decode().strip()
        except:
            total_minutes = "Error reading"
            current_temp = "Error reading"

    return render_template("index.html", runtime=total_minutes, current_temp=current_temp, ports=available_ports)

if __name__ == "__main__":
    app.run(debug=True)
