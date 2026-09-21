#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void xorEncrypt(char* message, const char* key, size_t len) {
    int keyLen = strlen(key);
    for (size_t i = 0; i < len; ++i) {
        message[i] ^= key[i % keyLen];
    }
}

void encryptFile(const char* filename, const char* message, const char* key) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error opening file for writing.\n");
        return;
    }

    size_t len = strlen(message);
    xorEncrypt((char*)message, key, len);
    fwrite(message, 1, len, file);

    fclose(file);
}

/*
 * Return codes:
 *   0  = success
 *  -1  = file could not be opened (doesn't exist)
 *  -2  = memory allocation failed
 *  -3  = file is empty
 */
int decryptFile(const char* filename, const char* key) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: file \"%s\" does not exist.\n", filename);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fileSize == 0) {
        printf("Error: file \"%s\" is empty.\n", filename);
        fclose(file);
        return -3;
    }

    char* buffer = (char*)malloc(fileSize + 1);
    if (!buffer) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return -2;
    }

    size_t bytesRead = fread(buffer, 1, fileSize, file);
    buffer[bytesRead] = '\0';

    xorEncrypt(buffer, key, bytesRead);

    fwrite(buffer, 1, bytesRead, stdout);
    printf("\n");

    free(buffer);
    fclose(file);
    return 0;
}

/*
 * Reads a line of arbitrary length from stdin into a dynamically
 * allocated buffer. Caller must free() the returned pointer.
 * Returns NULL on allocation failure.
 */
char* readLine(void) {
    size_t capacity = 256;
    size_t length = 0;
    char* buffer = (char*)malloc(capacity);
    if (!buffer) {
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (length + 1 >= capacity) {
            capacity *= 2;
            char* newBuffer = (char*)realloc(buffer, capacity);
            if (!newBuffer) {
                free(buffer);
                return NULL;
            }
            buffer = newBuffer;
        }
        buffer[length++] = (char)c;
    }
    buffer[length] = '\0';

    return buffer;
}

int main() {
    char choice[10];

    while (1) {
        printf("Enter 'encrypt' or 'decrypt': ");
        scanf("%s", choice);

        if (strcmp(choice, "encrypt") == 0 || strcmp(choice, "decrypt") == 0) {
            break;
        }

        printf("Invalid choice. Please enter 'encrypt' or 'decrypt'.\n");
    }

    if (strcmp(choice, "encrypt") == 0) {
        char filename[256];
        char key[256];

        printf("Enter the message to encrypt: ");
        scanf(" ");
        char* message = readLine();
        if (!message) {
            printf("Memory allocation failed while reading message.\n");
            return 1;
        }

        printf("Enter the filename: ");
        scanf("%s", filename);

        printf("Enter the key: ");
        scanf("%s", key);

        encryptFile(filename, message, key);

        free(message);
    }
    else {
        char filename[256];
        char key[256];

        printf("Enter the key: ");
        scanf("%s", key);

        int result;
        do {
            printf("Enter the filename to decrypt: ");
            scanf("%s", filename);

            result = decryptFile(filename, key);

            if (result == -3) {
                char tryAgain[10];
                printf("Would you like to try a different file? (yes/no): ");
                scanf("%s", tryAgain);
                if (strcmp(tryAgain, "no") == 0) {
                    break;
                }
                result = -1;
            }
        } while (result == -1);
    }

    return 0;
}