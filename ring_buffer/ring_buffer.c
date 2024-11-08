/*
 * @Author: Wayne 2546850503@qq.com
 * @Date: 2024-11-08 14:44:03
 * @LastEditors: Wayne 2546850503@qq.com
 * @LastEditTime: 2024-11-08 14:49:53
 * @FilePath: \undefinedd:\Desktop\gitclone\ring_buffer\ring_buffer.c
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include "ring_buffer.h"
#include <stdio.h>

void initRingBuffer(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->full = false;
}

bool isRingBufferEmpty(RingBuffer *rb) {
    return (rb->head == rb->tail && !rb->full);
}

bool isRingBufferFull(RingBuffer *rb) {
    return rb->full;
}

bool writeRingBuffer(RingBuffer *rb, int data) {
    if (isRingBufferFull(rb)) {
        printf("环形缓冲区已满，无法写入数据 %d\n", data);
        return false;
    }

    rb->buffer[rb->tail] = data;
    rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;

    if (rb->tail == rb->head) {
        rb->full = true;
    }
    return true;
}

bool readRingBuffer(RingBuffer *rb, int *data) {
    if (isRingBufferEmpty(rb)) {
        printf("环形缓冲区为空，无法读取数据\n");
        return false;
    }

    *data = rb->buffer[rb->head];
    rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
    rb->full = false;
    return true;
}
