#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

#if __STDC_VERSION__ < 199901L
#define MQTT_INLINE __inline
#else
#define MQTT_INLINE inline
#endif

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#endif
