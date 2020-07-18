#ifndef common_registry_h
#define common_registry_h

#ifdef __cplusplus
extern "C" {
#endif

int get_registry_value(const char *dir, const char *name, unsigned int *val);

#ifdef __cplusplus
}
#endif

#endif