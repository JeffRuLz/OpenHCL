#ifndef common_callbacks_h
#define common_callbacks_h

#ifdef __cplusplus
extern "C" {
#endif

int running();

int exitCallback(int arg1, int arg2, void *common);
int setupCallbacks(void);

void suspendMusic();
void resumeMusic();

#ifdef __cplusplus
}
#endif

#endif
