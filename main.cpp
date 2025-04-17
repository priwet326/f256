#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <clocale>

using namespace std;

// Неприводимый многочлен x^8 + x^4 + x^3 + x + 1 (0x11B)
const uint16_t IR_POLY = 0x11B;

// --- ОСНОВНЫЕ ОПЕРАЦИИ В GF(256) ---

// Умножение двух чисел в GF(256) с редукцией

uint8_t gf256_mul_and_div(uint8_t a, uint8_t b) {
  bool massBit[15] = {0};

  massBit[0] = (((a >> 0) & 1) & ((b >> 0) & 1));
  massBit[1] = (((a >> 1) & 1) & ((b >> 0) & 1)) ^
    (((a >> 0) & 1) & ((b >> 1) & 1));
  massBit[2] = (((a >> 2) & 1) & ((b >> 0) & 1)) ^
    (((a >> 1) & 1) & ((b >> 1) & 1)) ^
    (((a >> 0) & 1) & ((b >> 2) & 1));
  massBit[3] = (((a >> 3) & 1) & ((b >> 0) & 1)) ^
    (((a >> 2) & 1) & ((b >> 1) & 1)) ^
    (((a >> 1) & 1) & ((b >> 2) & 1)) ^
    (((a >> 0) & 1) & ((b >> 3) & 1));
  massBit[4] = (((a >> 4) & 1) & ((b >> 0) & 1)) ^
    (((a >> 3) & 1) & ((b >> 1) & 1)) ^
    (((a >> 2) & 1) & ((b >> 2) & 1)) ^
    (((a >> 1) & 1) & ((b >> 3) & 1)) ^
    (((a >> 0) & 1) & ((b >> 4) & 1));
  massBit[5] = (((a >> 5) & 1) & ((b >> 0) & 1)) ^
    (((a >> 4) & 1) & ((b >> 1) & 1)) ^
    (((a >> 3) & 1) & ((b >> 2) & 1)) ^
    (((a >> 2) & 1) & ((b >> 3) & 1)) ^
    (((a >> 1) & 1) & ((b >> 4) & 1)) ^
    (((a >> 0) & 1) & ((b >> 5) & 1));
  massBit[6] = (((a >> 6) & 1) & ((b >> 0) & 1)) ^
    (((a >> 5) & 1) & ((b >> 1) & 1)) ^
    (((a >> 4) & 1) & ((b >> 2) & 1)) ^
    (((a >> 3) & 1) & ((b >> 3) & 1)) ^
    (((a >> 2) & 1) & ((b >> 4) & 1)) ^
    (((a >> 1) & 1) & ((b >> 5) & 1)) ^
    (((a >> 0) & 1) & ((b >> 6) & 1));
  massBit[7] = (((a >> 7) & 1) & ((b >> 0) & 1)) ^
    (((a >> 6) & 1) & ((b >> 1) & 1)) ^
    (((a >> 5) & 1) & ((b >> 2) & 1)) ^
    (((a >> 4) & 1) & ((b >> 3) & 1)) ^
    (((a >> 3) & 1) & ((b >> 4) & 1)) ^
    (((a >> 2) & 1) & ((b >> 5) & 1)) ^
    (((a >> 1) & 1) & ((b >> 6) & 1)) ^
    (((a >> 0) & 1) & ((b >> 7) & 1));
  massBit[8] = (((a >> 7) & 1) & ((b >> 1) & 1)) ^
    (((a >> 6) & 1) & ((b >> 2) & 1)) ^
    (((a >> 5) & 1) & ((b >> 3) & 1)) ^
    (((a >> 4) & 1) & ((b >> 4) & 1)) ^
    (((a >> 3) & 1) & ((b >> 5) & 1)) ^
    (((a >> 2) & 1) & ((b >> 6) & 1)) ^
    (((a >> 1) & 1) & ((b >> 7) & 1));
  massBit[9] = (((a >> 7) & 1) & ((b >> 2) & 1)) ^
    (((a >> 6) & 1) & ((b >> 3) & 1)) ^
    (((a >> 5) & 1) & ((b >> 4) & 1)) ^
    (((a >> 4) & 1) & ((b >> 5) & 1)) ^
    (((a >> 3) & 1) & ((b >> 6) & 1)) ^
    (((a >> 2) & 1) & ((b >> 7) & 1));
  massBit[10] = (((a >> 7) & 1) & ((b >> 3) & 1)) ^
    (((a >> 6) & 1) & ((b >> 4) & 1)) ^
    (((a >> 5) & 1) & ((b >> 5) & 1)) ^
    (((a >> 4) & 1) & ((b >> 6) & 1)) ^
    (((a >> 3) & 1) & ((b >> 7) & 1));
  massBit[11] = (((a >> 7) & 1) & ((b >> 4) & 1)) ^
    (((a >> 6) & 1) & ((b >> 5) & 1)) ^
    (((a >> 5) & 1) & ((b >> 6) & 1)) ^
    (((a >> 4) & 1) & ((b >> 7) & 1));
  massBit[12] = (((a >> 7) & 1) & ((b >> 5) & 1)) ^
    (((a >> 6) & 1) & ((b >> 6) & 1)) ^
    (((a >> 5) & 1) & ((b >> 7) & 1));
  massBit[13] = (((a >> 7) & 1) & ((b >> 6) & 1)) ^
    (((a >> 6) & 1) & ((b >> 7) & 1));
  massBit[14] = (((a >> 7) & 1) & ((b >> 7) & 1));

  uint16_t poly = 0;
  for (int i = 0; i < 15; ++i) {
    if (massBit[i]) {
      poly |= (1 << i);  // Устанавливаем бит, если коэффициент = 1
    }
  }
  /* printf("%0b  aaa\n", poly); */
  for (int i = 14; i >= 8; --i) {
    if (poly & (1 << i)) {
      poly ^= (IR_POLY << (i - 8));
    }
  }

  return (uint8_t)poly;
}

/*
uint8_t gf256_mul_and_div(uint8_t a, uint8_t b) {
    uint8_t result = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1)
            result ^= a;
        bool carry = (a & 0x80); // Проверяем старший бит (x^7)
        a <<= 1;
        if (carry)
            a ^= 0x1B; // Редукция: x^8 = x^4 + x^3 + x + 1 (0x1B)
        b >>= 1;
    }
    return result;
}
*/

int gf256_degree(uint16_t p) {
    for (int i = 15; i >= 0; i--) {
        if (p & (1 << i))
            return i;
    }
    return -1;
}

uint8_t gf256_inv(uint8_t a) {
    if (a == 0) return 0;

    uint16_t r0 = IR_POLY, r1 = a;
    uint16_t s0 = 0, s1 = 1;

    while (r1 != 0) {
        int deg_r0 = gf256_degree(r0);
        int deg_r1 = gf256_degree(r1);
        int shift = deg_r0 - deg_r1;

        if (shift < 0) {
            swap(r0, r1);
            swap(s0, s1);
            continue;
        }

        r0 ^= (r1 << shift);
        s0 ^= (s1 << shift);
    }

    return static_cast<uint8_t>(s0);  // <-- Правильно!
}
/*
vector<uint8_t> decryption(vector<uint8_t> input, uint8_t a0, uint8_t a1, uint8_t b0, uint8_t b1){
  size_t size = input.size();
  vector<uint8_t> output(size);

  output[0] = gf256_mul_and_div(input[0] ^ b0, gf256_inv(a0));
  output[1] = gf256_mul_and_div(input[1] ^ b1, gf256_inv(a1));

  for(int i = 2; i < size; i += 2){
    a0 = gf256_mul_and_div(a1, a0);
    b0 = gf256_mul_and_div(b1, b0);
    output[i] = gf256_mul_and_div(input[i] ^ b0, gf256_inv(a0));
    a1 = gf256_mul_and_div(a0, a1);
    b1 = gf256_mul_and_div(b0, b1);
    output[i+1] = gf256_mul_and_div(input[i+1] ^ b0, gf256_inv(a0));
  }
  if(size % 2){
    a0 = gf256_mul_and_div(a1, a0);
    b0 = gf256_mul_and_div(b1, b0);
    output[size - 1] = gf256_mul_and_div(input[size - 1] ^ b0, gf256_inv(a0));
  }

  return output;
}
*/

vector<uint8_t> decryption(vector<uint8_t> input, uint8_t a0, uint8_t a1, uint8_t b0, uint8_t b1){
  size_t size = input.size();
  vector<uint8_t> output(size);

  output[0] = gf256_mul_and_div(input[0] ^ b0, gf256_inv(a0));
  output[1] = gf256_mul_and_div(input[1] ^ b1, gf256_inv(a1));

  for(int i = 2; i < size; i += 2){
    uint8_t next_a0 = gf256_mul_and_div(a1, a0);
    uint8_t next_b0 = gf256_mul_and_div(b1, b0);
    output[i] = gf256_mul_and_div(input[i] ^ next_b0, gf256_inv(next_a0));

    uint8_t next_a1 = gf256_mul_and_div(next_a0, a1);
    uint8_t next_b1 = gf256_mul_and_div(next_b0, b1);
    output[i+1] = gf256_mul_and_div(input[i+1] ^ next_b1, gf256_inv(next_a1));

    a0 = next_a0;
    b0 = next_b0;
    a1 = next_a1;
    b1 = next_b1;
  }

  if(size % 2){
    a0 = gf256_mul_and_div(a1, a0);
    b0 = gf256_mul_and_div(b1, b0);
    output[size - 1] = gf256_mul_and_div(input[size - 1] ^ b0, gf256_inv(a0));
  }

  return output;
}


vector<uint8_t> encryption(vector<uint8_t> input, uint8_t a0, uint8_t a1, uint8_t b0, uint8_t b1){
  size_t size = input.size();
  vector<uint8_t> output(size);

  output[0] = (gf256_mul_and_div(a0, input[0]) ^ b0);
  output[1] = (gf256_mul_and_div(a1, input[1]) ^ b1);

  for(int i = 2; i < size; i += 2){
    a0 = gf256_mul_and_div(a1, a0);
    b0 = gf256_mul_and_div(b1, b0);
    output[i]= (gf256_mul_and_div(input[i], a0) ^ b0);
    a1 = gf256_mul_and_div(a0, a1);
    b1 = gf256_mul_and_div(b0, b1);
    output[i + 1]= (gf256_mul_and_div(input[i + 1], a1) ^ b1);
  }
  if(size % 2){
    a0 = gf256_mul_and_div(a1, a0);
    b0 = gf256_mul_and_div(b1, b0);
    output[size - 1] = (gf256_mul_and_div(input[size - 1], a0) ^ b0);
  }

  return output;
}

// --- ФУНКЦИИ ДЛЯ РАБОТЫ С ФАЙЛАМИ ---
/*
// Чтение файла как массива байт
vector<uint8_t> read_file(const string& filename) {
  ifstream file(filename, ios::binary);
  return vector<uint8_t>(
      (istreambuf_iterator<char>(file)),
      istreambuf_iterator<char>()
      );
}

// Запись массива байт в файл
void write_file(const string& filename, const vector<uint8_t>& data) {
  ofstream file(filename, ios::binary);
  file.write(reinterpret_cast<const char*>(data.data()), data.size());
}
*/
void print_as_string(const std::vector<uint8_t>& data) {
  for (uint8_t byte : data) {
    std::cout << static_cast<char>(byte);
  }
  std::cout << std::endl;
}

vector<uint8_t> read_file(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file) {
        throw system_error(error_code(errno, generic_category()),
              "Не удалось открыть файл: " + filename);
    }

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw system_error(error_code(errno, generic_category()),
              "Ошибка чтения файла: " + filename);
    }

    return buffer;
}

// Запись массива байт в файл с обработкой ошибок
void write_file(const string& filename, const vector<uint8_t>& data) {
    ofstream file(filename, ios::binary);
    if (!file) {
        throw system_error(error_code(errno, generic_category()),
              "Не удалось создать файл: " + filename);
    }

    if (!file.write(reinterpret_cast<const char*>(data.data()), data.size())) {
        throw system_error(error_code(errno, generic_category()),
              "Ошибка записи в файл: " + filename);
    }
}

int main() {
  setlocale(LC_ALL, "ru_RU.UTF-8");
  vector<uint8_t> input1 = read_file("input.txt");
  print_as_string(input1);
  uint8_t a0 = 4;
  uint8_t a1 = 35;
  uint8_t b0 = 145;
  uint8_t b1 = 246;
  vector<uint8_t> output1 = encryption(input1, a0, a1, b0, b1);
  write_file("encryption.txt", output1);
  print_as_string(read_file("encryption.txt"));

  vector<uint8_t> input2 = read_file("encryption.txt");
  vector<uint8_t> output2 = decryption(input2, a0, a1, b0, b1);
  write_file("decryption.txt", output2);
  print_as_string(read_file("decryption.txt"));
  /* printf("%0b \n", (uint8_t)45); */
  /* printf("%0b \n", gf256_inv((uint8_t)45)); */
  /* printf("%0b \n", gf256_mul_and_div((uint8_t)45, gf256_inv((uint8_t)45))); */
}
