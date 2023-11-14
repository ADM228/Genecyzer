#include <cstdio>

#include "../src/BitConverter.cpp"

void printArr (uint8_t * ptr, size_t size) {
    printf("Contents: ");
    for (int i = 0; i < size; i++) {
        printf("%02X ", ptr[i]);
    }
    printf("; ");
}


int main () {

    // Test BitConverter
    auto array = new uint8_t[8]();
    writeBytes((uint64_t)0x123456789ABCDEF, array);
    printArr(array, 8); printf("Value: %16lX\n", uint64_t(0x123456789ABCDEF));
    writeBytes((uint64_t)0, array);
    writeBytes((uint32_t)0x12345678, array);
    printArr(array, 8); printf("Value: %16X\n", uint32_t(0x12345678));
    writeBytes((uint64_t)0, array);
    writeBytes((uint16_t)0x1234, array);
    printArr(array, 8); printf("Value: %16X\n", uint16_t(0x1234));
    writeBytes((uint64_t)0, array);

    writeBytes((uint64_t)0x123456789ABCDEF, (uint16_t *)array);
    printArr(array, 8); printf("Value: %16lX\n", uint64_t(0x123456789ABCDEF));
    writeBytes((uint64_t)0, (uint16_t *)array);
    writeBytes((uint32_t)0x12345678, (uint16_t *)array);
    printArr(array, 8); printf("Value: %16X\n", uint32_t(0x12345678));
    writeBytes((uint64_t)0, (uint16_t *)array);
    writeBytes((uint16_t)0x1234, (uint16_t *)array);
    printArr(array, 8); printf("Value: %16X\n", uint16_t(0x1234));
    writeBytes((uint64_t)0, (uint16_t *)array);
    delete[] array;

}