/** THIS IS AN AUTOMATICALLY GENERATED FILE.  DO NOT MODIFY
 * BY HAND!!
 *
 * Generated by lcm-gen
 **/

#include <stdint.h>
#include <stdlib.h>
#include <lcm/lcm_coretypes.h>
#include <lcm/lcm.h>

#ifndef _cam_t_h
#define _cam_t_h

#ifdef __cplusplus
extern "C" {
#endif

#include "info_t.h"
#define CAM_T_YES 1
#define CAM_T_NO 0

typedef struct _cam_t cam_t;
struct _cam_t
{
    info_t     info;
    int8_t     hasVision;
    double     p0[2];
    double     p1[2];
    double     p2[2];
    double     p3[2];
};
 
cam_t   *cam_t_copy(const cam_t *p);
void cam_t_destroy(cam_t *p);

typedef struct _cam_t_subscription_t cam_t_subscription_t;
typedef void(*cam_t_handler_t)(const lcm_recv_buf_t *rbuf, 
             const char *channel, const cam_t *msg, void *user);

int cam_t_publish(lcm_t *lcm, const char *channel, const cam_t *p);
cam_t_subscription_t* cam_t_subscribe(lcm_t *lcm, const char *channel, cam_t_handler_t f, void *userdata);
int cam_t_unsubscribe(lcm_t *lcm, cam_t_subscription_t* hid);

int  cam_t_encode(void *buf, int offset, int maxlen, const cam_t *p);
int  cam_t_decode(const void *buf, int offset, int maxlen, cam_t *p);
int  cam_t_decode_cleanup(cam_t *p);
int  cam_t_encoded_size(const cam_t *p);

// LCM support functions. Users should not call these
int64_t __cam_t_get_hash(void);
int64_t __cam_t_hash_recursive(const __lcm_hash_ptr *p);
int     __cam_t_encode_array(void *buf, int offset, int maxlen, const cam_t *p, int elements);
int     __cam_t_decode_array(const void *buf, int offset, int maxlen, cam_t *p, int elements);
int     __cam_t_decode_array_cleanup(cam_t *p, int elements);
int     __cam_t_encoded_array_size(const cam_t *p, int elements);
int     __cam_t_clone_array(const cam_t *p, cam_t *q, int elements);

#ifdef __cplusplus
}
#endif

#endif