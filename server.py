from flask_cors import CORS
from flask import Flask, request, jsonify
import time

app = Flask(__name__)
CORS(app)

poles = {
    "P01": {"status": "normal"},
    "P02": {"status": "fault"},
    "P03": {"status": "normal"},
    "P04": {"status": "normal"}
}

@app.route("/update_pole", methods=["POST"])
def update_pole():
    try:
        data = request.get_json(force=True)

        print("Raw data received:", data)

        pole_id = data.get("pole_id")
        status = data.get("status")

        if not pole_id or not status:
            return jsonify({"error": "Missing pole_id or status"}), 400

        poles[pole_id] = {
            "status": status,
            "time": time.strftime("%H:%M:%S")
        }

        print(f"Updated -> {pole_id}: {status}")

        return jsonify({"message": "Pole updated successfully"})

    except Exception as e:
        print("ERROR:", str(e))
        return jsonify({"error": str(e)}), 500

@app.route("/poles", methods=["GET"])
def get_poles():
    return jsonify(poles)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)