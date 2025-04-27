import os
from google.cloud import vision, translate

# ✅ 서비스 계정 키 설정
os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = "brailliant-ocr-f77fc2866f5b.json"

# ✅ 언어 이름 → Google 번역 코드 매핑
LANGUAGE_CODE_MAP = {
    "english": "en",
    "korean": "ko",
    "spanish": "es",
    "chinese": "zh"
}

def perform_ocr_and_translate(image_path, target_language=None):
    # ✅ OCR 클라이언트 초기화
    client = vision.ImageAnnotatorClient()

    with open(image_path, 'rb') as image_file:
        content = image_file.read()

    image = vision.Image(content=content)
    response = client.text_detection(image=image)
    texts = response.text_annotations
    print("🔎 OCR 결과:", texts)

    if not texts:
        return "", "No text found in image."

    detected_text = texts[0].description.strip()

    # ✅ 언어 미선택 시 원문 반환
    if not target_language:
        return detected_text, "No language selected. Using English as default."

    # ✅ 언어 코드 변환 (유효성 검사 포함)
    lang_code = LANGUAGE_CODE_MAP.get(target_language.lower())
    if not lang_code:
        return detected_text, f"Invalid language selected: {target_language}. Using English as default."
    
    # ✅ 번역 요청
    translate_client = translate.TranslationServiceClient()
    parent = "projects/brailliant-ocr/locations/global"  # 실제 프로젝트 ID 사용

    response = translate_client.translate_text(
        contents=[detected_text],
        target_language_code=lang_code,
        parent=parent
    )

    translated_text = response.translations[0].translated_text
    return translated_text, None
