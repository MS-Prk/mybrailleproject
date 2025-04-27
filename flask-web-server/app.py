from flask import Flask, render_template, request, session, redirect, url_for
import os
import threading
import requests
import time
from utils.ocr_processing import perform_ocr_and_translate
from utils.braille_translation import (
    translate_to_braille,
    convert_braille_to_array,
    convert_braille_to_dots_array
)
from utils.esp32_communication import send_to_devkit, check_devkit_connection#, request_capture_from_esp32cam

app = Flask(__name__)
app.secret_key = '9f3b0c57c1d74e2cb8fecd07f512ab90'

IMAGE_FOLDER = 'static/images'
RECENT_IMAGE_PATH = os.path.join(IMAGE_FOLDER, 'recent.jpeg')

connection_status = "Unconnected"
recognized_text = ""
warning_message = ""

word_index = 0
words_list = []
capture_flag = False

@app.route('/', endpoint='home')
def home():
    connection_status = check_devkit_connection()
    return render_template('home.html', connection_status="connected")

@app.route('/language_setting', methods=['GET', 'POST'])
def language_setting():
    if request.method == 'POST':
        selected_language = request.form.get('language')
        session['language'] = selected_language
        return redirect(url_for('language_setting'))

    saved_language = session.get('language', None)
    return render_template('language_setting.html', saved_language=saved_language)

def send_braille_in_background(command):
    threading.Thread(target=send_to_devkit, args=(command,), daemon=True).start()

@app.route('/start')
def start():
    global recognized_text, warning_message

    selected_language = session.get('language', 'english')
    speed = session.get('speed', '3')

    # ESP32-CAM에 촬영 요청
    #request_capture_from_esp32cam()

    # 최근 이미지 존재 확인
    if not os.path.exists(RECENT_IMAGE_PATH):
        warning_message = None  # 또는 "" 로 설정해도 됨
        return render_template('start.html', image='recent.jpeg', image_timestamp=int(time.time()), recognized_text="", braille_output=[], braille_dots=[], warning=warning_message)

    recognized_text, warning_message = perform_ocr_and_translate(RECENT_IMAGE_PATH, selected_language)
    braille_string = translate_to_braille(recognized_text, language=selected_language)
    braille_output = convert_braille_to_array(braille_string)
    braille_dots = convert_braille_to_dots_array(braille_string)

    send_braille_in_background(f"speed-{speed}:{braille_output}")

    return render_template(
        'start.html',
        image='recent.jpeg',
        image_timestamp=int(time.time()),
        recognized_text=recognized_text,
        braille_output=braille_output,
        braille_dots=braille_dots,
        warning=warning_message
    )

@app.route('/speed_setting')
def speed_setting():
    return render_template('speed_setting.html')

@app.route('/test-speed', methods=['POST'])
def test_speed():
    speed = request.form.get('speed', '3')
    print(f"🧪 테스트 속도: {speed}")
    send_braille_in_background(f"test-speed-{speed}")
    return redirect(url_for('speed_setting'))

@app.route('/confirm-speed', methods=['POST'])
def confirm_speed():
    speed = request.form.get('speed', '3')
    session['speed'] = speed
    print(f"✅ 설정된 속도 저장됨: {speed}")
    return redirect(url_for('speed_setting'))

@app.route('/upload', methods=['POST'])
def upload_image():
    image_data = request.get_data()
    if not image_data:
        return 'No image received', 400

    with open(RECENT_IMAGE_PATH, 'wb') as f:
        f.write(image_data)

    print("✅ ESP32 또는 curl로부터 이미지 저장 완료")
    requests.post("http://localhost:5001/reset_ocr")

    return 'Image uploaded successfully', 200

@app.route('/reset_ocr', methods=['POST'])
def reset_ocr():
    global word_index, words_list, recognized_text

    selected_language = session.get('language', 'english')
    recognized_text, _ = perform_ocr_and_translate(RECENT_IMAGE_PATH, selected_language)
    words_list = recognized_text.strip().split()
    word_index = 0

    print(f"🔄 단어 리스트 초기화: {words_list}")
    return "READY", 200

@app.route('/next_word', methods=['GET'])
def get_next_word():
    global word_index, words_list, capture_flag

    if word_index < len(words_list):
        word = words_list[word_index]
        word_index += 1
        print(f"👉 다음 단어: {word}")
        return word
    else:
        print("📸 모든 단어 출력 완료. ESP32-CAM에 재촬영 요청")
        capture_flag = True
        return "CAPTURE", 200

@app.route('/prev_word', methods=['GET'])
def get_prev_word():
    global word_index, words_list
    if word_index > 1:
        word_index -= 2
    return get_next_word()

@app.route('/word_done', methods=['POST'])
def word_done():
    print("✅ ESP32 DevKit에서 점자 출력 완료 신호 수신")
    return "ACK", 200

@app.route('/check_text', methods=['GET'])
def check_text():
    global recognized_text
    if recognized_text.strip():
        return "Text found", 200
    else:
        return "No text", 204
'''
@app.route('/capture_now', methods=['GET'])
def capture_now():
    global capture_flag
    if capture_flag:
        capture_flag = False
        return "GO", 200
    else:
        return "WAIT", 204
'''
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001, debug=True)
