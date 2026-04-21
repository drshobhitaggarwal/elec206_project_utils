from flask import Flask, request, jsonify
import csv
import os
from datetime import datetime

app = Flask(__name__)
CSV_FILE = 'sensor_log.csv'

# Ensure CSV has headers
if not os.path.exists(CSV_FILE):
    with open(CSV_FILE, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['Timestamp', 'Temperature', 'Pressure'])

@app.route('/data', methods=['POST'])
def receive_data():
    data = request.json
    temp = data.get('temp')
    press = data.get('press')
    cat = data.get('cat')
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    with open('sensor_log.csv', 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([timestamp, temp, press, cat])

    return jsonify({"status": "received"}), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)
