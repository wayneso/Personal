#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>

#define RING_BUFFER_SIZE 64  // 可根据需求调整缓冲区大小

typedef struct {
    int buffer[RING_BUFFER_SIZE];
    int head;
    int tail;
    bool full;
} RingBuffer;

// 初始化环形缓冲区
void initRingBuffer(RingBuffer *rb);

// 检查环形缓冲区是否为空
bool isRingBufferEmpty(RingBuffer *rb);

// 检查环形缓冲区是否已满
bool isRingBufferFull(RingBuffer *rb);

// 向环形缓冲区写入数据
bool writeRingBuffer(RingBuffer *rb, int data);

// 从环形缓冲区读取数据
bool readRingBuffer(RingBuffer *rb, int *data);

#endif // RING_BUFFER_H
