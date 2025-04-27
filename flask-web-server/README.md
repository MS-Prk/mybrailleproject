# Flask Web Server for Traffic Sign Detection and Braille Translation

This project is a Flask web server that accepts images from an ESP32-CAM, performs traffic sign detection using a YOLOv8 model, processes the detected text with OCR using the Google Cloud Vision API, translates the result into Braille, and communicates with another ESP32 board to control Braille motor output.

## Project Structure

```
flask-web-server
├── app.py                     # Main entry point of the Flask web server
├── static
│   └── images
│       └── recent.jpg        # Placeholder for the most recent image
├── templates
│   └── index.html            # HTML template for displaying the image and UI
├── models
│   └── yolo
│       └── yolov8_model.pt   # Pre-trained YOLOv8 model for traffic sign detection
├── utils
│   ├── braille_translation.py # Functions for translating text into Braille
│   ├── ocr_processing.py      # Functions for performing OCR using Google Cloud Vision API
│   ├── esp32_communication.py  # Functions for serial communication with the ESP32 board
│   └── traffic_sign_detection.py # Functions for running the YOLOv8 model
├── requirements.txt           # List of dependencies required for the project
└── README.md                  # Documentation for the project
```

## Setup Instructions

1. **Clone the repository:**
   ```
   git clone <repository-url>
   cd flask-web-server
   ```

2. **Install dependencies:**
   Make sure you have Python 3.7 or higher installed. Then, create a virtual environment and install the required packages:
   ```
   python -m venv venv
   source venv/bin/activate  # On Windows use `venv\Scripts\activate`
   pip install -r requirements.txt
   ```

3. **Set up Google Cloud Vision API:**
   - Create a Google Cloud project and enable the Vision API.
   - Set up authentication by creating a service account and downloading the JSON key file. Set the `GOOGLE_APPLICATION_CREDENTIALS` environment variable to the path of the JSON key file.

4. **Run the Flask server:**
   ```
   python app.py
   ```

5. **Access the web interface:**
   Open your web browser and go to `http://127.0.0.1:5000` to view the most recent image and interact with the application.

## Usage

- The server will accept images via HTTP POST requests from the ESP32-CAM.
- The most recent image will be displayed on the web page.
- Detected traffic signs will be processed, and the text will be translated into Braille.
- The Braille motor output will be controlled based on the detected signs.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.