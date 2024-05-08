#include <cstdio>

#include "../src/BitConverter.cpp"

void printArr (const uint8_t * ptr, size_t size) {
    printf("Contents: ");
    for (int i = 0; i < size; i++) {
        printf("%02X ", ptr[i]);
    }
    printf("; ");
}


int main () {

    // Test BitConverter
    auto array = new uint8_t[8]();
    BitConverter::writeBytes(array, (uint64_t)0x123456789ABCDEF);
    printArr(array, 8); printf("Value: %16lX\n", uint64_t(0x123456789ABCDEF));
    BitConverter::writeBytes(array, (uint64_t)0);
    BitConverter::writeBytes(array, (uint32_t)0x12345678);
    printArr(array, 8); printf("Value: %16X\n", uint32_t(0x12345678));
    BitConverter::writeBytes(array, (uint64_t)0);
    BitConverter::writeBytes(array, (uint16_t)0x1234);
    printArr(array, 8); printf("Value: %16X\n", uint16_t(0x1234));
    BitConverter::writeBytes(array, (uint64_t)0);

    BitConverter::writeBytes(array, (uint64_t)0x123456789ABCDEF);
    printArr(array, 8); printf("Value: %16lX\n", uint64_t(0x123456789ABCDEF));
    BitConverter::writeBytes(array, (uint64_t)0);
    BitConverter::writeBytes(array, (uint32_t)0x12345678);
    printArr(array, 8); printf("Value: %16X\n", uint32_t(0x12345678));
    BitConverter::writeBytes(array, (uint64_t)0);
    BitConverter::writeBytes(array, (uint16_t)0x1234);
    printArr(array, 8); printf("Value: %16X\n", uint16_t(0x1234));
    BitConverter::writeBytes(array, (uint64_t)0);
    delete[] array;

}