#from ultralytics import YOLO
import cv2
import numpy as np

'''
def load_model(model_path):
    model = YOLO(model_path)
    return model

def detect_traffic_signs(image_path, model):
    image = cv2.imread(image_path)
    results = model(image)

def process_results(results):
    detections = []
    for result in results:
        for box in result.boxes:
            x1, y1, x2, y2 = box.xyxy[0].tolist()
            confidence = box.conf[0].item()
            class_id = int(box.cls[0].item())
            detections.append({
                'bbox': [x1, y1, x2, y2],
                'confidence': confidence,
                'class_id': class_id
            })
    
    return detections

def draw_detections(image_path, detections):
    image = cv2.imread(image_path)
    for detection in detections:
        x1, y1, x2, y2 = detection['bbox']
        cv2.rectangle(image, (int(x1), int(y1)), (int(x2), int(y2)), (0, 255, 0), 2)
    
    output_path = image_path.replace('.jpg', '_detected.jpg')
    cv2.imwrite(output_path, image)
    return output_path
'''