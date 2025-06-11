#include "identifier.h"
#include "esp_random.h"
#include "stdint.h"
#include "stdio.h"

const char identifier_alphabet[] = "ABCDEHKLMNPQRSTUWXY123456789";

void identifier_generate(char* buffer, int length) {
    for (int i = 0; i < length; i++) {
        uint32_t random_number = esp_random();
        uint8_t random_index = random_number % (sizeof(identifier_alphabet) - 1);

        buffer[i] = identifier_alphabet[random_index];
    }
}

