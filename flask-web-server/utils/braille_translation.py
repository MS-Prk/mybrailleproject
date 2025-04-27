import subprocess

# ✅ 언어별 점자 테이블 매핑
LANGUAGE_TABLE_MAP = {
    'english': 'en-us-g2.ctb',
    'spanish': 'es.ctb',
    'korean': 'ko-g1.ctb',
    'chinese': 'zh-cn-g1.ctb'
}

def translate_to_braille(text, language='korean'):
    """
    주어진 언어에 맞는 점자 테이블로 텍스트를 변환합니다.
    """
    table = LANGUAGE_TABLE_MAP.get(language.lower(), 'ko-g1.ctb')  # 기본값은 한국어

    try:
        result = subprocess.run(
            ['lou_translate', table],
            input=text.encode(),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=True
        )
        return result.stdout.decode().strip()
    except subprocess.CalledProcessError as e:
        print(f"❌ Braille translation error for '{language}' using table '{table}':", e.stderr.decode())
        return ''

def convert_braille_to_array(braille_string):
    """
    점자 문자열을 8비트 배열로 변환 (ESP32 전송용)
    """
    braille_array = []
    for char in braille_string:
        byte = ord(char)
        bits = [(byte >> bit) & 1 for bit in range(7, -1, -1)]
        braille_array.extend(bits)
    return braille_array

def braille_to_dots(braille_char):
    """
    점자 1글자를 2×3 배열(6점식)로 변환
    """
    byte = ord(braille_char) - 0x2800
    dots = [(byte >> i) & 1 for i in range(6)]
    return [[dots[0], dots[3]], [dots[1], dots[4]], [dots[2], dots[5]]]

def convert_braille_to_dots_array(braille_string):
    """
    점자 문자열 전체를 2×3 배열로 변환하여 리스트로 반환
    """
    return [braille_to_dots(char) for char in braille_string]
