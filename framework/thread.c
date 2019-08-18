typedef struct ThreadData {
    SOCKET socket;
} ThreadData;

typedef struct ThreadPool {
    THREAD_T thread_id;
    SOCKET socket_id;
    int status; // [ 1 = Open, 0 = Closed ]
    int timer;
} ThreadPool;

// Fixed Maximum
#define MAX_THREAD 1000
#define MAX_THREAD_TIME 30000 // 30 Seconds
ThreadPool TP[MAX_THREAD];
static int THREAD_ACTIVE = 0;

DWORD OSAPI HandleThread();
int UnregisterThread(THREAD_T thread_id, SOCKET socket_id);
int RegisterThread(THREAD_T thread_id, SOCKET socket_id);




int RegisterThread(THREAD_T thread_id, SOCKET socket_id){
    int i;

    for(i = 0; i < MAX_THREAD; i++){
        if(TP[i].thread_id == (THREAD_T)-1){
            TP[i].thread_id = thread_id;
            TP[i].socket_id = socket_id;
            TP[i].status = 1;
            TP[i].timer = MAX_THREAD_TIME;
            THREAD_ACTIVE++;
            return 1;
        }
    }

    return 0;
}

int UnregisterThread(THREAD_T thread_id, SOCKET socket_id){
    int i;

    for(i = 0; i < MAX_THREAD; i++){
        if(TP[i].thread_id == thread_id){
            TP[i].thread_id = (THREAD_T)-1;
            TP[i].socket_id = (SOCKET)-1;
            TP[i].status = -1;
            TP[i].timer = -1;
            THREAD_ACTIVE--;
            return 1;
        }
    }

    return 0;
}

DWORD OSAPI HandleThread() {
    int i, isZero = 0;

    // Initialize Thread Pool
    for(i = 0; i < MAX_THREAD; i++){
        TP[i].thread_id = (THREAD_T)-1;
        TP[i].socket_id = (SOCKET)-1;
        TP[i].status = -1;
        TP[i].timer = -1;
    }

    // Check For Thread Timer
    while(1){
        if(THREAD_ACTIVE > 0){
            isZero = 0;
            for(i = 0; i < MAX_THREAD; i++){
                if(TP[i].timer > 0){
                    isZero = 1;
                    TP[i].timer--;
                }else if(TP[i].status != -1){
                    printf("\n>Thread Timeout: %d", (int)TP[i].thread_id);
                    // Kill Thread
                    closesocket(TP[i].socket_id);
 #if defined(MSDOS) || defined(WIN32) || defined(__CYGWIN__)
                    TerminateThread(TP[i].thread_id, 0);
                    CloseHandle((THREAD_T)TP[i].thread_id);
#else
                    pthread_cancel(TP[i].thread_id);
#endif
                    // Invalidate
                    TP[i].thread_id = (THREAD_T)-1;
                    TP[i].socket_id = (SOCKET)-1;
                    TP[i].status = -1;
                    TP[i].timer = -1;
                    THREAD_ACTIVE--;
                    return 1;
                }
            }

            // Fixing Thread Count: Lost Due To OS Intervention
            if(isZero == 0){THREAD_ACTIVE = 0;}

            sleep_ms(1);
        }else{
            sleep_ms(10);
        }
    }
}

