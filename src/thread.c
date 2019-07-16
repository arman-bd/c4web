typedef struct ThreadData {
    SOCKET socket;
} ThreadData;

typedef struct ThreadPool {
    HANDLE thread_id;
    SOCKET socket_id;
    int status; // [ 1 = Open, 0 = Closed ]
    int timer;
} ThreadPool;

// Fixed Maximum
#define MAX_THREAD 1000
#define MAX_THREAD_TIME 30000 // 30 Seconds
ThreadPool TP[MAX_THREAD];
static int THREAD_ACTIVE = 0;

int RegisterThread(HANDLE thread_id, SOCKET socket_id){
    int i;

    for(i = 0; i < MAX_THREAD; i++){
        if(TP[i].thread_id == (HANDLE)-1){
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

int UnregisterThread(HANDLE thread_id, SOCKET socket_id){
    int i;

    for(i = 0; i < MAX_THREAD; i++){
        if(TP[i].thread_id == thread_id){
            TP[i].thread_id = (HANDLE)-1;
            TP[i].socket_id = (SOCKET)-1;
            TP[i].status = -1;
            TP[i].timer = -1;
            THREAD_ACTIVE--;
            return 1;
        }
    }

    return 0;
}

DWORD WINAPI HandleThread() {
    int i, isZero = 0;

    // Initialize Thread Pool
    for(i = 0; i < MAX_THREAD; i++){
        TP[i].thread_id = (HANDLE)-1;
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
                }else{
                    printf("\n>Thread Timeout: %d", (int)TP[i].thread_id);
                    // Kill Thread
                    closesocket(TP[i].socket_id);
                    TerminateThread(TP[i].thread_id, 0);
                    CloseHandle((HANDLE)TP[i].thread_id);

                    // Invalidate
                    TP[i].thread_id = (HANDLE)-1;
                    TP[i].socket_id = (SOCKET)-1;
                    TP[i].status = -1;
                    TP[i].timer = -1;
                    THREAD_ACTIVE--;
                    return 1;
                }
            }

            // Fixing Thread Count: Lost Due To OS Intervention
            if(isZero == 0){THREAD_ACTIVE = 0;}

            Sleep(1);
        }else{
            Sleep(10);
        }
    }
}

