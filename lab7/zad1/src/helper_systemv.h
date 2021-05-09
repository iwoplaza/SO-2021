#ifndef LAB7_HELPER_SYSTEMV_H
#define LAB7_HELPER_SYSTEMV_H

typedef struct Helper_t {
    int sem_group_id;
    int shm_id;
    SharedMemoryData_t* shared_data;
} Helper_t;

#endif //LAB7_HELPER_SYSTEMV_H