#ifndef __LIGHTS_H__
#define __LIGHTS__
#ifdef __cplusplus
extern "C"{
#endif

char * putLights( char *, int );
char * setServo( char * json, int jsonLen );
bool getLights(u32_t ledno);
void lights_init();

#ifdef __cplusplus
}
#endif

#endif
