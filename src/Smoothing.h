#ifndef _SMOOTHING_H_
#define _SMOOTHING_H_

#define SMOOTHING_COUNT 3

struct Smoothing {

    int samples[SMOOTHING_COUNT];
    int index;
};

Smoothing smoothing_create() {

    Smoothing container;
    container.index = 0;

    for(unsigned int i = 0; i < SMOOTHING_COUNT; i++) {
        container.samples[i] = 0;
    }

    return container;
}

void smoothing_add(Smoothing& container, int value) {

    int i = container.index;

    container.samples[i] = value;
    i++;

    if(i >= SMOOTHING_COUNT) {
        i = 0;
    }

    container.index = i;
}

int smoothing_calc(const Smoothing& container) {

    int res = 0;

    for(unsigned int i = 0; i < SMOOTHING_COUNT; i++) {
        res = res + container.samples[i];
    }

    return res / SMOOTHING_COUNT;
}

#endif
// _SMOOTHING_H_