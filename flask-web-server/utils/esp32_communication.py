import requests

# ✅ 실제 사용 중인 DevKit IP로 변경하세요
ESP32_DEVKIT_IP = "192.168.0.136"  # 예시 IP
PORT = 8080
ESP32_CAM_IP = "192.168.0.201"

def request_capture_from_esp32cam():
    try:
        url = f"http://{ESP32_CAM_IP}:80/capture"
        response = requests.get(url, timeout=(300))
        print(f"📸 ESP32-CAM 응답: {response.text}")
    except Exception as e:
        print(f"❌ ESP32-CAM 촬영 요청 실패: {e}")

def send_to_devkit(data):
    """
    ESP32 DevKit v1로 점자 및 속도 데이터를 전송하는 함수 (Wi-Fi HTTP POST)
    예: data = "speed-3:[0,1,1,0,1,0,0,1]"
    """
    try:
        url = f"http://{ESP32_DEVKIT_IP}:{PORT}/receive"
        response = requests.post(url, data=data, timeout=(10,300))
        print(f"✅ DevKit 응답: {response.text}")
    except Exception as e:
        print(f"❌ DevKit 전송 실패: {e}")

def check_devkit_connection():
    """
    ESP32 DevKit v1와의 연결 상태를 확인하는 함수
    `/ping` 엔드포인트에서 200 OK 응답을 받으면 연결됨
    """
    try:
        url = f"http://{ESP32_DEVKIT_IP}:{PORT}/ping"
        response = requests.get(url, timeout=2)
        if response.status_code == 200:
            return "connected"
        else:
            return "unconnected"
    except:
        return "unconnected"
