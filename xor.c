#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void xorEncrypt(char *message, const char *key) {
    int keyLen = strlen(key);
    for (int i = 0; message[i] != '\0'; ++i) {
        message[i] ^= key[i % keyLen];
    }
}

void encryptFile(const char *filename, const char *message, const char *key) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error opening file for writing.\n");
        return;
    }

    xorEncrypt((char *)message, key);
    fwrite(message, 1, strlen(message), file);

    fclose(file);
}

void decryptFile(const char *filename, const char *key) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file for reading.\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(fileSize + 1);
    if (!buffer) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return;
    }

    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';

    xorEncrypt(buffer, key);

    printf("%s\n", buffer);

    free(buffer);
    fclose(file);
}

int main() {
    char choice[10];
    printf("Enter 'encrypt' or 'decrypt': ");
    scanf("%s", choice);

    if (strcmp(choice, "encrypt") == 0) {
        char message[256];
        char filename[256];
        char key[256];

        printf("Enter the message to encrypt: ");
        scanf(" ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';

        printf("Enter the filename: ");
        scanf("%s", filename);

        printf("Enter the key: ");
        scanf("%s", key);

        encryptFile(filename, message, key);
    } else if (strcmp(choice, "decrypt") == 0) {
        char filename[256];
        char key[256];

        printf("Enter the filename to decrypt: ");
        scanf("%s", filename);

        printf("Enter the key: ");
        scanf("%s", key);

        decryptFile(filename, key);
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}